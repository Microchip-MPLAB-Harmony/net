/*******************************************************************************
 IEEE 1588 PTP Implementation File

  Company:
    Microchip Technology Inc.

  File Name:
    ptp.c

  Summary:
    IEEE 1588 PTP API

  Description:
    Implementation of the Precision Time Protocol described in IEEE 1588
    - Reference: IEEE 1588-2008
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2017 released Microchip Technology Inc.  All rights reserved.

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
//DOM-IGNORE-END
#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_PTP

#include "tcpip/src/tcpip_private.h"
#include "tcpip/src/tcp_private.h"

#define TCPIP_STACK_USE_PTP // @@@@
#if defined(TCPIP_STACK_USE_PTP)

#include <string.h>

#include "./ptp_private.h"

#include "system/console/sys_console.h"

// data


// the one and only clock descriptor
static TCPIP_PTP_CLOCK_DCPT     ptpClockDcpt;

#if ((TCPIP_PTP_DEBUG_LEVEL & TCPIP_PTP_DEBUG_MASK_BASIC) != 0)
volatile int _PTPStayAssertLoop = 0;
static void _PTPAssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("PTP Assert: %s, in line: %d, \r\n", message, lineNo);
        while(_PTPStayAssertLoop != 0);
    }
}
// a debug condition, not really assertion
volatile int _PTPStayCondLoop = 0;
static void _PTPDbgCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("PTP Cond: %s, in line: %d, \r\n", message, lineNo);
        while(_PTPStayCondLoop != 0);
    }
}

#else
#define _PTPAssertCond(cond, message, lineNo)   do{if(cond);}while(0)
#define _PTPDbgCond(cond, message, lineNo)      do{if(cond);}while(0)
#endif  // (TCPIP_PTP_DEBUG_LEVEL)

// Message Process result
typedef enum
{
    TCPIP_PTP_PROC_RES_STATE_ERR,               // the message should be simply discarded because of the port state
    // Announce specific results
    TCPIP_PTP_PROC_RES_ANN_MASTER_UPDATE,       // port was in SLAVE state and a message from the current master updated the data sets
    TCPIP_PTP_PROC_RES_ANN_FOREIGN_UPDATE,      // a foreign master entry was updated
    TCPIP_PTP_PROC_RES_ANN_FOREIGN_NEW,         // a new foreign master entry was created
    TCPIP_PTP_PROC_RES_ANN_FOREIGN_NEW_FAIL,    // creation of a new foreign master entry failed
    // generic results
    TCPIP_PTP_PROC_RES_WRONG_MASTER_ERR,        // the sync message is not from the current master: discard
    TCPIP_PTP_PROC_RES_LOCAL_SYNCHRONIZE,       // the message was used to properly synchronize the slave clock

    // sync specific results
    TCPIP_PTP_PROC_RES_SYNC_2STEP_FOLLOW_UP,    // a 2-step message was received, the follow up message is expected
    
    // follow up specific results
    TCPIP_PTP_PROC_RES_FOLLOW_MATCH_ERR,       // the follow up message is not expected

    // Delay_Req/Delay_Resp specific results
    TCPIP_PTP_PROC_RES_DELAY_RESP_OK,           // success: the Delay_Resp message was sent
    TCPIP_PTP_PROC_RES_DELAY_REQ_OK,            // Delay_Req was sent
    TCPIP_PTP_PROC_RES_DELAY_REQ_RESP_OK,       // Delay_Resp_Req was processed
    TCPIP_PTP_PROC_RES_DELAY_RESP_MATCH_ERR,    // the Delay_Resp message is not expected


}TCPIP_PTP_PROC_MSG_RES;

// message transmit result
typedef enum
{
    TCPIP_PTP_TX_MSG_RES_OK,                    // message transmitted OK

    TCPIP_PTP_TX_MSG_RES_BUFFER_ERR,            // couldn't get a buffer to send the data
                                                // retry
    // 
}TCPIP_PTP_TX_MSG_RES;

// prototypes
//

// compares 2 clock identities
// returns:
//      0  - if CX == CY
//     -1  - if CX < CY
//      1  - if CX > CY
static __inline__ int __attribute__((always_inline)) _PTP_ClockId_Compare(const PTP_CLOCK_IDENTITY* CX, const PTP_CLOCK_IDENTITY* CY)
{
    return memcmp(CX->id, CY->id, sizeof(CX->id));
}

static int      _PTP_Compare_PortId(const PTP_PORT_IDENTITY* PA, const PTP_PORT_IDENTITY* PB);
static int      _PTP_Compare_PortToClockId(const PTP_PORT_IDENTITY* PA, const PTP_CLOCK_IDENTITY* CB);

static bool     _PTP_Message_IsOfClock(const PTP_MESSAGE_HEADER* H,  const TCPIP_PTP_PORT_DCPT* pPort);
static bool     _PTP_Message_Qualify(const TCPIP_PTP_PORT_DCPT* pPort, const PTP_MESSAGE_HEADER* pMsg);

static bool     _PTP_Announce_Validate(const PTP_ANNOUNCE_MESSAGE_DCPT* S);
static bool     _PTP_Announce_Qualify(const TCPIP_PTP_PORT_DCPT* pPort, const PTP_ANNOUNCE_MESSAGE_DCPT* S);
static int      _PTP_Announce_Compare(const PTP_ANNOUNCE_MESSAGE_DCPT* A, const PTP_ANNOUNCE_MESSAGE_DCPT* B, const PTP_PORT_IDENTITY* pRecvId, const TCPIP_PTP_PORT_DCPT* pPort);
static int      _PTP_Announce_Compare2(const PTP_ANNOUNCE_MESSAGE_DCPT* A, const PTP_ANNOUNCE_MESSAGE_DCPT* B, const PTP_PORT_IDENTITY* pRecvId, const TCPIP_PTP_PORT_DCPT* pPort);
static void     _PTP_Announce_FillFromPort(PTP_ANNOUNCE_MESSAGE_DCPT* dstAnn, PTP_PORT_IDENTITY* dstId, const TCPIP_PTP_PORT_DCPT* pPort);
static int      _PTP_Announce_UpdateForeignMaster(TCPIP_PTP_PORT_DCPT* pPort, PTP_ANNOUNCE_MESSAGE_DCPT* pAnnDcpt, PTP_FOREIGN_MASTER_DS** ppForDS);


static PTP_PORT_REC_STATE _PTP_StateDecision(TCPIP_PTP_PORT_DCPT* pPort);
static void     _PTP_StateDecision_Event(void);

static void     _PTP_UpdateDataSet_M1M2(TCPIP_PTP_PORT_DCPT* pPort, PTP_PORT_STATE portState);
static void     _PTP_UpdateDataSet_M3(TCPIP_PTP_PORT_DCPT* pPort, PTP_PORT_STATE portState);
static void     _PTP_UpdateDataSet_P1P2(TCPIP_PTP_PORT_DCPT* pPort, PTP_PORT_STATE portState);
static void     _PTP_UpdateDataSet_SlaveS1(TCPIP_PTP_PORT_DCPT* pPort, const PTP_ANNOUNCE_MESSAGE_DCPT* pAnnDcpt, PTP_PORT_STATE portState);

static TCPIP_PTP_PROC_MSG_RES _PTP_ProcessMessage_Announce(TCPIP_PTP_PORT_DCPT* pPort, PTP_ANNOUNCE_MESSAGE_DCPT* pAnnDcpt);
static TCPIP_PTP_PROC_MSG_RES _PTP_ProcessMessage_Sync(TCPIP_PTP_PORT_DCPT* pPort, PTP_MESSAGE_SYNC* pSyncMsg);
static TCPIP_PTP_PROC_MSG_RES _PTP_ProcessMessage_FollowUp(TCPIP_PTP_PORT_DCPT* pPort, PTP_MESSAGE_FOLLOW_UP* pFollowUpMsg);
static TCPIP_PTP_PROC_MSG_RES _PTP_ProcessMessage_DelayReq(TCPIP_PTP_PORT_DCPT* pPort, PTP_MESSAGE_DELAY_REQ* pDelayReqMsg);
static TCPIP_PTP_PROC_MSG_RES _PTP_ProcessMessage_DelayResp(TCPIP_PTP_PORT_DCPT* pPort, PTP_MESSAGE_DELAY_RESP* pDelayRespMsg);

static void     _PTP_Hton_Timestamp(PTP_TIMESTAMP* pTStamp);
static void     _PTP_HtonMessage_Header(PTP_MESSAGE_HEADER* pHeader);
static void     _PTP_HtonMessage_Announce(PTP_MESSAGE_ANNOUNCE* pMsgAnn);
static void     _PTP_HtonMessage_Sync(PTP_MESSAGE_SYNC* pMsgSync);

static TCPIP_PTP_TX_MSG_RES _PTP_TransmitMessage_Announce(TCPIP_PTP_PORT_DCPT* pPort);
static TCPIP_PTP_TX_MSG_RES _PTP_TransmitMessage_Sync(TCPIP_PTP_PORT_DCPT* pPort);
static TCPIP_PTP_TX_MSG_RES _PTP_TransmitMessage_DelayReq(TCPIP_PTP_PORT_DCPT* pPort);
static TCPIP_PTP_TX_MSG_RES _PTP_TransmitMessage_DelayResp(TCPIP_PTP_PORT_DCPT* pPort, PTP_MESSAGE_DELAY_REQ* pDelayReqMsg);


static void     _PTP_LocalClock_Synchronize(TCPIP_PTP_PORT_DCPT* pPort, PTP_MESSAGE_SYNC_GENERIC* pSyncMsg);
static void     _PTP_DelayReqResp_Mechanism(TCPIP_PTP_PORT_DCPT* pPort, PTP_MESSAGE_DELAY_RESP* pDelayRespMsg);
static void     _PTP_LocalClock_TStampEstimate(PTP_TIMESTAMP* pTStamp);

static void     _PTP_Message_SetHeader(TCPIP_PTP_PORT_DCPT* pPort, PTP_MESSAGE_HEADER* pHdr, PTP_MESSAGE_TYPE msgType, uint16_t msgLen);

static void     _PTP_BMC_Algorithm(void);
static const PTP_ANNOUNCE_MESSAGE_DCPT* _PTP_CalculateErbest(TCPIP_PTP_CLOCK_DCPT* pClock, bool updateClock);


// API 

bool TCPIP_PTP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackInit, const TCPIP_PTP_MODULE_CONFIG* pPtpInit)
{
    return false;
    // @@@@_aa

// @@@@ zuzi: initialize data!
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_PTP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackInit)
{
    // @@@@_aa
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)


// compares 2 port identities
// returns:
//      0  - if PA == PB
//     -1  - if PA < PB
//      1  - if PA > PB
static int _PTP_Compare_PortId(const PTP_PORT_IDENTITY* PA, const PTP_PORT_IDENTITY* PB)
{
    int clkIdRes = _PTP_ClockId_Compare(&PA->clockIdentity, &PB->clockIdentity);

    if(clkIdRes < 0)
    {   // PA < PB
        return -1;
    }
    else if(clkIdRes > 0)
    {   // PA > PB
        return 1;
    }
    // clock ids ==
    else if(PA->portNumber < PB->portNumber)
    {   // PA < PB
        return -1;
    }
    else if(PA->portNumber > PB->portNumber)
    {   // PA > PB
        return 1;
    }

    // PA == PB
    return 0;
}

// compares a port identity PA with a clock identity CB 
// returns:
//     -1  - if PA < CB
//      1  - if PA > CB
static int _PTP_Compare_PortToClockId(const PTP_PORT_IDENTITY* PA, const PTP_CLOCK_IDENTITY* CB)
{
    if(_PTP_ClockId_Compare(&PA->clockIdentity, CB) < 0)
    {   // PA < PB
        return -1;
    }

    // PA > CB
    return 1;
}


// returns true if the PTP message was sent from from any of the ports belonging to this clock
// false otherwise
// - H     - header of the PTP message
// - pPort - port receiving the message
// @@@@ NOTE: on such detection on a boundary clock the conflicting ports should be set to PASSIVE state
//      except the port with the lowest number!!!!
//      On an ordinary clock, this message should be simply ignored
// @@@@: update! Anything else needed
static bool _PTP_Message_IsOfClock(const PTP_MESSAGE_HEADER* H,  const TCPIP_PTP_PORT_DCPT* pPort)
{
    const TCPIP_PTP_CLOCK_DCPT* pClock = pPort->pClock;
    
    if(_PTP_ClockId_Compare(&pClock->defaultDS.clockIdentity, &H->sourcePortIdentity.clockIdentity) == 0)
    {   // belongs to this clock
        return true;
    }

    return false;
} 

// validates any message received on pPort
// returns true is message needs further processing
// false if it needs to be discarded
//
static bool _PTP_Message_Qualify(const TCPIP_PTP_PORT_DCPT* pPort, const PTP_MESSAGE_HEADER* pMsg)
{
    // if pMsg was sent by this port or from any other port on this clock
    // we discard it
    if(_PTP_Message_IsOfClock(pMsg, pPort))
    {
        return false;
    }

    if(pMsg->domainNumber != pPort->pClock->defaultDS.domainNumber)
    {   // not our domain
        return false;
    }


    return true;

}

// validates an Announce Message received on a port
// returns true if message needs to be stored/considered
// returns false if needs to be discarded
// NOTE: However, the alternate master option (see 17.4) specifies other uses for 
// Announce messages with this flag set to TRUE. 
static bool _PTP_Announce_Validate(const PTP_ANNOUNCE_MESSAGE_DCPT* S)
{
    if(S->msg.header.flagField.alternateMasterFlag)
    {
       return false;
    }

    // @@@@ anything else here?
   return true; 
}

// qualifies an Announce Message received on a port to be used for BMCA
// returns true if the message is qualified to be used with BMCA
//
// pPort - the descriptor for the port receiving this message
// S - the Announce message
// @@@@: update!
static bool _PTP_Announce_Qualify(const TCPIP_PTP_PORT_DCPT* pPort, const PTP_ANNOUNCE_MESSAGE_DCPT* S)
{
    // @@@@ if S is not the most recent received on this port from a given clock (?)
    return false;

    // get the foreign master DS associated with this announce message  
    if(S->pForeignDS != 0)
    {   // no associated foreign Master?
        return false;
    }
    _PTPAssertCond(S->pForeignDS->isValid != 0, __func__, __LINE__); 

    if(S->pForeignDS->foreignMasterAnnounceMessages < PTP_FOREIGN_MASTER_THRESHOLD)
    {   // no enough foreign master records (yet?)
        return false; 
    }

   // if the stepsRemoved field of S is >= 255
   if(S->msg.stepsRemoved >= 255)
   return false;


  return true; 

}


// data set comparison algorithm part 1 for 2 Announce messages (Ebest, Erbest)
// in:
//  - A, B - the announce messages
//  - pPort - descriptor of the port receiving the messages
//  - pRecvId   - the receiver Identity to be used for comparison
//  returns:
//      1    A > B - A better than B
//      2    A > B - A better by topology than B
//      0    A = B
//     -1    A < B - B better than A 
//     -2    A < B - B better by topology than A 
// NOTES:
//      - the Announce messages should be != 0!
//        If any is 0, then no comparison is actually needed!
//      - when comparing 2 announce messages
//              the pRecvId should be the parentDS.portIdentity
//              _PTP_Announce_Compare(A, B, &pPort->pClock->parentDS.parentPortIdentity, pPort)
//                  
//      - when comparing D0 with E(r)best the pRecvId is built from the defaultDS
//              _PTP_Announce_Compare(A, B, &recvId, pPort)
//              where A and recvId are built with a call to 
//              _PTP_Announce_FillFromPort()
//
static int _PTP_Announce_Compare(const PTP_ANNOUNCE_MESSAGE_DCPT* A, const PTP_ANNOUNCE_MESSAGE_DCPT* B, const PTP_PORT_IDENTITY* pRecvId, const TCPIP_PTP_PORT_DCPT* pPort)
{
    int gmIdRes = _PTP_ClockId_Compare(&A->msg.grandmasterIdentity, &B->msg.grandmasterIdentity);
    if(gmIdRes == 0)
    {   // GM Identities equal
        return _PTP_Announce_Compare2(A, B, pRecvId, pPort);
    }

    if(A->msg.grandmasterPriority1 < B->msg.grandmasterPriority1)
    {
        return 1;
    }
    else if(A->msg.grandmasterPriority1 > B->msg.grandmasterPriority1)
    {
        return -1;
    }
    // GM Priorities 1 equal

    if(A->msg.grandmasterClockQuality.clockClass < B->msg.grandmasterClockQuality.clockClass)
    {
        return 1;
    }
    else if(A->msg.grandmasterClockQuality.clockClass > B->msg.grandmasterClockQuality.clockClass)
    {
        return -1;
    }
    // GM class values equal

    if(A->msg.grandmasterClockQuality.clockAccuracy < B->msg.grandmasterClockQuality.clockAccuracy)
    {
        return 1;
    }
    else if(A->msg.grandmasterClockQuality.clockAccuracy > B->msg.grandmasterClockQuality.clockAccuracy)
    {
        return -1;
    }
    // GM accuracy values equal

    if(A->msg.grandmasterClockQuality.offsetScaledLogVariance < B->msg.grandmasterClockQuality.offsetScaledLogVariance)
    {
        return 1;
    }
    else if(A->msg.grandmasterClockQuality.offsetScaledLogVariance > B->msg.grandmasterClockQuality.offsetScaledLogVariance)
    {
        return -1;
    }
    // GM offsetScaledLogVariance values equal

    if(A->msg.grandmasterPriority2 < B->msg.grandmasterPriority2)
    {
        return 1;
    }
    else if(A->msg.grandmasterPriority2 > B->msg.grandmasterPriority2)
    {
        return -1;
    }
    // GM Priorities 2 equal
    
    return gmIdRes;
}

// data set comparison algorithm part 2 for 2 Announce messages (Ebest, Erbest)
// in:
//  - A, B - the announce messages
//  - pPort - port receiving this message 
//  - pRecvId   - the receiver Identity to be used for comparison
//
//  returns:
//      1    A > B - A better than B
//      2    A > B - A better by topology than B
//      0    A = B
//     -1    A < B - B better than A 
//     -2    A < B - B better by topology than A 
static int _PTP_Announce_Compare2(const PTP_ANNOUNCE_MESSAGE_DCPT* A, const PTP_ANNOUNCE_MESSAGE_DCPT* B, const PTP_PORT_IDENTITY* pRecvId, const TCPIP_PTP_PORT_DCPT* pPort)
{
    int rcvIdRes, sendIdRes;

    // compare the Steps Removed of A and B
    if(A->msg.stepsRemoved + 1 < B->msg.stepsRemoved)
    {
        return 1;
    }
    else if(A->msg.stepsRemoved > B->msg.stepsRemoved + 1)
    {
        return -1;
    }
    //  B-1 <= A <= B + 1
    
    // compare the Steps Removed of A and B
    if(A->msg.stepsRemoved < B->msg.stepsRemoved)
    {   // compare Receiver of B with Sender of B identities
        rcvIdRes = _PTP_Compare_PortId(pRecvId, &B->msg.header.sourcePortIdentity);
        _PTPAssertCond(rcvIdRes != 0, __func__, __LINE__);  // receiver == sender shouldn't happen!

        if(rcvIdRes < 0)
        {   // A better than B
           return 1;
        }

        return 2;   // A better by topology than B
    }
    else if(A->msg.stepsRemoved > B->msg.stepsRemoved)
    {   // compare Receiver of A with Sender of A identities
        rcvIdRes = _PTP_Compare_PortId(pRecvId, &A->msg.header.sourcePortIdentity);
        _PTPAssertCond(rcvIdRes != 0, __func__, __LINE__);  // receiver == sender shouldn't happen!

        if(rcvIdRes < 0)
        {   // B better than A
           return -1;
        }

        return -2;   // B better by topology than A
    }
    //  A Steps Removed == B Steps Removed

    // compare the identities of senders of A and B
    sendIdRes = _PTP_Compare_PortId(&A->msg.header.sourcePortIdentity, &B->msg.header.sourcePortIdentity);
    if(sendIdRes < 0)
    {
        return 2;   // A better by topology than B
    }
    else if(sendIdRes > 0)
    {
        return -2;   // B better by topology than A
    }
    // Identity A == Identity B

    // Compare port numbers of Receivers of A and B
    // Since the receiver is this clock, the receivers port numbers will always match.
    // this SHOULD not happen! TODO aa: @@@@ ????
    // compare pRecvId->portNumber to itself!
    _PTPAssertCond(false, __func__, __LINE__);

    return 1;
}

// populates an Announce Message and a port identity 
// with data from the receiving port defaultDS
// this allows using the same comparison algorithm between 2 Announce messages
// when we need to compare D0 to an Announce message. 
// in:
//      - dstAnn - the destination message to be populated
//      - dstId  - the destination port ID to be populated
//      - pPort   - the port to be used
static void _PTP_Announce_FillFromPort(PTP_ANNOUNCE_MESSAGE_DCPT* dstAnn, PTP_PORT_IDENTITY* dstId, const TCPIP_PTP_PORT_DCPT* pPort)
{
    const PTP_DEFAULT_DS* pDefDS = &pPort->pClock->defaultDS;

    dstAnn->msg.grandmasterPriority1 = pDefDS->priority1;
    dstAnn->msg.grandmasterPriority2 = pDefDS->priority2;

    memcpy(dstAnn->msg.grandmasterIdentity.id, pDefDS->clockIdentity.id, sizeof(dstAnn->msg.grandmasterIdentity));
    dstAnn->msg.grandmasterClockQuality.clockClass = pDefDS->clockQuality.clockClass;
    dstAnn->msg.grandmasterClockQuality.clockAccuracy = pDefDS->clockQuality.clockAccuracy;
    dstAnn->msg.grandmasterClockQuality.offsetScaledLogVariance = pDefDS->clockQuality.offsetScaledLogVariance;
    dstAnn->msg.stepsRemoved = 0;

    dstAnn->msg.header.sourcePortIdentity.clockIdentity = pDefDS->clockIdentity;
    dstAnn->msg.header.sourcePortIdentity.portNumber = 0;   // cf IEEE-SA - IEEE Std 1588-2008 Interpretation.html: Interpretation Response #2

    dstId->clockIdentity = pDefDS->clockIdentity;
    dstId->portNumber = 0;
}

// @@@@ zuzi: calculate Erbest for a clock ??? !!!!
//
// calculate Ebest accross all ports Erbest and store it to clock::Ebest !!!!
// if updateClock is true, the clock Ebest is updated
static const PTP_ANNOUNCE_MESSAGE_DCPT* _PTP_CalculateErbest(TCPIP_PTP_CLOCK_DCPT* pClock, bool updateClock)
{
    int ix;
    TCPIP_PTP_PORT_DCPT* pPort;

    const PTP_PORT_IDENTITY* pRecvId = &pClock->parentDS.parentPortIdentity;

    const PTP_ANNOUNCE_MESSAGE_DCPT* pEbest = 0; 

    pPort = pClock->portDcpt;
    for(ix = 0; ix < sizeof(pClock->portDcpt) / sizeof(*pClock->portDcpt); ix++, pPort++)
    {
        if((pPort->Erbest.msgFlags & TCPIP_ANNOUNCE_FLAG_EBEST_VALID) != 0)
        {
            pEbest = &pPort->Erbest;    // get the port to start with as the best
            break;
        }
    }

    if(pEbest)
    {
        ix++;
        pPort++;

        for( ; ix < sizeof(pClock->portDcpt) / sizeof(*pClock->portDcpt); ix++, pPort++)
        {
            if(_PTP_Announce_Compare(pEbest, &pPort->Erbest, pRecvId, pPort) < 0)
            {   // current port is better
                pEbest = &pPort->Erbest;
            }
        }
    }

    if(updateClock)
    {
        if(pEbest != 0)
        {
            pClock->Ebest = *pEbest;
        }
        else
        {
            pClock->Ebest.msgFlags &= ~TCPIP_ANNOUNCE_FLAG_EBEST_VALID;
        }
    }

    return pEbest;
}

// State Decision Event function: called when the event occurs!
// is the mechanism for using the data in received Announce messages to determine which is the BMC,
// and whether the local clock port or ports needs to change state. 
// The STATE_DECISION_EVENT shall: 
//  - Logically occur simultaneously on all ports of a clock 
//  - Occur at least once per Announce message transmission interval 
//  - Not occur when any port is in the INITIALIZING state 
// For nodes implementing the default BMCA, prior to or as the first action of 
// the STATE_DECISION_EVENT logic, each port N not in the DISABLED or FAULTY states shall 
// compute an updated value of Erbest (), reflecting the receipt of Announce messages since the last 
// STATE_DECISION_EVENT.
// Following the computation of the set of Erbest for all ports, the clock shall compute Ebest. 
// @@@@_aa: TODO!
static void _PTP_StateDecision_Event(void)
{

#if 0
    if(!all ports computed Erbest since last event)
    {
        each port computes Erbest, if needed;
    }

    Compute Ebest;

    apply the state decision algorithm;
    _PTP_StateDecision(TCPIP_PTP_PORT_DCPT* pPort);

    update data sets for all ports;
    When calling _PTP_UpdateDataSet_SlaveS1(), use the pClock->Ebest as the Ann message!

    make required state changes in all ports

#endif
}

// implements the state decision algorithm for the specified port
// The recommended state event results from the exercise of the BMCA initiated by a STATE_DECISION_EVENT. 
static PTP_PORT_REC_STATE _PTP_StateDecision(TCPIP_PTP_PORT_DCPT* pPort)
{
    PTP_ANNOUNCE_MESSAGE_DCPT portMsg;
    PTP_PORT_IDENTITY    portRecvId;
    TCPIP_PTP_CLOCK_DCPT* pClock;       // clock to which this port belongs to


    if((pPort->Erbest.msgFlags & TCPIP_ANNOUNCE_FLAG_EBEST_VALID) == 0 && pPort->portDS.portState == PTP_PORT_STATE_LISTENING)
    {
       return PTP_PORT_REC_STATE_LISTENING;
    } 

    pClock = pPort->pClock;

    // construct announce message for this port 
    _PTP_Announce_FillFromPort(&portMsg, &portRecvId, pPort);

    if(pClock->defaultDS.clockQuality.clockClass < PTP_CLOCK_CLASS_128_BARRIER)
    {   // our clock is better than 128 class
        if(_PTP_Announce_Compare(&portMsg, &pPort->Erbest, &portRecvId,  pPort) > 0)
        {   // this port better or better by topology than Erbest
            return PTP_PORT_REC_STATE_MASTER_M1;   // M1
        }

        return PTP_PORT_REC_STATE_PASSIVE_P1;      // P1
    } 

    if(_PTP_Announce_Compare(&portMsg, &pClock->Ebest, &portRecvId,  pPort) > 0)
    {   // this port better or better by topology than Ebest
        return PTP_PORT_REC_STATE_MASTER_M2;   // M2
    }

    // nope, Ebest seems to be better
    if(pClock->Ebest.recvPortNumber == pPort->portDS.portIdentity.portNumber)
    {   // Ebest received on this port..
        return PTP_PORT_REC_STATE_SLAVE_S1;        // S1
    }

    if(_PTP_Announce_Compare(&pClock->Ebest, &pPort->Erbest, &pClock->parentDS.parentPortIdentity,  pPort) == 2)
    {   // Ebest better by topology than Erbest
        return PTP_PORT_REC_STATE_PASSIVE_P2;      // P2
    }

    return PTP_PORT_REC_STATE_MASTER_M3;     // M3
} 

// update of the data sets for state decision codes M1 and M2
// pPort is the port which runs the state decision algorithm
// portState is the new reccommended state, as dictated by the clock state machine:
// probably PTP_PORT_STATE_MASTER.
static void _PTP_UpdateDataSet_M1M2(TCPIP_PTP_PORT_DCPT* pPort, PTP_PORT_STATE portState)
{
    TCPIP_PTP_CLOCK_DCPT* pClock = pPort->pClock;

    // currentDS
    PTP_CURRENT_DS* pCurrDS = &pClock->currentDS;
    pCurrDS->stepsRemoved = 0;
    pCurrDS->offsetFromMaster.scaledNanoseconds = 0;
    pCurrDS->meanPathDelay.scaledNanoseconds = 0;

    // parentDS
    PTP_PARENT_DS* pParentDS = &pClock->parentDS;
    PTP_DEFAULT_DS* pDefDS = &pClock->defaultDS;

    pParentDS->parentPortIdentity.clockIdentity = pClock->defaultDS.clockIdentity;
    pParentDS->parentPortIdentity.portNumber = 0;
    pParentDS->grandmasterIdentity = pDefDS->clockIdentity;
    pParentDS->grandmasterClockQuality = pDefDS->clockQuality;
    pParentDS->grandmasterPriority1 = pDefDS->priority1;
    pParentDS->grandmasterPriority2 = pDefDS->priority2;


    // timePropertiesDS
    PTP_TIME_PROPERTIES_DS* pTimeDS = &pClock->timePropertiesDS;

    if((pDefDS->clockQuality.clockClass == PTP_CLOCK_CLASS_PRIMARY_SYNC ) ||
            (pDefDS->clockQuality.clockClass == PTP_CLOCK_CLASS_PRIMARY_SYNC_HOLD ) ||
            (pDefDS->clockQuality.clockClass == PTP_CLOCK_CLASS_PRIMARY_SYNC_DEGRADE_A )) 
    {
        if(pTimeDS->ptpTimescale)
        {   // PTP timescale
            pTimeDS->leap59  = false;       // if known, to the value obtained from a aprimary reference
                                            // NOTE: specific rules of handling leap59 and leap61 on grandmasters!!!
                                            // @@@@ initialization/gps?
            pTimeDS->leap61 = false;        // if known, to the value obtained from a aprimary reference
                                            // NOTE: specific rules of handling leap59 and leap61 on grandmasters!!!
                                            // The update of timePropertiesDS::currentUtcOffset, leap59, and leap61 
                                            // shall occur within ±2 announceIntervals of midnight (UTC). See (9.4).
                                            // @@@@ initialization/gps?
            pTimeDS->currentUtcOffset = 0;  // @@@@ the value obtained from a primary reference
                                            // or the current number of leapSeconds when node is designed!

            pTimeDS->currentUtcOffsetValid = false; // true if is known to be correct, false otherwise
            pTimeDS->ptpTimescale = true;
            pTimeDS->timeTraceable = false;         // true if time is traceable to a primary reference
                                                    // false otherwise

            pTimeDS->frequencyTraceable = false;    // true if frequency is traceable to a primary reference
                                                    // false otherwise
            //
            pTimeDS->timeSource = PTP_TIME_SOURCE_INTERNAL_OSC; // if known, to the appropiate value
                                                                // else internal oscillator
                                                                // @@@@ config?
        }
    }
    else if(!pTimeDS->ptpTimescale)
    {   // ARB timescale
        pTimeDS->leap59  = false;
        pTimeDS->leap61 = false;
        pTimeDS->currentUtcOffset = 0;          // the current number of leapSeconds when node is designed! @@@ config
        pTimeDS->currentUtcOffsetValid = false; // true if is known to be correct, false otherwise
        pTimeDS->ptpTimescale = false;
        pTimeDS->timeTraceable = false;         // true if time is traceable to a primary reference
                                                // false otherwise

        pTimeDS->frequencyTraceable = false;    // true if frequency is traceable to a primary reference
                                                // false otherwise

        pTimeDS->timeSource = PTP_TIME_SOURCE_INTERNAL_OSC; // if known, to the appropiate value
                                                            // else internal oscillator
                                                            // @@@@ config?
    }



    // port DS Data Set
    pPort->portDS.portState = portState;
}

// update of the data sets for state decision code M3
// pPort is the port which runs the state decision algorithm
// portState is the new reccommended state, as dictated by the clock state machine:
// probably PTP_PORT_STATE_MASTER.
static void _PTP_UpdateDataSet_M3(TCPIP_PTP_PORT_DCPT* pPort, PTP_PORT_STATE portState)
{
    // port DS Data Set
    pPort->portDS.portState = portState;
}

// update of the data sets for state decision code P1 and P2
// pPort is the port which runs the state decision algorithm
// portState is the new reccommended state, as dictated by the clock state machine:
// probably PTP_PORT_STATE_PASSIVE.
static void _PTP_UpdateDataSet_P1P2(TCPIP_PTP_PORT_DCPT* pPort, PTP_PORT_STATE portState)
{
    // port DS Data Set
    pPort->portDS.portState = portState;
}

// update of the slave data sets:
//      - for state decision code S1
//      - as a result of receipt of an Announce message -  for regular Slave state
//
//  - pPort is the port which runs the state decision algorithm
//  - pAnnDcpt is the announce message to use for this update:
//      - pClock->Ebest - for S1 state
//      - an announce message received from the current parent clock - for Slave state
// portState is the new reccommended state, as dictated by the clock state machine:
// probably PTP_PORT_STATE_SLAVE.
static void _PTP_UpdateDataSet_SlaveS1(TCPIP_PTP_PORT_DCPT* pPort, const PTP_ANNOUNCE_MESSAGE_DCPT* pAnnDcpt, PTP_PORT_STATE portState)
{
    TCPIP_PTP_CLOCK_DCPT* pClock = pPort->pClock;

    // currentDS
    PTP_CURRENT_DS* pCurrDS = &pClock->currentDS;
    pCurrDS->stepsRemoved = 1 + pClock->Ebest.msg.stepsRemoved;

    // parentDS
    PTP_PARENT_DS* pParentDS = &pClock->parentDS;
    const PTP_MESSAGE_ANNOUNCE* pAnnMsg = &pAnnDcpt->msg;

    pParentDS->parentPortIdentity = pAnnMsg->header.sourcePortIdentity;
    pParentDS->grandmasterIdentity = pAnnMsg->grandmasterIdentity;
    pParentDS->grandmasterClockQuality = pAnnMsg->grandmasterClockQuality;
    pParentDS->grandmasterPriority1 = pAnnMsg->grandmasterPriority1;
    pParentDS->grandmasterPriority2 = pAnnMsg->grandmasterPriority2;
    

    // timePropertiesDS
    PTP_TIME_PROPERTIES_DS* pTimeDS = &pClock->timePropertiesDS;
    pTimeDS->currentUtcOffset = pAnnMsg->currentUtcOffset;
    pTimeDS->currentUtcOffsetValid = pAnnMsg->header.flagField.currentUtcOffsetValid; 
    pTimeDS->leap59 = pAnnMsg->header.flagField.leap59;
    pTimeDS->leap61 = pAnnMsg->header.flagField.leap61;
    pTimeDS->timeTraceable = pAnnMsg->header.flagField.timeTraceable;
    pTimeDS->frequencyTraceable = pAnnMsg->header.flagField.frequencyTraceable;
    pTimeDS->ptpTimescale = pAnnMsg->header.flagField.ptpTimescale;
    pTimeDS->timeSource = pAnnMsg->timeSource;

    // port DS Data Set
    pPort->portDS.portState = portState;

}

// processes an incoming announce message based on the current status
// once the processing is done the announce message can be discarded - @@@@
// return: an announce process result - mainly for trace and debug purposes
// Note: The function also updates the Foreign Master for the announce message!
static TCPIP_PTP_PROC_MSG_RES _PTP_ProcessMessage_Announce(TCPIP_PTP_PORT_DCPT* pPort, PTP_ANNOUNCE_MESSAGE_DCPT* pAnnDcpt)
{

    if(pPort->portDS.portState == PTP_PORT_STATE_INITIALIZING || pPort->portDS.portState == PTP_PORT_STATE_DISABLED || pPort->portDS.portState == PTP_PORT_STATE_FAULTY)
    {   // discard
        return TCPIP_PTP_PROC_RES_STATE_ERR;
    }

    TCPIP_PTP_CLOCK_DCPT* pClock = pPort->pClock;
    if(pPort->portDS.portState == PTP_PORT_STATE_SLAVE)
    {
        if(_PTP_Compare_PortId(&pClock->parentDS.parentPortIdentity, &pAnnDcpt->msg.header.sourcePortIdentity) == 0)
        {   // message from the current parent clock (master)
            // update the data sets
            _PTP_UpdateDataSet_SlaveS1(pPort, pAnnDcpt, PTP_PORT_STATE_SLAVE);
            return TCPIP_PTP_PROC_RES_ANN_MASTER_UPDATE;
        }
    }

    // update the foreign master
    PTP_FOREIGN_MASTER_DS* pForDS;

    int nForMsgs = _PTP_Announce_UpdateForeignMaster(pPort, pAnnDcpt, &pForDS);

    if(nForMsgs != 0)
    {   // updated an existing foreign master
        return TCPIP_PTP_PROC_RES_ANN_FOREIGN_UPDATE;
    }
    else if(pForDS != 0)
    {   // created a new one
        return TCPIP_PTP_PROC_RES_ANN_FOREIGN_NEW;
    }

    // failed to create a new one
    return TCPIP_PTP_PROC_RES_ANN_FOREIGN_NEW_FAIL;
    
}

// searches the foreign Master that belongs to this Announce message
// if found, it increments the # of announce messages
// Else it tries to add a new foreign Master and updates the Announce to point to it
// returns the current number of announce messages in the foreignMaster:
//      > 0 for an existing foreign master
//      = 0 when there's no existing foreign master and a new entry could/not be created
//  ppForDS, if != 0, can store the foreign master address
//  it will be 0 if a new foreign master entry couldn't be created
static int _PTP_Announce_UpdateForeignMaster(TCPIP_PTP_PORT_DCPT* pPort, PTP_ANNOUNCE_MESSAGE_DCPT* pAnnDcpt, PTP_FOREIGN_MASTER_DS** ppForDS)
{
    int ix;
    PTP_FOREIGN_MASTER_DS *pForDS, *pEmptyFor, *pFoundFor;
    PTP_PORT_IDENTITY* pSourcePortIdentity;
    
    
    pSourcePortIdentity = &pAnnDcpt->msg.header.sourcePortIdentity;
    pForDS = pPort->foreignMasterDS;
    pFoundFor = pEmptyFor = 0;
    for(ix = 0; ix < sizeof(pPort->foreignMasterDS) / sizeof(*pPort->foreignMasterDS); ix++, pForDS++)
    {
        if(pForDS->isValid)
        {
            if(_PTP_Compare_PortId(pSourcePortIdentity, &pForDS->foreignMasterPortIdentity) == 0)
            {   // found it
                _PTPAssertCond(pAnnDcpt->pForeignDS == pForDS, __func__, __LINE__); // should match!
                pForDS->foreignMasterAnnounceMessages++;
                pFoundFor = pForDS;
                break;
            }
        }
        else
        {
            pEmptyFor = pForDS;
        }
    }

    if(pFoundFor == 0)
    {   // couldn't find existing master
        if(pEmptyFor != 0)
        {   // but there's room for a new one
            pEmptyFor->foreignMasterPortIdentity = *pSourcePortIdentity;
            pEmptyFor->foreignMasterAnnounceMessages = 0;
            pEmptyFor->isValid = 1;
            pAnnDcpt->pForeignDS = pEmptyFor;
            pFoundFor = pEmptyFor;
        }
        else
        {
            _PTPDbgCond(false, __func__, __LINE__); // ran out of foreign master slots!
        }
    }

    if(ppForDS)
    {
        *ppForDS = pFoundFor;
    }

    return pFoundFor != 0 ? pFoundFor->foreignMasterAnnounceMessages : 0;
}

// process of a sync message received on port pPort based on the current status
// once the processing is done the sync message can be discarded - @@@@
static TCPIP_PTP_PROC_MSG_RES _PTP_ProcessMessage_Sync(TCPIP_PTP_PORT_DCPT* pPort, PTP_MESSAGE_SYNC* pSyncMsg)
{

    if(pPort->portDS.portState == PTP_PORT_STATE_INITIALIZING || pPort->portDS.portState == PTP_PORT_STATE_DISABLED || pPort->portDS.portState == PTP_PORT_STATE_FAULTY)
    {   // discard
        return TCPIP_PTP_PROC_RES_STATE_ERR;
    }

    if(pPort->portDS.portState != PTP_PORT_STATE_SLAVE && pPort->portDS.portState != PTP_PORT_STATE_UNCALIBRATED)
    {   // discard
        return TCPIP_PTP_PROC_RES_STATE_ERR;
    }

    TCPIP_PTP_CLOCK_DCPT* pClock = pPort->pClock;
    if(_PTP_Compare_PortId(&pClock->parentDS.parentPortIdentity, &pSyncMsg->header.sourcePortIdentity) != 0)
    {   // message not from the current master clock (parent)
        return TCPIP_PTP_PROC_RES_WRONG_MASTER_ERR;
    }

    // proper sync message
    // add the path asymmetry
    pSyncMsg->header.correctionField += pPort->pathAsymmetry;

    if(pSyncMsg->header.flagField.twoStepFlag != 0)
    {   // this is a two-step master
        // The port shall capture the sequenceId value of the Sync message
        // @@@@  do we need to queue it, expecting to have more of these???
        pPort->followUpIdentity = pSyncMsg->header.sourcePortIdentity;
        pPort->followUpSequenceId = pSyncMsg->header.sequenceId;
        return TCPIP_PTP_PROC_RES_SYNC_2STEP_FOLLOW_UP;
    }

    // Synchronize local clock!
    _PTP_LocalClock_Synchronize(pPort, (PTP_MESSAGE_SYNC_GENERIC*)pSyncMsg);
    return TCPIP_PTP_PROC_RES_LOCAL_SYNCHRONIZE;
}

// process of a received Follow_UP meessage received on pPort based on the current status
static TCPIP_PTP_PROC_MSG_RES _PTP_ProcessMessage_FollowUp(TCPIP_PTP_PORT_DCPT* pPort, PTP_MESSAGE_FOLLOW_UP* pFollowUpMsg)
{
    if(pPort->portDS.portState == PTP_PORT_STATE_INITIALIZING || pPort->portDS.portState == PTP_PORT_STATE_DISABLED || pPort->portDS.portState == PTP_PORT_STATE_FAULTY)
    {   // discard
        return TCPIP_PTP_PROC_RES_STATE_ERR;
    }

    if(pPort->portDS.portState != PTP_PORT_STATE_SLAVE && pPort->portDS.portState != PTP_PORT_STATE_UNCALIBRATED)
    {   // discard
        return TCPIP_PTP_PROC_RES_STATE_ERR;
    }

    TCPIP_PTP_CLOCK_DCPT* pClock = pPort->pClock;
    if(_PTP_Compare_PortId(&pClock->parentDS.parentPortIdentity, &pFollowUpMsg->header.sourcePortIdentity) != 0)
    {   // message not from the current master clock (parent)
        return TCPIP_PTP_PROC_RES_WRONG_MASTER_ERR;
    }

    // check that we're waiting for this
    if(pFollowUpMsg->header.sequenceId == pPort->followUpSequenceId)
    {
        if(_PTP_Compare_PortId(&pFollowUpMsg->header.sourcePortIdentity, &pPort->followUpIdentity) == 0)
        {   // follow up is associated
            // Synchronize local clock
            _PTP_LocalClock_Synchronize(pPort, (PTP_MESSAGE_SYNC_GENERIC*)pFollowUpMsg);

            // clear the follow up association
            pPort->followUpSequenceId = 0;
            memset(&pPort->followUpIdentity, 0, sizeof(pPort->followUpIdentity));
            return TCPIP_PTP_PROC_RES_LOCAL_SYNCHRONIZE;
        }
    }
    
        return TCPIP_PTP_PROC_RES_FOLLOW_MATCH_ERR;
}

// process of a received Delay_Req meessage received on pPort based on the current status
static TCPIP_PTP_PROC_MSG_RES _PTP_ProcessMessage_DelayReq(TCPIP_PTP_PORT_DCPT* pPort, PTP_MESSAGE_DELAY_REQ* pDelayReqMsg)
{
    if(pPort->portDS.portState == PTP_PORT_STATE_INITIALIZING || pPort->portDS.portState == PTP_PORT_STATE_DISABLED || pPort->portDS.portState == PTP_PORT_STATE_FAULTY)
    {   // discard
        return TCPIP_PTP_PROC_RES_STATE_ERR;
    }

    if(pPort->portDS.portState != PTP_PORT_STATE_MASTER)
    {   // discard
        return TCPIP_PTP_PROC_RES_STATE_ERR;
    }

    return TCPIP_PTP_PROC_RES_DELAY_RESP_OK;
    // @@@@ after that: transmit the Delay_Resp message
    // _PTP_TransmitMessage_DelayResp(pPort, pDelayReqMsg);
}

static TCPIP_PTP_PROC_MSG_RES _PTP_ProcessMessage_DelayResp(TCPIP_PTP_PORT_DCPT* pPort, PTP_MESSAGE_DELAY_RESP* pDelayRespMsg)
{
    if(pPort->portDS.portState == PTP_PORT_STATE_INITIALIZING || pPort->portDS.portState == PTP_PORT_STATE_DISABLED || pPort->portDS.portState == PTP_PORT_STATE_FAULTY)
    {   // discard
        return TCPIP_PTP_PROC_RES_STATE_ERR;
    }

    if(pPort->portDS.portState != PTP_PORT_STATE_SLAVE && pPort->portDS.portState != PTP_PORT_STATE_UNCALIBRATED)
    {   // discard
        return TCPIP_PTP_PROC_RES_STATE_ERR;
    }

    TCPIP_PTP_CLOCK_DCPT* pClock = pPort->pClock;
    if(_PTP_Compare_PortId(&pClock->parentDS.parentPortIdentity, &pDelayRespMsg->header.sourcePortIdentity) != 0)
    {   // message not from the current master clock (parent)
        return TCPIP_PTP_PROC_RES_WRONG_MASTER_ERR;
    }

    // check that we're waiting for this
    if(pDelayRespMsg->header.sequenceId == pPort->delayReqSequenceId)
    {
        if(_PTP_Compare_PortId(&pDelayRespMsg->requestingPortIdentity, &pPort->portDS.portIdentity) == 0)
        {   // Delay_resp is associated with this port
            // perform the Delay request-response mechanism
            _PTP_DelayReqResp_Mechanism(pPort, pDelayRespMsg);
            // clear the Delay_Resp association
            pPort->delayReqSequenceId = 0;

            return TCPIP_PTP_PROC_RES_DELAY_REQ_RESP_OK;
        }
    }

    return TCPIP_PTP_PROC_RES_DELAY_RESP_MATCH_ERR;
}

// populates a message header
// NOTE: initializes the WHOLE data structure - msgLen - to 0!
static void _PTP_Message_SetHeader(TCPIP_PTP_PORT_DCPT* pPort, PTP_MESSAGE_HEADER* pHdr, PTP_MESSAGE_TYPE msgType, uint16_t msgLen)
{
    TCPIP_PTP_CLOCK_DCPT* pClock = pPort->pClock;

    memset(pHdr, 0, msgLen);

    pHdr->messageType = msgType;
    pHdr->versionPTP = pPort->portDS.versionNumber;
    pHdr->messageLength = msgLen;
    pHdr->domainNumber = pClock->defaultDS.domainNumber;

#if 0
    pHdr->flagField.profileSpecific1 = 0;
    pHdr->flagField.profileSpecific2 = 0;   // TODO @@@@ support for specific profiles?
#endif


    if(msgType != PTP_MESSAGE_TYPE_ANNOUNCE && msgType != PTP_MESSAGE_TYPE_SIGNALING && msgType != PTP_MESSAGE_TYPE_MANAGEMENT)
    {   // @@@@
        // pHdr->correctionField : for Sync, etc @@@@ ???? how is this set???
    }

    pHdr->sourcePortIdentity = pPort->portDS.portIdentity;

    // set message specific: PTP_FLAG_FIELD, sequenceId, controlField and logMessageInterval

    // defaults for control field and logMessageInterval
    pHdr->controlField = PTP_CONTROL_ALL_OTHERS;
    pHdr->logMessageInterval = 0x7f;

    switch(msgType)
    {
        case PTP_MESSAGE_TYPE_SYNC:
            pHdr->flagField.twoStepFlag = pClock->defaultDS.twoStepFlag != 0;

            pHdr->sequenceId = pPort->sequenceId_Sync++;
            pHdr->controlField = PTP_CONTROL_SYNC;
            pHdr->logMessageInterval = pPort->portDS.logSyncInterval;
            break;

        case PTP_MESSAGE_TYPE_DELAY_REQ:
            pHdr->sequenceId = pPort->sequenceId_DelayReq++;
            pHdr->controlField = PTP_CONTROL_DELAY_REQ;
            break;

        case PTP_MESSAGE_TYPE_PDELAY_REQ:
            pHdr->sequenceId = pPort->sequenceId_PDelayReq++;
            break;

        case PTP_MESSAGE_TYPE_PDELAY_RESP:
            pHdr->flagField.twoStepFlag = pClock->defaultDS.twoStepFlag != 0;
            pHdr->logMessageInterval = pPort->portDS.logMinDelayReqInterval;
            break;

        case PTP_MESSAGE_TYPE_FOLLOW_UP:
            pHdr->controlField = PTP_CONTROL_FOLLOW_UP;
            pHdr->logMessageInterval = pPort->portDS.logSyncInterval;
            break;

        case PTP_MESSAGE_TYPE_DELAY_RESP:
            pHdr->controlField = PTP_CONTROL_DELAY_RESP;
            break;

        case PTP_MESSAGE_TYPE_ANNOUNCE:
            pHdr->flagField.leap61 = pClock->timePropertiesDS.leap61;
            pHdr->flagField.leap59 = pClock->timePropertiesDS.leap59;
            pHdr->flagField.currentUtcOffsetValid = pClock->timePropertiesDS.currentUtcOffsetValid;
            pHdr->flagField.ptpTimescale = pClock->timePropertiesDS.ptpTimescale;
            pHdr->flagField.timeTraceable = pClock->timePropertiesDS.timeTraceable;
            pHdr->flagField.frequencyTraceable = pClock->timePropertiesDS.frequencyTraceable;

            pHdr->sequenceId = pPort->sequenceId_Announce++;

            pHdr->logMessageInterval = pPort->portDS.logAnnounceInterval;
            break;

        case PTP_MESSAGE_TYPE_SIGNALING:
            pHdr->sequenceId = pPort->sequenceId_Signaling++;
            break;

        case PTP_MESSAGE_TYPE_MANAGEMENT:
            pHdr->sequenceId = pPort->sequenceId_Management++;
            pHdr->controlField = PTP_CONTROL_MANAGEMENT;
            break;

        default:
            break;

    }

}


// transmit an announce message
// A port in the MASTER state shall periodically transmit an Announce message:
// interval in seconds between message transmissions = 2 ^ portDS.logAnnounceInterval
// A node shall, with 90% confidence, issue messages with intervals within ±30% of the 
// value of the interval computed from portDS.logAnnounceInterval. 
//
// returns:
//      - TCPIP_PTP_TX_MSG_RES_OK is message transmitted OK
//      - TCPIP_PTP_TX_MSG_RES_BUFFER_ERR if no buffer available and a retry is needed
//
static TCPIP_PTP_TX_MSG_RES _PTP_TransmitMessage_Announce(TCPIP_PTP_PORT_DCPT* pPort)
{
    TCPIP_PTP_CLOCK_DCPT* pClock = pPort->pClock;
    PTP_MESSAGE_ANNOUNCE msgAnn;


    UDP_SOCKET s = pClock->clockSkt;
    if(TCPIP_UDP_PutIsReady(s) < sizeof(msgAnn))
    {   // not enough room in the buffer; @@@@ shouldn't happen ???
        return TCPIP_PTP_TX_MSG_RES_BUFFER_ERR;
    }

    _PTP_Message_SetHeader(pPort, &msgAnn.header, PTP_MESSAGE_TYPE_ANNOUNCE, sizeof(msgAnn));

    _PTP_LocalClock_TStampEstimate(&msgAnn.originTimestamp);
    

    msgAnn.currentUtcOffset = pClock->timePropertiesDS.currentUtcOffset;

    msgAnn.grandmasterPriority1 = pClock->parentDS.grandmasterPriority1;

    msgAnn.grandmasterClockQuality = pClock->parentDS.grandmasterClockQuality;

    msgAnn.grandmasterPriority2 = pClock->parentDS.grandmasterPriority2;

    msgAnn.grandmasterIdentity = pClock->parentDS.grandmasterIdentity;

    msgAnn.stepsRemoved = pClock->currentDS.stepsRemoved;

    msgAnn.timeSource = pClock->timePropertiesDS.timeSource;


    _PTP_HtonMessage_Announce(&msgAnn);

    // @@@@@ any adjustment to this socket: where do I send this message?
    TCPIP_UDP_ArrayPut(s, (uint8_t*)&msgAnn, sizeof(msgAnn));
    uint16_t flushLen = TCPIP_UDP_Flush(s);

    _PTPAssertCond(flushLen == sizeof(msgAnn), __func__, __LINE__);


    return TCPIP_PTP_TX_MSG_RES_OK; 

}

// convert host <-> network a message header 
static void _PTP_HtonMessage_Header(PTP_MESSAGE_HEADER* pHeader)
{
    pHeader->messageLength = TCPIP_Helper_htons(pHeader->messageLength);
    pHeader->flagField.val = TCPIP_Helper_htons(pHeader->flagField.val);
    pHeader->sourcePortIdentity.portNumber = TCPIP_Helper_htons(pHeader->sourcePortIdentity.portNumber);
    pHeader->sequenceId = TCPIP_Helper_htons(pHeader->sequenceId);
}

// convert host <-> network a timestamp
static void _PTP_Hton_Timestamp(PTP_TIMESTAMP* pTStamp)
{
    pTStamp->secondsField_16 = TCPIP_Helper_htons(pTStamp->secondsField_16);
    pTStamp->secondsField_32 = TCPIP_Helper_htonl(pTStamp->secondsField_32);
    pTStamp->nanosecondsField = TCPIP_Helper_htonl(pTStamp->nanosecondsField);
}

// convert host <-> network an Announce message 
static void _PTP_HtonMessage_Announce(PTP_MESSAGE_ANNOUNCE* pMsgAnn)
{
    // header
    _PTP_HtonMessage_Header(&pMsgAnn->header);

    // timestamp
    _PTP_Hton_Timestamp(&pMsgAnn->originTimestamp);

    pMsgAnn->currentUtcOffset = TCPIP_Helper_htons(pMsgAnn->currentUtcOffset);
    pMsgAnn->grandmasterClockQuality.offsetScaledLogVariance = TCPIP_Helper_htons(pMsgAnn->grandmasterClockQuality.offsetScaledLogVariance);
    pMsgAnn->stepsRemoved = TCPIP_Helper_htons(pMsgAnn->stepsRemoved);
}


// transmit a sync message
// A port in the MASTER state shall periodically transmit a sync message:
// interval in seconds between message transmissions = 2 ^ portDS.logSyncInterval
// A node shall, with 90% confidence, issue messages with intervals within ±30% of the 
// value of the interval computed from portDS.logSyncInterval. 
// 
// returns:
//      - TCPIP_PTP_TX_MSG_RES_OK is message transmitted OK
//      - TCPIP_PTP_TX_MSG_RES_BUFFER_ERR if no buffer available and a retry is needed
//
static TCPIP_PTP_TX_MSG_RES _PTP_TransmitMessage_Sync(TCPIP_PTP_PORT_DCPT* pPort)
{
    TCPIP_PTP_CLOCK_DCPT* pClock = pPort->pClock;
    PTP_MESSAGE_SYNC msgSync;


    UDP_SOCKET s = pClock->clockSkt;
    if(TCPIP_UDP_PutIsReady(s) < sizeof(msgSync))
    {   // not enough room in the buffer; @@@@ shouldn't happen ???
        return TCPIP_PTP_TX_MSG_RES_BUFFER_ERR;
    }


    _PTP_Message_SetHeader(pPort, &msgSync.header, PTP_MESSAGE_TYPE_SYNC, sizeof(msgSync));


    // The EgressTimestamp will be set by the MAC: _PTP_LocalClock_TStampEstimate(&msgSync.originTimestamp);

    _PTP_HtonMessage_Sync(&msgSync);

    // @@@@@ any adjustment to this socket: where do I send this message?
    TCPIP_UDP_ArrayPut(s, (uint8_t*)&msgSync, sizeof(msgSync));
    uint16_t flushLen = TCPIP_UDP_Flush(s);

    _PTPAssertCond(flushLen == sizeof(msgSync), __func__, __LINE__);


    return TCPIP_PTP_TX_MSG_RES_OK; 

}

// convert host <-> network an Sync message 
static void _PTP_HtonMessage_Sync(PTP_MESSAGE_SYNC* pMsgSync)
{
    // header
    _PTP_HtonMessage_Header(&pMsgSync->header);

    // timestamp
    _PTP_Hton_Timestamp(&pMsgSync->originTimestamp);

}

static TCPIP_PTP_TX_MSG_RES _PTP_TransmitMessage_DelayReq(TCPIP_PTP_PORT_DCPT* pPort)
{
    // @@@@@ TODO
    // @@@@ save pPort->delayReqSequenceId !
    return TCPIP_PTP_TX_MSG_RES_OK;
}

// transmit a Delay_Resp message as a result of receiving a Delay_Req
static TCPIP_PTP_TX_MSG_RES _PTP_TransmitMessage_DelayResp(TCPIP_PTP_PORT_DCPT* pPort, PTP_MESSAGE_DELAY_REQ* pDelayReqMsg)
{
    // @@@@@ TODO
    return TCPIP_PTP_TX_MSG_RES_OK;
}


// synchronize the slave/uncalibrated port to the incoming sync message
static void _PTP_LocalClock_Synchronize(TCPIP_PTP_PORT_DCPT* pPort, PTP_MESSAGE_SYNC_GENERIC* pSyncMsg)
{
    // calculate offsetFromMaster
    // minimize offsetFromMaster!
}

// performs the Delay request-response mechanism
static void _PTP_DelayReqResp_Mechanism(TCPIP_PTP_PORT_DCPT* pPort, PTP_MESSAGE_DELAY_RESP* pDelayRespMsg)
{
    // @@@@@ TODO
    // execute the Delay request-response mechanism
    // based on the contents of the  received Delay_Resp and associated Delay_Req messages

    // Update the value of portDS.logMinDelayReqInterval
    // to the value of the logMessageInterval member of the Delay_Resp message. 
    
}

// get the local clock timestamp estimation (usually for a TX message)
//
// This timestamp could be 0 or an estimate no worse than ±1 s
// of the local time of the originating clock when
// the message was transmitted.
static void _PTP_LocalClock_TStampEstimate(PTP_TIMESTAMP* pTStamp)
{
#if (TCPIP_PTP_TX_TIMESTAMP_ESTIMATE != 0)
    // @@@@ read the timestamp from the local clock (GMAC)
#else
    memset(pTStamp, 0, sizeof(*pTStamp));
#endif  // (TCPIP_PTP_TX_TIMESTAMP_ESTIMATE != 0)

}


// PTP BMC algorithm - BMCA
// 1. is run locally on all ports of a ordinary or boundary clock
//    it runs continuously, and readapts to changes in networks or clocks
// 2. The algorithm runs independently on each clock in a domain.
//    In other words, clocks do not negotiate which should be master and which should be slave
//    instead, each computes only the state of its own ports.
//    The algorithm avoids configurations with two masters, or none, or ones that thrash. 
// 3. Consists of 2 parts:
//      - data set comparison algorithm - determine which of the 2 clocks is better
//      - alg that computes the recommended state for the state of each port involved
//    Based on information contained in Announce messages received and on the default DS data set values of the given clock
//
static void _PTP_BMC_Algorithm(void)
{
}


// @@@@@@@@@@@@@@@@@@@@@@@ aa Notes:
#if 0
    - A port may implement both the delay request-response and the peer delay mechanisms
      PROVIDED only one mechanism is active at any time.
      The method of selection could be configuration/autoconfiguration. 

    - An ordinary or boundary clock that receives an Announce, Sync, Follow_Up, or Delay_Resp message
      in which the value of the header flag, alternateMasterFlag, is TRUE
      SHALL DISCARD the message except as provided in Clause 17.

    - An ordinary clock may be designed to be a SLAVE-ONLY or a NON-SLAVE-ONLY clock.
      Could be configuration  or via the management message SLAVE_ONLY.

      A slave-only clock can never enter the MASTER state.
      A boundary clock SHALL NOT be a slave-only clock.
    
    - Check the
        IEEE-SA - IEEE Std 1588-2008 Interpretation.html
        for clarifications and fixes to the standard!!!!


#endif

// test function
int    PTP_Test(void)
{
    PTP_ANNOUNCE_MESSAGE_DCPT A, B, C;
    PTP_PORT_IDENTITY recvPort;
    PTP_MESSAGE_SYNC sync1;
    PTP_MESSAGE_FOLLOW_UP follow1;
    PTP_MESSAGE_DELAY_REQ req1;
    PTP_MESSAGE_DELAY_RESP resp1;

    TCPIP_PTP_PORT_DCPT* pPort = ptpClockDcpt.portDcpt + 0;

    _PTP_Announce_FillFromPort(&A, &recvPort, ptpClockDcpt.portDcpt + 0);
    memset(&B, 0, sizeof(B));
    memset(&C, 0, sizeof(C));
    _PTP_Announce_Validate(&A);
    _PTP_Announce_Validate(&B);
    _PTP_Announce_Validate(&C);
    
    _PTP_Message_Qualify(pPort, &sync1.header);
    _PTP_Announce_Qualify(pPort, &A);

    int res1 = _PTP_Compare_PortToClockId(&pPort->pClock->parentDS.parentPortIdentity, &A.msg.grandmasterIdentity);
    res1 |= _PTP_Announce_Compare(&A, &B, &recvPort,  pPort);
    int res2 = _PTP_Announce_Compare(&B, &C, &pPort->pClock->parentDS.parentPortIdentity, pPort);

    PTP_PORT_REC_STATE newState = _PTP_StateDecision(pPort);

    _PTP_BMC_Algorithm();
    _PTP_CalculateErbest(&ptpClockDcpt, true);
    _PTP_StateDecision_Event();
    _PTP_UpdateDataSet_M1M2(pPort, PTP_PORT_STATE_MASTER);
    _PTP_UpdateDataSet_M3(pPort, PTP_PORT_STATE_MASTER);
    _PTP_UpdateDataSet_P1P2(pPort, PTP_PORT_STATE_PASSIVE);
    _PTP_UpdateDataSet_SlaveS1(pPort, &A, PTP_PORT_STATE_SLAVE);
    _PTP_ProcessMessage_Announce(pPort, &A);
    _PTP_ProcessMessage_Sync(pPort, &sync1);
    _PTP_ProcessMessage_FollowUp(pPort, &follow1);
    _PTP_ProcessMessage_DelayReq(pPort, &req1);
    _PTP_ProcessMessage_DelayResp(pPort, &resp1);
    _PTP_TransmitMessage_DelayResp(pPort, &req1);
    _PTP_TransmitMessage_DelayReq(pPort);
    _PTP_TransmitMessage_Announce(pPort);
    _PTP_TransmitMessage_Sync(pPort);

    if(res1 > res2)
    {
        return +1;
    }
    else if(res1 < res2)
    {
        return -1;
    }

    if(newState == PTP_PORT_REC_STATE_LISTENING)
    {
        return sizeof(PTP_MESSAGE_GENERIC);
    }
    
    return -1;
    

}

#endif  // defined(TCPIP_STACK_USE_PTP)

