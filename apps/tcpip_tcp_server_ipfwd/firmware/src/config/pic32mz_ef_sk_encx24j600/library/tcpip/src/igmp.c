/*******************************************************************************
  IGMP host module implementation

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    - Provides the implementation for IGMPv3 host with SSM and ASM support

*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2016-2018 Microchip Technology Inc. and its subsidiaries.

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

static tcpipSignalHandle    igmpSignalHandle = 0;

static OSAL_MUTEX_HANDLE_TYPE igmpListsMutex;       // IGMP synchronization object: protection for access to the IGMP
                                                    // lists between user threads and manager thread
                                                    // mutex selected so priority inheritance kicks in when IGMP itself(manager)
                                                    // has to block

static OSAL_MUTEX_HANDLE_TYPE igmpDcptMutex;        // IGMP synchronization object: protection for access to the IGMP
                                                    // global descriptor between user threads and manager thread
                                                    // mutex selected so priority inheritance kicks in when IGMP itself(manager)
                                                    // has to block
static TCPIP_IGMP_GROUPS_DCPT igmpGroupsDcpt;       // IGMP groups descriptor

static int                  igmpInterfaces;         // number of interfaces
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

static uint8_t      igmpRobustnessVar = TCPIP_IGMP_ROBUSTNESS_VARIABLE; // robustness variable count

static uint32_t     igmpUnsolicitRepInterval;       // unsolicited report interval, ms

#if (TCPIP_IGMP_USER_NOTIFICATION != 0)
static PROTECTED_SINGLE_LIST   igmpRegisteredUsers;
static const void*             igmpMemH;

#endif  // (TCPIP_IGMP_USER_NOTIFICATION != 0)


// prototypes
//

static void         TCPIP_IGMP_Process(void);
static void         TCPIP_IGMP_Tick(void);
static void         TCPIP_IGMP_SourceChangeTimeout(void);
static void         TCPIP_IGMP_GenQueryTimeout(void);
static void         TCPIP_IGMP_GroupQueryTimeout(void);

static IPV4_PACKET* _IGMP_AllocateTxPacketStruct (uint16_t totIGMPLen);
static bool         _IGMP_TxPktAcknowledge(TCPIP_MAC_PACKET* pTxPkt, const void* ackParam);
static TCPIP_IGMP_RESULT _IGMP_GenerateStateReport(IPV4_ADDR mcastAddress, int ifIx, TCPIP_IGMP_GIF_STATE_DCPT* pOldDcpt, TCPIP_IGMP_GIF_STATE_DCPT* pNewDcpt);
static TCPIP_IGMP_RESULT _IGMP_ScheduleFmcReport(IPV4_ADDR groupAddress, TCPIP_IGMPv3_RECORD_TYPE repType, int ifIx, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pGroupSources);
static TCPIP_IGMP_RESULT _IGMP_ScheduleSlcReport(IPV4_ADDR groupAddress, int ifIx, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pNewAllow, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pNewBlock);

static TCPIP_IGMP_SC_REPORT_NODE* _IGMP_FindGifSCReportsDisable(IPV4_ADDR groupAddress, int ifIx, TCPIP_IGMP_SC_REPORT_NODE** ppAllowNode, TCPIP_IGMP_SC_REPORT_NODE** ppBlockNode);

static TCPIP_IGMP_SC_REPORT_NODE* _IGMP_GetNewScReport(void);
static void     _IGMP_InsertScReport(SINGLE_LIST* pList, TCPIP_IGMP_SC_REPORT_NODE* pRepNode);
static void     _IGMP_DeleteScReport(SINGLE_LIST* pList, TCPIP_IGMP_SC_REPORT_NODE* pRepNode);

static TCPIP_IGMP_SC_REPORT_NODE* _IGMP_FindScheduledScReport(IPV4_ADDR groupAddress, int ifIx, TCPIP_IGMP_SC_REPORT_NODE** ppAllowNode, TCPIP_IGMP_SC_REPORT_NODE** ppBlockNode, bool removeAllowBlock);

static TCPIP_IGMP_QUERY_REPORT_NODE* _IGMP_FindScheduledQueryReport(TCPIP_IGMP_QUERY_TYPE qType, uint32_t groupAddress, int ifIx, bool remove);

static void     _IGMP_SetGifSCRecord(TCPIP_IGMP_SC_REPORT_NODE* pRepNode, IPV4_ADDR groupAddress, TCPIP_IGMPv3_RECORD_TYPE repType, int ifIx, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pGroupSources);

static size_t   _IGMP_GroupKeyHash(OA_HASH_DCPT* pOH, const void* key);
static int      _IGMP_GroupKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key);
static void     _IGMP_GroupKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key);
#if defined(OA_DOUBLE_HASH_PROBING)
static size_t   _IGMP_GroupProbeHash(OA_HASH_DCPT* pOH, const void* key);
#endif  // defined(OA_DOUBLE_HASH_PROBING)
static size_t   _IGMP_SourceKeyHash(OA_HASH_DCPT* pOH, const void* key);
static int      _IGMP_SourceKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key);
static void     _IGMP_SourceKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key);
#if defined(OA_DOUBLE_HASH_PROBING)
static size_t   _IGMP_SourceProbeHash(OA_HASH_DCPT* pOH, const void* key);
#endif  // defined(OA_DOUBLE_HASH_PROBING)

static bool     _IGMP_IsMcastValid(IPV4_ADDR addr);

static bool     _IGMP_IsSsmAddress(IPV4_ADDR mcastAddress);

static bool     _IGMP_CheckIfIndex(int ifIx, bool itExists);

static TCPIP_IGMP_RESULT _IGMP_CheckSubscribeParams(TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, const IPV4_ADDR* sourceList, size_t* listSize, int* pIfIx);

static TCPIP_IGMP_RESULT _IGMP_SocketRemove(UDP_SOCKET socket, int ifIx, IPV4_ADDR mcastAddress, size_t* nRemoves);

static TCPIP_IGMP_RESULT    _IGMP_SocketUpdateSources(UDP_SOCKET socket, int ifIx, IPV4_ADDR mcastAddress, TCPIP_IGMP_FILTER_TYPE filterMode, const IPV4_ADDR* sourceList, size_t* listSize);

static OA_HASH_DCPT* _IGMP_GetSourceHashDcpt(TCPIP_IGMP_GROUP_ENTRY* pGEntry);

static void     _IGMP_SourceEntryInit(TCPIP_IGMP_SOURCE_ENTRY* pSEntry);

static TCPIP_IGMP_SKT_RECORD*   _IGMP_SourceFindSktRecord(TCPIP_IGMP_SOURCE_ENTRY* pSEntry, UDP_SOCKET socket, int ifIx, TCPIP_IGMP_FILTER_TYPE filtType);

static TCPIP_IGMP_SKT_RECORD*   _IGMP_SourceCreateSktRecord(TCPIP_IGMP_SOURCE_ENTRY* pSEntry, UDP_SOCKET socket, int ifIx, TCPIP_IGMP_FILTER_TYPE filtType);

static int      _IGMP_GroupPurgeSktByFilter(TCPIP_IGMP_GROUP_ENTRY* pGEntry, UDP_SOCKET socket, int ifIx, TCPIP_IGMP_FILTER_TYPE* pPurgeFilt);

static bool     _IGMP_GroupPurgeSktBySource(TCPIP_IGMP_GROUP_ENTRY* pGEntry, UDP_SOCKET socket, int ifIx, const IPV4_ADDR* pPurgeAdd);

static TCPIP_IGMP_SIF_FILTER_ACTION     _IGMP_GetSifFilter(const TCPIP_IGMP_SOURCE_ENTRY* pSEntry, int ifIx, TCPIP_IGMP_FILTER_TYPE* pFilter);

static void     _IGMP_GetGifState(TCPIP_IGMP_GROUP_ENTRY* pGEntry, int ifIx, TCPIP_IGMP_GIF_STATE_DCPT* pGifDcpt);

static void     _IGMP_SubtractSources(TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pA, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pB, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pDest);
static void     _IGMP_UnionSources(TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pA, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pB, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pDest);
static void     _IGMP_IntersectSources(TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pA, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pB, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pDest);

static int      _IGMPSourceEntryCompare(const void* p1, const void* p2);

static bool     _IGMP_SendScReport(int ifIx, SINGLE_LIST* pReportList);

static void     _IGMP_SetScReportTransmitTime(TCPIP_IGMP_SC_REPORT_NODE* pRNode);

static void     _IGMP_ReportEvent(IPV4_ADDR mcastAddress, TCPIP_IGMP_EVENT_TYPE evType);

static bool     _IGMP_GroupEntryCheckRemove(TCPIP_IGMP_GROUP_ENTRY* pGEntry);

static void     _IGMP_ProcessV3Query(TCPIP_IGMPv3_QUERY_MESSAGE* pQuery, int ifIx);

static TCPIP_IGMP_QUERY_REPORT_NODE* _IGMP_GetNewQueryReport(TCPIP_IGMP_QUERY_TYPE qType);

static void     _IGMP_ScheduleGenQueryReport(TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGQNode, int ifIx, uint32_t tTransmit, bool insert);

static int      _IGMP_GenerateGenQueryReport(TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGq);

static bool     _IGMP_SendQueryReport(TCPIP_IGMP_QUERY_REPORT_NODE* pQNode, int nEntries);

static void     _IGMP_InitGroupQueryReport(TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pGNode, IPV4_ADDR groupAddress, int ifIx, uint32_t tTransmit);

static void     _IGMP_SourceScheduleGroupQueryReport(TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pGNode, int nRecSources, uint32_t* pRecSources, bool insert);

static bool     _IGMP_GenerateGroupQueryReport(TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pGrpNode);

// implementation
//

#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_BASIC) != 0)
volatile int _IGMPStayAssertLoop = 0;
static void _IGMPAssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("IGMP Assert: %s, in line: %d, \r\n", message, lineNo);
        while(_IGMPStayAssertLoop != 0);
    }
}
// a debug condition, not really assertion
volatile int _IGMPStayCondLoop = 0; 
static void _IGMPDebugCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("IGMP Cond: %s, in line: %d, \r\n", message, lineNo);
        while(_IGMPStayCondLoop != 0);
    }
}

#else
#define _IGMPAssertCond(cond, message, lineNo)
#define _IGMPDebugCond(cond, message, lineNo)
#endif  // (TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_BASIC)

#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_EVENTS) != 0)
static const char* _IGMP_DbgEvNameTbl[] = 
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
};

static void _IGMP_DbgEvent(IPV4_ADDR mcastAddress, TCPIP_IGMP_EVENT_TYPE evType)
{
    int evIx = 1;
    char addBuff[20];
    char dbgBuff[100];

    while(evType)
    {
        if((evType & 0x1) != 0)
        {
            _IGMPAssertCond(evIx < sizeof(_IGMP_DbgEvNameTbl) / sizeof(*_IGMP_DbgEvNameTbl), __func__, __LINE__);
            sprintf(dbgBuff, "%s ", _IGMP_DbgEvNameTbl[evIx]);  
        }
        evIx++;
        evType >>= 1;
    }

    TCPIP_Helper_IPAddressToString(&mcastAddress, addBuff, sizeof(addBuff));
    SYS_CONSOLE_PRINT("IGMP Event: %s, add: %s\r\n", dbgBuff, addBuff);
}
#else
#define _IGMP_DbgEvent(mcastAddress, evType)
#endif  // ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_EVENTS) != 0)

#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_REPORT_LISTS) != 0)
static void _IGMP_ScListPrint(SINGLE_LIST* pScList);
static void _IGMP_QueryListPrint(SINGLE_LIST* pQList, TCPIP_IGMP_QUERY_TYPE qType);

void TCPIP_IGMP_ReportListPrint(TCPIP_IGMP_REPORT_LIST_TYPE listType)
{
    SINGLE_LIST *pFreeScList, *pScList;
    SINGLE_LIST *pFreeGqList, *pGqList;
    SINGLE_LIST *pFreeGrpList, *pGrpList;
    bool needFree, needBusy;
    int nLists = 0;

    pFreeScList = pScList = 0;
    pFreeGqList = pGqList = 0;
    pFreeGrpList = pGrpList = 0;

    needFree = (listType & TCPIP_IGMP_REPORT_LIST_FLAG_FREE) != 0;
    needBusy = (listType & TCPIP_IGMP_REPORT_LIST_FLAG_BUSY) != 0;


    // detect the list types
    if((listType & TCPIP_IGMP_REPORT_LIST_SC) != 0)
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
    if((listType & TCPIP_IGMP_REPORT_LIST_GEN_QUERY) != 0)
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
    if((listType & TCPIP_IGMP_REPORT_LIST_GROUP_QUERY) != 0)
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

    if(nLists == 0)
    {
        SYS_CONSOLE_MESSAGE("IGMP Lists: no such list\r\n");
        return;
    }

    if(pFreeScList)
    {
        SYS_CONSOLE_PRINT("IGMP SC Free List: %d nodes\r\n", TCPIP_Helper_SingleListCount(pFreeScList));
    }
    if(pScList)
    {
        _IGMP_ScListPrint(pScList);
    }

    if(pFreeGqList)
    {
        SYS_CONSOLE_PRINT("IGMP GQ Free List: %d nodes\r\n", TCPIP_Helper_SingleListCount(pFreeGqList));
    }
    if(pGqList)
    {
        _IGMP_QueryListPrint(pGqList, TCPIP_IGMP_QUERY_GENERAL);
    }
    
    if(pFreeGrpList)
    {
        SYS_CONSOLE_PRINT("IGMP GRPQ Free List: %d nodes\r\n", TCPIP_Helper_SingleListCount(pFreeGrpList));
    }
    if(pGrpList)
    {
        _IGMP_QueryListPrint(pGrpList, TCPIP_IGMP_QUERY_GROUP_SPEC);
    }

}

static const char* _IGMP_RECORD_TYPE_STR_TBL[] =
{
    "invalid",
    "mode_inc",
    "mode_exc",
    "to_inc",
    "to_exc",
    "allow_new",
    "block_old",
};

static void _IGMP_ScListPrint(SINGLE_LIST* pScList)
{
    int32_t toTransMs;
    TCPIP_IGMP_SC_REPORT_NODE* pRNode;
    IPV4_ADDR groupAddr, srcAddr;
    const char* repType;
    int srcIx;
    uint32_t* pSrc;
    char addBuff[20];

    for(pRNode = (TCPIP_IGMP_SC_REPORT_NODE*)pScList->head; pRNode != 0; pRNode = pRNode->next)
    {
        toTransMs =  ((int32_t)(pRNode->tTransmit - SYS_TMR_TickCountGet()) * 1000) / SYS_TMR_TickCounterFrequencyGet();

        SYS_CONSOLE_PRINT("IGMP SC Node - if: %d, nTransmit: %d, active: %d, to Transmit: %d\r\n", pRNode->ifIx, pRNode->nTransmit, pRNode->active, toTransMs);

        groupAddr.Val = pRNode->repGroup;
        TCPIP_Helper_IPAddressToString(&groupAddr, addBuff, sizeof(addBuff));
        repType = _IGMP_RECORD_TYPE_STR_TBL[pRNode->repType];
        SYS_CONSOLE_PRINT("IGMP SC - group: %s, type: %s, nSources\r\n", addBuff, repType, pRNode->repSources.nSources);

        pSrc = pRNode->repSources.sourceAddresses;
        for(srcIx = 0; srcIx < pRNode->repSources.nSources; srcIx++, pSrc++)
        {
            srcAddr.Val = *pSrc;
            TCPIP_Helper_IPAddressToString(&srcAddr, addBuff, sizeof(addBuff));

            SYS_CONSOLE_PRINT("IGMP SC Source - source: %s\r\n", addBuff);
        }
    }

}

static const char* _IGMP_QueryTypeStr(TCPIP_IGMP_QUERY_TYPE qType, bool generic)
{
    if(generic)
    {
        return qType == TCPIP_IGMP_QUERY_GENERAL ? "GQ" : (qType == TCPIP_IGMP_QUERY_GROUP_SPEC  || qType == TCPIP_IGMP_QUERY_GROUP_SOURCE_SPEC) ? "GRPQ" : "invalid"; 
    }

    return qType == TCPIP_IGMP_QUERY_GENERAL ? "GQ" : qType == TCPIP_IGMP_QUERY_GROUP_SPEC ? "GSQ" : qType == TCPIP_IGMP_QUERY_GROUP_SOURCE_SPEC ? "GSSQ" : "invalid"; 
}

static void _IGMP_QueryListPrint(SINGLE_LIST* pQList, TCPIP_IGMP_QUERY_TYPE qType)
{
    int qIx, srcIx;
    TCPIP_IGMP_QUERY_REPORT_NODE* pQNode;
    const char* strType;
    int32_t toTransMs;
    TCPIP_IGMP_QUERY_SOURCES* pQSource;
    IPV4_ADDR groupAddr, srcAddr;
    uint32_t* pSrc;
    char addBuff[20];


    strType = _IGMP_QueryTypeStr(qType, true);


    for(pQNode = (TCPIP_IGMP_QUERY_REPORT_NODE*)pQList->head; pQNode != 0; pQNode = pQNode->next)
    {
        strType = _IGMP_QueryTypeStr(pQNode->queryType, false);
        toTransMs =  ((int32_t)(pQNode->tTransmit - SYS_TMR_TickCountGet()) * 1000) / SYS_TMR_TickCounterFrequencyGet();

        SYS_CONSOLE_PRINT("IGMP Q Node - if: %d, type: %s, qSources: %d, to Transmit: %d\r\n", pQNode->ifIx, strType, pQNode->nQSources, toTransMs);
        pQSource = pQNode->qSources;
        for(qIx = 0; qIx < pQNode->nQSources; qIx++, pQSource++)
        {
            groupAddr.Val = pQSource->repGroup;
            TCPIP_Helper_IPAddressToString(&groupAddr, addBuff, sizeof(addBuff));
            strType = pQSource->repType == TCPIP_IGMP_MODE_IS_INCLUDE ? "inc" : pQSource->repType == TCPIP_IGMP_MODE_IS_EXCLUDE ? "exc" : "invalid";
            SYS_CONSOLE_PRINT("IGMP qSource - group: %s, type: %s, nSources: %d\r\n", addBuff, strType, pQSource->repSources.nSources);
            pSrc = pQSource->repSources.sourceAddresses;
            for(srcIx = 0; srcIx < pQSource->repSources.nSources; srcIx++, pSrc++)
            {
                srcAddr.Val = *pSrc;
                TCPIP_Helper_IPAddressToString(&srcAddr, addBuff, sizeof(addBuff));

                SYS_CONSOLE_PRINT("IGMP qSource - source: %s\r\n", addBuff);
            }
        }
    }


}
#endif  // ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_REPORT_LISTS) != 0)

#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_QUERY_MSG) != 0)
// respTime in seconds
TCPIP_IGMP_RESULT TCPIP_IGMP_SendQuery(uint32_t* pGroupAdd, uint32_t* sourceList, uint16_t nSources, uint16_t respTime, int ifIx, TCPIP_IGMP_ROUTE_FLAG rFlag)
{
    uint16_t pktSize;
    TCPIP_NET_IF*    pNetIf;
    TCPIP_IGMPv3_QUERY_MESSAGE* pQuery;
    uint32_t*   pDest;
    IPV4_PACKET* pTxPkt;
    TCPIP_IPV4_PACKET_PARAMS pktParams;
    int srcIx;
    uint16_t mant, exp;
    TCPIP_IGMP_RESP_CODE_FLOAT rCodeFloat;

    if((rFlag & (TCPIP_IGMP_ROUTE_INTERNAL | TCPIP_IGMP_ROUTE_EXTERNAL)) == 0)
    {
        return TCPIP_IGMP_ARG_ERROR;
    }

    if(pGroupAdd == 0 || *pGroupAdd == 0)
    {   // general query
        pGroupAdd = 0;
        nSources = 0;
        sourceList = 0;
    }
    else if(nSources != 0 && sourceList == 0)
    {
        return TCPIP_IGMP_ARG_ERROR;
    }

    pNetIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(ifIx);
    if(!TCPIP_STACK_NetIsLinked(pNetIf))
    {
        return TCPIP_IGMP_IF_ERROR;
    }

    // calculate the max resp code
    mant = respTime * 10;
    exp = 0;
    do
    {
        mant /= 2;
        exp++;
    }while(mant > 0x1f && exp < 3);

    if(mant < 0x10 || exp > 10)
    {   // cannot be represented
        return TCPIP_IGMP_ARG_ERROR;
    }
    rCodeFloat.mant = mant - 0x10;
    rCodeFloat.exp = exp - 3;
    rCodeFloat.unit = 1;

    // calculate packet size
    pktSize = sizeof(TCPIP_IGMPv3_QUERY_MESSAGE) + nSources * sizeof(*((TCPIP_IGMPv3_QUERY_MESSAGE*)0)->sources);

    pTxPkt = _IGMP_AllocateTxPacketStruct(pktSize);
    if(pTxPkt == 0)
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


    TCPIP_IPV4_PacketFormatTx(pTxPkt, IP_PROT_IGMP, pktSize, &pktParams);

    // populate the query message
    pQuery = (TCPIP_IGMPv3_QUERY_MESSAGE*)pTxPkt->macPkt.pTransportLayer;
    memset(pQuery, 0, sizeof(*pQuery));
    pQuery->type = TCPIP_IGMP_MESSAGE_QUERY;
    pQuery->maxRespCode = rCodeFloat.val; 
    pQuery->groupAddress = pGroupAdd ? *pGroupAdd : 0;
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

    if((rFlag & TCPIP_IGMP_ROUTE_EXTERNAL) != 0)
    {
        if((rFlag & TCPIP_IGMP_ROUTE_INTERNAL) != 0)
        {   // route both int and ext
            pTxPkt->macPkt.modPktData = 1;
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
    pTxPkt->macPkt.modPktData = 0;
    pTxPkt->macPkt.pktFlags |= TCPIP_MAC_PKT_FLAG_MCAST;
    _TCPIPStackInsertRxPacket(pNetIf, &pTxPkt->macPkt, true);

    return TCPIP_IGMP_OK;
}

#endif // ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_QUERY_MSG) != 0)

#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
static uint32_t igmpLLockCount = 0;
#endif
// locks access to the IGMP report lists 
// between user threads and manager/dispatcher thread
static void igmpListsLock(void)
{
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
    _IGMPAssertCond(igmpLLockCount == 0, __func__, __LINE__);
#endif
    OSAL_MUTEX_Lock(&igmpListsMutex, OSAL_WAIT_FOREVER);
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
    igmpLLockCount++;
#endif
}

// unlocks access to IGMP lists and descriptor
static void igmpListsUnlock(void)
{
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
    _IGMPAssertCond(igmpLLockCount == 1, __func__, __LINE__);
#endif
    OSAL_MUTEX_Unlock(&igmpListsMutex);
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
    igmpLLockCount--;
#endif
}

// locks access to the IGMP global descriptor 
// between user threads and manager/dispatcher thread
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
static uint32_t igmpDLockCount = 0;
#endif
static __inline__ void __attribute__((always_inline)) igmpDcptLock(void)
{
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
    _IGMPAssertCond(igmpDLockCount == 0, __func__, __LINE__);
#endif
    OSAL_MUTEX_Lock(&igmpDcptMutex, OSAL_WAIT_FOREVER);
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
    igmpDLockCount++;
#endif
}
// unlocks access to IGMP lists and descriptor
static __inline__ void __attribute__((always_inline)) igmpDcptUnlock(void)
{
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
    _IGMPAssertCond(igmpDLockCount == 1, __func__, __LINE__);
#endif
    OSAL_MUTEX_Unlock(&igmpDcptMutex);
#if ((TCPIP_IGMP_DEBUG_LEVEL & TCPIP_IGMP_DEBUG_MASK_THREAD_LOCK) != 0)
    igmpDLockCount--;
#endif
}

// checks that an IPv4 address is multicast
static __inline__ bool __attribute__((always_inline)) _IGMP_IsMcastAddress(IPV4_ADDR addr)
{
    return ((addr.v[0] & 0xf0) == 0xe0);
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void _IGMPCleanup(void)
{

    if(igmpSignalHandle)
    {
        _TCPIPStackSignalHandlerDeregister(igmpSignalHandle);
        igmpSignalHandle = 0;
    }

    // Remove IGMP register users
#if (TCPIP_IGMP_USER_NOTIFICATION != 0)
    TCPIP_Notification_Deinitialize(&igmpRegisteredUsers, igmpMemH);
    igmpMemH = 0;
#endif  // (TCPIP_IGMP_USER_NOTIFICATION != 0)

    OSAL_MUTEX_Delete(&igmpListsMutex);
    OSAL_MUTEX_Delete(&igmpDcptMutex);
}

void TCPIP_IGMP_Deinitialize(const TCPIP_STACK_MODULE_CTRL *const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    if(igmpInitCount > 0)
    {   // we're up and running
        // one way or another this interface is going down

        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--igmpInitCount == 0)
            {   // all closed
                // release resources
                _IGMPCleanup();
            }
        }
    }

}
#else
#define _IGMPCleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)


bool TCPIP_IGMP_Initialize(const TCPIP_STACK_MODULE_CTRL *const stackCtrl, const TCPIP_IGMP_MODULE_CONFIG* pIgmpCfg)
{
    TCPIP_IGMP_GROUP_ENTRY* pGEntry;
    OA_HASH_DCPT    *gHashDcpt, *sHashDcpt;
    TCPIP_IGMP_SC_REPORT_NODE* pRepNode;
    TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGQNode;
    TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pQNode;
    int groupIx, ix;

    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack init
    if(igmpInitCount == 0)
    {   // first time we're run
        // hush compiler
        _IGMPAssertCond(true, __func__, __LINE__);
        _IGMPDebugCond(true, __func__, __LINE__);

        // check configuration data is not missing
        if(pIgmpCfg == 0)
        {
            return false;
        }

        igmpInterfaces = pIgmpCfg->nInterfaces;
        if(igmpInterfaces > stackCtrl->nIfs)
        {
            igmpInterfaces = stackCtrl->nIfs;
        }

        if(igmpInterfaces > TCPIP_IGMP_INTERFACES)
        {
            return false;
        }

        // create synchronization object
        if(OSAL_MUTEX_Create(&igmpListsMutex) != OSAL_RESULT_TRUE)
        {
            return false;
        }
        if(OSAL_MUTEX_Create(&igmpDcptMutex) != OSAL_RESULT_TRUE)
        {
            OSAL_MUTEX_Delete(&igmpListsMutex);
            return false;
        }

#if (TCPIP_IGMP_USER_NOTIFICATION != 0)
        igmpMemH = stackCtrl->memH;
        if(TCPIP_Notification_Initialize(&igmpRegisteredUsers) == false)
        {
            _IGMPCleanup();
            return false;
        }
#endif  // (TCPIP_IGMP_USER_NOTIFICATION != 0)

        // create the IGMP signal + timer
        igmpSignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_IGMP_Task, TCPIP_IGMP_TASK_TICK_RATE);
        if(igmpSignalHandle == 0 )
        {   // cannot create the IGMP signal notification
            _IGMPCleanup();
            return false;
        }

        // non critical initialization
        // initialize the SSM descriptor
        // check the SSM address range
        IPV4_ADDR ssmLow, ssmHigh;
        bool ssmRangeFail = false;
        if(pIgmpCfg->lowSsmAddress == 0)
        {
            ssmLow.Val = TCPIP_IGMP_IANA_SSM_LOW;
        }
        else
        {
            if(!TCPIP_Helper_StringToIPAddress(pIgmpCfg->lowSsmAddress, &ssmLow) || !_IGMP_IsMcastAddress(ssmLow))
            {   // wrong SSM range
                ssmRangeFail = true;
            }
        }
        if(pIgmpCfg->highSsmAddress == 0)
        {
            ssmHigh.Val = TCPIP_IGMP_IANA_SSM_HIGH;
        }
        else
        {
            if(!TCPIP_Helper_StringToIPAddress(pIgmpCfg->highSsmAddress, &ssmHigh) || !_IGMP_IsMcastAddress(ssmHigh))
            {   // wrong SSM range
                ssmRangeFail = true;
            }
        }

        if(ssmRangeFail)
        {   // wrong SSM range
            _IGMPCleanup();
            return false;
        }

        igmpSsmAddLow = ssmLow.Val;
        igmpSsmAddHigh = ssmHigh.Val;
        memset(&igmpGroupsDcpt, 0, sizeof(igmpGroupsDcpt));

        // populate the Group hash entries
        gHashDcpt = &igmpGroupsDcpt.gHashDcpt;
        gHashDcpt->memBlk = igmpGroupsDcpt.gEntryTbl;
        gHashDcpt->hEntrySize = sizeof(igmpGroupsDcpt.gEntryTbl[0]);
        gHashDcpt->hEntries = sizeof(igmpGroupsDcpt.gEntryTbl) / sizeof(*igmpGroupsDcpt.gEntryTbl);
        gHashDcpt->probeStep = TCPIP_IGMP_GROUP_HASH_PROBE_STEP;

        gHashDcpt->hashF = _IGMP_GroupKeyHash;
        gHashDcpt->cmpF = _IGMP_GroupKeyCompare;
        gHashDcpt->cpyF = _IGMP_GroupKeyCopy;
#if defined(OA_DOUBLE_HASH_PROBING)
        gHashDcpt->probeHash = _IGMP_GroupProbeHash;
#endif  // defined(OA_DOUBLE_HASH_PROBING)
        TCPIP_OAHASH_Initialize(gHashDcpt);

        // populate the Source hash entries
        sHashDcpt = &igmpGroupsDcpt.srcHashDcpt;
        sHashDcpt->hEntrySize = sizeof(pGEntry->srcEntryTbl[0]);
        sHashDcpt->hEntries = sizeof(pGEntry->srcEntryTbl) / sizeof(*pGEntry->srcEntryTbl);
        sHashDcpt->probeStep = TCPIP_IGMP_SOURCE_HASH_PROBE_STEP;

        sHashDcpt->hashF = _IGMP_SourceKeyHash;
        sHashDcpt->cmpF = _IGMP_SourceKeyCompare;
        sHashDcpt->cpyF = _IGMP_SourceKeyCopy;
#if defined(OA_DOUBLE_HASH_PROBING)
        sHashDcpt->probeHash = _IGMP_SourceProbeHash;
#endif  // defined(OA_DOUBLE_HASH_PROBING)


        // initialize every group hash
        pGEntry = igmpGroupsDcpt.gEntryTbl; 
        for(groupIx = 0; groupIx < sizeof(igmpGroupsDcpt.gEntryTbl) / sizeof(*igmpGroupsDcpt.gEntryTbl); groupIx++, pGEntry++)
        {
            sHashDcpt->memBlk = pGEntry->srcEntryTbl;
            TCPIP_OAHASH_Initialize(sHashDcpt);
        }

        // initialize the report pool
        TCPIP_Helper_SingleListInitialize(&igmpScFreeList);
        TCPIP_Helper_SingleListInitialize(&igmpScReportList);
        TCPIP_Helper_SingleListInitialize(&igmpGenQueryFreeList);
        TCPIP_Helper_SingleListInitialize(&igmpGroupQueryFreeList);
        TCPIP_Helper_SingleListInitialize(&igmpGenQueryReportList);
        TCPIP_Helper_SingleListInitialize(&igmpGroupQueryReportList);
        pRepNode = igmpScReportPool;
        for(ix = 0; ix < sizeof(igmpScReportPool) / sizeof(*igmpScReportPool); ix++, pRepNode++)
        {
            TCPIP_Helper_SingleListTailAdd(&igmpScFreeList, (SGL_LIST_NODE*)pRepNode);
        }
        
        pGQNode = igmpGenQueryReportPool;
        for(ix = 0; ix < sizeof(igmpGenQueryReportPool) / sizeof(*igmpGenQueryReportPool); ix++, pGQNode++)
        {
            pGQNode->nQSources = sizeof(pGQNode->qSources) / sizeof(*pGQNode->qSources);
            TCPIP_Helper_SingleListTailAdd(&igmpGenQueryFreeList, (SGL_LIST_NODE*)pGQNode);
        }

        pQNode = igmpGroupQueryReportPool;
        for(ix = 0; ix < sizeof(igmpGroupQueryReportPool) / sizeof(*igmpGroupQueryReportPool); ix++, pQNode++)
        {
            pQNode->nQSources = sizeof(pQNode->qSources) / sizeof(*pQNode->qSources);
            TCPIP_Helper_SingleListTailAdd(&igmpGroupQueryFreeList, (SGL_LIST_NODE*)pQNode);
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

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if((sigPend & TCPIP_MODULE_SIGNAL_RX_PENDING) != 0)
    { //  RX signal occurred
        TCPIP_IGMP_Process();
    }

    if((sigPend & TCPIP_MODULE_SIGNAL_TMO) != 0)
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
    int                 ifIx;
    TCPIP_MAC_PKT_ACK_RES ackRes;

    // extract queued IGMP packets
    while((pRxPkt = _TCPIPStackModuleRxExtract(TCPIP_THIS_MODULE_ID)) != 0)
    {
        TCPIP_PKT_FlightLogRx(pRxPkt, TCPIP_THIS_MODULE_ID);
        while(true)
        {
            ifIx = TCPIP_STACK_NetIndexGet(pRxPkt->pktIf);
            if(!_IGMP_CheckIfIndex(ifIx, true))
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

            if(TCPIP_IPV4_PacketOptionGet(pRxPkt, TCPIP_IPV4_OPTION_ROUTER_ALERT, 0, 0) == 0)
            {   // ignore if router alert not set
                ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
                break;
            }

            igmpTotLength = pRxPkt->totTransportLen;    // length of the 1st segment (if fragmented)

            pIgmpHdr = (TCPIP_IGMP_HEADER*)pRxPkt->pTransportLayer;
            if(igmpTotLength < sizeof(*pIgmpHdr))
            {
                ackRes = TCPIP_MAC_PKT_ACK_STRUCT_ERR;
                break;
            }

            // Validate the checksum
            // The checksum data includes the precomputed checksum in the header
            // so a valid packet will always have a checksum of 0x0000
            checksum = TCPIP_Helper_PacketChecksum(pRxPkt, (uint8_t*)pIgmpHdr, igmpTotLength, 0);
            if(checksum != 0)
            {
                ackRes = TCPIP_MAC_PKT_ACK_CHKSUM_ERR;
                break;
            }


            // do some processing here based on type of the IGMP packet
            if(pIgmpHdr->type != TCPIP_IGMP_MESSAGE_QUERY)
            {
                ackRes = TCPIP_MAC_PKT_ACK_TYPE_ERR;
                break;
            }

            // get the query version
            // right now just v3 queries are processed
            if(igmpTotLength < sizeof(TCPIP_IGMPv3_QUERY_MESSAGE))
            {   // discard unknown IGMP version
                ackRes = TCPIP_MAC_PKT_ACK_TYPE_ERR;
                break;
            }

            _IGMP_ProcessV3Query((TCPIP_IGMPv3_QUERY_MESSAGE*)pIgmpHdr, ifIx);
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
    int ifIx;
    int nGroups;      // global count of groups pending report
    SINGLE_LIST toReportList, newScList, pendingList, filtersDoneList, releaseList;
    TCPIP_IGMP_SC_REPORT_NODE *pRNode, *pFilt, *pAllow, *pBlock;
    IPV4_ADDR groupAddress;
    bool sendRes;
    uint32_t currTick;

    // do the SC list timeout processing
    for(ifIx = 0; ifIx < igmpInterfaces; ifIx++)
    {   
        if((igmpGroupsDcpt.ifMask & (1 << ifIx)) == 0)
        {   // not active
            continue;
        }

        // active interface
        TCPIP_Helper_SingleListInitialize(&toReportList);
        TCPIP_Helper_SingleListInitialize(&newScList);
        TCPIP_Helper_SingleListInitialize(&pendingList);
        TCPIP_Helper_SingleListInitialize(&filtersDoneList);
        TCPIP_Helper_SingleListInitialize(&releaseList);
        nGroups = 0;

        currTick = SYS_TMR_TickCountGet();

        // check if there are reports to be sent
        igmpListsLock();
        while((pRNode = (TCPIP_IGMP_SC_REPORT_NODE*)TCPIP_Helper_SingleListHeadRemove(&igmpScReportList)) != 0)
        {
            if(pRNode->ifIx == ifIx && pRNode->active && (int32_t)(currTick - pRNode->tTransmit) >= 0)
            {   // found timeout node
                TCPIP_Helper_SingleListTailAdd(&toReportList, (SGL_LIST_NODE*)pRNode);
                nGroups++;
            }
            else
            {   // not this interface or not active yet
                TCPIP_Helper_SingleListTailAdd(&newScList, (SGL_LIST_NODE*)pRNode);
            }
        }
        igmpScReportList = newScList;
        igmpListsUnlock();

        if(nGroups)
        {
            sendRes = _IGMP_SendScReport(ifIx, &toReportList);

            if(sendRes == false)
            {   // failed to transmit; reappend to pending
                TCPIP_Helper_SingleListAppend(&pendingList, &toReportList);
            }
            else
            {   // report sent fine; process again the toReportList to check for retransmissions
                while((pRNode = (TCPIP_IGMP_SC_REPORT_NODE*)TCPIP_Helper_SingleListHeadRemove(&toReportList)) != 0)
                {   
                    if(--pRNode->nTransmit == 0)
                    {    // done with this report; 
                        if(pRNode->repType == TCPIP_IGMP_FILTER_TO_INCLUDE || pRNode->repType == TCPIP_IGMP_FILTER_TO_EXCLUDE)
                        {   // when a filter is done, some SLC reports may be pending!
                            TCPIP_Helper_SingleListTailAdd(&filtersDoneList, (SGL_LIST_NODE*)pRNode);
                        }
                        else
                        {   // simply release the report
                            TCPIP_Helper_SingleListTailAdd(&releaseList, (SGL_LIST_NODE*)pRNode);
                        }
                    }
                    else
                    {    // still transmissions to go
                        _IGMP_SetScReportTransmitTime(pRNode);
                        TCPIP_Helper_SingleListTailAdd(&pendingList, (SGL_LIST_NODE*)pRNode);
                    }
                }
            }
        }
 
        // restore the SC busy list
        igmpListsLock();
        TCPIP_Helper_SingleListAppend(&igmpScReportList, &pendingList);

        // process the filters done list
        while((pRNode = (TCPIP_IGMP_SC_REPORT_NODE*)TCPIP_Helper_SingleListHeadRemove(&filtersDoneList)) != 0)
        {
            // find a pending SLC report for this group
            groupAddress.Val = pRNode->repGroup;
            pFilt = _IGMP_FindScheduledScReport(groupAddress, ifIx, &pAllow, &pBlock, false);
            (void)pFilt;
            _IGMPAssertCond(pFilt == 0, __func__, __LINE__);
            if(pAllow)
            {
                pAllow->active = 1;
            }
            if(pBlock)
            {
                pBlock->active = 1;
            }
            TCPIP_Helper_SingleListTailAdd(&releaseList, (SGL_LIST_NODE*)pRNode);
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
    int nGenEntries;
    uint32_t currTick;
    bool sendRes;


    TCPIP_Helper_SingleListInitialize(&newGQList);
    TCPIP_Helper_SingleListInitialize(&toTxList);
    TCPIP_Helper_SingleListInitialize(&releaseList);
    TCPIP_Helper_SingleListInitialize(&failSendList);
    currTick = SYS_TMR_TickCountGet();

    // quick list traverse: avoid keeping the lock for too long
    igmpListsLock();
    while((pGQNode = (TCPIP_IGMP_GEN_QUERY_REPORT_NODE*)TCPIP_Helper_SingleListHeadRemove(&igmpGenQueryReportList)) != 0)
    {
        _IGMPAssertCond(pGQNode->queryType == TCPIP_IGMP_QUERY_GENERAL, __func__, __LINE__);
        if((int32_t)(currTick - pGQNode->tTransmit) >= 0)
        {   // need to transmit
            TCPIP_Helper_SingleListTailAdd(&toTxList, (SGL_LIST_NODE*)pGQNode);
        }
        else
        {
            TCPIP_Helper_SingleListTailAdd(&newGQList, (SGL_LIST_NODE*)pGQNode);
        }
    }
    igmpGenQueryReportList = newGQList;
    igmpListsUnlock();

    // process the transmit list
    while((pGQNode = (TCPIP_IGMP_GEN_QUERY_REPORT_NODE*)TCPIP_Helper_SingleListHeadRemove(&toTxList)) != 0)
    {
        nGenEntries = _IGMP_GenerateGenQueryReport(pGQNode);
        if(nGenEntries)
        {   // OK, we've got something to transmit
            sendRes = _IGMP_SendQueryReport((TCPIP_IGMP_QUERY_REPORT_NODE*)pGQNode, nGenEntries);
        }
        else
        {
            sendRes = true;
        }

        if(sendRes == true)
        {   // transmitted OK
            TCPIP_Helper_SingleListTailAdd(&releaseList, (SGL_LIST_NODE*)pGQNode);
        }
        else
        {
            TCPIP_Helper_SingleListTailAdd(&failSendList, (SGL_LIST_NODE*)pGQNode);
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
    while((pQNode = (TCPIP_IGMP_GROUP_QUERY_REPORT_NODE*)TCPIP_Helper_SingleListHeadRemove(&igmpGroupQueryReportList)) != 0)
    {
        _IGMPAssertCond(pQNode->queryType == TCPIP_IGMP_QUERY_GROUP_SPEC || pQNode->queryType == TCPIP_IGMP_QUERY_GROUP_SOURCE_SPEC, __func__, __LINE__);

        if((int32_t)(currTick - pQNode->tTransmit) >= 0)
        {   // need to transmit this node
            TCPIP_Helper_SingleListTailAdd(&toTxList, (SGL_LIST_NODE*)pQNode);
        }
        else
        {   // not ready yet
            TCPIP_Helper_SingleListTailAdd(&newQueryList, (SGL_LIST_NODE*)pQNode);
        }
    }
    igmpGroupQueryReportList = newQueryList;
    igmpListsUnlock();

    // process the transmit list

    while((pQNode = (TCPIP_IGMP_GROUP_QUERY_REPORT_NODE*)TCPIP_Helper_SingleListHeadRemove(&toTxList)) != 0)
    {
        if(_IGMP_GenerateGroupQueryReport(pQNode))
        {   // OK, report generated; we've got something to transmit
            sendRes = _IGMP_SendQueryReport((TCPIP_IGMP_QUERY_REPORT_NODE*)pQNode, 1);
        }
        else
        {   // nothing to transmit
            sendRes = true;
        }

        if(sendRes == true)
        {   // transmitted OK
            TCPIP_Helper_SingleListTailAdd(&releaseList, (SGL_LIST_NODE*)pQNode);
        }
        else
        {
            TCPIP_Helper_SingleListTailAdd(&failSendList, (SGL_LIST_NODE*)pQNode);
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
static bool _IGMP_SendScReport(int ifIx, SINGLE_LIST* pReportList)
{
    uint16_t pktSize, groupSize, sourcesBytes, linkMtu, pktOvrhead;
    TCPIP_IGMPv3_GROUP_RECORD* pGroupRec;
    TCPIP_IGMPv3_REPORT_MESSAGE* pIgmpReport;
    TCPIP_IGMP_SC_REPORT_NODE *pRNode;
    IPV4_ADDR evGroup;
    TCPIP_NET_IF*    pNetIf;
    TCPIP_IPV4_PACKET_PARAMS pktParams;
    int nGroups, nSources, grpIx;

    TCPIP_IGMP_EVENT_TYPE evType = TCPIP_IGMP_EVENT_NONE;
    bool res = false;
    IPV4_PACKET* pTxPkt = 0;
    evGroup.Val = 0;

    pNetIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(ifIx);
    linkMtu = _TCPIPStackNetLinkMtu(pNetIf);
    pktOvrhead = sizeof(TCPIP_IGMP_HEADER) + sizeof(IPV4_HEADER);

    // calculate the packet size
    pktSize = sizeof(TCPIP_IGMPv3_REPORT_MESSAGE);
    nGroups = nSources = 0;
    for(pRNode = (TCPIP_IGMP_SC_REPORT_NODE*)pReportList->head; pRNode != 0; pRNode = pRNode->next)
    {
        groupSize = sizeof(TCPIP_IGMPv3_GROUP_RECORD) + pRNode->repSources.nSources * sizeof(*pGroupRec->sourceAddress);
        if(pktOvrhead + pktSize + groupSize > linkMtu)
        {   // exceeded MTU; truncate
            _IGMPDebugCond(true, __func__, __LINE__);
            evType |= TCPIP_IGMP_EVENT_PACKET_EXCEED_MTU;
            break;
        }

        nGroups++;
        nSources += pRNode->repSources.nSources;
        pktSize += groupSize;
    }
    

    while(nGroups)
    {
        pTxPkt = _IGMP_AllocateTxPacketStruct(pktSize);
        if(pTxPkt == 0)
        {   // failed to allocate
            evType |= TCPIP_IGMP_EVENT_PACKET_ALLOC_ERROR;
            break;
        }

        pTxPkt->netIfH = pNetIf;
        pTxPkt->srcAddress.Val = TCPIP_STACK_NetAddress(pTxPkt->netIfH);
        pTxPkt->destAddress.Val = TCPIP_IGMP_IANA_ALL_ROUTERS_MULTICAST;
        pTxPkt->macPkt.pDSeg->segLen += pktSize;
        pktParams.tosFlags = TCPIP_IPV4_PRECEDENCE_INET_CONTROL;
        pktParams.ttl = TCPIP_IGMP_PACKET_TTL;
        pktParams.df = 0;

        TCPIP_IPV4_PacketFormatTx(pTxPkt, IP_PROT_IGMP, pktSize, &pktParams);

        pIgmpReport = (TCPIP_IGMPv3_REPORT_MESSAGE*)pTxPkt->macPkt.pTransportLayer;
        memset(pIgmpReport, 0, sizeof(*pIgmpReport));
        pIgmpReport->type = TCPIP_IGMP_MESSAGE_V3_MEMBERSHIP; 
        pIgmpReport->checksum = 0;
        pIgmpReport->nGroupRecords = TCPIP_Helper_htons(nGroups);

        pGroupRec = pIgmpReport->groupRecords;
        pRNode = (TCPIP_IGMP_SC_REPORT_NODE*)pReportList->head;
        for(grpIx = 0; grpIx < nGroups; grpIx++, pRNode = pRNode->next)
        {
            memset(pGroupRec, 0, sizeof(*pGroupRec));
            pGroupRec->recordType = pRNode->repType;
            pGroupRec->nSources = TCPIP_Helper_htons(pRNode->repSources.nSources);
            pGroupRec->groupAddress = pRNode->repGroup;
            // set source addresses
            sourcesBytes = pRNode->repSources.nSources * sizeof(*pGroupRec->sourceAddress);
            if(sourcesBytes)
            {
                memcpy(pGroupRec->sourceAddress, pRNode->repSources.sourceAddresses, sourcesBytes);
            }
            pGroupRec = (TCPIP_IGMPv3_GROUP_RECORD*)((uint8_t*)pGroupRec + sizeof(*pGroupRec) + sourcesBytes);
        }

        // calculate checksum
        pIgmpReport->checksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pIgmpReport, pktSize, 0);

        TCPIP_PKT_FlightLogTx(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID);
        if(!TCPIP_IPV4_PacketTransmit(pTxPkt))
        {
            evType |= TCPIP_IGMP_EVENT_PACKET_TRANSMIT_ERROR;
            break;
        }

        res = true;
        break;
    }

    if(res == false)
    {   // failed somehow
        if(pTxPkt != 0)
        {
            TCPIP_PKT_FlightLogAcknowledge(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_IP_REJECT_ERR);
            TCPIP_PKT_PacketFree(&pTxPkt->macPkt);
        }
    }

    if(evType != TCPIP_IGMP_EVENT_NONE)
    {   // select head as representative
        evGroup.Val = ((TCPIP_IGMP_SC_REPORT_NODE*)pReportList->head)->repGroup;
        _IGMP_ReportEvent(evGroup, evType);
    }

    return res;
}

// sends a General Query report over the interface
static bool _IGMP_SendQueryReport(TCPIP_IGMP_QUERY_REPORT_NODE* pQNode, int nEntries)
{

    int qIx;
    int nSources, nRecords;
    TCPIP_IGMP_QUERY_SOURCES* pQuery;
    uint16_t pktSize, groupSize, sourcesBytes, linkMtu, pktOvrhead;
    TCPIP_IGMPv3_REPORT_MESSAGE* pIgmpReport;
    TCPIP_IGMPv3_GROUP_RECORD* pGroupRec;
    IPV4_ADDR evGroup;
    TCPIP_NET_IF*    pNetIf;
    TCPIP_IPV4_PACKET_PARAMS pktParams;

    TCPIP_IGMP_EVENT_TYPE evType = TCPIP_IGMP_EVENT_NONE;
    bool res = false;
    IPV4_PACKET* pTxPkt = 0;
    evGroup.Val = 0;

    if(pQNode->queryType == TCPIP_IGMP_QUERY_GENERAL)
    {
        _IGMPAssertCond(pQNode->nQSources == sizeof(((TCPIP_IGMP_GEN_QUERY_REPORT_NODE*)0)->qSources) / sizeof(*((TCPIP_IGMP_GEN_QUERY_REPORT_NODE*)0)->qSources), __func__, __LINE__);
    }
    else
    {
        _IGMPAssertCond(pQNode->nQSources == sizeof(((TCPIP_IGMP_GROUP_QUERY_REPORT_NODE*)0)->qSources) / sizeof(*((TCPIP_IGMP_GROUP_QUERY_REPORT_NODE*)0)->qSources), __func__, __LINE__);
    }

    pNetIf = (TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(pQNode->ifIx);
    linkMtu = _TCPIPStackNetLinkMtu(pNetIf);
    pktOvrhead = sizeof(TCPIP_IGMP_HEADER) + sizeof(IPV4_HEADER);

    // calculate the packet size
    pktSize = sizeof(TCPIP_IGMPv3_REPORT_MESSAGE);
    nRecords = 0;
    pQuery = pQNode->qSources;
    for(qIx = 0; qIx < pQNode->nQSources; qIx++, pQuery++)
    {
        if(pQuery->repType != TCPIP_IGMP_RECORD_NONE) 
        {   // valid query source
            groupSize = sizeof(TCPIP_IGMPv3_GROUP_RECORD) + pQuery->repSources.nSources * sizeof(pQuery->repSources.sourceAddresses[0]);
            if(pktOvrhead + pktSize + groupSize > linkMtu)
            {   // exceeded MTU; truncate;
                _IGMPDebugCond(true, __func__, __LINE__);
                evType |= TCPIP_IGMP_EVENT_PACKET_EXCEED_MTU;
                break;
            }

            pktSize += groupSize;
            nRecords++;
        }
    }

    while(nRecords)
    {
        pTxPkt = _IGMP_AllocateTxPacketStruct(pktSize);
        if(pTxPkt == 0)
        {   // failed to allocate
            evType |= TCPIP_IGMP_EVENT_PACKET_ALLOC_ERROR;
            break;
        }

        pTxPkt->netIfH = pNetIf;
        pTxPkt->srcAddress.Val = TCPIP_STACK_NetAddress(pTxPkt->netIfH);
        pTxPkt->destAddress.Val = TCPIP_IGMP_IANA_ALL_ROUTERS_MULTICAST;
        pTxPkt->macPkt.pDSeg->segLen += pktSize;
        pktParams.tosFlags = TCPIP_IPV4_PRECEDENCE_INET_CONTROL;
        pktParams.ttl = TCPIP_IGMP_PACKET_TTL;
        pktParams.df = 0;

        TCPIP_IPV4_PacketFormatTx(pTxPkt, IP_PROT_IGMP, pktSize, &pktParams);

        pIgmpReport = (TCPIP_IGMPv3_REPORT_MESSAGE*)pTxPkt->macPkt.pTransportLayer;
        memset(pIgmpReport, 0, sizeof(*pIgmpReport));
        pIgmpReport->type = TCPIP_IGMP_MESSAGE_V3_MEMBERSHIP; 
        pIgmpReport->checksum = 0;
        pIgmpReport->nGroupRecords = TCPIP_Helper_htons(nRecords);

        pGroupRec = pIgmpReport->groupRecords;
        pQuery = pQNode->qSources;
        for(qIx = 0; qIx < pQNode->nQSources; qIx++, pQuery++)
        {
            if(pQuery->repType != TCPIP_IGMP_RECORD_NONE) 
            {   // valid query source
                nSources = pQuery->repSources.nSources;
                memset(pGroupRec, 0, sizeof(*pGroupRec));
                pGroupRec->recordType = pQuery->repType;
                pGroupRec->nSources = TCPIP_Helper_htons(nSources);
                pGroupRec->groupAddress = pQuery->repGroup;

                // set source addresses
                sourcesBytes = nSources * sizeof(*pGroupRec->sourceAddress);
                if(sourcesBytes)
                {
                    memcpy(pGroupRec->sourceAddress, pQuery->repSources.sourceAddresses, sourcesBytes);
                }
                pGroupRec = (TCPIP_IGMPv3_GROUP_RECORD*)((uint8_t*)pGroupRec + sizeof(*pGroupRec) + sourcesBytes);
            }
        }

        // calculate checksum
        pIgmpReport->checksum = TCPIP_Helper_CalcIPChecksum((uint8_t*)pIgmpReport, pktSize, 0);

        TCPIP_PKT_FlightLogTx(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID);
        if(!TCPIP_IPV4_PacketTransmit(pTxPkt))
        {
            evType |= TCPIP_IGMP_EVENT_PACKET_TRANSMIT_ERROR;
            break;
        }

        res = true;
        break;
    }

    if(res == false)
    {   // failed somehow
        if(pTxPkt != 0)
        {
            TCPIP_PKT_FlightLogAcknowledge(&pTxPkt->macPkt, TCPIP_THIS_MODULE_ID, TCPIP_MAC_PKT_ACK_IP_REJECT_ERR);
            TCPIP_PKT_PacketFree(&pTxPkt->macPkt);
        }
    }

    if(evType != TCPIP_IGMP_EVENT_NONE)
    {   // select 1st qSource as representative
        evGroup.Val = pQNode->qSources->repGroup;
        _IGMP_ReportEvent(evGroup, evType);
    }

    return res;

}

/////////////////////////// API //////////////////////////////////////////
//

// all API operations need to access a lock/mutex!
// since they mess with the global hash descriptor and some user threads add, other delete from this global hash!!!!
TCPIP_IGMP_RESULT TCPIP_IGMP_Subscribe(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress,
  		              TCPIP_IGMP_FILTER_TYPE filterMode, const IPV4_ADDR* sourceList, size_t* listSize)
{
    int  ifIx;
    bool isSsm;
    IPV4_ADDR   allSourcesAdd;
    const IPV4_ADDR* pSources;
    TCPIP_IGMP_RESULT checkRes;
    size_t nSources;
    TCPIP_IGMP_RESULT res;

    // minimal sanity check
    checkRes = _IGMP_CheckSubscribeParams(hNet, mcastAddress, sourceList, listSize, &ifIx);
    if(checkRes != TCPIP_IGMP_OK)
    {
        return checkRes;
    }

    if(filterMode != TCPIP_IGMP_FILTER_INCLUDE && filterMode != TCPIP_IGMP_FILTER_EXCLUDE)
    {
        return TCPIP_IGMP_FILTER_ERROR;
    }

    isSsm = _IGMP_IsSsmAddress(mcastAddress);

    if(isSsm && filterMode == TCPIP_IGMP_FILTER_EXCLUDE)
    {   // exclude is invalid for SSM
        return TCPIP_IGMP_SSM_FILTER_ERROR;
    }

    igmpDcptLock();
    while(true)
    {
        if(!_IGMP_CheckIfIndex(ifIx, false))
        {
            res = TCPIP_IGMP_IF_ERROR;
            break;
        }

        nSources = listSize ? *listSize : 0;
        if(nSources == 0)
        {
            if(filterMode == TCPIP_IGMP_FILTER_INCLUDE)
            {   // include nothing means remove socket from (G, if)
                res = _IGMP_SocketRemove(socket, ifIx, mcastAddress, listSize); 
                break;
            }

            // exclude nothing i.e. include all is valid for ASM; use the special source address
            nSources = 1;
            allSourcesAdd.Val = TCPIP_IGMP_ASM_ALL_SOURCES; 
            pSources = &allSourcesAdd; 
        }
        else
        {
            pSources = sourceList;
        }

        // non empty list
        res = _IGMP_SocketUpdateSources(socket, ifIx, mcastAddress, filterMode, pSources, &nSources);
        break;
    }
    igmpDcptUnlock();

    return res;
}

TCPIP_IGMP_RESULT TCPIP_IGMP_SubscribeGet(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress,
  		              TCPIP_IGMP_FILTER_TYPE* filterMode, IPV4_ADDR* sourceList, size_t* listSize)
{
    int ifIx;
    TCPIP_IGMP_RESULT res;

    // minimal sanity check
    if(listSize == 0)
    {
        return TCPIP_IGMP_ARG_ERROR;
    }

    res = _IGMP_CheckSubscribeParams(hNet, mcastAddress, sourceList, listSize, &ifIx);
    if(res != TCPIP_IGMP_OK)
    {
        return res;
    }

    igmpDcptLock();
    while(true)
    {
        if(!_IGMP_CheckIfIndex(ifIx, true))
        {
            res = TCPIP_IGMP_IF_ERROR;
            break;
        }

        TCPIP_IGMP_GROUP_ENTRY* pGEntry;
        pGEntry = (TCPIP_IGMP_GROUP_ENTRY*)TCPIP_OAHASH_EntryLookup(&igmpGroupsDcpt.gHashDcpt, &mcastAddress);

        if(pGEntry == 0)
        {    // no such mcast exists
            res = TCPIP_IGMP_GROUP_INVALID;
            break;
        }

        OA_HASH_DCPT *sHashDcpt = _IGMP_GetSourceHashDcpt(pGEntry);
        int srcIx;
        TCPIP_IGMP_SOURCE_ENTRY* pSEntry;
        TCPIP_IGMP_SKT_RECORD* pRec;
        TCPIP_IGMP_FILTER_TYPE sktFilter = 0;
        size_t nSources = 0;
        size_t nSourceSlots = *listSize;

        for(srcIx = 0; srcIx < sHashDcpt->hEntries; srcIx++)
        {
            pSEntry = (TCPIP_IGMP_SOURCE_ENTRY*)TCPIP_OAHASH_EntryGet(sHashDcpt, srcIx);
            if(pSEntry->hEntry.flags.busy != 0)
            {   // found a valid source entry
                pRec = _IGMP_SourceFindSktRecord(pSEntry, socket, ifIx, 0);
                if(pRec)
                {
                    if(sktFilter == 0)
                    {
                        sktFilter = pRec->filter;
                    }
                    else
                    {   // there is only one filter mode per socket for one G!
                        _IGMPAssertCond(sktFilter == pRec->filter, __func__, __LINE__);
                    }
                    nSources++;
                    if(nSourceSlots)
                    {
                        *sourceList++ = pSEntry->srcAddress;
                        nSourceSlots--;
                    }
                }
            }
        }

        // report the number of available sources
        *listSize = nSources;

        if(filterMode)
        {
            *filterMode = sktFilter;
        }

        res = sktFilter == TCPIP_IGMP_FILTER_NONE ? TCPIP_IGMP_SOCKET_INVALID : TCPIP_IGMP_OK;
        break;
    }
    igmpDcptUnlock();

    return res;
}

TCPIP_IGMP_RESULT TCPIP_IGMP_Unsubscribe(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, const IPV4_ADDR* sourceList, size_t* listSize)
{
    int ix, ifIx, removeCnt;
    TCPIP_IGMP_GIF_STATE_DCPT oldStateDcpt, newStateDcpt;
    TCPIP_IGMP_GROUP_ENTRY* pGEntry;
    TCPIP_IGMP_RESULT repRes;
    size_t nSources;
    
    // minimal sanity check
    repRes = _IGMP_CheckSubscribeParams(hNet, mcastAddress, sourceList, listSize, &ifIx);

    if(repRes != TCPIP_IGMP_OK)
    {
        return repRes;
    }


    nSources = listSize ? *listSize : 0;

    if(nSources == 0)
    {   // unsubscribe ALL
        return TCPIP_IGMP_Subscribe(socket, hNet, mcastAddress, TCPIP_IGMP_FILTER_INCLUDE, 0, 0);
    }

    igmpDcptLock();
    while(true)
    {
        pGEntry = (TCPIP_IGMP_GROUP_ENTRY*)TCPIP_OAHASH_EntryLookup(&igmpGroupsDcpt.gHashDcpt, &mcastAddress);

        if(pGEntry == 0)
        {    // no such mcast exists
            repRes = TCPIP_IGMP_GROUP_INVALID;
            break;
        }

        if(!_IGMP_CheckIfIndex(ifIx, true))
        {
            repRes = TCPIP_IGMP_IF_ERROR;
            break;
        }

        // old state snapshot
        _IGMP_GetGifState(pGEntry, ifIx, &oldStateDcpt);

        removeCnt = 0;
        for(ix = 0; ix < nSources; ix++, sourceList++)
        {
            if(_IGMP_GroupPurgeSktBySource(pGEntry, socket, ifIx, sourceList))
            {
                removeCnt++;
            }
        }

        // get new state snapshot
        _IGMP_GetGifState(pGEntry, ifIx, &newStateDcpt);


        // removed some stuff; check if this entry is free
        _IGMP_GroupEntryCheckRemove(pGEntry);

        // generate report
        repRes = _IGMP_GenerateStateReport(mcastAddress, ifIx, &oldStateDcpt, &newStateDcpt);

        if(listSize)
        {
            *listSize = removeCnt;
        }
        break;
    }
    igmpDcptUnlock();

    return repRes; 
}

TCPIP_IGMP_RESULT TCPIP_IGMP_IncludeSource(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, IPV4_ADDR sourceAddress)
{
    size_t listSize = 1;
    return TCPIP_IGMP_Subscribe(socket, hNet, mcastAddress, TCPIP_IGMP_FILTER_INCLUDE, &sourceAddress, &listSize);
}

TCPIP_IGMP_RESULT TCPIP_IGMP_ExcludeSource(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, IPV4_ADDR sourceAddress)
{
    size_t listSize = 1;
    return TCPIP_IGMP_Subscribe(socket, hNet, mcastAddress, TCPIP_IGMP_FILTER_EXCLUDE, &sourceAddress, &listSize);
}

TCPIP_IGMP_RESULT TCPIP_IGMP_Join(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress)
{
    return TCPIP_IGMP_Subscribe(socket, hNet, mcastAddress, TCPIP_IGMP_FILTER_EXCLUDE, 0, 0);
}

TCPIP_IGMP_RESULT TCPIP_IGMP_Leave(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress)
{
    return TCPIP_IGMP_Subscribe(socket, hNet, mcastAddress, TCPIP_IGMP_FILTER_INCLUDE, 0, 0);
}

#if (TCPIP_IGMP_USER_NOTIFICATION != 0)
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
    return 0;
}

// deregister the event handler
bool TCPIP_IGMP_HandlerDeRegister(TCPIP_IGMP_HANDLE hIgmp)
{
    if(igmpMemH && hIgmp)
    {
        if(TCPIP_Notification_Remove((SGL_LIST_NODE*)hIgmp, &igmpRegisteredUsers, igmpMemH))
        {
            return true;
        }
    }
    return false;
}
#endif  // (TCPIP_IGMP_USER_NOTIFICATION != 0)


TCPIP_IGMP_RESULT TCPIP_IGMP_GroupsGet(IPV4_ADDR* groupsList, int listSize, int* pnGroups)
{
    OA_HASH_DCPT *gHashDcpt;
    TCPIP_IGMP_GROUP_ENTRY* pGEntry;
    int groupIx, nEntries;


    if(listSize != 0 && groupsList == 0)
    {
        return TCPIP_IGMP_ARG_ERROR;
    }

    igmpDcptLock();
    gHashDcpt = &igmpGroupsDcpt.gHashDcpt;
    nEntries = 0;
    for(groupIx = 0; groupIx < gHashDcpt->hEntries; groupIx++)
    {
        pGEntry = (TCPIP_IGMP_GROUP_ENTRY*)TCPIP_OAHASH_EntryGet(gHashDcpt, groupIx);
        if(pGEntry->hEntry.flags.busy != 0)
        {   // found a valid group entry
            if(nEntries < listSize)
            {
                *(groupsList + nEntries) = pGEntry->gAddress;
            }
            nEntries++;
        }
    }
    igmpDcptUnlock();
        
    if(pnGroups)
    {
        *pnGroups = nEntries;
    }

    return TCPIP_IGMP_OK;

}

TCPIP_IGMP_RESULT TCPIP_IGMP_GroupInfoGet(TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, TCPIP_IGMP_GROUP_INFO* pInfo)
{
    int  ifIx;
    TCPIP_IGMP_GROUP_ENTRY* pGEntry;
    TCPIP_IGMP_GIF_STATE_DCPT currStateDcpt;
    TCPIP_IGMP_RESULT res;

    if(pInfo == 0 || (pInfo->listSize != 0 && pInfo->sourceList == 0))
    {
        return TCPIP_IGMP_ARG_ERROR;
    }

    pInfo->presentSources = 0;
    pInfo->filterType = TCPIP_IGMP_FILTER_NONE;

    if(hNet == 0)
    {
        hNet = TCPIP_STACK_NetMulticastGet();
    }

    ifIx = TCPIP_STACK_NetIndexGet(hNet);
    if(ifIx < 0)
    {  
        return TCPIP_IGMP_IF_ERROR;
    }

    igmpDcptLock();
    while(true)
    {
        pGEntry = (TCPIP_IGMP_GROUP_ENTRY*)TCPIP_OAHASH_EntryLookup(&igmpGroupsDcpt.gHashDcpt, &mcastAddress);

        if(pGEntry == 0)
        {    // no entry
            res = TCPIP_IGMP_GROUP_INVALID; 
            break;
        }

        _IGMP_GetGifState(pGEntry, ifIx, &currStateDcpt);

        pInfo->presentSources = currStateDcpt.groupSources.nSources;
        pInfo->filterType = currStateDcpt.ifFilter;

        if(pInfo->listSize)
        {
            int nCopy = pInfo->presentSources > pInfo->listSize ? pInfo->listSize : pInfo->presentSources;
            memcpy(pInfo->sourceList, currStateDcpt.groupSources.sourceAddresses, nCopy * sizeof(*pInfo->sourceList));
        }

        res = TCPIP_IGMP_OK;
        break;
    }
    igmpDcptUnlock();

    return res;
}

// check that the socket is allowed to receive the multicast traffic
bool TCPIP_IGMP_IsMcastEnabled(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, IPV4_ADDR sourceAddress)
{
    int ifIx;
    OA_HASH_DCPT *sHashDcpt;
    TCPIP_IGMP_GROUP_ENTRY* pGEntry;
    TCPIP_IGMP_SOURCE_ENTRY* pSEntry;
    TCPIP_IGMP_SKT_RECORD* pRec;
    IPV4_ADDR exclNone;
    bool res = false;

    if(!_IGMP_IsSsmAddress(mcastAddress))
    {
        return true;
    }

    igmpDcptLock();
    while(true)
    {
        // search group
        pGEntry = (TCPIP_IGMP_GROUP_ENTRY*)TCPIP_OAHASH_EntryLookup(&igmpGroupsDcpt.gHashDcpt, &mcastAddress);

        if(pGEntry == 0)
        {    // no such mcast exists
            break;
        }

        ifIx = TCPIP_STACK_NetIndexGet(hNet);

        // search source
        // get the source hash for this G Entry
        sHashDcpt = _IGMP_GetSourceHashDcpt(pGEntry);
        pSEntry = (TCPIP_IGMP_SOURCE_ENTRY*)TCPIP_OAHASH_EntryLookup(sHashDcpt, &sourceAddress);
        if(pSEntry != 0)
        {
            pRec = _IGMP_SourceFindSktRecord(pSEntry, socket, ifIx, 0);
            if(pRec)
            {   // found socket in this entry;
                res = (pRec->filter == TCPIP_IGMP_FILTER_INCLUDE);
            }
        }
        else
        {   // we need to search the TCPIP_IGMP_ASM_ALL_SOURCES just in case the socket used exclude {}
            exclNone.Val = TCPIP_IGMP_ASM_ALL_SOURCES;
            pSEntry = (TCPIP_IGMP_SOURCE_ENTRY*)TCPIP_OAHASH_EntryLookup(sHashDcpt, &exclNone);
            if(pSEntry != 0)
            {
                pRec = _IGMP_SourceFindSktRecord(pSEntry, socket, ifIx, 0);
                if(pRec)
                {   // found socket for exclude none/include all
                    _IGMPAssertCond(pRec->filter == TCPIP_IGMP_FILTER_EXCLUDE, __func__, __LINE__);
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
static void _IGMP_ReportEvent(IPV4_ADDR mcastAddress, TCPIP_IGMP_EVENT_TYPE evType)
{
    _IGMP_DbgEvent(mcastAddress, evType);

#if (TCPIP_IGMP_USER_NOTIFICATION != 0)
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
#endif  // (TCPIP_IGMP_USER_NOTIFICATION != 0)
}




// packet deallocation function
// packet was transmitted by the IP layer
static bool _IGMP_TxPktAcknowledge(TCPIP_MAC_PACKET* pTxPkt, const void* ackParam)
{
    if(pTxPkt->modPktData != 0)
    {   // redirect internally. once!
        pTxPkt->modPktData = 0;
        pTxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_MCAST;
        TCPIP_PKT_FlightLogTx(pTxPkt, TCPIP_THIS_MODULE_ID);
        _TCPIPStackInsertRxPacket((TCPIP_NET_IF*)pTxPkt->pktIf, pTxPkt, true);
    }
    else
    {
        TCPIP_PKT_PacketFree(pTxPkt);
    }

    return false;
}

static IPV4_PACKET * _IGMP_AllocateTxPacketStruct (uint16_t totIGMPLen)
{
    IPV4_PACKET *pPkt;
    TCPIP_IPV4_OPTION_DCPT optionDcpt[1];

    optionDcpt[0].optionType = TCPIP_IPV4_OPTION_ROUTER_ALERT;
    optionDcpt[0].optionSize = 0;


    pPkt = TCPIP_IPV4_PacketAlloc(totIGMPLen, optionDcpt, 1, TCPIP_MAC_PKT_FLAG_IGMP);

    if (pPkt != 0)
    {
        TCPIP_PKT_PacketAcknowledgeSet(&pPkt->macPkt, _IGMP_TxPktAcknowledge, 0);
    }

    return pPkt;
}

static size_t _IGMP_GroupKeyHash(OA_HASH_DCPT* pOH, const void* key)
{
    // for a Group Entry the key is the gAddress IP address
    return fnv_32_hash(key, sizeof(((TCPIP_IGMP_GROUP_ENTRY*)0)->gAddress)) % (pOH->hEntries);
}

static int _IGMP_GroupKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    return ((TCPIP_IGMP_GROUP_ENTRY*)hEntry)->gAddress.Val != ((const IPV4_ADDR*)key)->Val; 
}

static void _IGMP_GroupKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    ((TCPIP_IGMP_GROUP_ENTRY*)dstEntry)->gAddress.Val = ((const IPV4_ADDR*)key)->Val;
}



#if defined(OA_DOUBLE_HASH_PROBING)
static size_t _IGMP_GroupProbeHash(OA_HASH_DCPT* pOH, const void* key)
{
    // the key is the gAddress IP address
    return fnv_32a_hash(key, sizeof(((TCPIP_IGMP_GROUP_ENTRY*)0)->gAddress)) % (pOH->hEntries);
}
#endif  // defined(OA_DOUBLE_HASH_PROBING)

static size_t _IGMP_SourceKeyHash(OA_HASH_DCPT* pOH, const void* key)
{
    // for a Source Entry the key is the srcAddress IP address
    return fnv_32_hash(key, sizeof(((TCPIP_IGMP_SOURCE_ENTRY*)0)->srcAddress)) % (pOH->hEntries);
}

static int _IGMP_SourceKeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key)
{
    return ((TCPIP_IGMP_SOURCE_ENTRY*)hEntry)->srcAddress.Val != ((const IPV4_ADDR*)key)->Val; 
}

static void _IGMP_SourceKeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* key)
{
    ((TCPIP_IGMP_SOURCE_ENTRY*)dstEntry)->srcAddress.Val = ((const IPV4_ADDR*)key)->Val;
}

#if defined(OA_DOUBLE_HASH_PROBING)
static size_t _IGMP_SourceProbeHash(OA_HASH_DCPT* pOH, const void* key)
{
    // the key is the srcAddress IP address
    return fnv_32a_hash(key, sizeof(((TCPIP_IGMP_SOURCE_ENTRY*)0)->srcAddress)) % (pOH->hEntries);
}
#endif  // defined(OA_DOUBLE_HASH_PROBING)


static TCPIP_IGMP_RESULT _IGMP_SocketRemove(UDP_SOCKET socket, int ifIx, IPV4_ADDR mcastAddress, size_t* nRemoves)
{
    int  removeCnt;
    TCPIP_IGMP_GROUP_ENTRY* pGEntry;
    TCPIP_IGMP_FILTER_TYPE purgeFilter;
    TCPIP_IGMP_GIF_STATE_DCPT oldStateDcpt, newStateDcpt;

    pGEntry = (TCPIP_IGMP_GROUP_ENTRY*)TCPIP_OAHASH_EntryLookup(&igmpGroupsDcpt.gHashDcpt, &mcastAddress);

    if(pGEntry == 0)
    {    // no such mcast exists
        return TCPIP_IGMP_GROUP_INVALID;
    }

    // get old state
    _IGMP_GetGifState(pGEntry, ifIx, &oldStateDcpt);
    
    purgeFilter = 0;    // remove every instance of the socket
    removeCnt = _IGMP_GroupPurgeSktByFilter(pGEntry, socket, ifIx, &purgeFilter);

    if(removeCnt == 0)
    {   // nothing changed
        return TCPIP_IGMP_SOCKET_INVALID;
    }

    if(nRemoves)
    {
        *nRemoves = removeCnt;
    }

    // get new state
    _IGMP_GetGifState(pGEntry, ifIx, &newStateDcpt);

    // rmoved some stuff; check if this entry is free
    _IGMP_GroupEntryCheckRemove(pGEntry);

    return _IGMP_GenerateStateReport(mcastAddress, ifIx, &oldStateDcpt, &newStateDcpt);

}

static TCPIP_IGMP_RESULT _IGMP_GenerateStateReport(IPV4_ADDR mcastAddress, int ifIx, TCPIP_IGMP_GIF_STATE_DCPT* pOldDcpt, TCPIP_IGMP_GIF_STATE_DCPT* pNewDcpt)
{
    TCPIP_IGMPv3_RECORD_TYPE    recType;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES  new_old, old_new;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES  *pAllow, *pBlock;

    if(pOldDcpt->ifFilter != pNewDcpt->ifFilter)
    {   // change filter report!
        // SSM should not change filter!
        _IGMPAssertCond(!_IGMP_IsSsmAddress(mcastAddress), __func__, __LINE__);
        recType = (pNewDcpt->ifFilter == TCPIP_IGMP_FILTER_INCLUDE) ? TCPIP_IGMP_FILTER_TO_INCLUDE : TCPIP_IGMP_FILTER_TO_EXCLUDE;
        return _IGMP_ScheduleFmcReport(mcastAddress, recType, ifIx, &pNewDcpt->groupSources);
    }

    // no change in filter: Source List change record
    // calculate (new-old) and (old-new) differences
    _IGMP_SubtractSources(&pNewDcpt->groupSources, &pOldDcpt->groupSources, &new_old);
    _IGMP_SubtractSources(&pOldDcpt->groupSources, &pNewDcpt->groupSources, &old_new);

    pAllow = pBlock = 0;
    if(pNewDcpt->ifFilter == TCPIP_IGMP_FILTER_INCLUDE)
    {
        if(new_old.nSources != 0)
        {
            pAllow = &new_old;
        }
        if(old_new.nSources != 0)
        {
            pBlock = &old_new;
        }
    }
    else
    {
        _IGMPAssertCond(!_IGMP_IsSsmAddress(mcastAddress), __func__, __LINE__);
        if(new_old.nSources != 0)
        {
            pBlock = &new_old;
        }
        if(old_new.nSources != 0)
        {
            pAllow = &old_new;
        }
    }

    return _IGMP_ScheduleSlcReport(mcastAddress, ifIx, pAllow, pBlock);
}


// updates the sources for a (G, skt, if) tuple;  non empty list
// filterMode is the (new) filter type for the socket
static TCPIP_IGMP_RESULT _IGMP_SocketUpdateSources(UDP_SOCKET socket, int ifIx, IPV4_ADDR mcastAddress, TCPIP_IGMP_FILTER_TYPE filterMode, const IPV4_ADDR* sourceList, size_t* listSize)
{
    int srcIx;
    int nSources, nNewSources;
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
        pGEntry = (TCPIP_IGMP_GROUP_ENTRY*)TCPIP_OAHASH_EntryLookupOrInsert(&igmpGroupsDcpt.gHashDcpt, &mcastAddress);

        if(pGEntry == 0)
        {    // full hash...
            updateRes = TCPIP_IGMP_MCAST_CACHE_FULL;
            break;
        }

        // old state snapshot
        _IGMP_GetGifState(pGEntry, ifIx, &oldStateDcpt);

        // get the source hash for this G Entry
        sHashDcpt = _IGMP_GetSourceHashDcpt(pGEntry);

        if(pGEntry->hEntry.flags.newEntry == 0)
        {   // existing source; if filter changes INCLUDE <-> EXCLUDE, remove the old conflicting socket filter
            // so even if we can't add all sources at least we have a consistent (G, skt, if) state
            purgeFilter = filterMode == TCPIP_IGMP_FILTER_INCLUDE ? TCPIP_IGMP_FILTER_EXCLUDE : TCPIP_IGMP_FILTER_INCLUDE;
            _IGMP_GroupPurgeSktByFilter(pGEntry, socket, ifIx, &purgeFilter);
        }

        nSources = *listSize;

        for(srcIx = 0; srcIx < nSources; srcIx++, sourceList++)
        {
            pSEntry = (TCPIP_IGMP_SOURCE_ENTRY*)TCPIP_OAHASH_EntryLookupOrInsert(sHashDcpt, sourceList);
            if(pSEntry == 0)
            {    // full source hash...
                updateRes = TCPIP_IGMP_SOURCE_CACHE_FULL;
                break;
            }

            if(pSEntry->hEntry.flags.newEntry != 0)
            {   // just adding this source
                _IGMP_SourceEntryInit(pSEntry);
                pRec = 0;
            }
            else
            {
                pRec = _IGMP_SourceFindSktRecord(pSEntry, socket, ifIx, 0);
            }

            if(pRec == 0)
            {
                pRec = _IGMP_SourceCreateSktRecord(pSEntry, socket, ifIx, filterMode);
                if(pRec == 0)
                {   // all records full
                    updateRes = TCPIP_IGMP_SOCKET_RECORD_FULL;
                    break;
                }
            }
            else
            {   // existent old source
                _IGMPAssertCond(pRec->filter == filterMode, __func__, __LINE__);
            }
            nNewSources++;    // count the old ones, even if already existent...
        }

        // get new state snapshot
        _IGMP_GetGifState(pGEntry, ifIx, &newStateDcpt);

        // generate report
        repRes = _IGMP_GenerateStateReport(mcastAddress, ifIx, &oldStateDcpt, &newStateDcpt);
        
        break;
    }

    *listSize = nNewSources;

    return updateRes == TCPIP_IGMP_OK ? repRes : updateRes; 
}

// gets the source hash descriptor associated to this G Entry
static OA_HASH_DCPT* _IGMP_GetSourceHashDcpt(TCPIP_IGMP_GROUP_ENTRY* pGEntry)
{
    OA_HASH_DCPT *sHashDcpt;
    sHashDcpt = &igmpGroupsDcpt.srcHashDcpt;
    sHashDcpt->memBlk = pGEntry->srcEntryTbl;
    return sHashDcpt;
}

static void _IGMP_SourceEntryInit(TCPIP_IGMP_SOURCE_ENTRY* pSEntry)
{
    memset(pSEntry->sktRec, 0, sizeof(pSEntry->sktRec));
    pSEntry->nRecords = 0;
}

// finds an existing socket record for the source entry
// if filtType == 0, then any type of filter matches
// else filter has to match
static TCPIP_IGMP_SKT_RECORD* _IGMP_SourceFindSktRecord(TCPIP_IGMP_SOURCE_ENTRY* pSEntry, UDP_SOCKET socket, int ifIx, TCPIP_IGMP_FILTER_TYPE filtType)
{
    TCPIP_IGMP_SKT_RECORD* pRec;
    int recIx;

    if(pSEntry->nRecords != 0)
    {
        pRec = pSEntry->sktRec;
        for(recIx = 0; recIx < sizeof(pSEntry->sktRec) / sizeof(*pSEntry->sktRec); recIx++, pRec++)
        {
            if(pRec->filter != 0 && pRec->sktNo == socket && pRec->ifIndex == ifIx && (filtType == 0 || filtType == pRec->filter))
            {   // found matching record;
                return pRec;
            }
        }
    }

    return 0;
}


// creates a new socket record for the source entry
// filtType != 0 !!!
static TCPIP_IGMP_SKT_RECORD* _IGMP_SourceCreateSktRecord(TCPIP_IGMP_SOURCE_ENTRY* pSEntry, UDP_SOCKET socket, int ifIx, TCPIP_IGMP_FILTER_TYPE filtType)
{
    TCPIP_IGMP_SKT_RECORD* pRec;
    int recIx;

    _IGMPAssertCond(filtType != 0, __func__, __LINE__);

    pRec = pSEntry->sktRec;
    for(recIx = 0; recIx < sizeof(pSEntry->sktRec) / sizeof(*pSEntry->sktRec); recIx++, pRec++)
    {
        if(pRec->filter == 0) 
        {   // found empty slot
            pRec->filter = filtType;
            pRec->ifIndex = (uint8_t)ifIx;
            pRec->sktNo = socket;

            pSEntry->nRecords++;
            return pRec;
        }
    }

    return 0;
}

// purges a socket record across the whole group entry if the socket has the purgeFilter type!
// otherwise the socket remains in place
// if purgeFilter == 0, then deletes the socket record no matter the type
// returns the number of removed records or 0 if socket not found
static int _IGMP_GroupPurgeSktByFilter(TCPIP_IGMP_GROUP_ENTRY* pGEntry, UDP_SOCKET socket, int ifIx, TCPIP_IGMP_FILTER_TYPE* pPurgeFilt)
{
    int srcIx;
    OA_HASH_DCPT *sHashDcpt;
    TCPIP_IGMP_SOURCE_ENTRY* pSEntry;
    TCPIP_IGMP_SKT_RECORD* pRec;
    TCPIP_IGMP_FILTER_TYPE sktFilter = 0;
    TCPIP_IGMP_FILTER_TYPE purgeFilter = *pPurgeFilt;
    int removeCnt = 0;

    sHashDcpt = _IGMP_GetSourceHashDcpt(pGEntry);
    for(srcIx = 0; srcIx < sHashDcpt->hEntries; srcIx++)
    {
        pSEntry = (TCPIP_IGMP_SOURCE_ENTRY*)TCPIP_OAHASH_EntryGet(sHashDcpt, srcIx);
        if(pSEntry->hEntry.flags.busy != 0)
        {   // found a valid source entry
            pRec = _IGMP_SourceFindSktRecord(pSEntry, socket, ifIx, 0);
            if(pRec)
            {   // found socket in this entry 
                if(sktFilter == 0)
                {   // first match
                    sktFilter = (TCPIP_IGMP_FILTER_TYPE)pRec->filter;
                }
                else
                {
                    _IGMPAssertCond(sktFilter == pRec->filter, __func__, __LINE__);
                }

                if(purgeFilter == 0 || sktFilter == purgeFilter)
                {   // purged
                    pRec->filter = 0;
                    removeCnt++;
                    if(--pSEntry->nRecords == 0)
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

    *pPurgeFilt = sktFilter;
    return removeCnt;
}

// purges a (socket, if, source) tuple across the whole group entry
// returns true if the socket source removed or false if socket not found in this source
static bool _IGMP_GroupPurgeSktBySource(TCPIP_IGMP_GROUP_ENTRY* pGEntry, UDP_SOCKET socket, int ifIx, const IPV4_ADDR* pPurgeAdd)
{
    OA_HASH_DCPT *sHashDcpt;
    TCPIP_IGMP_SOURCE_ENTRY* pSEntry;
    TCPIP_IGMP_SKT_RECORD* pRec;

    pRec = 0;
    sHashDcpt = _IGMP_GetSourceHashDcpt(pGEntry);
    pSEntry = (TCPIP_IGMP_SOURCE_ENTRY*)TCPIP_OAHASH_EntryLookup(sHashDcpt, pPurgeAdd);
    if(pSEntry != 0)
    {
        pRec = _IGMP_SourceFindSktRecord(pSEntry, socket, ifIx, 0);
        if(pRec)
        {   // found socket in this entry; purge 
            pRec->filter = 0;
            if(--pSEntry->nRecords == 0)
            {   // this source Entry is empty
                TCPIP_OAHASH_EntryRemove(sHashDcpt, &pSEntry->hEntry);
            }
        }
    }

    return pRec != 0;
}

// calculates the state of a (Group, If) pair
// state consists of a filter mode and list of sources
// if there are no sources for this (G, if), filter is INCLUDE and sources is {}
static void _IGMP_GetGifState(TCPIP_IGMP_GROUP_ENTRY* pGEntry, int ifIx, TCPIP_IGMP_GIF_STATE_DCPT* pGifDcpt)
{
    int srcIx;
    TCPIP_IGMP_SIF_FILTER_ACTION srcAction;
    OA_HASH_DCPT *sHashDcpt;
    TCPIP_IGMP_SOURCE_ENTRY*    pSEntry;
    TCPIP_IGMP_FILTER_TYPE      gifFilter, sifFilter;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES    incSources;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES    excSources;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES*   pGrSources;


    // init the structures
    memset(pGifDcpt, 0, sizeof(*pGifDcpt));
    memset(&incSources, 0, sizeof(incSources));
    memset(&excSources, 0, sizeof(excSources));

    gifFilter = TCPIP_IGMP_FILTER_INCLUDE;

    sHashDcpt = _IGMP_GetSourceHashDcpt(pGEntry);
    for(srcIx = 0; srcIx < sHashDcpt->hEntries; srcIx++)
    {
        pSEntry = (TCPIP_IGMP_SOURCE_ENTRY*)TCPIP_OAHASH_EntryGet(sHashDcpt, srcIx);
        if(pSEntry->hEntry.flags.busy != 0)
        {   // found a valid source entry
            _IGMPAssertCond(pSEntry->nRecords != 0, __func__, __LINE__);

            srcAction = _IGMP_GetSifFilter(pSEntry, ifIx, &sifFilter);

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
            // else the source is not used
        }
    }

    pGifDcpt->ifFilter = gifFilter;
    pGrSources = (gifFilter == TCPIP_IGMP_FILTER_EXCLUDE) ? &excSources : &incSources;
    memcpy(&pGifDcpt->groupSources, pGrSources, sizeof(*pGrSources));

}


// calculates the source filter for a (Source, if) pair
// returns TCPIP_IGMP_SIF_FILTER_ACTION if the source needs to be counted/added/ excluded, etc.
// the pFilter is updated per source or 0 if source not used
// Note:
//  - if all sockets include the source, then the filter is INCLUDE and the source is counted
//  - if all sockets exclude the source then the filter is EXCLUDE and the source is counted
//  - if some include and some exclude, filter is EXCLUDE but the source is not counted
//  - if some sockets have exclude {}, i.e. ASM_ALL_SOURCES on G, then EXCLUDE_NONE is returned
static TCPIP_IGMP_SIF_FILTER_ACTION _IGMP_GetSifFilter(const TCPIP_IGMP_SOURCE_ENTRY* pSEntry, int ifIx, TCPIP_IGMP_FILTER_TYPE* pFilter)
{
    TCPIP_IGMP_SIF_FILTER_ACTION srcAction = TCPIP_IGMP_SIF_ACTION_NONE;

    *pFilter = TCPIP_IGMP_FILTER_NONE;

    if(pSEntry->nRecords != 0)
    {
        int recIx;
        const TCPIP_IGMP_SKT_RECORD* pRec = pSEntry->sktRec;
        int nIncludes = 0;
        int nExcludes = 0;

        if(pSEntry->srcAddress.Val == TCPIP_IGMP_ASM_ALL_SOURCES)
        {
            for(recIx = 0; recIx < sizeof(pSEntry->sktRec) / sizeof(*pSEntry->sktRec); recIx++, pRec++)
            {
                if(pRec->filter != 0 && pRec->ifIndex == ifIx)
                {   // found socket record;
                    _IGMPAssertCond(pRec->filter == TCPIP_IGMP_FILTER_EXCLUDE, __func__, __LINE__);
                    *pFilter = TCPIP_IGMP_FILTER_EXCLUDE;
                    return TCPIP_IGMP_SIF_ACTION_EXCLUDE_NONE;
                }
            }
            return TCPIP_IGMP_SIF_ACTION_NONE;
        }

        for(recIx = 0; recIx < sizeof(pSEntry->sktRec) / sizeof(*pSEntry->sktRec); recIx++, pRec++)
        {
            if(pRec->filter != 0 && pRec->ifIndex == ifIx)
            {   // found socket record;
                if(pRec->filter == TCPIP_IGMP_FILTER_INCLUDE)
                {   
                    nIncludes++;
                }
                else
                {
                    nExcludes++;
                }
            }
        }

        // since nRecords != 0, then we should have something
        _IGMPAssertCond((nExcludes + nIncludes) != 0, __func__, __LINE__);

        if(nExcludes)
        {
            *pFilter = TCPIP_IGMP_FILTER_EXCLUDE;
            if(nIncludes == 0)
            {   // excluded source
                srcAction = TCPIP_IGMP_SIF_ACTION_ADD;
            }
        }
        else if(nIncludes)
        {   // included source
            *pFilter = TCPIP_IGMP_FILTER_INCLUDE;
            srcAction = TCPIP_IGMP_SIF_ACTION_ADD;
        }

    }

    return srcAction;
}

// calculates A-B and places the result at pDest
static void _IGMP_SubtractSources(TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pA, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pB, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pDest)
{
    uint32_t *ptrA, *aEnd;
    uint32_t *ptrB, *bEnd;
    uint32_t *pRes;

    // clear destination
    memset(pDest, 0, sizeof(*pDest));

    if(pA == 0)
    {
        return;
    }

    ptrA = pA->sourceAddresses; 
    aEnd = ptrA + pA->nSources;

    pRes = pDest->sourceAddresses;

    if(pB != 0)
    {
        ptrB = pB->sourceAddresses; 
        bEnd = ptrB + pB->nSources;

        // sort A and B
        qsort(ptrA, pA->nSources, sizeof(*ptrA), _IGMPSourceEntryCompare);
        qsort(ptrB, pB->nSources, sizeof(*ptrB), _IGMPSourceEntryCompare);


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
    memcpy(pRes, ptrA, (uint8_t*)aEnd - (uint8_t*)ptrA);
    pDest->nSources += aEnd - ptrA;

}

// calculates A+B and places the result at pDest
// pDest has to be large enough to accomodate both sources!
static void _IGMP_UnionSources(TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pA, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pB, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pDest)
{
    uint32_t *ptrA, *aEnd;
    uint32_t *ptrB, *bEnd;
    uint32_t *pRes;

    // clear destination
    memset(pDest, 0, sizeof(*pDest));

    if(pA == 0 || pB == 0)
    {
        if(pB != 0)
        {
            memcpy(pDest, pB, sizeof(*pDest));
        }
        else if(pA != 0)
        {
            memcpy(pDest, pA, sizeof(*pDest));
        }
        return;
    }

    ptrA = pA->sourceAddresses; 
    aEnd = ptrA + pA->nSources;

    ptrB = pB->sourceAddresses; 
    bEnd = ptrB + pB->nSources;

    // sort A and B
    qsort(ptrA, pA->nSources, sizeof(*ptrA), _IGMPSourceEntryCompare);
    qsort(ptrB, pB->nSources, sizeof(*ptrB), _IGMPSourceEntryCompare);

    pRes = pDest->sourceAddresses;

    while (true)
    {
        if (ptrA == aEnd)
        {
            memcpy(pRes, ptrB, (uint8_t*)bEnd - (uint8_t*)ptrB);
            pDest->nSources += bEnd - ptrB;
            break;
        }

        if (ptrB == bEnd)
        {
            memcpy(pRes, ptrA, (uint8_t*)aEnd - (uint8_t*)ptrA);
            pDest->nSources += aEnd - ptrA;
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
static void _IGMP_IntersectSources(TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pA, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pB, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pDest)
{
    uint32_t *ptrA, *aEnd;
    uint32_t *ptrB, *bEnd;
    uint32_t *pRes;

    // clear destination
    memset(pDest, 0, sizeof(*pDest));

    if(pA == 0 || pB == 0)
    {
        return;
    }

    ptrA = pA->sourceAddresses; 
    aEnd = ptrA + pA->nSources;

    ptrB = pB->sourceAddresses; 
    bEnd = ptrB + pB->nSources;

    // sort A and B
    qsort(ptrA, pA->nSources, sizeof(*ptrA), _IGMPSourceEntryCompare);
    qsort(ptrB, pB->nSources, sizeof(*ptrB), _IGMPSourceEntryCompare);

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
static int _IGMPSourceEntryCompare(const void* p1, const void* p2)
{
    uint32_t add1 = *((uint32_t*)p1);
    uint32_t add2 = *((uint32_t*)p2);
   

    if(add1 < add2)
    {
        return -1;
    }
    else if(add1 > add2)
    {
        return 1;
    }

    return 0;    
}

// schedule Filter mode change (FMC) report
// searches the current SC reports and deletes any allow/block report belonging to this group
// returns TCPIP_IGMP_OK or TCPIP_IGMP_REPORT_POOL_EMPTY if no report node could be found
static TCPIP_IGMP_RESULT _IGMP_ScheduleFmcReport(IPV4_ADDR groupAddress, TCPIP_IGMPv3_RECORD_TYPE repType, int ifIx, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pGroupSources)
{
    TCPIP_IGMP_SC_REPORT_NODE* pFiltNode;


    igmpListsLock();

    pFiltNode = _IGMP_FindScheduledScReport(groupAddress, ifIx, 0, 0, true);

    if(pFiltNode)
    {
        pFiltNode->active = 0;
    }

    igmpListsUnlock();

    if(pFiltNode == 0)
    {   // couldn't find an old report for (G, if)
        pFiltNode = _IGMP_GetNewScReport();
    }

    if(pFiltNode == 0)
    {   // couldn't find a slot for the new report
        _IGMP_ReportEvent(groupAddress, TCPIP_IGMP_EVENT_GET_SC_REPORT_ERROR);
        _IGMPDebugCond(true, __func__, __LINE__);
        return TCPIP_IGMP_REPORT_POOL_EMPTY;
    }

    _IGMP_SetGifSCRecord(pFiltNode, groupAddress, repType, ifIx, pGroupSources);
    
    // add it to the report list
    _IGMP_InsertScReport(&igmpScReportList, pFiltNode);

    return TCPIP_IGMP_OK; 
}


// sets a Source Change report node info
static void _IGMP_SetGifSCRecord(TCPIP_IGMP_SC_REPORT_NODE* pRepNode, IPV4_ADDR groupAddress, TCPIP_IGMPv3_RECORD_TYPE repType, int ifIx, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pGroupSources)
{
    pRepNode->nTransmit = igmpRobustnessVar;
    pRepNode->repType = repType;
    pRepNode->ifIx = ifIx;
    pRepNode->active = true;
    _IGMP_SetScReportTransmitTime(pRepNode);
    pRepNode->repGroup = groupAddress.Val;
    
    memcpy(&pRepNode->repSources, pGroupSources, sizeof(pRepNode->repSources));
}

// gets a new available node report from the free list
static TCPIP_IGMP_SC_REPORT_NODE* _IGMP_GetNewScReport(void)
{
    igmpListsLock();
    TCPIP_IGMP_SC_REPORT_NODE* pNewRep = (TCPIP_IGMP_SC_REPORT_NODE*)TCPIP_Helper_SingleListHeadRemove(&igmpScFreeList);
    igmpListsUnlock();

    return pNewRep;
}

// inserts a new node in a SC report list
static void _IGMP_InsertScReport(SINGLE_LIST* pList, TCPIP_IGMP_SC_REPORT_NODE* pRepNode)
{
    igmpListsLock();
    TCPIP_Helper_SingleListTailAdd(pList, (SGL_LIST_NODE*)pRepNode);
    igmpListsUnlock();
}

// deletes a report node from a SC report list
static void _IGMP_DeleteScReport(SINGLE_LIST* pList, TCPIP_IGMP_SC_REPORT_NODE* pRepNode)
{
    igmpListsLock();

    TCPIP_IGMP_SC_REPORT_NODE* pRem = (TCPIP_IGMP_SC_REPORT_NODE*)TCPIP_Helper_SingleListNodeRemove(pList, (SGL_LIST_NODE*)pRepNode);
    _IGMPAssertCond(pRem == pRepNode, __func__, __LINE__);
    TCPIP_Helper_SingleListTailAdd(&igmpScFreeList, (SGL_LIST_NODE*)pRem);
    
    igmpListsUnlock();
}

// schedule a source list change (SLC)
// returns TCPIP_IGMP_OK or TCPIP_IGMP_REPORT_POOL_EMPTY if no report node could be found
static TCPIP_IGMP_RESULT _IGMP_ScheduleSlcReport(IPV4_ADDR groupAddress, int ifIx, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pNewAllow, TCPIP_IGMP_GROUP_SOURCE_ADDRESSES* pNewBlock)
{
    TCPIP_IGMP_SC_REPORT_NODE *pAllowRep, *pBlockRep, *pFmcRep;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES unionAllow, unionBlock;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES resultAllow, resultBlock;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES *pUnionAllow, *pUnionBlock;
   
    pFmcRep = _IGMP_FindGifSCReportsDisable(groupAddress, ifIx, &pAllowRep, &pBlockRep);

    if(pAllowRep)
    {
        _IGMP_UnionSources(pNewAllow, &pAllowRep->repSources, &unionAllow);
        pUnionAllow = &unionAllow;
    }
    else
    {   // no pending allow report
        pUnionAllow = pNewAllow;
    }

    if(pBlockRep)
    {
        _IGMP_UnionSources(pNewBlock, &pBlockRep->repSources, &unionBlock);
        pUnionBlock = &unionBlock;
    }
    else
    {   // no pending allow report
        pUnionBlock = pNewBlock;
    }

    // finally, calculate:
    // what's allowed: union(allows) - union(blocks)
    // what's blocked: union(blocks) - union(allows)
    _IGMP_SubtractSources(pUnionAllow, pUnionBlock, &resultAllow);
    _IGMP_SubtractSources(pUnionBlock, pUnionAllow, &resultBlock);

    // create the reports
    if(resultAllow.nSources)
    {   // need to allow something
        if(pAllowRep == 0)
        {   // grab a new one
            pAllowRep = _IGMP_GetNewScReport();
        }

        if(pAllowRep == 0)
        {   // couldn't find a slot for the new report; this is BAD!
            _IGMPDebugCond(true, __func__, __LINE__);
            return TCPIP_IGMP_REPORT_POOL_EMPTY;
        }
    }
    else
    {   // no need to transmit allow report
        if(pAllowRep != 0)
        {   // delete the report
            _IGMP_DeleteScReport(&igmpScReportList, pAllowRep);
        }

        pAllowRep = 0;
    }
    
    if(resultBlock.nSources)
    {   // need to block something
        if(pBlockRep == 0)
        {   // grab a new one
            pBlockRep = _IGMP_GetNewScReport();
        }

        if(pBlockRep == 0)
        {   // couldn't find a slot for the new report; this is BAD!
            _IGMPDebugCond(true, __func__, __LINE__);
            return TCPIP_IGMP_REPORT_POOL_EMPTY;
        }
    }
    else
    {   // no need to transmit allow report
        if(pBlockRep != 0)
        {   // delete the report
            _IGMP_DeleteScReport(&igmpScReportList, pBlockRep);
        }

        pBlockRep = 0;
    }

    if(pAllowRep)
    {
        _IGMP_SetGifSCRecord(pAllowRep, groupAddress, TCPIP_IGMP_SOURCE_ALLOW_NEW, ifIx, &resultAllow);
        if(pFmcRep)
        {   // will be activated once the filter is transmitted!
            pAllowRep->active = 0;
        }
        _IGMP_InsertScReport(&igmpScReportList, pAllowRep);
    }
    
    if(pBlockRep)
    {
        _IGMP_SetGifSCRecord(pBlockRep, groupAddress, TCPIP_IGMP_SOURCE_BLOCK_OLD, ifIx, &resultBlock);
        if(pFmcRep)
        {   // will be activated once the filter is transmitted!
            pBlockRep->active = 0;
        }
        _IGMP_InsertScReport(&igmpScReportList, pBlockRep);
    }
    
    return TCPIP_IGMP_OK; 
}

// finds SC reports for (G, if)
// and gets the pointers for the allow ones and block ones
// Disables the allow and block
// returns the filter report if it exists
static TCPIP_IGMP_SC_REPORT_NODE* _IGMP_FindGifSCReportsDisable(IPV4_ADDR groupAddress, int ifIx, TCPIP_IGMP_SC_REPORT_NODE** ppAllowNode, TCPIP_IGMP_SC_REPORT_NODE** ppBlockNode)
{
    TCPIP_IGMP_SC_REPORT_NODE *pAllow, *pBlock, *pFmc;

    pAllow = pBlock = pFmc = 0;

    igmpListsLock();

    // search for allow, block and filter
    pFmc = _IGMP_FindScheduledScReport(groupAddress, ifIx, &pAllow, &pBlock, false);
    if(pAllow)
    {   // check that allow/block are pending
        pAllow->active = 0;
    }
    if(pBlock)
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
static TCPIP_IGMP_SC_REPORT_NODE* _IGMP_FindScheduledScReport(IPV4_ADDR groupAddress, int ifIx, TCPIP_IGMP_SC_REPORT_NODE** ppAllowNode, TCPIP_IGMP_SC_REPORT_NODE** ppBlockNode, bool removeAllowBlock)
{
    TCPIP_IGMP_SC_REPORT_NODE *pRNode;
    TCPIP_IGMP_SC_REPORT_NODE *pAllow, *pBlock, *pFilt;
    SINGLE_LIST  newBusyList;
    bool removeNode;

    pAllow = pBlock = pFilt = 0;
    TCPIP_Helper_SingleListInitialize(&newBusyList);


    while((pRNode = (TCPIP_IGMP_SC_REPORT_NODE*)TCPIP_Helper_SingleListHeadRemove(&igmpScReportList)) != 0)
    {
        removeNode = false;

        if(pRNode->ifIx == ifIx && pRNode->repGroup == groupAddress.Val)
        {   // found node
            if(pRNode->repType == TCPIP_IGMP_FILTER_TO_INCLUDE || pRNode->repType == TCPIP_IGMP_FILTER_TO_EXCLUDE)
            {
                _IGMPAssertCond(pFilt == 0, __func__, __LINE__);
                pFilt = pRNode;
            }
            else if(pRNode->repType == TCPIP_IGMP_SOURCE_ALLOW_NEW || pRNode->repType == TCPIP_IGMP_SOURCE_BLOCK_OLD)
            {
                if(pRNode->repType == TCPIP_IGMP_SOURCE_ALLOW_NEW)
                {
                    _IGMPAssertCond(pAllow == 0, __func__, __LINE__);
                    pAllow = pRNode;
                }
                else
                {
                    _IGMPAssertCond(pBlock == 0, __func__, __LINE__);
                    pBlock = pRNode;
                }
                
                if(removeAllowBlock)
                {
                    removeNode = true;
                }
            }
            else
            {   // unknown type
                _IGMPAssertCond(false, __func__, __LINE__);
            }
        }
        // else not ours

        if(removeNode)
        {
            TCPIP_Helper_SingleListTailAdd(&igmpScFreeList, (SGL_LIST_NODE*)pRNode);
        }
        else
        {   // keep it
            TCPIP_Helper_SingleListTailAdd(&newBusyList, (SGL_LIST_NODE*)pRNode);
        }
    }

    if(pFilt)
    {
        if(pAllow)
        {   // check that allow/block are pending
            _IGMPAssertCond(pAllow->active == 0, __func__, __LINE__);
        }
        if(pBlock)
        {   // check that allow/block are pending
            _IGMPAssertCond(pBlock->active == 0, __func__, __LINE__);
        }
    }
    
    if(ppAllowNode)
    {
        *ppAllowNode = pAllow;
    }

    if(ppBlockNode)
    {
        *ppBlockNode = pBlock;
    }

    igmpScReportList = newBusyList;

    return pFilt;

}

// searches for a General Query or group (G, if) report into the proper Query list
// returns found query node 
// access to the list should be LOCKED!
static TCPIP_IGMP_QUERY_REPORT_NODE* _IGMP_FindScheduledQueryReport(TCPIP_IGMP_QUERY_TYPE qType, uint32_t groupAddress, int ifIx, bool remove)
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

    for(pQNode = (TCPIP_IGMP_QUERY_REPORT_NODE*)pList->head, pPrev = 0; pQNode != 0; pPrev = pQNode, pQNode = pQNode->next)
    {
        if(pQNode->ifIx == ifIx)
        {   // matching interface
           if(qType == TCPIP_IGMP_QUERY_GENERAL || pQNode->qSources[0].repGroup == groupAddress)
           {   // matching group; found node
               if(remove)
               {
                   pRemN = (TCPIP_IGMP_QUERY_REPORT_NODE*)TCPIP_Helper_SingleListNextRemove(pList, (SGL_LIST_NODE*)pPrev);
                   (void)pRemN;
                   _IGMPAssertCond(pRemN == pQNode, __func__, __LINE__);
               }
               return pQNode;
           }
        }
    }

    return 0;   // not found
}


// checks that an IPv4 multicast address is valid
static bool _IGMP_IsMcastValid(IPV4_ADDR addr)
{
    if(_IGMP_IsMcastAddress(addr))
    {
        return addr.Val != TCPIP_IGMP_IANA_ALL_SYSTEMS_MULTICAST && addr.Val != TCPIP_IGMP_IANA_ALL_ROUTERS_MULTICAST && addr.Val != TCPIP_IGMP_IANA_SSM_RESERVED;
    }

    return false;
}

static bool  _IGMP_IsSsmAddress(IPV4_ADDR mcastAdd)
{
    return (TCPIP_Helper_ntohl(igmpSsmAddLow) <= TCPIP_Helper_ntohl(mcastAdd.Val) &&  TCPIP_Helper_ntohl(mcastAdd.Val) <= TCPIP_Helper_ntohl(igmpSsmAddHigh));
}

// performs a minimal sanity check for subscribe parameters
static TCPIP_IGMP_RESULT _IGMP_CheckSubscribeParams(TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, const IPV4_ADDR* sourceList, size_t* listSize, int* pIfIx)
{
    int ifIx;
    TCPIP_NET_IF* pNetIf;

    size_t nSources = listSize ? *listSize : 0;

    // minimal sanity check
    if(nSources)
    {
#if (TCPIP_IGMPV2_SUPPORT_ONLY != 0)
        return TCPIP_IGMP_SOURCE_INVALID;
#endif
       if(sourceList == 0)
       {
           return TCPIP_IGMP_ARG_ERROR;
       }
    }

    if(!_IGMP_IsMcastValid(mcastAddress))
    {
        return TCPIP_IGMP_MCAST_ADDRESS_ERROR;
    }

    if(hNet == 0)
    {
        hNet = TCPIP_STACK_NetMulticastGet();
    }

    ifIx = TCPIP_STACK_NetIndexGet(hNet);
    pNetIf = _TCPIPStackHandleToNetUp(hNet);

    if(ifIx < 0 || pNetIf == 0 || (pNetIf->startFlags & TCPIP_NETWORK_CONFIG_MULTICAST_ON) == 0)
    {
        return TCPIP_IGMP_IF_ERROR;
    }


    *pIfIx = ifIx;
    return TCPIP_IGMP_OK;
}

// checks that the interface index is a valid slot or a new one can be taken
static bool _IGMP_CheckIfIndex(int ifIx, bool itExists)
{

    if(ifIx < igmpInterfaces)
    {   
        if((igmpGroupsDcpt.ifMask & (1 << ifIx)) != 0)
        {
            return true;
        }
        else if(itExists == false)
        {
            igmpGroupsDcpt.ifMask |= (1 << ifIx);
            return true;
        }
    }

    return false;

}

static void _IGMP_SetScReportTransmitTime(TCPIP_IGMP_SC_REPORT_NODE* pRNode)
{
    pRNode->tTransmit = SYS_TMR_TickCountGet() + (SYS_RANDOM_PseudoGet() % igmpUnsolicitRepInterval) * SYS_TMR_TickCounterFrequencyGet() / 1000;
}


// checks if a Group Entry could be removed and removes it if possible
static bool _IGMP_GroupEntryCheckRemove(TCPIP_IGMP_GROUP_ENTRY* pGEntry)
{
    OA_HASH_DCPT *sHashDcpt;
    sHashDcpt = _IGMP_GetSourceHashDcpt(pGEntry);
    if(sHashDcpt->fullSlots == 0)
    {   // no more sources in this group
        TCPIP_OAHASH_EntryRemove(&igmpGroupsDcpt.gHashDcpt, &pGEntry->hEntry);
        return true;
    }

    return false;
}

static void _IGMP_ProcessV3Query(TCPIP_IGMPv3_QUERY_MESSAGE* pQuery, int ifIx)
{
    TCPIP_IGMP_QUERY_TYPE qType;
    uint32_t maxRespTime, tReport;
    TCPIP_IGMP_RESP_CODE_FLOAT rCodeFloat;
    TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGq;
    TCPIP_IGMP_GROUP_QUERY_REPORT_NODE *pGsq;
    IPV4_ADDR groupAddress;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES newReqSources, unionSources;
    uint16_t nSources = 0;

    igmpRobustnessVar = pQuery->qrv < TCPIP_IGMP_ROBUSTNESS_LOW_LIMIT ? TCPIP_IGMP_ROBUSTNESS_VARIABLE : pQuery->qrv;
    
    if((groupAddress.Val = pQuery->groupAddress) == 0)
    {
        qType = TCPIP_IGMP_QUERY_GENERAL;
    }
    else
    {
        if(pQuery->nSources == 0)
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
        maxRespTime = pQuery->maxRespCode * 100;
    }
    else
    {
        rCodeFloat.val = pQuery->maxRespCode;
        maxRespTime = ((rCodeFloat.mant | 0x10) << (rCodeFloat.exp + 3)) * 100;
    }

    tReport = SYS_TMR_TickCountGet() + (SYS_RANDOM_PseudoGet() % maxRespTime) * SYS_TMR_TickCounterFrequencyGet() / 1000;

    igmpListsLock();
    pGq = (TCPIP_IGMP_GEN_QUERY_REPORT_NODE*)_IGMP_FindScheduledQueryReport(TCPIP_IGMP_QUERY_GENERAL, 0, ifIx, false);
    igmpListsUnlock();

    // check the current status report rules
    // #1 rule: check for existing GQ scheduled earlier
    if(pGq != 0 && pGq->tTransmit < tReport)
    {
        return;
    }

    // #2 rule: if a GQ was received, schedule it and cancel a previous GQ
    if(qType == TCPIP_IGMP_QUERY_GENERAL)
    {
        bool insert = false;
        if(pGq == 0)
        {   // couldn't find an old GQ report
            pGq = (TCPIP_IGMP_GEN_QUERY_REPORT_NODE*)_IGMP_GetNewQueryReport(TCPIP_IGMP_QUERY_GENERAL);
            insert = true;
        }

        if(pGq != 0)
        {   // prepare the GQ report and schedule it;
            _IGMP_ScheduleGenQueryReport(pGq, ifIx, tReport, insert);
        }
        else
        {   // couldn't find a slot for the new GQ report; should NOT happen!
            _IGMPAssertCond(true, __func__, __LINE__);
            _IGMP_ReportEvent(groupAddress, TCPIP_IGMP_EVENT_GET_GEN_QUERY_REPORT_ERROR);
        }
        return;
    }

    // #3 rule: GSQ or GSSQ received; if no pending response, then schedule a group report 
    igmpListsLock();
    pGsq = (TCPIP_IGMP_GROUP_QUERY_REPORT_NODE*)_IGMP_FindScheduledQueryReport(TCPIP_IGMP_QUERY_GROUP_SPEC, groupAddress.Val, ifIx, false);
    igmpListsUnlock();
    if(pGsq == 0)
    {
        pGsq = (TCPIP_IGMP_GROUP_QUERY_REPORT_NODE*)_IGMP_GetNewQueryReport(qType);
        if(pGsq != 0)
        {
            _IGMP_InitGroupQueryReport(pGsq, groupAddress, ifIx, tReport);
            _IGMP_SourceScheduleGroupQueryReport(pGsq, nSources, pQuery->sources, true);
        }
        else
        {   // couldn't find a slot for the new group report
            _IGMPDebugCond(true, __func__, __LINE__);
            _IGMP_ReportEvent(groupAddress, TCPIP_IGMP_EVENT_GET_GROUP_QUERY_REPORT_ERROR);
        }
        return;
    }

    // #4 rule: if GSQ received or pending GSQ, then schedule a GSQ 
    if(qType == TCPIP_IGMP_QUERY_GROUP_SPEC || pGsq->qSources[0].repSources.nSources == 0)
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
    memcpy(&newReqSources, pQuery->sources, nSources * sizeof(newReqSources.sourceAddresses[0]));
    
    _IGMP_UnionSources(&newReqSources, &pGsq->qSources[0].repSources, &unionSources);
    _IGMP_SourceScheduleGroupQueryReport(pGsq, unionSources.nSources, unionSources.sourceAddresses, false);
    

}

// gets a new query report node
static TCPIP_IGMP_QUERY_REPORT_NODE* _IGMP_GetNewQueryReport(TCPIP_IGMP_QUERY_TYPE qType)
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
    pQNode = (TCPIP_IGMP_QUERY_REPORT_NODE*)TCPIP_Helper_SingleListHeadRemove(pList);
    igmpListsUnlock();

    if(qType == TCPIP_IGMP_QUERY_GENERAL)
    {   // GQ should naever fail
        _IGMPAssertCond(pQNode != 0, __func__, __LINE__);
    }

    return pQNode;

}

// prepare a Gen Query report for which the tTransmit expired 
static int _IGMP_GenerateGenQueryReport(TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGq)
{
    OA_HASH_DCPT *gHashDcpt;
    TCPIP_IGMP_GROUP_ENTRY* pGEntry;
    TCPIP_IGMP_GIF_STATE_DCPT currStateDcpt, *pDcpt;
    TCPIP_IGMP_QUERY_SOURCES* pQSource;
    int groupIx, nEntries;

    // mark all qSources as invalid!
    _IGMPAssertCond(pGq->nQSources == sizeof(pGq->qSources) / sizeof(*pGq->qSources), __func__, __LINE__);
    memset(pGq->qSources, 0, pGq->nQSources * sizeof(*pGq->qSources));

    igmpDcptLock();
    gHashDcpt = &igmpGroupsDcpt.gHashDcpt;
    nEntries = 0;
    pQSource = pGq->qSources;
    for(groupIx = 0; groupIx < gHashDcpt->hEntries; groupIx++)
    {
        pGEntry = (TCPIP_IGMP_GROUP_ENTRY*)TCPIP_OAHASH_EntryGet(gHashDcpt, groupIx);
        if(pGEntry->hEntry.flags.busy != 0)
        {   // found a valid group entry; get state snapshot
            pDcpt = &currStateDcpt;
            _IGMP_GetGifState(pGEntry, pGq->ifIx, pDcpt);

            if(pDcpt->ifFilter == TCPIP_IGMP_FILTER_INCLUDE && pDcpt->groupSources.nSources == 0)
            {   // empty sources
                continue;
            }
            else
            {   // found valid entry
                pQSource->repGroup = pGEntry->gAddress.Val;
                pQSource->repType = pDcpt->ifFilter == TCPIP_IGMP_FILTER_INCLUDE ? TCPIP_IGMP_MODE_IS_INCLUDE : TCPIP_IGMP_MODE_IS_EXCLUDE;
                memcpy(&pQSource->repSources, &pDcpt->groupSources, sizeof(pQSource->repSources));
                nEntries++;
                pQSource++;
                _IGMPAssertCond(nEntries <= sizeof(pGq->qSources) / sizeof(*pGq->qSources), __func__, __LINE__);
            }
        }
    }
    igmpDcptUnlock();

    return nEntries;
}

// prepare a Group Query report for which the tTransmit expired 
// returns number of records that need to be sent, 0 if none 
static bool _IGMP_GenerateGroupQueryReport(TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pGrpNode)
{
    OA_HASH_DCPT *gHashDcpt;
    TCPIP_IGMP_GROUP_ENTRY* pGEntry;
    TCPIP_IGMP_GIF_STATE_DCPT currStateDcpt, *pDcpt;
    TCPIP_IGMP_QUERY_SOURCES* pQSource;
    TCPIP_IGMP_GROUP_SOURCE_ADDRESSES resultSources;
    IPV4_ADDR groupAddress;
    bool genReport;

    _IGMPAssertCond(pGrpNode->nQSources == sizeof(pGrpNode->qSources) / sizeof(*pGrpNode->qSources), __func__, __LINE__);

    gHashDcpt = &igmpGroupsDcpt.gHashDcpt;
    groupAddress.Val = pGrpNode->qSources[0].repGroup;
    genReport = false;

    // pGrpNode holds the router requested sources!

    igmpDcptLock();
    pGEntry = (TCPIP_IGMP_GROUP_ENTRY*)TCPIP_OAHASH_EntryLookup(gHashDcpt, &groupAddress);
    if(pGEntry != 0 && pGEntry->hEntry.flags.busy != 0)
    {   // found a valid group entry; get state snapshot
        pDcpt = &currStateDcpt;
        _IGMP_GetGifState(pGEntry, pGrpNode->ifIx, pDcpt);

        if(pDcpt->ifFilter != TCPIP_IGMP_FILTER_INCLUDE || pDcpt->groupSources.nSources != 0)
        {   // reception state for this group: valid non empty sources
            pQSource = pGrpNode->qSources;
            pQSource->repGroup = groupAddress.Val;
            if(pGrpNode->qSources[0].repSources.nSources == 0)
            {   // A Group Specific report is needed
                pQSource->repType = pDcpt->ifFilter == TCPIP_IGMP_FILTER_INCLUDE ? TCPIP_IGMP_MODE_IS_INCLUDE : TCPIP_IGMP_MODE_IS_EXCLUDE;
                memcpy(&pQSource->repSources, &pDcpt->groupSources, sizeof(pQSource->repSources));
                genReport = true;
            }
            else
            {   // A Group and source specific report is needed
                pQSource->repType = TCPIP_IGMP_MODE_IS_INCLUDE;
                if(pDcpt->ifFilter == TCPIP_IGMP_FILTER_INCLUDE)
                {
                    _IGMP_IntersectSources(&pGrpNode->qSources[0].repSources, &currStateDcpt.groupSources, &resultSources);
                }
                else
                {
                    _IGMP_SubtractSources(&pGrpNode->qSources[0].repSources, &currStateDcpt.groupSources, &resultSources);
                }

                if(resultSources.nSources != 0)
                {   // need to send something
                    memcpy(&pQSource->repSources, &resultSources, sizeof(pQSource->repSources));
                    genReport = true;
                }
            }
        }
    }
    igmpDcptUnlock();

    return genReport;
}


// sets up a Gen Query node and inserts it in a Gen Query report list
static void _IGMP_ScheduleGenQueryReport(TCPIP_IGMP_GEN_QUERY_REPORT_NODE* pGQNode, int ifIx, uint32_t tTransmit, bool insert)
{
    pGQNode->ifIx = ifIx;
    pGQNode->queryType = TCPIP_IGMP_QUERY_GENERAL;
    pGQNode->tTransmit = tTransmit;

    if(insert)
    {
        igmpListsLock();
        TCPIP_Helper_SingleListTailAdd(&igmpGenQueryReportList, (SGL_LIST_NODE*)pGQNode);
        igmpListsUnlock();
    }
}

// initializes a Group Query node in a GS mode (no sources updated) 
// it does NOT insert in the report list!
static void _IGMP_InitGroupQueryReport(TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pGNode, IPV4_ADDR groupAddress, int ifIx, uint32_t tTransmit)
{
    pGNode->ifIx = ifIx;
    pGNode->queryType = TCPIP_IGMP_QUERY_GROUP_SPEC;
    pGNode->tTransmit = tTransmit;
    pGNode->qSources[0].repType = 0;    // updated at transmit time
    pGNode->qSources[0].repGroup = groupAddress.Val;
    pGNode->qSources[0].repSources.nSources = 0;
}

// sets up a Group Query node with sources and inserts it in a Group Query report list
static void _IGMP_SourceScheduleGroupQueryReport(TCPIP_IGMP_GROUP_QUERY_REPORT_NODE* pGNode, int nRecSources, uint32_t* pRecSources, bool insert)
{
    pGNode->queryType = nRecSources == 0 ? TCPIP_IGMP_QUERY_GROUP_SPEC : TCPIP_IGMP_QUERY_GROUP_SOURCE_SPEC;
    pGNode->qSources[0].repSources.nSources = nRecSources;

    int nAvlblSources = sizeof(pGNode->qSources[0].repSources.sourceAddresses) / sizeof(*pGNode->qSources[0].repSources.sourceAddresses);
    if(nRecSources)
    {
        if(nRecSources > nAvlblSources)
        {   // truncate
            IPV4_ADDR groupAddr;
            groupAddr.Val = pGNode->qSources[0].repGroup;
            _IGMP_ReportEvent(groupAddr, TCPIP_IGMP_EVENT_QUERY_SOURCE_NUMBER_EXCEEDED);
            nRecSources = nAvlblSources;
        }
        // store the sources for report
        memcpy(pGNode->qSources[0].repSources.sourceAddresses, pRecSources, nRecSources * sizeof(*pRecSources));
    }

    if(insert)
    {
        igmpListsLock();
        TCPIP_Helper_SingleListTailAdd(&igmpGroupQueryReportList, (SGL_LIST_NODE*)pGNode);
        igmpListsUnlock();
    }

}


#endif  // defined(TCPIP_STACK_USE_IGMP)
#endif  // defined(TCPIP_STACK_USE_IPV4)


