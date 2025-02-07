/*******************************************************************************
  IGMP host module implementation

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    - Provides the implementation for IGMPv3 host with SSM and ASM support

*******************************************************************************/

/*
Copyright (C) 2016-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_IGMP
#include <stdlib.h>
#include <string.h>

#include "tcpip/src/tcpip_private.h"


#if defined(TCPIP_STACK_USE_IPV4)
#if defined(TCPIP_STACK_USE_IGMP)


#include "igmp_private.h"



// local variables
//
static int                  igmpInitCount = 0;      // IGMP module initialization count

static TCPIP_SIGNAL_HANDLE    igmpSignalHandle = NULL;

static OSAL_MUTEX_HANDLE_TYPE igmpListsMutex;       // IGMP synchronization object: protection for access to the IGMP
                                                    // lists between user threads and manager thread
                                                    // mutex selected so priority inheritance kicks in when IGMP itself(manager)
                                                    // has to block

static OSAL_MUTEX_HANDLE_TYPE igmpDcptMutex;        // IGMP synchronization object: protection for access to the IGMP
                                                    // global descriptor between user threads and manager thread
                                                    // mutex selected so priority inheritance kicks in when IGMP itself(manager)
                                                    // has to block
static TCPIP_IGMP_GROUPS_DCPT igmpGroupsDcpt;       // IGMP groups descriptor

static size_t               igmpInterfaces;         // number of interfaces
static uint32_t             igmpSsmAddLow;          // SSM range descriptor
static uint32_t             igmpSsmAddHigh;

static TCPIP_IGMP_SC_REPORT_NODE   igmpScReportPool[TCPIP_IGMP_MCAST_GROUPS * 3];   // IGMP State Change reports pool
                                                                                    // Max 3 reports per group may be needed:
                                                                                    // allow, block, and a filter is active.

static SINGLE_LIST  igmpScFreeList;             // pool of free report nodes to be used for reports
static SINGLE_LIST  igmpScReportList;           // list of reports scheduled to be sent

static TCPIP_IGMP_GEN_QUERY_REPORT_NODE igmpGenQueryReportPool[TCPIP_IGMP_GEN_QUERY_POOL * TCPIP_IGMP_INTERFACES];    // IGMP General query reports pool per interface
static TCPIP_IGMP_GROUP_QUERY_REPORT_NODE     igmpGroupQueryReportPool[TCPIP_IGMP_GROUP_QUERY_POOL];    // IGMP Group query reports pool


static SINGLE_LIST  igmpGenQueryFreeList;        // pool of free GQ reports that could be scheduled 
static SINGLE_LIST  igmpGroupQueryFreeList;      // pool of free Group reports that could be scheduled 

static SINGLE_LIST  igmpGenQueryReportList;     // list of GQ reports scheduled to be sent
static SINGLE_LIST  igmpGroupQueryReportList;   // list of Group reports scheduled to be sent

static uint8_t      igmpRobustnessVar = (uint8_t)TCPIP_IGMP_ROBUSTNESS_VARIABLE; // robustness variable count

static uint32_t     igmpUnsolicitRepInterval;       // unsolicited report interval, ms

#if defined(TCPIP_IGMP_USER_NOTIFICATION ) && (TCPIP_IGMP_USER_NOTIFICATION != 0)
static PROTECTED_SINGLE_LIST   igmpRegisteredUsers;
static const void*             igmpMemH;

#endif  // defined(TCPIP_IGMP_USER_NOTIFICATION ) && (TCPIP_IGMP_USER_NOTIFICATION != 0)

// conversion functions/helpers
//
static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_RepNode2SglNode(TCPIP_IGMP_SC_REPORT_NODE* pRepNode)
{
    union
    {
        TCPIP_IGMP_SC_REPORT_NODE* pRepNode;
        SGL_LIST_NODE*  node;
    }U_REP_NODE_SGL_NODE;

    U_REP_NODE_SGL_NODE.pRepNode = pRepNode;
    return U_REP_NODE_SGL_NODE.node;
}

static __inline__ TCPIP_IGMP_SC_REPORT_NODE* __attribute__((always_inline)) FC_SglNode2RepNode(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        TCPIP_IGMP_SC_REPORT_NODE* pRepNode;
    }U_SGL_NODE_REP_NODE;

    U_SGL_NODE_REP_NODE.node = node;
    return U_SGL_NODE_REP_NODE.pRepNode;
}


static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_GqNode2SglNode(TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGqNode)
{
    union
    {
        TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGqNode;
        SGL_LIST_NODE*  node;
    }U_GQ_NODE_SGL_NODE;

    U_GQ_NODE_SGL_NODE.pGqNode = pGqNode;
    return U_GQ_NODE_SGL_NODE.node;
}

static __inline__ TCPIP_IGMP_GEN_QUERY_REPORT_NODE* __attribute__((always_inline)) FC_SglNode2GqNode(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGqNode;
    }U_SGL_NODE_GQ_NODE;

    U_SGL_NODE_GQ_NODE.node = node;
    return U_SGL_NODE_GQ_NODE.pGqNode;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_GroupNode2SglNode(TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pGroupNode)
{
    union
    {
        TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pGroupNode;
        SGL_LIST_NODE*  node;
    }U_GROUP_NODE_SGL_NODE;

    U_GROUP_NODE_SGL_NODE.pGroupNode = pGroupNode;
    return U_GROUP_NODE_SGL_NODE.node;
}

static __inline__ TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* __attribute__((always_inline)) FC_SglNode2GroupNode(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pGroupNode;
    }U_SGL_NODE_GROUP_NODE;

    U_SGL_NODE_GROUP_NODE.node = node;
    return U_SGL_NODE_GROUP_NODE.pGroupNode;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_QRepNode2SglNode(TCPIP_IGMP_QUERY_REPORT_NODE* pQrNode)
{
    union
    {
        TCPIP_IGMP_QUERY_REPORT_NODE* pQrNode;
        SGL_LIST_NODE*  node;
    }U_QUERY_REP_NODE_SGL_NODE;

    U_QUERY_REP_NODE_SGL_NODE.pQrNode = pQrNode;
    return U_QUERY_REP_NODE_SGL_NODE.node;
}

static __inline__ TCPIP_IGMP_QUERY_REPORT_NODE* __attribute__((always_inline)) FC_SglNode2QRepNode(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        TCPIP_IGMP_QUERY_REPORT_NODE* pQrNode;
    }U_SGL_NODE_QUERY_REP_NODE;

    U_SGL_NODE_QUERY_REP_NODE.node = node;
    return U_SGL_NODE_QUERY_REP_NODE.pQrNode;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_IgmpHndl2SglNode(TCPIP_IGMP_HANDLE igHandle)
{
    union
    {
        TCPIP_IGMP_HANDLE igHandle;
        SGL_LIST_NODE*  node;
    }U_IGMP_HNDL_SGL_NODE;

    U_IGMP_HNDL_SGL_NODE.igHandle = igHandle;
    return U_IGMP_HNDL_SGL_NODE.node;
}


static __inline__ TCPIP_IGMP_GEN_QUERY_REPORT_NODE* __attribute__((always_inline)) FC_RepNode2GqNode(TCPIP_IGMP_QUERY_REPORT_NODE* repNode)
{
    union
    {
        TCPIP_IGMP_QUERY_REPORT_NODE* repNode;
        TCPIP_IGMP_GEN_QUERY_REPORT_NODE*  gqNode;
    }U_REP_NODE_GQ_NODE;

    U_REP_NODE_GQ_NODE.repNode = repNode;
    return U_REP_NODE_GQ_NODE.gqNode;
}

static __inline__ TCPIP_IGMP_HEADER* __attribute__((always_inline)) FC_U8ptr2IgmpHdr(uint8_t* pBuff)
{
    union
    {
        uint8_t* pBuff;
        TCPIP_IGMP_HEADER*  pHdr;
    }U_UPTR_IGMP_HDR;

    U_UPTR_IGMP_HDR.pBuff = pBuff;
    return U_UPTR_IGMP_HDR.pHdr;
}

static __inline__ TCPIP_IGMPv3_QUERY_MESSAGE* __attribute__((always_inline)) FC_U8ptr2QueryMsg(uint8_t* pBuff)
{
    union
    {
        uint8_t* pBuff;
        TCPIP_IGMPv3_QUERY_MESSAGE*  pQMsg;
    }U_UPTR_Q_MSG;

    U_UPTR_Q_MSG.pBuff = pBuff;
    return U_UPTR_Q_MSG.pQMsg;
}

static __inline__ TCPIP_IGMPv3_QUERY_MESSAGE* __attribute__((always_inline)) FC_IgmpHdr2QueryMsg(TCPIP_IGMP_HEADER* pHdr)
{
    union
    {
        TCPIP_IGMP_HEADER* pHdr;
        TCPIP_IGMPv3_QUERY_MESSAGE*  pQMsg;
    }U_IGMP_HDR_Q_MSG;

    U_IGMP_HDR_Q_MSG.pHdr = pHdr;
    return U_IGMP_HDR_Q_MSG.pQMsg;
}

static __inline__ TCPIP_IGMP_QUERY_REPORT_NODE* __attribute__((always_inline)) FC_GrRep2QRep(TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pGrRep)
{
    union
    {
        TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pGrRep;
        TCPIP_IGMP_QUERY_REPORT_NODE*  pQRep;
    }U_GROUP_REP_Q_REP;

    U_GROUP_REP_Q_REP.pGrRep = pGrRep;
    return U_GROUP_REP_Q_REP.pQRep;
}

static __inline__ TCPIP_IGMP_QUERY_REPORT_NODE* __attribute__((always_inline)) FC_GenQRep2QRep(TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGenRep)
{
    union
    {
        TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGenRep;
        TCPIP_IGMP_QUERY_REPORT_NODE*  pQRep;
    }U_GEN_REP_Q_REP;

    U_GEN_REP_Q_REP.pGenRep = pGenRep;
    return U_GEN_REP_Q_REP.pQRep;
}

static __inline__ TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* __attribute__((always_inline)) FC_QRepNode2GrRepNode(TCPIP_IGMP_QUERY_REPORT_NODE* repNode)
{
    union
    {
        TCPIP_IGMP_QUERY_REPORT_NODE* repNode;
        TCPIP_IGMP_GROUP_QUERY_REPORT_NODE*  grpQRep;
    }U_REP_NODE_GROUP_REP_NODE;

    U_REP_NODE_GROUP_REP_NODE.repNode = repNode;
    return U_REP_NODE_GROUP_REP_NODE.grpQRep;
}

static __inline__ TCPIP_IGMPv3_REPORT_MESSAGE* __attribute__((always_inline)) FC_U8ptr2RepMsg(uint8_t* pBuff)
{
    union
    {
        uint8_t* pBuff;
        TCPIP_IGMPv3_REPORT_MESSAGE*  pRepMsg;
    }U_UPTR_REP_MSG;

    U_UPTR_REP_MSG.pBuff = pBuff;
    return U_UPTR_REP_MSG.pRepMsg;
}

static __inline__ TCPIP_IGMPv3_GROUP_RECORD* __attribute__((always_inline)) FC_U8ptr2GrpRec(uint8_t* pBuff)
{
    union
    {
        uint8_t* pBuff;
        TCPIP_IGMPv3_GROUP_RECORD*  pGrpRec;
    }U_UPTR_GRP_REC;

    U_UPTR_GRP_REC.pBuff = pBuff;
    return U_UPTR_GRP_REC.pGrpRec;
}

static __inline__ TCPIP_IGMP_GROUP_ENTRY* __attribute__((always_inline)) FC_HashE2GrpE(OA_HASH_ENTRY* hE)
{
    union
    {
        OA_HASH_ENTRY* hE;
        TCPIP_IGMP_GROUP_ENTRY*  pGrpE;
    }U_HASH_E_GRP_E;

    U_HASH_E_GRP_E.hE = hE;
    return U_HASH_E_GRP_E.pGrpE;
}

static __inline__ TCPIP_IGMP_SOURCE_ENTRY* __attribute__((always_inline)) FC_HashE2SrcE(OA_HASH_ENTRY* hE)
{
    union
    {
        OA_HASH_ENTRY* hE;
        TCPIP_IGMP_SOURCE_ENTRY*  pSrcE;
    }U_HASH_E_SRC_E;

    U_HASH_E_SRC_E.hE = hE;
    return U_HASH_E_SRC_E.pSrcE;
}


// prototypes
//

static void         TCPIP_IGMP_Process(void);
static void         TCPIP_IGMP_Tick(void);
static void         TCPIP_IGMP_SourceChangeTimeout(void);
static void         TCPIP_IGMP_GenQueryTimeout(void);
static void         TCPIP_IGMP_GroupQueryTimeout(void);

static IPV4_PACKET* F_IGMP_AllocateTxPacketStruct (uint16_t totIGMPLen);
static void         F_IGMP_TxPktAcknowledge(TCPIP_MAC_PACKET* pTxPkt, const void* ackParam);
static TCPIP_IGMP_RESULT F_IGMP_GenerateStateReport(IPV4_ADDR mcastAddress, size_t ifIx, TCPIP_IGMP_GIF_STATE_DCPT* pOldDcpt, TCPIP_IGMP_GIF_STATE_DCPT* pNewDcpt);
static TCPIP_IGMP_RESULT F_IGMP_ScheduleFmcReport(IPV4_ADDR groupAddress, TCPIP_IGMPv3_RECORD_TYPE repType, size_t ifIx, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pGroupSources);
static TCPIP_IGMP_RESULT F_IGMP_ScheduleSlcReport(IPV4_ADDR groupAddress, size_t ifIx, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pNewAllow, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pNewBlock);

static TCPIP_IGMP_SC_REPORT_NODE* F_IGMP_FindGifSCReportsDisable(IPV4_ADDR groupAddress, size_t ifIx, TCPIP_IGMP_SC_REPORT_NODE** ppAllowNode, TCPIP_IGMP_SC_REPORT_NODE** ppBlockNode);

static TCPIP_IGMP_SC_REPORT_NODE* F_IGMP_GetNewScReport(void);
static void     F_IGMP_InsertScReport(SINGLE_LIST* pList, TCPIP_IGMP_SC_REPORT_NODE* pRepNode);
static void     F_IGMP_DeleteScReport(SINGLE_LIST* pList, TCPIP_IGMP_SC_REPORT_NODE* pRepNode);

static TCPIP_IGMP_SC_REPORT_NODE* F_IGMP_FindScheduledScReport(IPV4_ADDR groupAddress, size_t ifIx, TCPIP_IGMP_SC_REPORT_NODE** ppAllowNode, TCPIP_IGMP_SC_REPORT_NODE** ppBlockNode, bool removeAllowBlock);

static TCPIP_IGMP_QUERY_REPORT_NODE* F_IGMP_FindScheduledQueryReport(TCPIP_IGMP_QUERY_TYPE qType, uint32_t groupAddress, size_t ifIx, bool repRemove);

static void     F_IGMP_SetGifSCRecord(TCPIP_IGMP_SC_REPORT_NODE* pRepNode, IPV4_ADDR groupAddress, TCPIP_IGMPv3_RECORD_TYPE repType, size_t ifIx, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pGroupSources);

static size_t   F_IGMP_GroupKeyHash(OA_HASH_DCPT* pOH, const void* key);
static int      F_IGMP_GroupKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key);
static void     F_IGMP_GroupKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key);
#if defined(OA_DOUBLE_HASH_PROBING)
static size_t   F_IGMP_GroupProbeHash(OA_HASH_DCPT* pOH, const void* key);
#endif  // defined(OA_DOUBLE_HASH_PROBING)
static size_t   F_IGMP_SourceKeyHash(OA_HASH_DCPT* pOH, const void* key);
static int      F_IGMP_SourceKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key);
static void     F_IGMP_SourceKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key);
#if defined(OA_DOUBLE_HASH_PROBING)
static size_t   F_IGMP_SourceProbeHash(OA_HASH_DCPT* pOH, const void* key);
#endif  // defined(OA_DOUBLE_HASH_PROBING)

static bool     F_IGMP_IsMcastValid(IPV4_ADDR addr);

static bool  F_IGMP_IsSsmAddress(IPV4_ADDR mcastAdd);

static bool     F_IGMP_CheckIfIndex(size_t ifIx, bool itExists);

static TCPIP_IGMP_RESULT F_IGMP_CheckSubscribeParams(TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, const IPV4_ADDR* sourceList, size_t* listSize, size_t* pIfIx);

static TCPIP_IGMP_RESULT F_IGMP_SocketRemove(UDP_SOCKET uSkt, size_t ifIx, IPV4_ADDR mcastAddress, size_t* nRemoves);

static TCPIP_IGMP_RESULT    F_IGMP_SocketUpdateSources(UDP_SOCKET uSkt, size_t ifIx, IPV4_ADDR mcastAddress, TCPIP_IGMP_FILTER_TYPE filterMode, const IPV4_ADDR* sourceList, size_t* listSize);

static OA_HASH_DCPT* F_IGMP_GetSourceHashDcpt(TCPIP_IGMP_GROUP_ENTRY* pGEntry);

static void     F_IGMP_SourceEntryInit(TCPIP_IGMP_SOURCE_ENTRY* pSEntry);

static TCPIP_IGMP_SKT_RECORD*   F_IGMP_SourceFindSktRecord(TCPIP_IGMP_SOURCE_ENTRY* pSEntry, UDP_SOCKET uSkt, size_t ifIx, TCPIP_IGMP_FILTER_TYPE filtType);

static TCPIP_IGMP_SKT_RECORD*   F_IGMP_SourceCreateSktRecord(TCPIP_IGMP_SOURCE_ENTRY* pSEntry, UDP_SOCKET uSkt, size_t ifIx, TCPIP_IGMP_FILTER_TYPE filtType);

static size_t   F_IGMP_GroupPurgeSktByFilter(TCPIP_IGMP_GROUP_ENTRY* pGEntry, UDP_SOCKET uSkt, size_t ifIx, TCPIP_IGMP_FILTER_TYPE* pPurgeFilt);

static bool     F_IGMP_GroupPurgeSktBySource(TCPIP_IGMP_GROUP_ENTRY* pGEntry, UDP_SOCKET uSkt, size_t ifIx, const IPV4_ADDR* pPurgeAdd);

static TCPIP_IGMP_SIF_FILTER_ACTION     F_IGMP_GetSifFilter(const TCPIP_IGMP_SOURCE_ENTRY* pSEntry, size_t ifIx, TCPIP_IGMP_FILTER_TYPE* pFilter);

static void     F_IGMP_GetGifState(TCPIP_IGMP_GROUP_ENTRY* pGEntry, size_t ifIx, TCPIP_IGMP_GIF_STATE_DCPT* pGifDcpt);

static void     F_IGMP_SubtractSources(TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pA, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pB, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pDest);
static void     F_IGMP_UnionSources(TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pA, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pB, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pDest);
static void     F_IGMP_IntersectSources(TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pA, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pB, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pDest);

static int      F_IGMPSourceEntryCompare(const void* p1, const void* p2);

static bool     F_IGMP_SendScReport(size_t ifIx, SINGLE_LIST* pReportList);

static void     F_IGMP_SetScReportTransmitTime(TCPIP_IGMP_SC_REPORT_NODE* pRNode);

static void     F_IGMP_ReportEvent(IPV4_ADDR mcastAddress, TCPIP_IGMP_EVENT_TYPE evType);

static bool     F_IGMP_GroupEntryCheckRemove(TCPIP_IGMP_GROUP_ENTRY* pGEntry);

static void     F_IGMP_ProcessV3Query(TCPIP_IGMPv3_QUERY_MESSAGE* pQuery, size_t ifIx);

static TCPIP_IGMP_QUERY_REPORT_NODE* F_IGMP_GetNewQueryReport(TCPIP_IGMP_QUERY_TYPE qType);

static void     F_IGMP_ScheduleGenQueryReport(TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGQNode, size_t ifIx, uint32_t tTransmit, bool insert);

static size_t   F_IGMP_GenerateGenQueryReport(TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGq);

static bool     F_IGMP_SendQueryReport(TCPIP_IGMP_QUERY_REPORT_NODE* pQNode, size_t nEntries);

static void     F_IGMP_InitGroupQueryReport(TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pGNode, IPV4_ADDR groupAddress, size_t ifIx, uint32_t tTransmit);

static void     F_IGMP_SourceScheduleGroupQueryReport(TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pGNode, size_t nRecSources, uint32_t* pRecSources, bool insert);

static bool     F_IGMP_GenerateGroupQueryReport(TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pGrpNode);

// implementation
//

#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_EVENTS) != 0)
static const char* T_IGMP_DbgEvNameTbl[] = 
{
    // general events
    "none",         // TCPIP_IGMP_EVENT_NONE
    "mtu",          // TCPIP_IGMP_EVENT_PACKET_EXCEED_MTU
    "alloc",        // TCPIP_IGMP_EVENT_PACKET_ALLOC_ERROR
    "transmit",     // TCPIP_IGMP_EVENT_PACKET_TRANSMIT_ERROR
    "sc_report",    // TCPIP_IGMP_EVENT_GET_SC_REPORT_ERROR
    "gq_report",    // TCPIP_IGMP_EVENT_GET_GEN_QUERY_REPORT_ERROR
    "grp_report",   // TCPIP_IGMP_EVENT_GET_GROUP_QUERY_REPORT_ERROR
    "src_exceed",   // TCPIP_IGMP_EVENT_QUERY_SOURCE_NUMBER_EXCEEDED
    "illegal",      // TCPIP_IGMP_EVENT_ illegal value
};

static void F_IGMP_DbgEvent(IPV4_ADDR mcastAddress, TCPIP_IGMP_EVENT_TYPE evType)
{
    char addBuff[20] = {'\0'};
    char dbgBuff[100] = {'\0'};

    TCPIP_UINT16_VAL evMask;
    evMask.Val = (uint16_t)evType; 
    if(evMask.v[1] != 0U)
    {
        (void)FC_sprintf(dbgBuff, sizeof(dbgBuff), "%s ", "unknown");
    } 
    else
    {
        uint8_t evIx = 1U;
        while(evMask.v[0] != 0U)
        {
            if((evMask.v[0] & 0x1U) != 0U)
            {
                (void)FC_sprintf(dbgBuff, sizeof(dbgBuff), "%s ", T_IGMP_DbgEvNameTbl[evIx]);  
            }
            evIx++;
            evMask.v[0] >>= 1U;
        }
    }

    (void)TCPIP_Helper_IPAddressToString(&mcastAddress, addBuff, sizeof(addBuff));
    SYS_CONSOLE_PRINT("IGMP Event: %s, add: %s\r\n", dbgBuff, addBuff);
}
#else
#define F_IGMP_DbgEvent(mcastAddress, evType)
#endif  // ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_EVENTS) != 0)

#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_REPORT_LISTS) != 0)
static void F_IGMP_ScListPrint(SINGLE_LIST* pScList);
static void F_IGMP_QueryListPrint(SINGLE_LIST* pQList, TCPIP_IGMP_QUERY_TYPE qType);

void TCPIP_IGMP_ReportListPrint(TCPIP_IGMP_REPORT_LIST_TYPE listType)
{
    SINGLE_LIST *pFreeScList, *pScList;
    SINGLE_LIST *pFreeGqList, *pGqList;
    SINGLE_LIST *pFreeGrpList, *pGrpList;
    bool needFree, needBusy;
    size_t nLists = 0U;

    pFreeScList = pScList = NULL;
    pFreeGqList = pGqList = NULL;
    pFreeGrpList = pGrpList = NULL;

    needFree = ((uint16_t)listType & (uint16_t)TCPIP_IGMP_REPORT_LIST_FLAG_FREE) != 0U;
    needBusy = ((uint16_t)listType & (uint16_t)TCPIP_IGMP_REPORT_LIST_FLAG_BUSY) != 0U;


    // detect the list types
    if(((uint16_t)listType & (uint16_t)TCPIP_IGMP_REPORT_LIST_SC) != 0U)
    {
        if(needFree)
        {
            pFreeScList = &igmpScFreeList;
        }
        if(needBusy)
        {
            pScList = &igmpScReportList;
        }
        nLists++;
    }
    if(((uint16_t)listType & (uint16_t)TCPIP_IGMP_REPORT_LIST_GEN_QUERY) != 0U)
    {
        if(needFree)
        {
            pFreeGqList = &igmpGenQueryFreeList;
        }
        if(needBusy)
        {
            pGqList = &igmpGenQueryReportList;
        }
        nLists++;
    }
    if(((uint16_t)listType & (uint16_t)TCPIP_IGMP_REPORT_LIST_GROUP_QUERY) != 0U)
    {
        if(needFree)
        {
            pFreeGrpList = &igmpGroupQueryFreeList;
        }
        if(needBusy)
        {
            pGrpList = &igmpGroupQueryReportList;
        }
        nLists++;
    }

    if(nLists == 0U)
    {
        SYS_CONSOLE_MESSAGE("IGMP Lists: no such list\r\n");
        return;
    }

    if(pFreeScList != NULL)
    {
        SYS_CONSOLE_PRINT("IGMP SC Free List: %d nodes\r\n", TCPIP_Helper_SingleListCount(pFreeScList));
    }
    if(pScList != NULL)
    {
        F_IGMP_ScListPrint(pScList);
    }

    if(pFreeGqList != NULL)
    {
        SYS_CONSOLE_PRINT("IGMP GQ Free List: %d nodes\r\n", TCPIP_Helper_SingleListCount(pFreeGqList));
    }
    if(pGqList != NULL)
    {
        F_IGMP_QueryListPrint(pGqList, TCPIP_IGMP_QUERY_GENERAL);
    }
    
    if(pFreeGrpList != NULL)
    {
        SYS_CONSOLE_PRINT("IGMP GRPQ Free List: %d nodes\r\n", TCPIP_Helper_SingleListCount(pFreeGrpList));
    }
    if(pGrpList != NULL)
    {
        F_IGMP_QueryListPrint(pGrpList, TCPIP_IGMP_QUERY_GROUP_SPEC);
    }

}

static const char* T_IGMP_RECORD_TYPE_STR_TBL[] =
{
    "invalid",
    "mode_inc",
    "mode_exc",
    "to_inc",
    "to_exc",
    "allow_new",
    "block_old",
};

static void F_IGMP_ScListPrint(SINGLE_LIST* pScList)
{
    uint32_t toTransMs;
    TCPIP_IGMP_SC_REPORT_NODE* pRNode;
    IPV4_ADDR groupAddr, srcAddr;
    const char* repType;
    size_t srcIx;
    uint32_t* pSrc;
    char addBuff[20] = {'\0'};

    for(pRNode = FC_SglNode2RepNode(pScList->head); pRNode != NULL; pRNode = pRNode->next)
    {
        uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet();
        toTransMs =  ((pRNode->tTransmit - SYS_TMR_TickCountGet()) * 1000U) / sysFreq;

        SYS_CONSOLE_PRINT("IGMP SC Node - if: %d, nTransmit: %d, active: %d, to Transmit: %d\r\n", pRNode->ifIx, pRNode->nTransmit, pRNode->active, toTransMs);

        groupAddr.Val = pRNode->repGroup;
        (void)TCPIP_Helper_IPAddressToString(&groupAddr, addBuff, sizeof(addBuff));
        repType = T_IGMP_RECORD_TYPE_STR_TBL[pRNode->repType];
        SYS_CONSOLE_PRINT("IGMP SC - group: %s, type: %s, nSources\r\n", addBuff, repType, pRNode->repSources.nSources);

        pSrc = pRNode->repSources.sourceAddresses;
        for(srcIx = 0; srcIx < pRNode->repSources.nSources; srcIx++)
        {
            srcAddr.Val = *pSrc;
            (void)TCPIP_Helper_IPAddressToString(&srcAddr, addBuff, sizeof(addBuff));

            SYS_CONSOLE_PRINT("IGMP SC Source - source: %s\r\n", addBuff);
            pSrc++;
        }
    }

}

static const char* F_IGMP_QueryTypeStr(TCPIP_IGMP_QUERY_TYPE qType, bool generic)
{
    if(generic)
    {
        return qType == TCPIP_IGMP_QUERY_GENERAL ? "GQ" : (qType == TCPIP_IGMP_QUERY_GROUP_SPEC  || qType == TCPIP_IGMP_QUERY_GROUP_SOURCE_SPEC) ? "GRPQ" : "invalid"; 
    }

    return qType == TCPIP_IGMP_QUERY_GENERAL ? "GQ" : qType == TCPIP_IGMP_QUERY_GROUP_SPEC ? "GSQ" : qType == TCPIP_IGMP_QUERY_GROUP_SOURCE_SPEC ? "GSSQ" : "invalid"; 
}

static void F_IGMP_QueryListPrint(SINGLE_LIST* pQList, TCPIP_IGMP_QUERY_TYPE qType)
{
    size_t qIx, srcIx;
    TCPIP_IGMP_QUERY_REPORT_NODE* pQNode;
    const char* strType;
    uint32_t toTransMs;
    TCPIP_IGMP_QUERY_SOURCES* pQSource;
    IPV4_ADDR groupAddr, srcAddr;
    uint32_t* pSrc;
    char addBuff[20] = {'\0'};


    strType = F_IGMP_QueryTypeStr(qType, true);

    for(pQNode = FC_SglNode2QRepNode(pQList->head); pQNode != NULL; pQNode = pQNode->next)
    {
        strType = F_IGMP_QueryTypeStr((TCPIP_IGMP_QUERY_TYPE)pQNode->queryType, false);
        uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet();
        toTransMs =  ((pQNode->tTransmit - SYS_TMR_TickCountGet()) * 1000U) / sysFreq;

        SYS_CONSOLE_PRINT("IGMP Q Node - if: %d, type: %s, qSources: %d, to Transmit: %d\r\n", pQNode->ifIx, strType, pQNode->nQSources, toTransMs);
        pQSource = pQNode->qSources;
        for(qIx = 0; qIx < pQNode->nQSources; qIx++)
        {
            groupAddr.Val = pQSource->repGroup;
            (void)TCPIP_Helper_IPAddressToString(&groupAddr, addBuff, sizeof(addBuff));
            strType = pQSource->repType == TCPIP_IGMP_MODE_IS_INCLUDE ? "inc" : pQSource->repType == TCPIP_IGMP_MODE_IS_EXCLUDE ? "exc" : "invalid";
            SYS_CONSOLE_PRINT("IGMP qSource - group: %s, type: %s, nSources: %d\r\n", addBuff, strType, pQSource->repSources.nSources);
            pSrc = pQSource->repSources.sourceAddresses;
            for(srcIx = 0; srcIx < pQSource->repSources.nSources; srcIx++)
            {
                srcAddr.Val = *pSrc;
                (void)TCPIP_Helper_IPAddressToString(&srcAddr, addBuff, sizeof(addBuff));

                SYS_CONSOLE_PRINT("IGMP qSource - source: %s\r\n", addBuff);
                pSrc++;
            }
            pQSource++;
        }
    }


}
#else
void TCPIP_IGMP_ReportListPrint(TCPIP_IGMP_REPORT_LIST_TYPE listType)
{
}

#endif  // ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_REPORT_LISTS) != 0)

#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_QUERY_MSG) != 0)
// respTime in seconds
TCPIP_IGMP_RESULT TCPIP_IGMP_SendQuery(uint32_t* pGroupAdd, uint32_t* sourceList, uint16_t nSources, uint16_t respTime, size_t ifIx, uint8_t rFlag)
{
    uint16_t pktSize;
    const TCPIP_NET_IF*    pNetIf;
    TCPIP_IGMPv3_QUERY_MESSAGE* pQuery;
    uint32_t*   pDest;
    IPV4_PACKET* pTxPkt;
    TCPIP_IPV4_PACKET_PARAMS pktParams;
    size_t srcIx;
    uint16_t r_mant, r_exp;
    TCPIP_IGMP_RESP_CODE_FLOAT rCodeFloat;

    if((rFlag & ((uint8_t)TCPIP_IGMP_ROUTE_INTERNAL | (uint8_t)TCPIP_IGMP_ROUTE_EXTERNAL)) == 0U)
    {
        return TCPIP_IGMP_ARG_ERROR;
    }

    if(pGroupAdd == NULL || *pGroupAdd == 0U)
    {   // general query
        pGroupAdd = NULL;
        nSources = 0;
        sourceList = NULL;
    }
    else if(nSources != 0U && sourceList == NULL)
    {
        return TCPIP_IGMP_ARG_ERROR;
    }
    else
    {
        // do nothing
    }

    pNetIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet((int)ifIx);
    if(!TCPIP_STACK_NetIsLinked(pNetIf))
    {
        return TCPIP_IGMP_IF_ERROR;
    }

    // calculate the max resp code
    r_mant = respTime * 10U;
    r_exp = 0U;
    do
    {
        r_mant /= 2U;
        r_exp++;
    }while(r_mant > 0x1fU && r_exp < 3U);

    if(r_mant < 0x10U || r_exp > 10U)
    {   // cannot be represented
        return TCPIP_IGMP_ARG_ERROR;
    }
    rCodeFloat.r_mant = (uint8_t)(r_mant - 0x10U);
    rCodeFloat.r_exp = (uint8_t)(r_exp - 3U);
    rCodeFloat.unit = 1U;

    // calculate packet size
    pktSize = (uint16_t)sizeof(TCPIP_IGMPv3_QUERY_MESSAGE_BARE) + nSources * (uint16_t)sizeof(*((TCPIP_IGMPv3_QUERY_MESSAGE*)0)->sources);

    pTxPkt = F_IGMP_AllocateTxPacketStruct(pktSize);
    if(pTxPkt == NULL)
    {   // failed to allocate
        return  TCPIP_IGMP_PACKET_ALLOC_ERROR;
    }

    pTxPkt->netIfH = pNetIf;
    pTxPkt->srcAddress.Val = TCPIP_STACK_NetAddress(pTxPkt->netIfH);
    pTxPkt->destAddress.Val = TCPIP_IGMP_IANA_ALL_SYSTEMS_MULTICAST;
    pTxPkt->macPkt.pDSeg->segLen += pktSize;
    pktParams.tosFlags = TCPIP_IPV4_PRECEDENCE_INET_CONTROL;
    pktParams.ttl = TCPIP_IGMP_PACKET_TTL;
    pktParams.df = 0;


    TCPIP_IPV4_PacketFormatTx(pTxPkt, (uint8_t)IP_PROT_IGMP, pktSize, &pktParams);

    // populate the query message
    pQuery = FC_U8ptr2QueryMsg(pTxPkt->macPkt.pTransportLayer);
    (void)memset(pQuery, 0, sizeof(*pQuery));
    pQuery->type = (uint8_t)TCPIP_IGMP_MESSAGE_QUERY;
    pQuery->maxRespCode = rCodeFloat.val; 
    pQuery->groupAddress = pGroupAdd != NULL ? *pGroupAdd : 0U;
    pQuery->qqic = TCPIP_IGMP_QQIC_INTERVAL_CODE;
    pQuery->nSources = TCPIP_Helper_htons(nSources);
    // set sources
    pDest = pQuery->sources;
    for(srcIx = 0; srcIx < nSources; srcIx++)
    {
        *pDest++ = *sourceList++;
    }

    // calculate checksum
    pQuery->checksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pQuery, pktSize, 0);

    TCPIP_PKT_FlightLogTx(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID);

    if((rFlag & (uint8_t)TCPIP_IGMP_ROUTE_EXTERNAL) != 0U)
    {
        if((rFlag & (uint8_t)TCPIP_IGMP_ROUTE_INTERNAL) != 0U)
        {   // route both int and ext
            pTxPkt->macPkt.modPktData = 1U;
        }
        if(!TCPIP_IPV4_PacketTransmit(pTxPkt))
        {
            TCPIP_PKT_FlightLogAcknowledge(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_IP_REJECT_ERR);
            TCPIP_PKT_PacketFree(&pTxPkt->macPkt);
            return TCPIP_IGMP_PACKET_TRANSMIT_ERROR;
        }
        return TCPIP_IGMP_OK;
    }

    // route internally only
    pTxPkt->macPkt.modPktData = 0U;
    pTxPkt->macPkt.pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_MCAST;
    TCPIPStackInsertRxPacket(pNetIf, &pTxPkt->macPkt, true);

    return TCPIP_IGMP_OK;
}
#else
TCPIP_IGMP_RESULT TCPIP_IGMP_SendQuery(uint32_t* pGroupAdd, uint32_t* sourceList, uint16_t nSources, uint16_t respTime, size_t ifIx, uint8_t rFlag)
{
    return TCPIP_IGMP_IF_ERROR; 
}
#endif // ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_QUERY_MSG) != 0)

#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
static uint32_t igmpLLockCount = 0U;
#endif
// locks access to the IGMP report lists 
// between user threads and manager/dispatcher thread
static void igmpListsLock(void)
{
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
    TCPIPStack_Assert(igmpLLockCount == 0U, __FILE__, __func__, __LINE__);
#endif
    (void)OSAL_MUTEX_Lock(&igmpListsMutex, OSAL_WAIT_FOREVER);
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
    igmpLLockCount++;
#endif
}

// unlocks access to IGMP lists and descriptor
static void igmpListsUnlock(void)
{
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
    TCPIPStack_Assert(igmpLLockCount == 1U, __FILE__, __func__, __LINE__);
#endif
    (void)OSAL_MUTEX_Unlock(&igmpListsMutex);
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
    igmpLLockCount--;
#endif
}

// locks access to the IGMP global descriptor 
// between user threads and manager/dispatcher thread
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
static uint32_t igmpDLockCount = 0U;
#endif
static __inline__ void __attribute__((always_inline)) igmpDcptLock(void)
{
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
    TCPIPStack_Assert(igmpDLockCount == 0U, __FILE__, __func__, __LINE__);
#endif
    (void)OSAL_MUTEX_Lock(&igmpDcptMutex, OSAL_WAIT_FOREVER);
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
    igmpDLockCount++;
#endif
}
// unlocks access to IGMP lists and descriptor
static __inline__ void __attribute__((always_inline)) igmpDcptUnlock(void)
{
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
    TCPIPStack_Assert(igmpDLockCount == 1U, __FILE__, __func__, __LINE__);
#endif
    (void)OSAL_MUTEX_Unlock(&igmpDcptMutex);
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
    igmpDLockCount--;
#endif
}

// checks that an IPv4 address is multicast
static __inline__ bool __attribute__((always_inline)) F_IGMP_IsMcastAddress(IPV4_ADDR addr)
{
    return ((addr.v[0] & 0xf0U) == 0xe0U);
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void F_IGMPCleanup(void)
{

    if(igmpSignalHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(igmpSignalHandle);
        igmpSignalHandle = NULL;
    }

    // Remove IGMP register users
#if defined(TCPIP_IGMP_USER_NOTIFICATION ) && (TCPIP_IGMP_USER_NOTIFICATION != 0)
    TCPIP_Notification_Deinitialize(&igmpRegisteredUsers, igmpMemH);
    igmpMemH = 0;
#endif  // defined(TCPIP_IGMP_USER_NOTIFICATION ) && (TCPIP_IGMP_USER_NOTIFICATION != 0)

    (void)OSAL_MUTEX_Delete(&igmpListsMutex);
    (void)OSAL_MUTEX_Delete(&igmpDcptMutex);
}

void TCPIP_IGMP_Deinitialize(const TCPIP_STACK_MODULE_CTRL *const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    if(igmpInitCount > 0)
    {   // we're up and running
        // one way or another this interface is going down

        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--igmpInitCount == 0)
            {   // all closed
                // release resources
                F_IGMPCleanup();
            }
        }
    }

}
#else
#define F_IGMPCleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)


bool TCPIP_IGMP_Initialize(const TCPIP_STACK_MODULE_CTRL *const stackCtrl, const void* initData)
{
    TCPIP_IGMP_GROUP_ENTRY* pGEntry;
    OA_HASH_DCPT    *gHashDcpt, *sHashDcpt;
    TCPIP_IGMP_SC_REPORT_NODE* pRepNode;
    TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGQNode;
    TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pQNode;
    size_t groupIx, ix;

    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack init
    if(igmpInitCount == 0)
    {   // first time we're run
        // hush compiler
        TCPIPStack_Condition(true, __FILE__, __func__, __LINE__);

        // check configuration data is not missing
        if(initData == NULL)
        {
            return false;
        }
        const TCPIP_IGMP_MODULE_CONFIG* pIgmpCfg = (const TCPIP_IGMP_MODULE_CONFIG*)initData;

        igmpInterfaces = pIgmpCfg->nInterfaces;
        if(igmpInterfaces > stackCtrl->nIfs)
        {
            igmpInterfaces = stackCtrl->nIfs;
        }

        if(igmpInterfaces > (size_t)TCPIP_IGMP_INTERFACES)
        {
            return false;
        }

        // create synchronization object
        if(OSAL_MUTEX_Create(&igmpListsMutex) != OSAL_RESULT_SUCCESS)
        {
            return false;
        }
        if(OSAL_MUTEX_Create(&igmpDcptMutex) != OSAL_RESULT_SUCCESS)
        {
            (void)OSAL_MUTEX_Delete(&igmpListsMutex);
            return false;
        }

#if defined(TCPIP_IGMP_USER_NOTIFICATION ) && (TCPIP_IGMP_USER_NOTIFICATION != 0)
        igmpMemH = stackCtrl->memH;
        if(TCPIP_Notification_Initialize(&igmpRegisteredUsers) == false)
        {
            F_IGMPCleanup();
            return false;
        }
#endif  // defined(TCPIP_IGMP_USER_NOTIFICATION ) && (TCPIP_IGMP_USER_NOTIFICATION != 0)

        // create the IGMP signal + timer
        igmpSignalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_IGMP_Task, TCPIP_IGMP_TASK_TICK_RATE);
        if(igmpSignalHandle == NULL )
        {   // cannot create the IGMP signal notification
            F_IGMPCleanup();
            return false;
        }

        // non critical initialization
        // initialize the SSM descriptor
        // check the SSM address range
        IPV4_ADDR ssmLow, ssmHigh;
        bool ssmRangeFail = false;
        if(pIgmpCfg->lowSsmAddress == NULL)
        {
            ssmLow.Val = TCPIP_IGMP_IANA_SSM_LOW;
        }
        else
        {
            if(!TCPIP_Helper_StringToIPAddress(pIgmpCfg->lowSsmAddress, &ssmLow) || !F_IGMP_IsMcastAddress(ssmLow))
            {   // wrong SSM range
                ssmRangeFail = true;
            }
        }
        if(pIgmpCfg->highSsmAddress == NULL)
        {
            ssmHigh.Val = TCPIP_IGMP_IANA_SSM_HIGH;
        }
        else
        {
            if(!TCPIP_Helper_StringToIPAddress(pIgmpCfg->highSsmAddress, &ssmHigh) || !F_IGMP_IsMcastAddress(ssmHigh))
            {   // wrong SSM range
                ssmRangeFail = true;
            }
        }

        if(ssmRangeFail)
        {   // wrong SSM range
            F_IGMPCleanup();
            return false;
        }

        igmpSsmAddLow = ssmLow.Val;
        igmpSsmAddHigh = ssmHigh.Val;
        (void)memset(&igmpGroupsDcpt, 0, sizeof(igmpGroupsDcpt));

        // populate the Group hash entries
        gHashDcpt = &igmpGroupsDcpt.gHashDcpt;
        gHashDcpt->memBlk = igmpGroupsDcpt.gEntryTbl;
        gHashDcpt->hEntrySize = sizeof(igmpGroupsDcpt.gEntryTbl[0]);
        gHashDcpt->hEntries = sizeof(igmpGroupsDcpt.gEntryTbl) / sizeof(*igmpGroupsDcpt.gEntryTbl);
        gHashDcpt->probeStep = TCPIP_IGMP_GROUP_HASH_PROBE_STEP;

        gHashDcpt->hashF = &F_IGMP_GroupKeyHash;
        gHashDcpt->cmpF = &F_IGMP_GroupKeyCompare;
        gHashDcpt->cpyF = &F_IGMP_GroupKeyCopy;
#if defined(OA_DOUBLE_HASH_PROBING)
        gHashDcpt->probeHash = &F_IGMP_GroupProbeHash;
#endif  // defined(OA_DOUBLE_HASH_PROBING)
        TCPIP_OAHASH_Initialize(gHashDcpt);

        // populate the Source hash entries
        sHashDcpt = &igmpGroupsDcpt.srcHashDcpt;
        sHashDcpt->hEntrySize = sizeof(pGEntry->srcEntryTbl[0]);
        sHashDcpt->hEntries = sizeof(pGEntry->srcEntryTbl) / sizeof(*pGEntry->srcEntryTbl);
        sHashDcpt->probeStep = TCPIP_IGMP_SOURCE_HASH_PROBE_STEP;

        sHashDcpt->hashF = &F_IGMP_SourceKeyHash;
        sHashDcpt->cmpF = &F_IGMP_SourceKeyCompare;
        sHashDcpt->cpyF = &F_IGMP_SourceKeyCopy;
#if defined(OA_DOUBLE_HASH_PROBING)
        sHashDcpt->probeHash = &F_IGMP_SourceProbeHash;
#endif  // defined(OA_DOUBLE_HASH_PROBING)


        // initialize every group hash
        pGEntry = igmpGroupsDcpt.gEntryTbl; 
        for(groupIx = 0; groupIx < sizeof(igmpGroupsDcpt.gEntryTbl) / sizeof(*igmpGroupsDcpt.gEntryTbl); groupIx++)
        {
            sHashDcpt->memBlk = pGEntry->srcEntryTbl;
            TCPIP_OAHASH_Initialize(sHashDcpt);
            pGEntry++;
        }

        // initialize the report pool
        TCPIP_Helper_SingleListInitialize(&igmpScFreeList);
        TCPIP_Helper_SingleListInitialize(&igmpScReportList);
        TCPIP_Helper_SingleListInitialize(&igmpGenQueryFreeList);
        TCPIP_Helper_SingleListInitialize(&igmpGroupQueryFreeList);
        TCPIP_Helper_SingleListInitialize(&igmpGenQueryReportList);
        TCPIP_Helper_SingleListInitialize(&igmpGroupQueryReportList);
        pRepNode = igmpScReportPool;
        for(ix = 0; ix < sizeof(igmpScReportPool) / sizeof(*igmpScReportPool); ix++)
        {
            TCPIP_Helper_SingleListTailAdd(&igmpScFreeList, FC_RepNode2SglNode(pRepNode));
            pRepNode++;
        }
        
        pGQNode = igmpGenQueryReportPool;
        for(ix = 0; ix < sizeof(igmpGenQueryReportPool) / sizeof(*igmpGenQueryReportPool); ix++)
        {
            pGQNode->nQSources = sizeof(pGQNode->qSources) / sizeof(*pGQNode->qSources);
            TCPIP_Helper_SingleListTailAdd(&igmpGenQueryFreeList, FC_GqNode2SglNode(pGQNode));
            pGQNode++;
        }

        pQNode = igmpGroupQueryReportPool;
        for(ix = 0; ix < sizeof(igmpGroupQueryReportPool) / sizeof(*igmpGroupQueryReportPool); ix++)
        {
            pQNode->nQSources = sizeof(pQNode->qSources) / sizeof(*pQNode->qSources);
            TCPIP_Helper_SingleListTailAdd(&igmpGroupQueryFreeList, FC_GroupNode2SglNode(pQNode));
            pQNode++;
        }

        igmpUnsolicitRepInterval = pIgmpCfg->reportInterval;
    }
            
    // Reset state machine and flags to default values

    igmpInitCount++;

    return true;
}


void  TCPIP_IGMP_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, (TCPIP_MODULE_SIGNAL)TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_RX_PENDING) != 0U)
    { //  RX signal occurred
        TCPIP_IGMP_Process();
    }

    if(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_TMO) != 0U)
    { // regular TMO occurred
        TCPIP_IGMP_Tick();
    }

}

static void TCPIP_IGMP_Process(void)
{
    TCPIP_MAC_PACKET*   pRxPkt;
    const IPV4_ADDR*    rxDestAdd;
    TCPIP_IGMP_HEADER*  pIgmpHdr;
    uint16_t            igmpTotLength;
    uint16_t            checksum;
    size_t ifIx;
    TCPIP_MAC_PKT_ACK_RES ackRes;

    // extract queued IGMP packets
    while((pRxPkt = TCPIPStackModuleRxExtract(TCPIP_THIS_MODULE_ID)) != NULL)
    {
        TCPIP_PKT_FlightLogRx(pRxPkt, TCPIP_THIS_MODULE_ID);
        while(true)
        {
            int iIx = TCPIP_STACK_NetIndexGet(pRxPkt->pktIf);
            TCPIPStack_Assert(iIx >= 0, __FILE__,  __func__, __LINE__);
            ifIx = (size_t)iIx;
            if(!F_IGMP_CheckIfIndex(ifIx, true))
            {   // not listening on this interface
                ackRes = TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;
                break;
            }

            rxDestAdd = TCPIP_IPV4_PacketGetDestAddress(pRxPkt);
            if(rxDestAdd->Val != TCPIP_IGMP_IANA_ALL_SYSTEMS_MULTICAST)
            {   // ignore if not to 224.0.0.1!
                ackRes = TCPIP_MAC_PKT_ACK_PROTO_DEST_ERR;
                break;
            }

            if(TCPIP_IPV4_PacketOptionGet(pRxPkt, TCPIP_IPV4_OPTION_ROUTER_ALERT, NULL, 0) == 0U)
            {   // ignore if router alert not set
                ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
                break;
            }

            igmpTotLength = pRxPkt->totTransportLen;    // length of the 1st segment (if fragmented)

            pIgmpHdr = FC_U8ptr2IgmpHdr(pRxPkt->pTransportLayer);
            if(igmpTotLength < sizeof(*pIgmpHdr))
            {
                ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
                break;
            }

            // Validate the checksum
            // The checksum data includes the precomputed checksum in the header
            // so a valid packet will always have a checksum of 0x0000
            checksum = TCPIP_Helper_PacketChecksum(pRxPkt, (uint8_t*)pIgmpHdr, igmpTotLength, 0);
            if(checksum != 0U)
            {
                ackRes = TCPIP_MAC_PKT_ACK_CHKSUM_ERR;
                break;
            }


            // do some processing here based on type of the IGMP packet
            if(pIgmpHdr->type != (uint8_t)TCPIP_IGMP_MESSAGE_QUERY)
            {
                ackRes = TCPIP_MAC_PKT_ACK_TYPE_ERR;
                break;
            }

            // get the query version
            // right now just v3 queries are processed
            if(igmpTotLength < sizeof(TCPIP_IGMPv3_QUERY_MESSAGE_BARE))
            {   // discard unknown IGMP version
                ackRes = TCPIP_MAC_PKT_ACK_TYPE_ERR;
                break;
            }

            F_IGMP_ProcessV3Query(FC_IgmpHdr2QueryMsg(pIgmpHdr), ifIx);
            ackRes = TCPIP_MAC_PKT_ACK_RX_OK;
            break;
        }

        // acknowledge the packet and be done with it
        TCPIP_PKT_PacketAcknowledge(pRxPkt, ackRes); 
    }
}


// Performs periodic IGMP processing
static void TCPIP_IGMP_Tick(void)
{
    TCPIP_IGMP_SourceChangeTimeout();
    TCPIP_IGMP_GenQueryTimeout();
    TCPIP_IGMP_GroupQueryTimeout();
}


// process timeout for status change list
static void TCPIP_IGMP_SourceChangeTimeout(void)
{
    size_t ifIx;
    size_t nGroups;      // global count of groups pending report
    SINGLE_LIST toReportList, newScList, pendingList, filtersDoneList, releaseList;
    TCPIP_IGMP_SC_REPORT_NODE *pRNode, *pFilt, *pAllow, *pBlock;
    IPV4_ADDR groupAddress;
    bool sendRes;
    uint32_t currTick;

    // do the SC list timeout processing
    for(ifIx = 0; ifIx < igmpInterfaces; ifIx++)
    {   
        if((igmpGroupsDcpt.ifMask & (1UL << ifIx)) == 0U)
        {   // not active
            continue;
        }

        // active interface
        TCPIP_Helper_SingleListInitialize(&toReportList);
        TCPIP_Helper_SingleListInitialize(&newScList);
        TCPIP_Helper_SingleListInitialize(&pendingList);
        TCPIP_Helper_SingleListInitialize(&filtersDoneList);
        TCPIP_Helper_SingleListInitialize(&releaseList);
        nGroups = 0U;

        currTick = SYS_TMR_TickCountGet();

        // check if there are reports to be sent
        igmpListsLock();
        while((pRNode = FC_SglNode2RepNode(TCPIP_Helper_SingleListHeadRemove(&igmpScReportList))) != NULL)
        {
            if(pRNode->ifIx == ifIx && pRNode->active != 0U && ((int32_t)currTick - (int32_t)pRNode->tTransmit) >= 0)
            {   // found timeout node
                TCPIP_Helper_SingleListTailAdd(&toReportList, FC_RepNode2SglNode(pRNode));
                nGroups++;
            }
            else
            {   // not this interface or not active yet
                TCPIP_Helper_SingleListTailAdd(&newScList, FC_RepNode2SglNode(pRNode));
            }
        }
        igmpScReportList = newScList;
        igmpListsUnlock();

        if(nGroups != 0U)
        {
            sendRes = F_IGMP_SendScReport(ifIx, &toReportList);

            if(sendRes == false)
            {   // failed to transmit; reappend to pending
                TCPIP_Helper_SingleListAppend(&pendingList, &toReportList);
            }
            else
            {   // report sent fine; process again the toReportList to check for retransmissions
                while((pRNode = FC_SglNode2RepNode(TCPIP_Helper_SingleListHeadRemove(&toReportList))) != NULL)
                {   
                    if(--pRNode->nTransmit == 0U)
                    {    // done with this report; 
                        if(pRNode->repType == (uint8_t)TCPIP_IGMP_FILTER_TO_INCLUDE || pRNode->repType == (uint8_t)TCPIP_IGMP_FILTER_TO_EXCLUDE)
                        {   // when a filter is done, some SLC reports may be pending!
                            TCPIP_Helper_SingleListTailAdd(&filtersDoneList, FC_RepNode2SglNode(pRNode));
                        }
                        else
                        {   // simply release the report
                            TCPIP_Helper_SingleListTailAdd(&releaseList, FC_RepNode2SglNode(pRNode));
                        }
                    }
                    else
                    {    // still transmissions to go
                        F_IGMP_SetScReportTransmitTime(pRNode);
                        TCPIP_Helper_SingleListTailAdd(&pendingList, FC_RepNode2SglNode(pRNode));
                    }
                }
            }
        }
 
        // restore the SC busy list
        igmpListsLock();
        TCPIP_Helper_SingleListAppend(&igmpScReportList, &pendingList);

        // process the filters done list
        while((pRNode = FC_SglNode2RepNode(TCPIP_Helper_SingleListHeadRemove(&filtersDoneList))) != NULL)
        {
            // find a pending SLC report for this group
            groupAddress.Val = pRNode->repGroup;
            pFilt = F_IGMP_FindScheduledScReport(groupAddress, ifIx, &pAllow, &pBlock, false);
            (void)pFilt;
            TCPIPStack_Assert(pFilt == NULL, __FILE__, __func__, __LINE__);
            if(pAllow != NULL)
            {
                pAllow->active = 1;
            }
            if(pBlock != NULL)
            {
                pBlock->active = 1;
            }
            TCPIP_Helper_SingleListTailAdd(&releaseList, FC_RepNode2SglNode(pRNode));
        }

        // restore the SC free list
        TCPIP_Helper_SingleListAppend(&igmpScFreeList, &releaseList);
        igmpListsUnlock();

    }
}


// process timeout for Gen Query list
static void TCPIP_IGMP_GenQueryTimeout(void)
{
    TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGQNode;
    SINGLE_LIST newGQList, toTxList, releaseList, failSendList;
    size_t nGenEntries;
    uint32_t currTick;
    bool sendRes;


    TCPIP_Helper_SingleListInitialize(&newGQList);
    TCPIP_Helper_SingleListInitialize(&toTxList);
    TCPIP_Helper_SingleListInitialize(&releaseList);
    TCPIP_Helper_SingleListInitialize(&failSendList);
    currTick = SYS_TMR_TickCountGet();

    // quick list traverse: avoid keeping the lock for too long
    igmpListsLock();
    while((pGQNode = FC_SglNode2GqNode(TCPIP_Helper_SingleListHeadRemove(&igmpGenQueryReportList))) != NULL)
    {
        TCPIPStack_Assert(pGQNode->queryType == (uint8_t)TCPIP_IGMP_QUERY_GENERAL, __FILE__, __func__, __LINE__);
        if(((int32_t)currTick - (int32_t)pGQNode->tTransmit) >= 0)
        {   // need to transmit
            TCPIP_Helper_SingleListTailAdd(&toTxList, FC_GqNode2SglNode(pGQNode));
        }
        else
        {
            TCPIP_Helper_SingleListTailAdd(&newGQList, FC_GqNode2SglNode(pGQNode));
        }
    }
    igmpGenQueryReportList = newGQList;
    igmpListsUnlock();

    // process the transmit list
    while((pGQNode = FC_SglNode2GqNode(TCPIP_Helper_SingleListHeadRemove(&toTxList))) != NULL)
    {
        nGenEntries = F_IGMP_GenerateGenQueryReport(pGQNode);
        if(nGenEntries != 0U)
        {   // OK, we've got something to transmit
            sendRes = F_IGMP_SendQueryReport(FC_GenQRep2QRep(pGQNode), nGenEntries);
        }
        else
        {
            sendRes = true;
        }

        if(sendRes == true)
        {   // transmitted OK
            TCPIP_Helper_SingleListTailAdd(&releaseList, FC_GqNode2SglNode(pGQNode));
        }
        else
        {
            TCPIP_Helper_SingleListTailAdd(&failSendList, FC_GqNode2SglNode(pGQNode));
        }
    }

    // finally process the remaining lists
    igmpListsLock();
    // failed list will be rescheduled
    TCPIP_Helper_SingleListAppend(&igmpGenQueryReportList, &failSendList);
    
    // release the sent list
    TCPIP_Helper_SingleListAppend(&igmpGenQueryFreeList, &releaseList);
    igmpListsUnlock();

}


// the Group Query list timeout processing
static void TCPIP_IGMP_GroupQueryTimeout(void)
{
    TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pQNode;
    SINGLE_LIST newQueryList, toTxList, releaseList, failSendList;
    uint32_t currTick;
    bool sendRes;

    TCPIP_Helper_SingleListInitialize(&newQueryList);
    TCPIP_Helper_SingleListInitialize(&toTxList);
    TCPIP_Helper_SingleListInitialize(&releaseList);
    TCPIP_Helper_SingleListInitialize(&failSendList);

    currTick = SYS_TMR_TickCountGet();

    // quick list traverse: avoid keeping the lock for too long
    igmpListsLock();
    while((pQNode = FC_SglNode2GroupNode(TCPIP_Helper_SingleListHeadRemove(&igmpGroupQueryReportList))) != NULL)
    {
        TCPIPStack_Assert(pQNode->queryType == (uint8_t)TCPIP_IGMP_QUERY_GROUP_SPEC || pQNode->queryType == (uint8_t)TCPIP_IGMP_QUERY_GROUP_SOURCE_SPEC, __FILE__, __func__, __LINE__);

        if(((int32_t)currTick - (int32_t)pQNode->tTransmit) >= 0)
        {   // need to transmit this node
            TCPIP_Helper_SingleListTailAdd(&toTxList, FC_GroupNode2SglNode(pQNode));
        }
        else
        {   // not ready yet
            TCPIP_Helper_SingleListTailAdd(&newQueryList, FC_GroupNode2SglNode(pQNode));
        }
    }
    igmpGroupQueryReportList = newQueryList;
    igmpListsUnlock();

    // process the transmit list

    while((pQNode = FC_SglNode2GroupNode(TCPIP_Helper_SingleListHeadRemove(&toTxList))) != NULL)
    {
        if(F_IGMP_GenerateGroupQueryReport(pQNode))
        {   // OK, report generated; we've got something to transmit
            sendRes = F_IGMP_SendQueryReport(FC_GrRep2QRep(pQNode), 1);
        }
        else
        {   // nothing to transmit
            sendRes = true;
        }

        if(sendRes == true)
        {   // transmitted OK
            TCPIP_Helper_SingleListTailAdd(&releaseList, FC_GroupNode2SglNode(pQNode));
        }
        else
        {
            TCPIP_Helper_SingleListTailAdd(&failSendList, FC_GroupNode2SglNode(pQNode));
        }
    }


    // finally process the remaining lists
    igmpListsLock();
    // failed list will be rescheduled
    TCPIP_Helper_SingleListAppend(&igmpGroupQueryReportList, &failSendList);
    
    // release the sent list
    TCPIP_Helper_SingleListAppend(&igmpGroupQueryFreeList, &releaseList);
    igmpListsUnlock();
}

// sends a SC report over the interface
static bool F_IGMP_SendScReport(size_t ifIx, SINGLE_LIST* pReportList)
{
    uint16_t pktSize, groupSize, sourcesBytes, linkMtu, pktOvrhead;
    TCPIP_IGMPv3_GROUP_RECORD* pGroupRec;
    TCPIP_IGMPv3_REPORT_MESSAGE* pIgmpReport;
    TCPIP_IGMP_SC_REPORT_NODE *pRNode;
    IPV4_ADDR evGroup;
    const TCPIP_NET_IF*    pNetIf;
    TCPIP_IPV4_PACKET_PARAMS pktParams;
    size_t nGroups, nSources, grpIx;

    uint16_t evType = (uint16_t)TCPIP_IGMP_EVENT_NONE;
    bool res = false;
    IPV4_PACKET* pTxPkt = NULL;
    evGroup.Val = 0;

    pNetIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(ifIx);
    linkMtu = TCPIPStackNetLinkMtu(pNetIf);
    pktOvrhead = sizeof(TCPIP_IGMP_HEADER) + sizeof(IPV4_HEADER);

    // calculate the packet size
    pktSize = (uint16_t)sizeof(TCPIP_IGMPv3_REPORT_MESSAGE_BARE);
    nGroups = nSources = 0U;
    for(pRNode = FC_SglNode2RepNode(pReportList->head); pRNode != NULL; pRNode = pRNode->next)
    {
        groupSize = (uint16_t)sizeof(TCPIP_IGMPv3_GROUP_RECORD_BARE) + (uint16_t)pRNode->repSources.nSources * (uint16_t)sizeof(*pGroupRec->sourceAddress);
        if(pktOvrhead + pktSize + groupSize > linkMtu)
        {   // exceeded MTU; truncate
            TCPIPStack_Condition(true, __FILE__, __func__, __LINE__);
            evType |= (uint16_t)TCPIP_IGMP_EVENT_PACKET_EXCEED_MTU;
            break;
        }

        nGroups++;
        nSources += pRNode->repSources.nSources;
        pktSize += groupSize;
    }
    

    while(nGroups != 0U)
    {
        pTxPkt = F_IGMP_AllocateTxPacketStruct(pktSize);
        if(pTxPkt == NULL)
        {   // failed to allocate
            evType |= (uint16_t)TCPIP_IGMP_EVENT_PACKET_ALLOC_ERROR;
            break;
        }

        pTxPkt->netIfH = pNetIf;
        pTxPkt->srcAddress.Val = TCPIP_STACK_NetAddress(pTxPkt->netIfH);
        pTxPkt->destAddress.Val = TCPIP_IGMP_IANA_ALL_ROUTERS_MULTICAST;
        pTxPkt->macPkt.pDSeg->segLen += pktSize;
        pktParams.tosFlags = TCPIP_IPV4_PRECEDENCE_INET_CONTROL;
        pktParams.ttl = TCPIP_IGMP_PACKET_TTL;
        pktParams.df = 0;

        TCPIP_IPV4_PacketFormatTx(pTxPkt, (uint8_t)IP_PROT_IGMP, pktSize, &pktParams);

        pIgmpReport = FC_U8ptr2RepMsg(pTxPkt->macPkt.pTransportLayer);
        (void)memset(pIgmpReport, 0, sizeof(TCPIP_IGMPv3_REPORT_MESSAGE_BARE));
        pIgmpReport->type = (uint8_t)TCPIP_IGMP_MESSAGE_V3_MEMBERSHIP; 
        pIgmpReport->checksum = 0;
        pIgmpReport->nGroupRecords = TCPIP_Helper_htons((uint16_t)nGroups);

        pGroupRec = pIgmpReport->groupRecords;
        pRNode = FC_SglNode2RepNode(pReportList->head);
        for(grpIx = 0; grpIx < nGroups; grpIx++)
        {
            (void)memset(pGroupRec, 0, sizeof(TCPIP_IGMPv3_GROUP_RECORD_BARE));
            pGroupRec->recordType = pRNode->repType;
            pGroupRec->nSources = TCPIP_Helper_htons((uint16_t)pRNode->repSources.nSources);
            pGroupRec->groupAddress = pRNode->repGroup;
            // set source addresses
            sourcesBytes = (uint16_t)pRNode->repSources.nSources * (uint16_t)sizeof(*pGroupRec->sourceAddress);
            if(sourcesBytes != 0U)
            {
                (void)memcpy(pGroupRec->sourceAddress, pRNode->repSources.sourceAddresses, sourcesBytes);
            }
            pGroupRec = FC_U8ptr2GrpRec((uint8_t*)pGroupRec + sizeof(TCPIP_IGMPv3_GROUP_RECORD_BARE) + sourcesBytes);
            pRNode = pRNode->next;
        }

        // calculate checksum
        pIgmpReport->checksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pIgmpReport, pktSize, 0);

        TCPIP_PKT_FlightLogTx(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID);
        if(!TCPIP_IPV4_PacketTransmit(pTxPkt))
        {
            evType |= (uint16_t)TCPIP_IGMP_EVENT_PACKET_TRANSMIT_ERROR;
            break;
        }

        res = true;
        break;
    }

    if(res == false)
    {   // failed somehow
        if(pTxPkt != NULL)
        {
            TCPIP_PKT_FlightLogAcknowledge(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_IP_REJECT_ERR);
            TCPIP_PKT_PacketFree(&pTxPkt->macPkt);
        }
    }

    if(evType != (uint16_t)TCPIP_IGMP_EVENT_NONE)
    {   // select head as representative
        evGroup.Val = (FC_SglNode2RepNode(pReportList->head))->repGroup;
        F_IGMP_ReportEvent(evGroup, (TCPIP_IGMP_EVENT_TYPE)evType);
    }

    return res;
}

// sends a General Query report over the interface
static bool F_IGMP_SendQueryReport(TCPIP_IGMP_QUERY_REPORT_NODE* pQNode, size_t nEntries)
{

    size_t qIx;
    size_t nSources, nRecords;
    TCPIP_IGMP_QUERY_SOURCES* pQuery;
    uint16_t pktSize, groupSize, sourcesBytes, linkMtu, pktOvrhead;
    TCPIP_IGMPv3_REPORT_MESSAGE* pIgmpReport;
    TCPIP_IGMPv3_GROUP_RECORD* pGroupRec;
    IPV4_ADDR evGroup;
    const TCPIP_NET_IF*    pNetIf;
    TCPIP_IPV4_PACKET_PARAMS pktParams;

    uint16_t evType = (uint16_t)TCPIP_IGMP_EVENT_NONE;
    bool res = false;
    IPV4_PACKET* pTxPkt = NULL;
    evGroup.Val = 0;

    if(pQNode->queryType == (uint8_t)TCPIP_IGMP_QUERY_GENERAL)
    {
        TCPIPStack_Assert(pQNode->nQSources == sizeof(((TCPIP_IGMP_GEN_QUERY_REPORT_NODE*)0)->qSources) / sizeof(*((TCPIP_IGMP_GEN_QUERY_REPORT_NODE*)0)->qSources), __FILE__, __func__, __LINE__);
    }
    else
    {
        TCPIPStack_Assert(pQNode->nQSources == sizeof(((TCPIP_IGMP_GROUP_QUERY_REPORT_NODE*)0)->qSources) / sizeof(*((TCPIP_IGMP_GROUP_QUERY_REPORT_NODE*)0)->qSources), __FILE__, __func__, __LINE__);
    }

    pNetIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet((size_t)pQNode->ifIx);
    linkMtu = TCPIPStackNetLinkMtu(pNetIf);
    pktOvrhead = sizeof(TCPIP_IGMP_HEADER) + sizeof(IPV4_HEADER);

    // calculate the packet size
    pktSize = (uint16_t)sizeof(TCPIP_IGMPv3_REPORT_MESSAGE_BARE);
    nRecords = 0U;
    pQuery = pQNode->qSources;
    for(qIx = 0; qIx < pQNode->nQSources; qIx++)
    {
        if(pQuery->repType != TCPIP_IGMP_RECORD_NONE) 
        {   // valid query source
            groupSize = (uint16_t)(sizeof(TCPIP_IGMPv3_GROUP_RECORD_BARE) + pQuery->repSources.nSources * sizeof(pQuery->repSources.sourceAddresses[0]));
            if(pktOvrhead + pktSize + groupSize > linkMtu)
            {   // exceeded MTU; truncate;
                TCPIPStack_Condition(true, __FILE__, __func__, __LINE__);
                evType |= (uint16_t)TCPIP_IGMP_EVENT_PACKET_EXCEED_MTU;
                break;
            }

            pktSize += groupSize;
            nRecords++;
        }
        pQuery++;
    }

    while(nRecords != 0U)
    {
        pTxPkt = F_IGMP_AllocateTxPacketStruct(pktSize);
        if(pTxPkt == NULL)
        {   // failed to allocate
            evType |= (uint16_t)TCPIP_IGMP_EVENT_PACKET_ALLOC_ERROR;
            break;
        }

        pTxPkt->netIfH = pNetIf;
        pTxPkt->srcAddress.Val = TCPIP_STACK_NetAddress(pTxPkt->netIfH);
        pTxPkt->destAddress.Val = TCPIP_IGMP_IANA_ALL_ROUTERS_MULTICAST;
        pTxPkt->macPkt.pDSeg->segLen += pktSize;
        pktParams.tosFlags = TCPIP_IPV4_PRECEDENCE_INET_CONTROL;
        pktParams.ttl = TCPIP_IGMP_PACKET_TTL;
        pktParams.df = 0;

        TCPIP_IPV4_PacketFormatTx(pTxPkt, (uint8_t)IP_PROT_IGMP, pktSize, &pktParams);

        pIgmpReport = FC_U8ptr2RepMsg(pTxPkt->macPkt.pTransportLayer);
        (void)memset(pIgmpReport, 0, sizeof(TCPIP_IGMPv3_REPORT_MESSAGE_BARE));
        pIgmpReport->type = (uint8_t)TCPIP_IGMP_MESSAGE_V3_MEMBERSHIP; 
        pIgmpReport->checksum = 0;
        pIgmpReport->nGroupRecords = TCPIP_Helper_htons((uint16_t)nRecords);

        pGroupRec = pIgmpReport->groupRecords;
        pQuery = pQNode->qSources;
        for(qIx = 0; qIx < pQNode->nQSources; qIx++)
        {
            if(pQuery->repType != TCPIP_IGMP_RECORD_NONE) 
            {   // valid query source
                nSources = pQuery->repSources.nSources;
                (void)memset(pGroupRec, 0, sizeof(TCPIP_IGMPv3_GROUP_RECORD_BARE));
                pGroupRec->recordType = (uint8_t)pQuery->repType;
                pGroupRec->nSources = TCPIP_Helper_htons((uint16_t)nSources);
                pGroupRec->groupAddress = pQuery->repGroup;

                // set source addresses
                sourcesBytes = (uint16_t)(nSources * sizeof(*pGroupRec->sourceAddress));
                if(sourcesBytes != 0U)
                {
                    (void)memcpy(pGroupRec->sourceAddress, pQuery->repSources.sourceAddresses, sourcesBytes);
                }
                pGroupRec = FC_U8ptr2GrpRec((uint8_t*)pGroupRec + sizeof(TCPIP_IGMPv3_GROUP_RECORD_BARE) + sourcesBytes);
            }
            pQuery++;
        }

        // calculate checksum
        pIgmpReport->checksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pIgmpReport, pktSize, 0);

        TCPIP_PKT_FlightLogTx(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID);
        if(!TCPIP_IPV4_PacketTransmit(pTxPkt))
        {
            evType |= (uint16_t)TCPIP_IGMP_EVENT_PACKET_TRANSMIT_ERROR;
            break;
        }

        res = true;
        break;
    }

    if(res == false)
    {   // failed somehow
        if(pTxPkt != NULL)
        {
            TCPIP_PKT_FlightLogAcknowledge(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_IP_REJECT_ERR);
            TCPIP_PKT_PacketFree(&pTxPkt->macPkt);
        }
    }

    if(evType != (uint16_t)TCPIP_IGMP_EVENT_NONE)
    {   // select 1st qSource as representative
        evGroup.Val = pQNode->qSources->repGroup;
        F_IGMP_ReportEvent(evGroup, (TCPIP_IGMP_EVENT_TYPE)evType);
    }

    return res;

}

/////////////////////////// API //////////////////////////////////////////
//

// all API operations need to access a lock/mutex!
// since they mess with the global hash descriptor and some user threads add, other delete from this global hash!!!!
TCPIP_IGMP_RESULT TCPIP_IGMP_Subscribe(UDP_SOCKET uSkt, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress,
                      TCPIP_IGMP_FILTER_TYPE filterMode, const IPV4_ADDR* sourceList, size_t* listSize)
{
    size_t  ifIx;
    bool isSsm;
    IPV4_ADDR   allSourcesAdd;
    const IPV4_ADDR* pSources;
    TCPIP_IGMP_RESULT checkRes;
    size_t nSources;
    TCPIP_IGMP_RESULT res;

    // minimal sanity check
    checkRes = F_IGMP_CheckSubscribeParams(hNet, mcastAddress, sourceList, listSize, &ifIx);
    if(checkRes != TCPIP_IGMP_OK)
    {
        return checkRes;
    }

    if(filterMode != TCPIP_IGMP_FILTER_INCLUDE && filterMode != TCPIP_IGMP_FILTER_EXCLUDE)
    {
        return TCPIP_IGMP_FILTER_ERROR;
    }

    isSsm = F_IGMP_IsSsmAddress(mcastAddress);

    if(isSsm && filterMode == TCPIP_IGMP_FILTER_EXCLUDE)
    {   // exclude is invalid for SSM
        return TCPIP_IGMP_SSM_FILTER_ERROR;
    }

    igmpDcptLock();
    while(true)
    {
        if(!F_IGMP_CheckIfIndex(ifIx, false))
        {
            res = TCPIP_IGMP_IF_ERROR;
            break;
        }

        nSources = listSize != NULL ? *listSize : 0U;
        if(nSources == 0U)
        {
            if(filterMode == TCPIP_IGMP_FILTER_INCLUDE)
            {   // include nothing means remove socket from (G, if)
                res = F_IGMP_SocketRemove(uSkt, ifIx, mcastAddress, listSize); 
                break;
            }

            // exclude nothing i.e. include all is valid for ASM; use the special source address
            nSources = 1U;
            allSourcesAdd.Val = TCPIP_IGMP_ASM_ALL_SOURCES; 
            pSources = &allSourcesAdd; 
        }
        else
        {
            pSources = sourceList;
        }

        // non empty list
        res = F_IGMP_SocketUpdateSources(uSkt, ifIx, mcastAddress, filterMode, pSources, &nSources);
        break;
    }
    igmpDcptUnlock();

    return res;
}

TCPIP_IGMP_RESULT TCPIP_IGMP_SubscribeGet(UDP_SOCKET uSkt, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress,
                      TCPIP_IGMP_FILTER_TYPE* filterMode, IPV4_ADDR* sourceList, size_t* listSize)
{
    size_t ifIx;
    TCPIP_IGMP_RESULT res;

    // minimal sanity check
    if(listSize == NULL)
    {
        return TCPIP_IGMP_ARG_ERROR;
    }

    res = F_IGMP_CheckSubscribeParams(hNet, mcastAddress, sourceList, listSize, &ifIx);
    // save the list size
    size_t nSourceSlots = *listSize;
    *listSize = 0;

    if(res != TCPIP_IGMP_OK)
    {
        return res;
    }

    igmpDcptLock();
    while(true)
    {
        if(!F_IGMP_CheckIfIndex(ifIx, true))
        {
            res = TCPIP_IGMP_IF_ERROR;
            break;
        }

        TCPIP_IGMP_GROUP_ENTRY* pGEntry;
        pGEntry = FC_HashE2GrpE(TCPIP_OAHASH_EntryLookup(&igmpGroupsDcpt.gHashDcpt, &mcastAddress));

        if(pGEntry == NULL)
        {    // no such mcast exists
            res = TCPIP_IGMP_GROUP_INVALID;
            break;
        }

        OA_HASH_DCPT *sHashDcpt = F_IGMP_GetSourceHashDcpt(pGEntry);
        size_t srcIx;
        TCPIP_IGMP_SOURCE_ENTRY* pSEntry;
        TCPIP_IGMP_SKT_RECORD* pRec;
        uint8_t sktFilter = 0U;
        size_t nSources = 0U;

        for(srcIx = 0U; srcIx < sHashDcpt->hEntries; srcIx++)
        {
            pSEntry = FC_HashE2SrcE(TCPIP_OAHASH_EntryGet(sHashDcpt, srcIx));
            if(pSEntry->hEntry.flags.busy != 0U)
            {   // found a valid source entry
                pRec = F_IGMP_SourceFindSktRecord(pSEntry, uSkt, ifIx, TCPIP_IGMP_FILTER_NONE);
                if(pRec != NULL)
                {
                    if(sktFilter == 0U)
                    {
                        sktFilter = pRec->filter;
                    }
                    else
                    {   // there is only one filter mode per socket for one G!
                        TCPIPStack_Assert(sktFilter == pRec->filter, __FILE__, __func__, __LINE__);
                    }
                    nSources++;
                    if(nSourceSlots != 0U)
                    {
                        *sourceList++ = pSEntry->srcAddress;
                        nSourceSlots--;
                    }
                }
            }
        }

        // report the number of available sources
        *listSize = nSources;

        if(filterMode != NULL)
        {
            *filterMode = (TCPIP_IGMP_FILTER_TYPE)sktFilter;
        }

        res = sktFilter == (uint8_t)TCPIP_IGMP_FILTER_NONE ? TCPIP_IGMP_SOCKET_INVALID : TCPIP_IGMP_OK;
        break;
    }
    igmpDcptUnlock();

    return res;
}

TCPIP_IGMP_RESULT TCPIP_IGMP_Unsubscribe(UDP_SOCKET uSkt, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, const IPV4_ADDR* sourceList, size_t* listSize)
{
    size_t ix, ifIx, removeCnt;
    TCPIP_IGMP_GIF_STATE_DCPT oldStateDcpt, newStateDcpt;
    TCPIP_IGMP_GROUP_ENTRY* pGEntry;
    TCPIP_IGMP_RESULT repRes;
    size_t nSources;
    
    // minimal sanity check
    repRes = F_IGMP_CheckSubscribeParams(hNet, mcastAddress, sourceList, listSize, &ifIx);

    if(repRes != TCPIP_IGMP_OK)
    {
        return repRes;
    }


    nSources = listSize != NULL ? *listSize : 0U;

    if(nSources == 0U)
    {   // unsubscribe ALL
        return TCPIP_IGMP_Subscribe(uSkt, hNet, mcastAddress, TCPIP_IGMP_FILTER_INCLUDE, NULL, NULL);
    }

    igmpDcptLock();
    while(true)
    {
        pGEntry = FC_HashE2GrpE(TCPIP_OAHASH_EntryLookup(&igmpGroupsDcpt.gHashDcpt, &mcastAddress));

        if(pGEntry == NULL)
        {    // no such mcast exists
            repRes = TCPIP_IGMP_GROUP_INVALID;
            break;
        }

        if(!F_IGMP_CheckIfIndex(ifIx, true))
        {
            repRes = TCPIP_IGMP_IF_ERROR;
            break;
        }

        // old state snapshot
        F_IGMP_GetGifState(pGEntry, ifIx, &oldStateDcpt);

        removeCnt = 0U;
        for(ix = 0U; ix < nSources; ix++)
        {
            if(F_IGMP_GroupPurgeSktBySource(pGEntry, uSkt, ifIx, sourceList))
            {
                removeCnt++;
            }
            sourceList++;
        }

        // get new state snapshot
        F_IGMP_GetGifState(pGEntry, ifIx, &newStateDcpt);


        // removed some stuff; check if this entry is free
        (void)F_IGMP_GroupEntryCheckRemove(pGEntry);

        // generate report
        repRes = F_IGMP_GenerateStateReport(mcastAddress, ifIx, &oldStateDcpt, &newStateDcpt);

        *listSize = removeCnt;
        break;
    }
    igmpDcptUnlock();

    return repRes; 
}

TCPIP_IGMP_RESULT TCPIP_IGMP_IncludeSource(UDP_SOCKET uSkt, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, IPV4_ADDR sourceAddress)
{
    size_t listSize = 1;
    return TCPIP_IGMP_Subscribe(uSkt, hNet, mcastAddress, TCPIP_IGMP_FILTER_INCLUDE, &sourceAddress, &listSize);
}

TCPIP_IGMP_RESULT TCPIP_IGMP_ExcludeSource(UDP_SOCKET uSkt, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, IPV4_ADDR sourceAddress)
{
    size_t listSize = 1;
    return TCPIP_IGMP_Subscribe(uSkt, hNet, mcastAddress, TCPIP_IGMP_FILTER_EXCLUDE, &sourceAddress, &listSize);
}

TCPIP_IGMP_RESULT TCPIP_IGMP_Join(UDP_SOCKET uSkt, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress)
{
    return TCPIP_IGMP_Subscribe(uSkt, hNet, mcastAddress, TCPIP_IGMP_FILTER_EXCLUDE, NULL, NULL);
}

TCPIP_IGMP_RESULT TCPIP_IGMP_Leave(UDP_SOCKET uSkt, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress)
{
    return TCPIP_IGMP_Subscribe(uSkt, hNet, mcastAddress, TCPIP_IGMP_FILTER_INCLUDE, NULL, NULL);
}

#if defined(TCPIP_IGMP_USER_NOTIFICATION ) && (TCPIP_IGMP_USER_NOTIFICATION != 0)
TCPIP_IGMP_HANDLE TCPIP_IGMP_HandlerRegister(IPV4_ADDR mcastAddress, TCPIP_IGMP_EVENT_HANDLER handler, const void* hParam)
{
    if(handler && igmpMemH)
    {
        TCPIP_IGMP_LIST_NODE igmpNode;
        igmpNode.handler = handler;
        igmpNode.hParam = hParam;
        igmpNode.mcastAddress = mcastAddress;

        return (TCPIP_IGMP_LIST_NODE*)TCPIP_Notification_Add(&igmpRegisteredUsers, igmpMemH, &igmpNode, sizeof(igmpNode));
    }
    return NULL;
}

// deregister the event handler
bool TCPIP_IGMP_HandlerDeRegister(TCPIP_IGMP_HANDLE hIgmp)
{
    if(igmpMemH && hIgmp)
    {
        if(TCPIP_Notification_Remove(FC_IgmpHndl2SglNode(hIgmp), &igmpRegisteredUsers, igmpMemH))
        {
            return true;
        }
    }
    return false;
}
#else
TCPIP_IGMP_HANDLE TCPIP_IGMP_HandlerRegister(IPV4_ADDR mcastAddress, TCPIP_IGMP_EVENT_HANDLER handler, const void* hParam)
{
    return NULL;
}
bool TCPIP_IGMP_HandlerDeRegister(TCPIP_IGMP_HANDLE hIgmp)
{
    return false;
}
#endif  // defined(TCPIP_IGMP_USER_NOTIFICATION ) && (TCPIP_IGMP_USER_NOTIFICATION != 0)


TCPIP_IGMP_RESULT TCPIP_IGMP_GroupsGet(IPV4_ADDR* groupsList, size_t listSize, size_t* pnGroups)
{
    OA_HASH_DCPT *gHashDcpt;
    TCPIP_IGMP_GROUP_ENTRY* pGEntry;
    size_t groupIx, nEntries;


    if(listSize != 0U && groupsList == NULL)
    {
        return TCPIP_IGMP_ARG_ERROR;
    }

    igmpDcptLock();
    gHashDcpt = &igmpGroupsDcpt.gHashDcpt;
    nEntries = 0U;
    for(groupIx = 0U; groupIx < gHashDcpt->hEntries; groupIx++)
    {
        pGEntry = FC_HashE2GrpE(TCPIP_OAHASH_EntryGet(gHashDcpt, groupIx));
        if(pGEntry->hEntry.flags.busy != 0U)
        {   // found a valid group entry
            if(nEntries < listSize)
            {
                *(groupsList + nEntries) = pGEntry->gAddress;
            }
            nEntries++;
        }
    }
    igmpDcptUnlock();
        
    if(pnGroups != NULL)
    {
        *pnGroups = nEntries;
    }

    return TCPIP_IGMP_OK;

}

TCPIP_IGMP_RESULT TCPIP_IGMP_GroupInfoGet(TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, TCPIP_IGMP_GROUP_INFO* pInfo)
{
    size_t ifIx;
    TCPIP_IGMP_GROUP_ENTRY* pGEntry;
    TCPIP_IGMP_GIF_STATE_DCPT currStateDcpt;
    TCPIP_IGMP_RESULT res;

    if(pInfo == NULL || (pInfo->listSize != 0U && pInfo->sourceList == NULL))
    {
        return TCPIP_IGMP_ARG_ERROR;
    }

    pInfo->presentSources = 0;
    pInfo->filterType = TCPIP_IGMP_FILTER_NONE;

    if(hNet == NULL)
    {
        hNet = TCPIP_STACK_NetMulticastGet();
    }

    int iIx = TCPIP_STACK_NetIndexGet(hNet);
    if(iIx < 0)
    {  
        return TCPIP_IGMP_IF_ERROR;
    }

    ifIx = (size_t)iIx;
    igmpDcptLock();
    while(true)
    {
        pGEntry = FC_HashE2GrpE(TCPIP_OAHASH_EntryLookup(&igmpGroupsDcpt.gHashDcpt, &mcastAddress));

        if(pGEntry == NULL)
        {    // no entry
            res = TCPIP_IGMP_GROUP_INVALID; 
            break;
        }

        F_IGMP_GetGifState(pGEntry, ifIx, &currStateDcpt);

        pInfo->presentSources = currStateDcpt.groupSources.nSources;
        pInfo->filterType = currStateDcpt.ifFilter;

        if(pInfo->listSize != 0U)
        {
            size_t nCopy = pInfo->presentSources > pInfo->listSize ? pInfo->listSize : pInfo->presentSources;
            (void)memcpy(&pInfo->sourceList->Val, currStateDcpt.groupSources.sourceAddresses, nCopy * sizeof(*pInfo->sourceList));
        }

        res = TCPIP_IGMP_OK;
        break;
    }
    igmpDcptUnlock();

    return res;
}

// check that the socket is allowed to receive the multicast traffic
bool TCPIP_IGMP_IsMcastEnabled(UDP_SOCKET uSkt, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, IPV4_ADDR sourceAddress)
{
    size_t ifIx;
    OA_HASH_DCPT *sHashDcpt;
    TCPIP_IGMP_GROUP_ENTRY* pGEntry;
    TCPIP_IGMP_SOURCE_ENTRY* pSEntry;
    TCPIP_IGMP_SKT_RECORD* pRec;
    IPV4_ADDR exclNone;
    bool res = false;

    if(!F_IGMP_IsSsmAddress(mcastAddress))
    {
        return true;
    }

    igmpDcptLock();
    while(true)
    {
        // search group
        pGEntry = FC_HashE2GrpE(TCPIP_OAHASH_EntryLookup(&igmpGroupsDcpt.gHashDcpt, &mcastAddress));

        if(pGEntry == NULL)
        {    // no such mcast exists
            break;
        }

        int iIx = TCPIP_STACK_NetIndexGet(hNet);
        if(iIx < 0)
        {
            return false;
        }
        ifIx = (size_t)iIx;

        // search source
        // get the source hash for this G Entry
        sHashDcpt = F_IGMP_GetSourceHashDcpt(pGEntry);
        pSEntry = FC_HashE2SrcE(TCPIP_OAHASH_EntryLookup(sHashDcpt, &sourceAddress));
        if(pSEntry != NULL)
        {
            pRec = F_IGMP_SourceFindSktRecord(pSEntry, uSkt, ifIx, TCPIP_IGMP_FILTER_NONE);
            if(pRec != NULL)
            {   // found socket in this entry;
                res = (pRec->filter == (uint8_t)TCPIP_IGMP_FILTER_INCLUDE);
            }
        }
        else
        {   // we need to search the TCPIP_IGMP_ASM_ALL_SOURCES just in case the socket used exclude {}
            exclNone.Val = TCPIP_IGMP_ASM_ALL_SOURCES;
            pSEntry = FC_HashE2SrcE(TCPIP_OAHASH_EntryLookup(sHashDcpt, &exclNone));
            if(pSEntry != NULL)
            {
                pRec = F_IGMP_SourceFindSktRecord(pSEntry, uSkt, ifIx, TCPIP_IGMP_FILTER_NONE);
                if(pRec != NULL)
                {   // found socket for exclude none/include all
                    TCPIPStack_Assert(pRec->filter == (uint8_t)TCPIP_IGMP_FILTER_EXCLUDE, __FILE__, __func__, __LINE__);
                    res = true;
                }
            }
        }

        break;
    }
    igmpDcptUnlock();

    return res;
}

/////////////////////////// helpers //////////////////////////////////////////
//
static void F_IGMP_ReportEvent(IPV4_ADDR mcastAddress, TCPIP_IGMP_EVENT_TYPE evType)
{
    F_IGMP_DbgEvent(mcastAddress, evType);

#if defined(TCPIP_IGMP_USER_NOTIFICATION ) && (TCPIP_IGMP_USER_NOTIFICATION != 0)
    TCPIP_IGMP_LIST_NODE* dNode;

    TCPIP_Notification_Lock(&igmpRegisteredUsers);
    for(dNode = (TCPIP_IGMP_LIST_NODE*)igmpRegisteredUsers.list.head; dNode != 0; dNode = dNode->next)
    {
        if(dNode->mcastAddress.Val == 0 || dNode->mcastAddress.Val == mcastAddress.Val)
        {   // trigger event
            (*dNode->handler)(mcastAddress, evType, dNode->hParam);
        }
    }    
    TCPIP_Notification_Unlock(&igmpRegisteredUsers);
#endif  // defined(TCPIP_IGMP_USER_NOTIFICATION ) && (TCPIP_IGMP_USER_NOTIFICATION != 0)
}




// packet deallocation function
// packet was transmitted by the IP layer
static void F_IGMP_TxPktAcknowledge(TCPIP_MAC_PACKET* pTxPkt, const void* ackParam)
{
    if(pTxPkt->modPktData != 0U)
    {   // redirect internally. once!
        pTxPkt->modPktData = 0U;
        pTxPkt->pktFlags |= (uint32_t)TCPIP_MAC_PKT_FLAG_MCAST;
        TCPIP_PKT_FlightLogTx(pTxPkt, TCPIP_THIS_MODULE_ID);
        TCPIPStackInsertRxPacket((const TCPIP_NET_IF*)pTxPkt->pktIf, pTxPkt, true);
    }
    else
    {
        TCPIP_PKT_PacketFree(pTxPkt);
    }
}

static IPV4_PACKET * F_IGMP_AllocateTxPacketStruct (uint16_t totIGMPLen)
{
    IPV4_PACKET *pPkt;
    TCPIP_IPV4_OPTION_DCPT optionDcpt[1];

    optionDcpt[0].optionType = TCPIP_IPV4_OPTION_ROUTER_ALERT;
    optionDcpt[0].optionSize = 0;


    uint32_t pktFlags = (uint32_t)TCPIP_MAC_PKT_FLAG_IGMP;
    pPkt = TCPIP_IPV4_PacketAlloc(totIGMPLen, optionDcpt, 1, (TCPIP_MAC_PACKET_FLAGS)pktFlags);

    if (pPkt != NULL)
    {
        TCPIP_PKT_PacketAcknowledgeSet(&pPkt->macPkt, &F_IGMP_TxPktAcknowledge, NULL);
    }

    return pPkt;
}

static size_t F_IGMP_GroupKeyHash(OA_HASH_DCPT* pOH, const void* key)
{
    // for a Group Entry the key is the gAddress IP address
    return fnv_32_hash(key, sizeof(((TCPIP_IGMP_GROUP_ENTRY*)0)->gAddress)) % (pOH->hEntries);
}

static int F_IGMP_GroupKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    TCPIP_IGMP_GROUP_ENTRY* pGrpE = FC_HashE2GrpE(hEntry);
    return pGrpE->gAddress.Val == ((const IPV4_ADDR*)key)->Val ? 0 : 1; 
}

static void F_IGMP_GroupKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    TCPIP_IGMP_GROUP_ENTRY* pDstE = FC_HashE2GrpE(dstEntry);
    pDstE->gAddress.Val = ((const IPV4_ADDR*)key)->Val;
}



#if defined(OA_DOUBLE_HASH_PROBING)
static size_t F_IGMP_GroupProbeHash(OA_HASH_DCPT* pOH, const void* key)
{
    // the key is the gAddress IP address
    return fnv_32a_hash(key, sizeof(((TCPIP_IGMP_GROUP_ENTRY*)0)->gAddress)) % (pOH->hEntries);
}
#endif  // defined(OA_DOUBLE_HASH_PROBING)

static size_t F_IGMP_SourceKeyHash(OA_HASH_DCPT* pOH, const void* key)
{
    // for a Source Entry the key is the srcAddress IP address
    return fnv_32_hash(key, sizeof(((TCPIP_IGMP_SOURCE_ENTRY*)0)->srcAddress)) % (pOH->hEntries);
}

static int F_IGMP_SourceKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    TCPIP_IGMP_SOURCE_ENTRY* pSrcE = FC_HashE2SrcE(hEntry); 
    return pSrcE->srcAddress.Val != ((const IPV4_ADDR*)key)->Val ? 1: 0; 
}

static void F_IGMP_SourceKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    TCPIP_IGMP_SOURCE_ENTRY* pDstE = FC_HashE2SrcE(dstEntry); 
    pDstE->srcAddress.Val = ((const IPV4_ADDR*)key)->Val;
}

#if defined(OA_DOUBLE_HASH_PROBING)
static size_t F_IGMP_SourceProbeHash(OA_HASH_DCPT* pOH, const void* key)
{
    // the key is the srcAddress IP address
    return fnv_32a_hash(key, sizeof(((TCPIP_IGMP_SOURCE_ENTRY*)0)->srcAddress)) % (pOH->hEntries);
}
#endif  // defined(OA_DOUBLE_HASH_PROBING)


static TCPIP_IGMP_RESULT F_IGMP_SocketRemove(UDP_SOCKET uSkt, size_t ifIx, IPV4_ADDR mcastAddress, size_t* nRemoves)
{
    size_t  removeCnt;
    TCPIP_IGMP_GROUP_ENTRY* pGEntry;
    TCPIP_IGMP_FILTER_TYPE purgeFilter;
    TCPIP_IGMP_GIF_STATE_DCPT oldStateDcpt, newStateDcpt;

    pGEntry = FC_HashE2GrpE(TCPIP_OAHASH_EntryLookup(&igmpGroupsDcpt.gHashDcpt, &mcastAddress));

    if(pGEntry == NULL)
    {    // no such mcast exists
        return TCPIP_IGMP_GROUP_INVALID;
    }

    // get old state
    F_IGMP_GetGifState(pGEntry, ifIx, &oldStateDcpt);
    
    purgeFilter = TCPIP_IGMP_FILTER_NONE;    // remove every instance of the socket
    removeCnt = F_IGMP_GroupPurgeSktByFilter(pGEntry, uSkt, ifIx, &purgeFilter);

    if(removeCnt == 0U)
    {   // nothing changed
        return TCPIP_IGMP_SOCKET_INVALID;
    }

    if(nRemoves != NULL)
    {
        *nRemoves = removeCnt;
    }

    // get new state
    F_IGMP_GetGifState(pGEntry, ifIx, &newStateDcpt);

    // rmoved some stuff; check if this entry is free
    (void)F_IGMP_GroupEntryCheckRemove(pGEntry);

    return F_IGMP_GenerateStateReport(mcastAddress, ifIx, &oldStateDcpt, &newStateDcpt);

}

static TCPIP_IGMP_RESULT F_IGMP_GenerateStateReport(IPV4_ADDR mcastAddress, size_t ifIx, TCPIP_IGMP_GIF_STATE_DCPT* pOldDcpt, TCPIP_IGMP_GIF_STATE_DCPT* pNewDcpt)
{
    TCPIP_IGMPv3_RECORD_TYPE    recType;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES  new_old, old_new;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES  *pAllow, *pBlock;

    if(pOldDcpt->ifFilter != pNewDcpt->ifFilter)
    {   // change filter report!
        // SSM should not change filter!
        TCPIPStack_Assert(!F_IGMP_IsSsmAddress(mcastAddress), __FILE__, __func__, __LINE__);
        recType = (pNewDcpt->ifFilter == TCPIP_IGMP_FILTER_INCLUDE) ? TCPIP_IGMP_FILTER_TO_INCLUDE : TCPIP_IGMP_FILTER_TO_EXCLUDE;
        return F_IGMP_ScheduleFmcReport(mcastAddress, recType, ifIx, &pNewDcpt->groupSources);
    }

    // no change in filter: Source List change record
    // calculate (new-old) and (old-new) differences
    F_IGMP_SubtractSources(&pNewDcpt->groupSources, &pOldDcpt->groupSources, &new_old);
    F_IGMP_SubtractSources(&pOldDcpt->groupSources, &pNewDcpt->groupSources, &old_new);

    pAllow = pBlock = NULL;
    if(pNewDcpt->ifFilter == TCPIP_IGMP_FILTER_INCLUDE)
    {
        if(new_old.nSources != 0U)
        {
            pAllow = &new_old;
        }
        if(old_new.nSources != 0U)
        {
            pBlock = &old_new;
        }
    }
    else
    {
        TCPIPStack_Assert(!F_IGMP_IsSsmAddress(mcastAddress), __FILE__, __func__, __LINE__);
        if(new_old.nSources != 0U)
        {
            pBlock = &new_old;
        }
        if(old_new.nSources != 0U)
        {
            pAllow = &old_new;
        }
    }

    return F_IGMP_ScheduleSlcReport(mcastAddress, ifIx, pAllow, pBlock);
}


// updates the sources for a (G, skt, if) tuple;  non empty list
// filterMode is the (new) filter type for the socket
static TCPIP_IGMP_RESULT F_IGMP_SocketUpdateSources(UDP_SOCKET uSkt, size_t ifIx, IPV4_ADDR mcastAddress, TCPIP_IGMP_FILTER_TYPE filterMode, const IPV4_ADDR* sourceList, size_t* listSize)
{
    size_t srcIx;
    size_t nSources, nNewSources;
    OA_HASH_DCPT *sHashDcpt;
    TCPIP_IGMP_GROUP_ENTRY* pGEntry;
    TCPIP_IGMP_SOURCE_ENTRY* pSEntry;
    TCPIP_IGMP_SKT_RECORD* pRec;
    TCPIP_IGMP_FILTER_TYPE purgeFilter;
    TCPIP_IGMP_GIF_STATE_DCPT oldStateDcpt, newStateDcpt;
    TCPIP_IGMP_RESULT updateRes, repRes;
    
    updateRes  = repRes = TCPIP_IGMP_OK;
    nNewSources = 0;
    while(true)
    {
        pGEntry = FC_HashE2GrpE(TCPIP_OAHASH_EntryLookupOrInsert(&igmpGroupsDcpt.gHashDcpt, &mcastAddress));

        if(pGEntry == NULL)
        {    // full hash...
            updateRes = TCPIP_IGMP_MCAST_CACHE_FULL;
            break;
        }

        // old state snapshot
        F_IGMP_GetGifState(pGEntry, ifIx, &oldStateDcpt);

        // get the source hash for this G Entry
        sHashDcpt = F_IGMP_GetSourceHashDcpt(pGEntry);

        if(pGEntry->hEntry.flags.newEntry == 0U)
        {   // existing source; if filter changes INCLUDE <-> EXCLUDE, remove the old conflicting socket filter
            // so even if we can't add all sources at least we have a consistent (G, skt, if) state
            purgeFilter = filterMode == TCPIP_IGMP_FILTER_INCLUDE ? TCPIP_IGMP_FILTER_EXCLUDE : TCPIP_IGMP_FILTER_INCLUDE;
            (void)F_IGMP_GroupPurgeSktByFilter(pGEntry, uSkt, ifIx, &purgeFilter);
        }

        nSources = *listSize;

        for(srcIx = 0; srcIx < nSources; srcIx++)
        {
            pSEntry = FC_HashE2SrcE(TCPIP_OAHASH_EntryLookupOrInsert(sHashDcpt, sourceList));
            if(pSEntry == NULL)
            {    // full source hash...
                updateRes = TCPIP_IGMP_SOURCE_CACHE_FULL;
                break;
            }

            if(pSEntry->hEntry.flags.newEntry != 0U)
            {   // just adding this source
                F_IGMP_SourceEntryInit(pSEntry);
                pRec = NULL;
            }
            else
            {
                pRec = F_IGMP_SourceFindSktRecord(pSEntry, uSkt, ifIx, TCPIP_IGMP_FILTER_NONE);
            }

            if(pRec == NULL)
            {
                pRec = F_IGMP_SourceCreateSktRecord(pSEntry, uSkt, ifIx, filterMode);
                if(pRec == NULL)
                {   // all records full
                    updateRes = TCPIP_IGMP_SOCKET_RECORD_FULL;
                    break;
                }
            }
            else
            {   // existent old source
                TCPIPStack_Assert(pRec->filter == (uint8_t)filterMode, __FILE__, __func__, __LINE__);
            }
            nNewSources++;    // count the old ones, even if already existent...
            sourceList++;
        }

        // get new state snapshot
        F_IGMP_GetGifState(pGEntry, ifIx, &newStateDcpt);

        // generate report
        repRes = F_IGMP_GenerateStateReport(mcastAddress, ifIx, &oldStateDcpt, &newStateDcpt);
        
        break;
    }

    *listSize = nNewSources;

    return updateRes == TCPIP_IGMP_OK ? repRes : updateRes; 
}

// gets the source hash descriptor associated to this G Entry
static OA_HASH_DCPT* F_IGMP_GetSourceHashDcpt(TCPIP_IGMP_GROUP_ENTRY* pGEntry)
{
    OA_HASH_DCPT *sHashDcpt;
    sHashDcpt = &igmpGroupsDcpt.srcHashDcpt;
    sHashDcpt->memBlk = pGEntry->srcEntryTbl;
    return sHashDcpt;
}

static void F_IGMP_SourceEntryInit(TCPIP_IGMP_SOURCE_ENTRY* pSEntry)
{
    (void)memset(pSEntry->sktRec, 0, sizeof(pSEntry->sktRec));
    pSEntry->nRecords = 0;
}

// finds an existing socket record for the source entry
// if filtType == 0, then any type of filter matches
// else filter has to match
static TCPIP_IGMP_SKT_RECORD* F_IGMP_SourceFindSktRecord(TCPIP_IGMP_SOURCE_ENTRY* pSEntry, UDP_SOCKET uSkt, size_t ifIx, TCPIP_IGMP_FILTER_TYPE filtType)
{
    TCPIP_IGMP_SKT_RECORD* pRec;
    size_t recIx;

    if(pSEntry->nRecords != 0U)
    {
        pRec = pSEntry->sktRec;
        for(recIx = 0; recIx < sizeof(pSEntry->sktRec) / sizeof(*pSEntry->sktRec); recIx++)
        {
            if(pRec->filter != 0U && pRec->sktNo == (uint16_t)uSkt && pRec->ifIndex == ifIx && ((uint8_t)filtType == 0U || (uint8_t)filtType == pRec->filter))
            {   // found matching record;
                return pRec;
            }
            pRec++;
        }
    }

    return NULL;
}


// creates a new socket record for the source entry
// filtType != 0 !!!
static TCPIP_IGMP_SKT_RECORD* F_IGMP_SourceCreateSktRecord(TCPIP_IGMP_SOURCE_ENTRY* pSEntry, UDP_SOCKET uSkt, size_t ifIx, TCPIP_IGMP_FILTER_TYPE filtType)
{
    TCPIP_IGMP_SKT_RECORD* pRec;
    size_t recIx;

    TCPIPStack_Assert(filtType != TCPIP_IGMP_FILTER_NONE, __FILE__, __func__, __LINE__);

    pRec = pSEntry->sktRec;
    for(recIx = 0; recIx < sizeof(pSEntry->sktRec) / sizeof(*pSEntry->sktRec); recIx++)
    {
        if(pRec->filter == 0U) 
        {   // found empty slot
            pRec->filter = (uint8_t)filtType;
            pRec->ifIndex = (uint8_t)ifIx;
            pRec->sktNo = (uint16_t)uSkt;

            pSEntry->nRecords++;
            return pRec;
        }
        pRec++;
    }

    return NULL;
}

// purges a socket record across the whole group entry if the socket has the purgeFilter type!
// otherwise the socket remains in place
// if purgeFilter == 0, then deletes the socket record no matter the type
// returns the number of removed records or 0 if socket not found
static size_t F_IGMP_GroupPurgeSktByFilter(TCPIP_IGMP_GROUP_ENTRY* pGEntry, UDP_SOCKET uSkt, size_t ifIx, TCPIP_IGMP_FILTER_TYPE* pPurgeFilt)
{
    size_t srcIx;
    OA_HASH_DCPT *sHashDcpt;
    TCPIP_IGMP_SOURCE_ENTRY* pSEntry;
    TCPIP_IGMP_SKT_RECORD* pRec;
    uint8_t sktFilter = 0U;
    TCPIP_IGMP_FILTER_TYPE purgeFilter = *pPurgeFilt;
    size_t removeCnt = 0U;

    sHashDcpt = F_IGMP_GetSourceHashDcpt(pGEntry);
    for(srcIx = 0; srcIx < sHashDcpt->hEntries; srcIx++)
    {
        pSEntry = FC_HashE2SrcE(TCPIP_OAHASH_EntryGet(sHashDcpt, srcIx));
        if(pSEntry->hEntry.flags.busy != 0U)
        {   // found a valid source entry
            pRec = F_IGMP_SourceFindSktRecord(pSEntry, uSkt, ifIx, TCPIP_IGMP_FILTER_NONE);
            if(pRec != NULL)
            {   // found socket in this entry 
                if(sktFilter == 0U)
                {   // first match
                    sktFilter = pRec->filter;
                }
                else
                {
                    TCPIPStack_Assert(sktFilter == pRec->filter, __FILE__, __func__, __LINE__);
                }

                if((uint8_t)purgeFilter == 0U || sktFilter == (uint8_t)purgeFilter)
                {   // purged
                    pRec->filter = 0;
                    removeCnt++;
                    if(--pSEntry->nRecords == 0U)
                    {   // this source Entry is empty
                        TCPIP_OAHASH_EntryRemove(sHashDcpt, &pSEntry->hEntry);
                    }
                }

#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_CONSISTENCY) == 0)
                else
                {   // socket type does not match the required purge type
                    break;
                }
#endif  // ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_CONSISTENCY) == 0)
            }
        }
    }

    *pPurgeFilt = (TCPIP_IGMP_FILTER_TYPE)sktFilter;
    return removeCnt;
}

// purges a (socket, if, source) tuple across the whole group entry
// returns true if the socket source removed or false if socket not found in this source
static bool F_IGMP_GroupPurgeSktBySource(TCPIP_IGMP_GROUP_ENTRY* pGEntry, UDP_SOCKET uSkt, size_t ifIx, const IPV4_ADDR* pPurgeAdd)
{
    OA_HASH_DCPT *sHashDcpt;
    TCPIP_IGMP_SOURCE_ENTRY* pSEntry;
    TCPIP_IGMP_SKT_RECORD* pRec;

    pRec = NULL;
    sHashDcpt = F_IGMP_GetSourceHashDcpt(pGEntry);
    pSEntry = FC_HashE2SrcE(TCPIP_OAHASH_EntryLookup(sHashDcpt, pPurgeAdd));
    if(pSEntry != NULL)
    {
        pRec = F_IGMP_SourceFindSktRecord(pSEntry, uSkt, ifIx, TCPIP_IGMP_FILTER_NONE);
        if(pRec != NULL)
        {   // found socket in this entry; purge 
            pRec->filter = 0;
            if(--pSEntry->nRecords == 0U)
            {   // this source Entry is empty
                TCPIP_OAHASH_EntryRemove(sHashDcpt, &pSEntry->hEntry);
            }
        }
    }

    return pRec != NULL;
}

// calculates the state of a (Group, If) pair
// state consists of a filter mode and list of sources
// if there are no sources for this (G, if), filter is INCLUDE and sources is {}
static void F_IGMP_GetGifState(TCPIP_IGMP_GROUP_ENTRY* pGEntry, size_t ifIx, TCPIP_IGMP_GIF_STATE_DCPT* pGifDcpt)
{
    size_t srcIx;
    TCPIP_IGMP_SIF_FILTER_ACTION srcAction;
    OA_HASH_DCPT *sHashDcpt;
    TCPIP_IGMP_SOURCE_ENTRY*    pSEntry;
    TCPIP_IGMP_FILTER_TYPE      gifFilter, sifFilter;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES    incSources;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES    excSources;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES*   pGrSources;


    // init the structures
    (void)memset(pGifDcpt, 0, sizeof(*pGifDcpt));
    (void)memset(&incSources, 0, sizeof(incSources));
    (void)memset(&excSources, 0, sizeof(excSources));

    gifFilter = TCPIP_IGMP_FILTER_INCLUDE;

    sHashDcpt = F_IGMP_GetSourceHashDcpt(pGEntry);
    for(srcIx = 0; srcIx < sHashDcpt->hEntries; srcIx++)
    {
        pSEntry = FC_HashE2SrcE(TCPIP_OAHASH_EntryGet(sHashDcpt, srcIx));
        if(pSEntry->hEntry.flags.busy != 0U)
        {   // found a valid source entry
            TCPIPStack_Assert(pSEntry->nRecords != 0U, __FILE__, __func__, __LINE__);

            srcAction = F_IGMP_GetSifFilter(pSEntry, ifIx, &sifFilter);

            if(sifFilter == TCPIP_IGMP_FILTER_EXCLUDE)
            {   // switch to exclude mode
                gifFilter = TCPIP_IGMP_FILTER_EXCLUDE;
                if(srcAction == TCPIP_IGMP_SIF_ACTION_EXCLUDE_NONE)
                {   // include all
                    excSources.nSources = 0;
                    break;
                }
                else if(srcAction == TCPIP_IGMP_SIF_ACTION_ADD)
                {
                    excSources.sourceAddresses[excSources.nSources++] = pSEntry->srcAddress.Val;
                }
                else
                {
                    // do nothing
                }
            }
            else if(sifFilter == TCPIP_IGMP_FILTER_INCLUDE)
            {
                if(gifFilter != TCPIP_IGMP_FILTER_EXCLUDE)
                {   // in include mode
                    gifFilter = TCPIP_IGMP_FILTER_INCLUDE;
                    incSources.sourceAddresses[incSources.nSources++] = pSEntry->srcAddress.Val;
                }
                // else already in exclude mode, so ignore
            }
            else
            {
                // else the source is not used
            }
        }
    }

    pGifDcpt->ifFilter = gifFilter;
    pGrSources = (gifFilter == TCPIP_IGMP_FILTER_EXCLUDE) ? &excSources : &incSources;
    (void)memcpy(&pGifDcpt->groupSources, pGrSources, sizeof(*pGrSources));

}


// calculates the source filter for a (Source, if) pair
// returns TCPIP_IGMP_SIF_FILTER_ACTION if the source needs to be counted/added/ excluded, etc.
// the pFilter is updated per source or 0 if source not used
// Note:
//  - if all sockets include the source, then the filter is INCLUDE and the source is counted
//  - if all sockets exclude the source then the filter is EXCLUDE and the source is counted
//  - if some include and some exclude, filter is EXCLUDE but the source is not counted
//  - if some sockets have exclude {}, i.e. ASM_ALL_SOURCES on G, then EXCLUDE_NONE is returned
static TCPIP_IGMP_SIF_FILTER_ACTION F_IGMP_GetSifFilter(const TCPIP_IGMP_SOURCE_ENTRY* pSEntry, size_t ifIx, TCPIP_IGMP_FILTER_TYPE* pFilter)
{
    TCPIP_IGMP_SIF_FILTER_ACTION srcAction = TCPIP_IGMP_SIF_ACTION_NONE;

    *pFilter = TCPIP_IGMP_FILTER_NONE;

    if(pSEntry->nRecords != 0U)
    {
        size_t recIx;
        const TCPIP_IGMP_SKT_RECORD* pRec = pSEntry->sktRec;
        size_t nIncludes = 0U;
        size_t nExcludes = 0U;

        if(pSEntry->srcAddress.Val == TCPIP_IGMP_ASM_ALL_SOURCES)
        {
            for(recIx = 0; recIx < sizeof(pSEntry->sktRec) / sizeof(*pSEntry->sktRec); recIx++)
            {
                if(pRec->filter != 0U && pRec->ifIndex == ifIx)
                {   // found socket record;
                    TCPIPStack_Assert(pRec->filter == (uint8_t)TCPIP_IGMP_FILTER_EXCLUDE, __FILE__, __func__, __LINE__);
                    *pFilter = TCPIP_IGMP_FILTER_EXCLUDE;
                    return TCPIP_IGMP_SIF_ACTION_EXCLUDE_NONE;
                }
                pRec++;
            }
            return TCPIP_IGMP_SIF_ACTION_NONE;
        }

        for(recIx = 0; recIx < sizeof(pSEntry->sktRec) / sizeof(*pSEntry->sktRec); recIx++)
        {
            if(pRec->filter != 0U && pRec->ifIndex == ifIx)
            {   // found socket record;
                if(pRec->filter == (uint8_t)TCPIP_IGMP_FILTER_INCLUDE)
                {   
                    nIncludes++;
                }
                else
                {
                    nExcludes++;
                }
            }
            pRec++;
        }

        // since nRecords != 0, then we should have something
        TCPIPStack_Assert((nExcludes + nIncludes) != 0U, __FILE__, __func__, __LINE__);

        if(nExcludes != 0U)
        {
            *pFilter = TCPIP_IGMP_FILTER_EXCLUDE;
            if(nIncludes == 0U)
            {   // excluded source
                srcAction = TCPIP_IGMP_SIF_ACTION_ADD;
            }
        }
        else if(nIncludes != 0U)
        {   // included source
            *pFilter = TCPIP_IGMP_FILTER_INCLUDE;
            srcAction = TCPIP_IGMP_SIF_ACTION_ADD;
        }
        else
        {
            // do nothing
        }

    }

    return srcAction;
}

// calculates A-B and places the result at pDest
static void F_IGMP_SubtractSources(TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pA, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pB, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pDest)
{
    uint32_t *ptrA, *aEnd;
    uint32_t *ptrB, *bEnd;
    uint32_t *pRes;

    // clear destination
    (void)memset(pDest, 0, sizeof(*pDest));

    if(pA == NULL)
    {
        return;
    }

    ptrA = pA->sourceAddresses; 
    aEnd = ptrA + pA->nSources;

    pRes = pDest->sourceAddresses;

    if(pB != NULL)
    {
        ptrB = pB->sourceAddresses; 
        bEnd = ptrB + pB->nSources;

        // sort A and B
        FC_Sort(ptrA, pA->nSources, sizeof(*ptrA), &F_IGMPSourceEntryCompare);
        FC_Sort(ptrB, pB->nSources, sizeof(*ptrB), &F_IGMPSourceEntryCompare);


        while(ptrA != aEnd && ptrB != bEnd)
        {
            if(*ptrA < *ptrB)
            {   // only in A
                *pRes++ = *ptrA++;
                pDest->nSources++;
            }
            else if(*ptrB < *ptrA)
            {   // only in B
                ptrB++;
            }
            else
            {   // both A and B
                ptrA++; ptrB++;
            }
        }
    }

    // collect whatever's left in A, if any
    uint32_t offset = FC_PtrDiff2UI32(aEnd - ptrA);
    (void)memcpy(pRes, ptrA, (size_t)offset);
    pDest->nSources += (size_t)offset;

}

// calculates A+B and places the result at pDest
// pDest has to be large enough to accomodate both sources!
static void F_IGMP_UnionSources(TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pA, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pB, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pDest)
{
    uint32_t *ptrA, *aEnd;
    uint32_t *ptrB, *bEnd;
    uint32_t *pRes;

    // clear destination
    (void)memset(pDest, 0, sizeof(*pDest));

    if(pA == NULL || pB == NULL)
    {
        if(pB != NULL)
        {
            (void)memcpy(pDest, pB, sizeof(*pDest));
        }
        else if(pA != NULL)
        {
            (void)memcpy(pDest, pA, sizeof(*pDest));
        }
        else
        {
            // do nothing
        }
        return;
    }

    ptrA = pA->sourceAddresses; 
    aEnd = ptrA + pA->nSources;

    ptrB = pB->sourceAddresses; 
    bEnd = ptrB + pB->nSources;

    // sort A and B
    FC_Sort(ptrA, pA->nSources, sizeof(*ptrA), &F_IGMPSourceEntryCompare);
    FC_Sort(ptrB, pB->nSources, sizeof(*ptrB), &F_IGMPSourceEntryCompare);

    pRes = pDest->sourceAddresses;

    while (true)
    {
        if (ptrA == aEnd)
        {
            uint32_t offset = FC_PtrDiff2UI32(bEnd - ptrB);
            (void)memcpy(pRes, ptrB, (size_t)offset);
            pDest->nSources += (size_t)offset;
            break;
        }

        if (ptrB == bEnd)
        {
            uint32_t offset = FC_PtrDiff2UI32(aEnd - ptrA);
            (void)memcpy(pRes, ptrA, (size_t)offset);
            pDest->nSources += (size_t)offset;
            break;
        }

        if(*ptrA < *ptrB)
        {
           *pRes++ = *ptrA++;
        }
        else if(*ptrB < *ptrA)
        {
           *pRes++ = *ptrB++;
        }
        else
        {
            *pRes++ = *ptrA++; ptrB++;
        }
        pDest->nSources++;
    }
}

// calculates intersection of (A, B) and places the result at pDest
static void F_IGMP_IntersectSources(TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pA, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pB, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pDest)
{
    uint32_t *ptrA, *aEnd;
    uint32_t *ptrB, *bEnd;
    uint32_t *pRes;

    // clear destination
    (void)memset(pDest, 0, sizeof(*pDest));

    if(pA == NULL || pB == NULL)
    {
        return;
    }

    ptrA = pA->sourceAddresses; 
    aEnd = ptrA + pA->nSources;

    ptrB = pB->sourceAddresses; 
    bEnd = ptrB + pB->nSources;

    // sort A and B
    FC_Sort(ptrA, pA->nSources, sizeof(*ptrA), &F_IGMPSourceEntryCompare);
    FC_Sort(ptrB, pB->nSources, sizeof(*ptrB), &F_IGMPSourceEntryCompare);

    pRes = pDest->sourceAddresses;


    while(ptrA != aEnd && ptrB != bEnd)
    {
        if(*ptrA < *ptrB)
        {   // A only
            ptrA++;
        }
        else if(*ptrB < *ptrA)
        {   // B only
            ptrB++;
        }
        else
        {   // both
            *pRes++ = *ptrA++;
            ptrB++;
            pDest->nSources++;
        }
    }
}




// sorting function
static int F_IGMPSourceEntryCompare(const void* p1, const void* p2)
{
    uint32_t add1 = *((const uint32_t*)p1);
    uint32_t add2 = *((const uint32_t*)p2);
   

    if(add1 < add2)
    {
        return -1;
    }
    else if(add1 > add2)
    {
        return 1;
    }
    else
    {
        // do nothing
    }

    return 0;    
}

// schedule Filter mode change (FMC) report
// searches the current SC reports and deletes any allow/block report belonging to this group
// returns TCPIP_IGMP_OK or TCPIP_IGMP_REPORT_POOL_EMPTY if no report node could be found
static TCPIP_IGMP_RESULT F_IGMP_ScheduleFmcReport(IPV4_ADDR groupAddress, TCPIP_IGMPv3_RECORD_TYPE repType, size_t ifIx, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pGroupSources)
{
    TCPIP_IGMP_SC_REPORT_NODE* pFiltNode;


    igmpListsLock();

    pFiltNode = F_IGMP_FindScheduledScReport(groupAddress, ifIx, NULL, NULL, true);

    if(pFiltNode != NULL)
    {
        pFiltNode->active = 0;
    }

    igmpListsUnlock();

    if(pFiltNode == NULL)
    {   // couldn't find an old report for (G, if)
        pFiltNode = F_IGMP_GetNewScReport();
    }

    if(pFiltNode == NULL)
    {   // couldn't find a slot for the new report
        F_IGMP_ReportEvent(groupAddress, TCPIP_IGMP_EVENT_GET_SC_REPORT_ERROR);
        TCPIPStack_Condition(true, __FILE__, __func__, __LINE__);
        return TCPIP_IGMP_REPORT_POOL_EMPTY;
    }

    F_IGMP_SetGifSCRecord(pFiltNode, groupAddress, repType, ifIx, pGroupSources);
    
    // add it to the report list
    F_IGMP_InsertScReport(&igmpScReportList, pFiltNode);

    return TCPIP_IGMP_OK; 
}


// sets a Source Change report node info
static void F_IGMP_SetGifSCRecord(TCPIP_IGMP_SC_REPORT_NODE* pRepNode, IPV4_ADDR groupAddress, TCPIP_IGMPv3_RECORD_TYPE repType, size_t ifIx, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pGroupSources)
{
    pRepNode->nTransmit = igmpRobustnessVar;
    pRepNode->repType = (uint8_t)repType;
    pRepNode->ifIx = (uint8_t)ifIx;
    pRepNode->active = 1U;
    F_IGMP_SetScReportTransmitTime(pRepNode);
    pRepNode->repGroup = groupAddress.Val;
    
    (void)memcpy(&pRepNode->repSources, pGroupSources, sizeof(pRepNode->repSources));
}

// gets a new available node report from the free list
static TCPIP_IGMP_SC_REPORT_NODE* F_IGMP_GetNewScReport(void)
{
    igmpListsLock();
    TCPIP_IGMP_SC_REPORT_NODE* pNewRep = FC_SglNode2RepNode(TCPIP_Helper_SingleListHeadRemove(&igmpScFreeList));
    igmpListsUnlock();

    return pNewRep;
}

// inserts a new node in a SC report list
static void F_IGMP_InsertScReport(SINGLE_LIST* pList, TCPIP_IGMP_SC_REPORT_NODE* pRepNode)
{
    igmpListsLock();
    TCPIP_Helper_SingleListTailAdd(pList, FC_RepNode2SglNode(pRepNode));
    igmpListsUnlock();
}

// deletes a report node from a SC report list
static void F_IGMP_DeleteScReport(SINGLE_LIST* pList, TCPIP_IGMP_SC_REPORT_NODE* pRepNode)
{
    igmpListsLock();

    TCPIP_IGMP_SC_REPORT_NODE* pRem = FC_SglNode2RepNode(TCPIP_Helper_SingleListNodeRemove(pList, FC_RepNode2SglNode(pRepNode)));
    TCPIPStack_Assert(pRem == pRepNode, __FILE__, __func__, __LINE__);
    TCPIP_Helper_SingleListTailAdd(&igmpScFreeList, FC_RepNode2SglNode(pRem));
    
    igmpListsUnlock();
}

// schedule a source list change (SLC)
// returns TCPIP_IGMP_OK or TCPIP_IGMP_REPORT_POOL_EMPTY if no report node could be found
static TCPIP_IGMP_RESULT F_IGMP_ScheduleSlcReport(IPV4_ADDR groupAddress, size_t ifIx, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pNewAllow, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pNewBlock)
{
    TCPIP_IGMP_SC_REPORT_NODE *pAllowRep, *pBlockRep, *pFmcRep;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES unionAllow, unionBlock;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES resultAllow, resultBlock;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES *pUnionAllow, *pUnionBlock;
   
    pFmcRep = F_IGMP_FindGifSCReportsDisable(groupAddress, ifIx, &pAllowRep, &pBlockRep);

    if(pAllowRep != NULL)
    {
        F_IGMP_UnionSources(pNewAllow, &pAllowRep->repSources, &unionAllow);
        pUnionAllow = &unionAllow;
    }
    else
    {   // no pending allow report
        pUnionAllow = pNewAllow;
    }

    if(pBlockRep != NULL)
    {
        F_IGMP_UnionSources(pNewBlock, &pBlockRep->repSources, &unionBlock);
        pUnionBlock = &unionBlock;
    }
    else
    {   // no pending allow report
        pUnionBlock = pNewBlock;
    }

    // finally, calculate:
    // what's allowed: union(allows) - union(blocks)
    // what's blocked: union(blocks) - union(allows)
    F_IGMP_SubtractSources(pUnionAllow, pUnionBlock, &resultAllow);
    F_IGMP_SubtractSources(pUnionBlock, pUnionAllow, &resultBlock);

    // create the reports
    if(resultAllow.nSources != 0U)
    {   // need to allow something
        if(pAllowRep == NULL)
        {   // grab a new one
            pAllowRep = F_IGMP_GetNewScReport();
        }

        if(pAllowRep == NULL)
        {   // couldn't find a slot for the new report; this is BAD!
            TCPIPStack_Condition(true, __FILE__, __func__, __LINE__);
            return TCPIP_IGMP_REPORT_POOL_EMPTY;
        }
    }
    else
    {   // no need to transmit allow report
        if(pAllowRep != NULL)
        {   // delete the report
            F_IGMP_DeleteScReport(&igmpScReportList, pAllowRep);
        }

        pAllowRep = NULL;
    }
    
    if(resultBlock.nSources != 0U)
    {   // need to block something
        if(pBlockRep == NULL)
        {   // grab a new one
            pBlockRep = F_IGMP_GetNewScReport();
        }

        if(pBlockRep == NULL)
        {   // couldn't find a slot for the new report; this is BAD!
            TCPIPStack_Condition(true, __FILE__, __func__, __LINE__);
            return TCPIP_IGMP_REPORT_POOL_EMPTY;
        }
    }
    else
    {   // no need to transmit allow report
        if(pBlockRep != NULL)
        {   // delete the report
            F_IGMP_DeleteScReport(&igmpScReportList, pBlockRep);
        }

        pBlockRep = NULL;
    }

    if(pAllowRep != NULL)
    {
        F_IGMP_SetGifSCRecord(pAllowRep, groupAddress, TCPIP_IGMP_SOURCE_ALLOW_NEW, ifIx, &resultAllow);
        if(pFmcRep != NULL)
        {   // will be activated once the filter is transmitted!
            pAllowRep->active = 0;
        }
        F_IGMP_InsertScReport(&igmpScReportList, pAllowRep);
    }
    
    if(pBlockRep != NULL)
    {
        F_IGMP_SetGifSCRecord(pBlockRep, groupAddress, TCPIP_IGMP_SOURCE_BLOCK_OLD, ifIx, &resultBlock);
        if(pFmcRep != NULL)
        {   // will be activated once the filter is transmitted!
            pBlockRep->active = 0;
        }
        F_IGMP_InsertScReport(&igmpScReportList, pBlockRep);
    }
    
    return TCPIP_IGMP_OK; 
}

// finds SC reports for (G, if)
// and gets the pointers for the allow ones and block ones
// Disables the allow and block
// returns the filter report if it exists
static TCPIP_IGMP_SC_REPORT_NODE* F_IGMP_FindGifSCReportsDisable(IPV4_ADDR groupAddress, size_t ifIx, TCPIP_IGMP_SC_REPORT_NODE** ppAllowNode, TCPIP_IGMP_SC_REPORT_NODE** ppBlockNode)
{
    TCPIP_IGMP_SC_REPORT_NODE *pAllow, *pBlock, *pFmc;

    pAllow = pBlock = pFmc = NULL;

    igmpListsLock();

    // search for allow, block and filter
    pFmc = F_IGMP_FindScheduledScReport(groupAddress, ifIx, &pAllow, &pBlock, false);
    if(pAllow != NULL)
    {   // check that allow/block are pending
        pAllow->active = 0;
    }
    if(pBlock != NULL)
    {   // check that allow/block are pending
        pBlock->active = 0;
    }

    igmpListsUnlock();


    *ppAllowNode = pAllow;
    *ppBlockNode = pBlock;

    return pFmc;

}

// searches for a (G, if) report into the SC list
// ppAllowNode and ppBlockNode could be NULL
// access to the list should be LOCKED!
static TCPIP_IGMP_SC_REPORT_NODE* F_IGMP_FindScheduledScReport(IPV4_ADDR groupAddress, size_t ifIx, TCPIP_IGMP_SC_REPORT_NODE** ppAllowNode, TCPIP_IGMP_SC_REPORT_NODE** ppBlockNode, bool removeAllowBlock)
{
    TCPIP_IGMP_SC_REPORT_NODE *pRNode;
    TCPIP_IGMP_SC_REPORT_NODE *pAllow, *pBlock, *pFilt;
    SINGLE_LIST  newBusyList;
    bool removeNode;

    pAllow = pBlock = pFilt = NULL;
    TCPIP_Helper_SingleListInitialize(&newBusyList);


    while((pRNode = FC_SglNode2RepNode(TCPIP_Helper_SingleListHeadRemove(&igmpScReportList))) != NULL)
    {
        removeNode = false;

        if(pRNode->ifIx == ifIx && pRNode->repGroup == groupAddress.Val)
        {   // found node
            if(pRNode->repType == (uint8_t)TCPIP_IGMP_FILTER_TO_INCLUDE || pRNode->repType == (uint8_t)TCPIP_IGMP_FILTER_TO_EXCLUDE)
            {
                TCPIPStack_Assert(pFilt == NULL, __FILE__, __func__, __LINE__);
                pFilt = pRNode;
            }
            else if(pRNode->repType == (uint8_t)TCPIP_IGMP_SOURCE_ALLOW_NEW || pRNode->repType == (uint8_t)TCPIP_IGMP_SOURCE_BLOCK_OLD)
            {
                if(pRNode->repType == (uint8_t)TCPIP_IGMP_SOURCE_ALLOW_NEW)
                {
                    TCPIPStack_Assert(pAllow == NULL, __FILE__, __func__, __LINE__);
                    pAllow = pRNode;
                }
                else
                {
                    TCPIPStack_Assert(pBlock == NULL, __FILE__, __func__, __LINE__);
                    pBlock = pRNode;
                }
                
                if(removeAllowBlock)
                {
                    removeNode = true;
                }
            }
            else
            {   // unknown type
                TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
            }
        }
        // else not ours

        if(removeNode)
        {
            TCPIP_Helper_SingleListTailAdd(&igmpScFreeList, FC_RepNode2SglNode(pRNode));
        }
        else
        {   // keep it
            TCPIP_Helper_SingleListTailAdd(&newBusyList, FC_RepNode2SglNode(pRNode));
        }
    }

    if(pFilt != NULL)
    {
        if(pAllow != NULL)
        {   // check that allow/block are pending
            TCPIPStack_Assert(pAllow->active == 0U, __FILE__, __func__, __LINE__);
        }
        if(pBlock != NULL)
        {   // check that allow/block are pending
            TCPIPStack_Assert(pBlock->active == 0U, __FILE__, __func__, __LINE__);
        }
    }
    
    if(ppAllowNode != NULL)
    {
        *ppAllowNode = pAllow;
    }

    if(ppBlockNode != NULL)
    {
        *ppBlockNode = pBlock;
    }

    igmpScReportList = newBusyList;

    return pFilt;

}

// searches for a General Query or group (G, if) report into the proper Query list
// returns found query node 
// access to the list should be LOCKED!
static TCPIP_IGMP_QUERY_REPORT_NODE* F_IGMP_FindScheduledQueryReport(TCPIP_IGMP_QUERY_TYPE qType, uint32_t groupAddress, size_t ifIx, bool repRemove)
{
    TCPIP_IGMP_QUERY_REPORT_NODE *pQNode, *pPrev, *pRemN;
    SINGLE_LIST*  pList;

    if(qType == TCPIP_IGMP_QUERY_GENERAL)
    {
        pList = &igmpGenQueryReportList;
    }
    else
    {
        pList = &igmpGroupQueryReportList;
    }

    pPrev = NULL;
    pQNode = FC_SglNode2QRepNode(pList->head);
    while(pQNode != NULL)
    {
        if(pQNode->ifIx == ifIx)
        {   // matching interface
            if(qType == TCPIP_IGMP_QUERY_GENERAL || pQNode->qSources[0].repGroup == groupAddress)
            {   // matching group; found node
                if(repRemove)
                {
                    pRemN = FC_SglNode2QRepNode(TCPIP_Helper_SingleListNextRemove(pList, FC_QRepNode2SglNode(pPrev)));
                    (void)pRemN;
                    TCPIPStack_Assert(pRemN == pQNode, __FILE__, __func__, __LINE__);
                }
                return pQNode;
            }
        }
        pPrev = pQNode;
        pQNode = pQNode->next;
    }

    return NULL;   // not found
}


// checks that an IPv4 multicast address is valid
static bool F_IGMP_IsMcastValid(IPV4_ADDR addr)
{
    if(F_IGMP_IsMcastAddress(addr))
    {
        return addr.Val != TCPIP_IGMP_IANA_ALL_SYSTEMS_MULTICAST && addr.Val != TCPIP_IGMP_IANA_ALL_ROUTERS_MULTICAST && addr.Val != TCPIP_IGMP_IANA_SSM_RESERVED;
    }

    return false;
}

static bool  F_IGMP_IsSsmAddress(IPV4_ADDR mcastAdd)
{
    return (TCPIP_Helper_ntohl(igmpSsmAddLow) <= TCPIP_Helper_ntohl(mcastAdd.Val) &&  TCPIP_Helper_ntohl(mcastAdd.Val) <= TCPIP_Helper_ntohl(igmpSsmAddHigh));
}

// performs a minimal sanity check for subscribe parameters
static TCPIP_IGMP_RESULT F_IGMP_CheckSubscribeParams(TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, const IPV4_ADDR* sourceList, size_t* listSize, size_t* pIfIx)
{
    int ifIx;
    TCPIP_NET_IF* pNetIf;

    size_t nSources = listSize != NULL ? *listSize : 0U;

    // minimal sanity check
    if(nSources != 0U)
    {
#if defined(TCPIP_IGMPV2_SUPPORT_ONLY) && (TCPIP_IGMPV2_SUPPORT_ONLY != 0)
        return TCPIP_IGMP_SOURCE_INVALID;
#endif
       if(sourceList == NULL)
       {
           return TCPIP_IGMP_ARG_ERROR;
       }
    }

    if(!F_IGMP_IsMcastValid(mcastAddress))
    {
        return TCPIP_IGMP_MCAST_ADDRESS_ERROR;
    }

    if(hNet == NULL)
    {
        hNet = TCPIP_STACK_NetMulticastGet();
    }

    ifIx = TCPIP_STACK_NetIndexGet(hNet);
    pNetIf = TCPIPStackHandleToNetUp(hNet);

    if(ifIx < 0 || pNetIf == NULL || (pNetIf->startFlags & (uint16_t)TCPIP_NETWORK_CONFIG_MULTICAST_ON) == 0U)
    {
        return TCPIP_IGMP_IF_ERROR;
    }


    *pIfIx = (size_t)ifIx;
    return TCPIP_IGMP_OK;
}

// checks that the interface index is a valid slot or a new one can be taken
static bool F_IGMP_CheckIfIndex(size_t ifIx, bool itExists)
{

    if(ifIx < igmpInterfaces)
    {   
        if((igmpGroupsDcpt.ifMask & (1UL << ifIx)) != 0U)
        {
            return true;
        }
        else if(itExists == false)
        {
            igmpGroupsDcpt.ifMask |= (1UL << ifIx);
            return true;
        }
        else
        {
            // do nothing
        }
    }

    return false;

}

static void F_IGMP_SetScReportTransmitTime(TCPIP_IGMP_SC_REPORT_NODE* pRNode)
{
    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet();
    pRNode->tTransmit = SYS_TMR_TickCountGet() + (SYS_RANDOM_PseudoGet() % igmpUnsolicitRepInterval) * sysFreq / 1000U;
}


// checks if a Group Entry could be removed and removes it if possible
static bool F_IGMP_GroupEntryCheckRemove(TCPIP_IGMP_GROUP_ENTRY* pGEntry)
{
    OA_HASH_DCPT *sHashDcpt;
    sHashDcpt = F_IGMP_GetSourceHashDcpt(pGEntry);
    if(sHashDcpt->fullSlots == 0U)
    {   // no more sources in this group
        TCPIP_OAHASH_EntryRemove(&igmpGroupsDcpt.gHashDcpt, &pGEntry->hEntry);
        return true;
    }

    return false;
}

static void F_IGMP_ProcessV3Query(TCPIP_IGMPv3_QUERY_MESSAGE* pQuery, size_t ifIx)
{
    TCPIP_IGMP_QUERY_TYPE qType;
    uint32_t maxRespTime, tReport;
    TCPIP_IGMP_RESP_CODE_FLOAT rCodeFloat;
    TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGq;
    TCPIP_IGMP_GROUP_QUERY_REPORT_NODE *pGsq;
    IPV4_ADDR groupAddress;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES newReqSources, unionSources;
    uint16_t nSources = 0;

    igmpRobustnessVar = pQuery->qrv < (unsigned)TCPIP_IGMP_ROBUSTNESS_LOW_LIMIT ? (uint8_t)TCPIP_IGMP_ROBUSTNESS_VARIABLE : (uint8_t)pQuery->qrv;
    
    if((groupAddress.Val = pQuery->groupAddress) == 0U)
    {
        qType = TCPIP_IGMP_QUERY_GENERAL;
    }
    else
    {
        if(pQuery->nSources == 0U)
        {
            qType = TCPIP_IGMP_QUERY_GROUP_SPEC;
        }
        else
        {
            qType = TCPIP_IGMP_QUERY_GROUP_SOURCE_SPEC;
            nSources = TCPIP_Helper_ntohs(pQuery->nSources);
        }
    }

    // calculate maxRespTime in milliseconds
    if(pQuery->maxRespCode < TCPIP_IGMP_MAX_RESP_CODE_LIMIT)
    {
        maxRespTime = (uint32_t)pQuery->maxRespCode * 100U;
    }
    else
    {
        rCodeFloat.val = pQuery->maxRespCode;
        maxRespTime = (((uint32_t)rCodeFloat.r_mant | 0x10U) << ((uint32_t)rCodeFloat.r_exp + 3U)) * 100U;
    }

    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet();
    tReport = SYS_TMR_TickCountGet() + (SYS_RANDOM_PseudoGet() % maxRespTime) * sysFreq / 1000U;

    igmpListsLock();
    pGq = FC_RepNode2GqNode(F_IGMP_FindScheduledQueryReport(TCPIP_IGMP_QUERY_GENERAL, 0, ifIx, false));
    igmpListsUnlock();

    // check the current status report rules
    // #1 rule: check for existing GQ scheduled earlier
    if(pGq != NULL && pGq->tTransmit < tReport)
    {
        return;
    }

    // #2 rule: if a GQ was received, schedule it and cancel a previous GQ
    if(qType == TCPIP_IGMP_QUERY_GENERAL)
    {
        bool insert = false;
        if(pGq == NULL)
        {   // couldn't find an old GQ report
            pGq = FC_RepNode2GqNode(F_IGMP_GetNewQueryReport(TCPIP_IGMP_QUERY_GENERAL));
            insert = true;
        }

        if(pGq != NULL)
        {   // prepare the GQ report and schedule it;
            F_IGMP_ScheduleGenQueryReport(pGq, ifIx, tReport, insert);
        }
        else
        {   // couldn't find a slot for the new GQ report; should NOT happen!
            TCPIPStack_Assert(true, __FILE__, __func__, __LINE__);
            F_IGMP_ReportEvent(groupAddress, TCPIP_IGMP_EVENT_GET_GEN_QUERY_REPORT_ERROR);
        }
        return;
    }

    // #3 rule: GSQ or GSSQ received; if no pending response, then schedule a group report 
    igmpListsLock();
    pGsq = FC_QRepNode2GrRepNode(F_IGMP_FindScheduledQueryReport(TCPIP_IGMP_QUERY_GROUP_SPEC, groupAddress.Val, ifIx, false));
    igmpListsUnlock();
    if(pGsq == NULL)
    {
        pGsq = FC_QRepNode2GrRepNode(F_IGMP_GetNewQueryReport(qType));
        if(pGsq != NULL)
        {
            F_IGMP_InitGroupQueryReport(pGsq, groupAddress, ifIx, tReport);
            F_IGMP_SourceScheduleGroupQueryReport(pGsq, nSources, pQuery->sources, true);
        }
        else
        {   // couldn't find a slot for the new group report
            TCPIPStack_Condition(true, __FILE__, __func__, __LINE__);
            F_IGMP_ReportEvent(groupAddress, TCPIP_IGMP_EVENT_GET_GROUP_QUERY_REPORT_ERROR);
        }
        return;
    }

    // #4 rule: if GSQ received or pending GSQ, then schedule a GSQ 
    if(qType == TCPIP_IGMP_QUERY_GROUP_SPEC || pGsq->qSources[0].repSources.nSources == 0U)
    {
        pGsq->qSources[0].repSources.nSources = 0;  // clear old pending sources
        if(tReport < pGsq->tTransmit)
        {
            pGsq->tTransmit = tReport;
        }
        return;
    }

    // #5 rule: GSSQ received and pending GSSQ, then schedule an augumented GSSQ 
    if(tReport < pGsq->tTransmit)
    {
        pGsq->tTransmit = tReport;
    }
    
    newReqSources.nSources = nSources;
    (void)memcpy(newReqSources.sourceAddresses, pQuery->sources, nSources * sizeof(newReqSources.sourceAddresses[0]));
    
    F_IGMP_UnionSources(&newReqSources, &pGsq->qSources[0].repSources, &unionSources);
    F_IGMP_SourceScheduleGroupQueryReport(pGsq, unionSources.nSources, unionSources.sourceAddresses, false);
    

}

// gets a new query report node
static TCPIP_IGMP_QUERY_REPORT_NODE* F_IGMP_GetNewQueryReport(TCPIP_IGMP_QUERY_TYPE qType)
{
    SINGLE_LIST* pList;
    TCPIP_IGMP_QUERY_REPORT_NODE* pQNode;

    if(qType == TCPIP_IGMP_QUERY_GENERAL)
    {
        pList = &igmpGenQueryFreeList; 
    }
    else
    {
        pList = &igmpGroupQueryFreeList; 
    }

    igmpListsLock();
    pQNode = FC_SglNode2QRepNode(TCPIP_Helper_SingleListHeadRemove(pList));
    igmpListsUnlock();

    if(qType == TCPIP_IGMP_QUERY_GENERAL)
    {   // GQ should naever fail
        TCPIPStack_Assert(pQNode != NULL, __FILE__, __func__, __LINE__);
    }

    return pQNode;

}

// prepare a Gen Query report for which the tTransmit expired 
static size_t F_IGMP_GenerateGenQueryReport(TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGq)
{
    OA_HASH_DCPT *gHashDcpt;
    TCPIP_IGMP_GROUP_ENTRY* pGEntry;
    TCPIP_IGMP_GIF_STATE_DCPT currStateDcpt, *pDcpt;
    TCPIP_IGMP_QUERY_SOURCES* pQSource;
    size_t groupIx, nEntries;

    // mark all qSources as invalid!
    TCPIPStack_Assert(pGq->nQSources == sizeof(pGq->qSources) / sizeof(*pGq->qSources), __FILE__, __func__, __LINE__);
    (void)memset(pGq->qSources, 0, pGq->nQSources * sizeof(*pGq->qSources));

    igmpDcptLock();
    gHashDcpt = &igmpGroupsDcpt.gHashDcpt;
    nEntries = 0;
    pQSource = pGq->qSources;
    for(groupIx = 0; groupIx < gHashDcpt->hEntries; groupIx++)
    {
        pGEntry = FC_HashE2GrpE(TCPIP_OAHASH_EntryGet(gHashDcpt, groupIx));
        if(pGEntry->hEntry.flags.busy != 0U)
        {   // found a valid group entry; get state snapshot
            pDcpt = &currStateDcpt;
            F_IGMP_GetGifState(pGEntry, pGq->ifIx, pDcpt);

            if(pDcpt->ifFilter == TCPIP_IGMP_FILTER_INCLUDE && pDcpt->groupSources.nSources == 0U)
            {   // empty sources
                continue;
            }
            else
            {   // found valid entry
                pQSource->repGroup = pGEntry->gAddress.Val;
                pQSource->repType = pDcpt->ifFilter == TCPIP_IGMP_FILTER_INCLUDE ? TCPIP_IGMP_MODE_IS_INCLUDE : TCPIP_IGMP_MODE_IS_EXCLUDE;
                (void)memcpy(&pQSource->repSources, &pDcpt->groupSources, sizeof(pQSource->repSources));
                nEntries++;
                pQSource++;
                TCPIPStack_Assert(nEntries <= sizeof(pGq->qSources) / sizeof(*pGq->qSources), __FILE__, __func__, __LINE__);
            }
        }
    }
    igmpDcptUnlock();

    return nEntries;
}

// prepare a Group Query report for which the tTransmit expired 
// returns number of records that need to be sent, 0 if none 
static bool F_IGMP_GenerateGroupQueryReport(TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pGrpNode)
{
    OA_HASH_DCPT *gHashDcpt;
    TCPIP_IGMP_GROUP_ENTRY* pGEntry;
    TCPIP_IGMP_GIF_STATE_DCPT currStateDcpt, *pDcpt;
    TCPIP_IGMP_QUERY_SOURCES* pQSource;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES resultSources;
    IPV4_ADDR groupAddress;
    bool genReport;

    TCPIPStack_Assert(pGrpNode->nQSources == sizeof(pGrpNode->qSources) / sizeof(*pGrpNode->qSources), __FILE__, __func__, __LINE__);

    gHashDcpt = &igmpGroupsDcpt.gHashDcpt;
    groupAddress.Val = pGrpNode->qSources[0].repGroup;
    genReport = false;

    // pGrpNode holds the router requested sources!

    igmpDcptLock();
    pGEntry = FC_HashE2GrpE(TCPIP_OAHASH_EntryLookup(gHashDcpt, &groupAddress));
    if(pGEntry != NULL && pGEntry->hEntry.flags.busy != 0U)
    {   // found a valid group entry; get state snapshot
        pDcpt = &currStateDcpt;
        F_IGMP_GetGifState(pGEntry, pGrpNode->ifIx, pDcpt);

        if(pDcpt->ifFilter != TCPIP_IGMP_FILTER_INCLUDE || pDcpt->groupSources.nSources != 0U)
        {   // reception state for this group: valid non empty sources
            pQSource = pGrpNode->qSources;
            pQSource->repGroup = groupAddress.Val;
            if(pGrpNode->qSources[0].repSources.nSources == 0U)
            {   // A Group Specific report is needed
                pQSource->repType = pDcpt->ifFilter == TCPIP_IGMP_FILTER_INCLUDE ? TCPIP_IGMP_MODE_IS_INCLUDE : TCPIP_IGMP_MODE_IS_EXCLUDE;
                (void)memcpy(&pQSource->repSources, &pDcpt->groupSources, sizeof(pQSource->repSources));
                genReport = true;
            }
            else
            {   // A Group and source specific report is needed
                pQSource->repType = TCPIP_IGMP_MODE_IS_INCLUDE;
                if(pDcpt->ifFilter == TCPIP_IGMP_FILTER_INCLUDE)
                {
                    F_IGMP_IntersectSources(&pGrpNode->qSources[0].repSources, &currStateDcpt.groupSources, &resultSources);
                }
                else
                {
                    F_IGMP_SubtractSources(&pGrpNode->qSources[0].repSources, &currStateDcpt.groupSources, &resultSources);
                }

                if(resultSources.nSources != 0U)
                {   // need to send something
                    (void)memcpy(&pQSource->repSources, &resultSources, sizeof(pQSource->repSources));
                    genReport = true;
                }
            }
        }
    }
    igmpDcptUnlock();

    return genReport;
}


// sets up a Gen Query node and inserts it in a Gen Query report list
static void F_IGMP_ScheduleGenQueryReport(TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGQNode, size_t ifIx, uint32_t tTransmit, bool insert)
{
    pGQNode->ifIx = (uint8_t)ifIx;
    pGQNode->queryType = (uint8_t)TCPIP_IGMP_QUERY_GENERAL;
    pGQNode->tTransmit = tTransmit;

    if(insert)
    {
        igmpListsLock();
        TCPIP_Helper_SingleListTailAdd(&igmpGenQueryReportList, FC_GqNode2SglNode(pGQNode));
        igmpListsUnlock();
    }
}

// initializes a Group Query node in a GS mode (no sources updated) 
// it does NOT insert in the report list!
static void F_IGMP_InitGroupQueryReport(TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pGNode, IPV4_ADDR groupAddress, size_t ifIx, uint32_t tTransmit)
{
    pGNode->ifIx = (uint8_t)ifIx;
    pGNode->queryType = (uint8_t)TCPIP_IGMP_QUERY_GROUP_SPEC;
    pGNode->tTransmit = tTransmit;
    pGNode->qSources[0].repType = TCPIP_IGMP_RECORD_NONE;    // updated at transmit time
    pGNode->qSources[0].repGroup = groupAddress.Val;
    pGNode->qSources[0].repSources.nSources = 0U;
}

// sets up a Group Query node with sources and inserts it in a Group Query report list
static void F_IGMP_SourceScheduleGroupQueryReport(TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pGNode, size_t nRecSources, uint32_t* pRecSources, bool insert)
{
    pGNode->queryType = nRecSources == 0U ? (uint8_t)TCPIP_IGMP_QUERY_GROUP_SPEC : (uint8_t)TCPIP_IGMP_QUERY_GROUP_SOURCE_SPEC;
    pGNode->qSources[0].repSources.nSources = nRecSources;

    size_t nAvlblSources = sizeof(pGNode->qSources[0].repSources.sourceAddresses) / sizeof(*pGNode->qSources[0].repSources.sourceAddresses);
    if(nRecSources != 0U)
    {
        if(nRecSources > nAvlblSources)
        {   // truncate
            IPV4_ADDR groupAddr;
            groupAddr.Val = pGNode->qSources[0].repGroup;
            F_IGMP_ReportEvent(groupAddr, TCPIP_IGMP_EVENT_QUERY_SOURCE_NUMBER_EXCEEDED);
            nRecSources = nAvlblSources;
        }
        // store the sources for report
        (void)memcpy(pGNode->qSources[0].repSources.sourceAddresses, pRecSources, nRecSources * sizeof(*pRecSources));
    }

    if(insert)
    {
        igmpListsLock();
        TCPIP_Helper_SingleListTailAdd(&igmpGroupQueryReportList, FC_GroupNode2SglNode(pGNode));
        igmpListsUnlock();
    }

}


#endif  // defined(TCPIP_STACK_USE_IGMP)
#endif  // defined(TCPIP_STACK_USE_IPV4)


