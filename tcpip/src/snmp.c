/*******************************************************************************
  Simple Network Management Protocol (SNMP) Version 1 Agent
  Simple Network Management Protocol (SNMP) Version 2 community based Agent

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    -Provides SNMP API for doing stuff
    -Reference: RFC 1157 (for SNMP V1)
                RFC 3416 (for SNMPv2C)
*******************************************************************************/

/*
Copyright (C) 2012-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutorynwarranties, including, but not
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

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_SNMP_SERVER

#include "tcpip/src/tcpip_private.h"
#if defined(TCPIP_STACK_USE_SNMP_SERVER)
#include "tcpip/src/snmp_private.h"
#if defined(TCPIP_STACK_USE_SNMPV3_SERVER)
#include "tcpip/src/snmpv3_private.h"
#endif
#include "tcpip/src/common/sys_fs_shell.h"
#include "tcpip/snmp.h"
#include "mib.h"


#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
#include "tcpip/snmpv3.h"
#endif



/****************************************************************************
  Section:
    Global Variables
  ***************************************************************************/
static uint16_t SNMPTxOffset;   //Snmp udp buffer tx offset
static uint16_t SNMPRxOffset;

static SNMP_STATUS SNMPStatus;  //MIB file access status

static reqVarErrStatus snmpReqVarErrStatus;

static uint8_t getBuffer[TCPIP_SNMP_MAX_MSG_SIZE+16+1];

#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
static uint16_t msgSecrtyParamLenOffset;
static SNMPV3_STACK_DCPT_STUB* Snmpv3StkStubPtr=NULL;
#endif 


static const void*  SnmpStackMemH = NULL;        // memory handle

static SNMP_STACK_DCPT_STUB*  SnmpStackDcptMemStubPtr=NULL;
TCPIP_SNMP_DCPT gSnmpDcpt;

// file shell object for file access
const SYS_FS_SHELL_OBJ*  snmpFileShell = NULL;
static const char *snmpMountPath = TCPIP_SNMP_MOUNT_POINT;
#if defined (TCPIP_STACK_USE_IPV6)
static void SNMP_IPV6_Notify(TCPIP_NET_HANDLE hNet, IPV6_EVENT_TYPE evType, const void* evParam, const void* usrParam);
#endif

// SNMP stack Error enum value
typedef enum
{
    SNMP_STACK_FAIL=0,
    SNMP_STACK_NO_ERROR,             
    SNMP_PKT_IS_NOTPRESENT,
}SNMP_STACK_ERROR;

// SNMP state machine for TCPIP_SNMP_ProcessVariables
typedef enum
{
    SM_PKT_STRUCT_LEN_OFFSET=0u,
    SM_RESPONSE_PDU_LEN_OFFSET,
    SM_ERROR_STATUS_OFFSET,
    SM_ERROR_INDEX_OFFSET,
    SM_FIND_NO_OF_REQUESTED_VARBINDS,
    SM_FIND_NO_OF_RESPONSE_VARBINDS,
    SM_VARBIND_STRUCT_OFFSET,
    SM_VARSTRUCT_LEN_OFFSET,
    SM_POPULATE_REQ_OID,
    SM_FIND_OID_IN_MIB,
    SM_NON_REPETITIONS,
    SM_MAX_REPETITIONS
}SM_SNMP;

//ASN format datatype for snmp v1 and v2c
static const SNMP_DATA_TYPE_INFO dataTypeTable[] =
{
    { ASN_INT,           1       }, //INT8_VAL          
    { ASN_INT,           2       }, //INT16_VAL         
    { ASN_INT,           4       }, //INT32_VAL         
    { OCTET_STRING,      0xffU    }, //BYTE_ARRAY        
    { OCTET_STRING,      0xffU    }, //ASCII_ARRAY       
    { SNMP_IP_ADDR,      4       }, //IPADDRESS        
    { SNMP_COUNTER32,    4       }, //COUNTER32         
    { SNMP_TIME_TICKS,   4       }, //TIME_TICKS_VAL    
    { SNMP_GAUGE32,      4       }, //GAUTE32           
    { ASN_OID,           0xffU    }  //OID_VAL           
};

static int snmpInitCount = 0;      // SNMP module initialization count

static SYS_FS_HANDLE snmpFileDescrptr = SYS_FS_HANDLE_INVALID;
static SYS_FS_HANDLE snmpTrapFileDescriptr =  SYS_FS_HANDLE_INVALID;

/****************************************************************************
  Section:
    Function Prototypes
  ***************************************************************************/
static uint8_t TCPIP_SNMP_OIDsCountGet(uint16_t pdulen);
static SNMP_ERR_STATUS TCPIP_SNMP_ProcessPDUHeader(PDU_INFO * pduDbPtr, char* community, uint8_t* len);
static bool TCPIP_SNMP_ProcessHeaderGetSet(PDU_INFO * pduDbPtr);
static bool TCPIP_SNMP_ProcessVariables(PDU_INFO * pduDbPtr,char* community, uint8_t len);
static bool F_SNMP_CommunityConfiguration(const TCPIP_SNMP_MODULE_CONFIG *snmpDataConfig);
static void F_SNMP_Trap_Initialize(void);
static bool TCPIP_SNMP_CommunityStringIsValid(char* community, uint8_t* len);
static bool TCPIP_SNMP_PDUIsValid(SNMP_ACTION* pdu);
static bool TCPIP_SNMP_OIDStringGetByAddr(SYS_FS_HANDLE fileDescr,OID_INFO* rec, uint8_t* oidString, uint8_t* len);
static void TCPIP_SNMP_MIBRecordRead(SYS_FS_HANDLE fileDescr,uint32_t h, OID_INFO* rec);
static void TCPIP_SNMP_ErrorStatusSet(uint16_t errorStatusOffset, uint16_t errorIndexOffset,SNMP_ERR_STATUS errorStatus,uint8_t errorIndex,SNMP_BUFFER_DATA *snmpPutTxData);
static bool TCPIP_SNMP_PvtMIBObjIsRequested(uint8_t* OIDValuePtr);
static bool TCPIP_SNMP_MemoryAllocate(const TCPIP_STACK_MODULE_CTRL* const stackData);
static void TCPIP_SNMP_CreateTrapSocket(void);

static void F_SnmpEnable(void);
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
static bool F_SNMP_ValidatePktReceivedIntf(const TCPIP_NET_IF *pNetIfFromDcpt);
static bool F_SNMPV3_TRAP_Initialize(const TCPIP_SNMP_MODULE_CONFIG *snmpDataConfig);
static bool F_SNMPV3_Stack_parameterInit(const TCPIP_SNMP_MODULE_CONFIG *snmpDataConfig);
static void F_SNMPv3_CreatePasswordLocalizationKey(const TCPIP_NET_IF* pNetIf);
#endif /* TCPIP_STACK_USE_SNMPV3_SERVER */

static void TCPIP_SNMP_Process(void);
static void F_SNMPSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);

uint32_t F_SNMP_BuildIntegerLen(uint32_t integerVal);
uint32_t F_SNMP_BuildIntegerValue(SNMP_VAL inputVal,uint32_t dataLen);

/****************************************************************************
  ===========================================================================
  Section:
    SNMP v1 and v2c Agent Routines
  ===========================================================================
  ***************************************************************************/
uint8_t TCPIP_SNMP_GetDataFromUDPBuff(TCPIP_SNMP_DATABUF *getbuf)
{
    uint8_t val = 0;
    getbuf->wrPtr = getbuf->head+SNMPRxOffset;
    val = getbuf->wrPtr[0];
    getbuf->wrPtr = getbuf->wrPtr+1;
    SNMPRxOffset = FC_PtrDiff2UI16(getbuf->wrPtr - getbuf->head);
    return val;
}

int TCPIP_SNMP_GetArrayOfDataFromUDPBuff(TCPIP_SNMP_DATABUF *getbuf,int bytes,uint8_t *buf)
{
    int nBytes = 0;

    getbuf->wrPtr = getbuf->head+SNMPRxOffset;

    nBytes = getbuf->endPtr - getbuf->wrPtr;
    if(bytes < nBytes)
    {
        nBytes =  bytes;
    }

    if(buf == NULL)
    {
        getbuf->wrPtr = getbuf->wrPtr+nBytes;
        return 0;
    }
    (void)memcpy(buf,getbuf->wrPtr, (size_t)nBytes);
    getbuf->wrPtr = getbuf->wrPtr+nBytes;
    SNMPRxOffset = FC_PtrDiff2UI16(getbuf->wrPtr - getbuf->head);
    return nBytes;
}

void TCPIP_SNMP_CopyOfDataToINUDPBuff(TCPIP_SNMP_DATABUF *getbuf,int val)
{
    getbuf->wrPtr = getbuf->head+SNMPRxOffset;
    getbuf->wrPtr[0] = (uint8_t)val;
    getbuf->wrPtr = getbuf->wrPtr+1;
    SNMPRxOffset = FC_PtrDiff2UI16(getbuf->wrPtr - getbuf->head);
}



/**
 * int32_t F_SNMP_BuildIntegerLen(uint32_t integerVal)
 *
 * builds an ASN object containing an integer
 *
 * \return 0 if OK, -1 on error.
 **/
uint32_t F_SNMP_BuildIntegerLen(uint32_t integerVal) /* integer value */
{
    uint32_t intSize=0;
    intSize = sizeof (uint32_t);
    /*
     * Truncate "unnecessary" bytes off of the most significant end of this
     * 2's complement integer.
     * There should be no sequence of 9 consecutive 1's or 0's at the most
     * significant end of the integer.
     */
    while( ( (integerVal & SNMP_MASK_FF8) == 0U || (integerVal & SNMP_MASK_FF8) == SNMP_MASK_FF8) && intSize > 1U)
    {
        intSize--;
        integerVal <<= 8;
    }
    
    return intSize;
}
 /*
 * ParseLength:
 * Parse length for all the data types
 * 
 */
static uint32_t F_SNMP_ParseLength(void)
{
    uint32_t    length;
    uint32_t    lenlen;
    uint32_t    i;

    length = (uint32_t)TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);

    if (length < 0x80U)
    {
        return (length);
    }

    /* long form. */
    lenlen = length & 0x7fUL;

    if ((lenlen > 4UL) || (lenlen < 1UL)) 
    {   
        return 0xffffffffUL;
    }

    length = 0;
    for (i = 0; i < lenlen; i++) 
    {
        length = (length << 8) + (uint32_t)TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
    }
   
    if((int32_t)length < 0) 
    {
        return 0xffffffffUL;
    }

    return (length);
}/* end of ParseLength() */


/*
 * Parse SNMP integer
 * For range 0->127(0xf7) , the length will be 1 . 
 * For  128(0x80 )-> 32767(0x7fff) , the length will be 2.
 * For range 0x8000 to 0x7fffff, the length will be 3 . 
 * For range 0x80000000 to 0x 7fffffff , the length will be 4.
 */
static uint32_t F_SNMP_ParseInteger(uint32_t dataLen,uint32_t *origSetValue)
{
    uint8_t byte_value;
    uint32_t    value=0;
    uint32_t sign;
    uint32_t i=0,temp=0;
    
    
    byte_value = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
    sign = ((byte_value & 0x80U) == 0x00U) ? 0x00U : 0xffU;
    value = (value << 8) + byte_value;
    for (i = 1; i < dataLen; i++) 
    {
        temp = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
        value = (value << 8) + temp;
    }
    *origSetValue = value;
    /* now fill in the upper bits with the appropriate sign extension. */
    
    for (i = dataLen; i < 4U; i++) 
    {
        value = value + (sign << i * 8U);
    }
    
    /* this is after two's complement of the incoming SET value .*/
    return value;
}

uint32_t F_SNMP_BuildIntegerValue(SNMP_VAL inputVal,uint32_t dataLen)
{
    uint32_t i;
    
    if((inputVal.v[3] & 0x0ffU) == 0xffU)
    {
        for (i = dataLen; i < 4U; i++) 
        {
            if((inputVal.v[i] & 0x80U) != 0U)
            {
                inputVal.v[i] = 0;
            }                
        }           
    }

    return inputVal.dword;
}


#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_SNMP_FreeMemory(void)
{
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
    if(Snmpv3StkStubPtr->PduHeaderBuf.head != NULL)
    {
        (void)TCPIP_HEAP_Free(SnmpStackMemH, Snmpv3StkStubPtr->PduHeaderBuf.head);
        Snmpv3StkStubPtr->PduHeaderBuf.head=NULL;
        Snmpv3StkStubPtr->PduHeaderBuf.length=0;
    }
    if(Snmpv3StkStubPtr->ScopedPduRespnsBuf.head != NULL)
    {
        (void)TCPIP_HEAP_Free(SnmpStackMemH, Snmpv3StkStubPtr->ScopedPduRespnsBuf.head);
        Snmpv3StkStubPtr->ScopedPduRespnsBuf.head=NULL;
        Snmpv3StkStubPtr->ScopedPduRespnsBuf.length = 0;
    }
    if(Snmpv3StkStubPtr->TrapMsgHeaderBuf.head != NULL)
    {
        (void)TCPIP_HEAP_Free(SnmpStackMemH, Snmpv3StkStubPtr->TrapMsgHeaderBuf.head);
        Snmpv3StkStubPtr->TrapMsgHeaderBuf.length=0;
        Snmpv3StkStubPtr->TrapMsgHeaderBuf.head = NULL;
    }
    if(Snmpv3StkStubPtr->TrapScopdPduRespnsBuf.head != NULL)
    {
        (void)TCPIP_HEAP_Free(SnmpStackMemH, Snmpv3StkStubPtr->TrapScopdPduRespnsBuf.head);
        Snmpv3StkStubPtr->TrapScopdPduRespnsBuf.length = 0;
        Snmpv3StkStubPtr->TrapScopdPduRespnsBuf.head = NULL;
    }
#endif
    if(SnmpStackDcptMemStubPtr != NULL)
    {
        (void)TCPIP_HEAP_Free(SnmpStackMemH, SnmpStackDcptMemStubPtr);
        SnmpStackDcptMemStubPtr = NULL;
    }
    if(gSnmpDcpt.snmpSignalHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(gSnmpDcpt.snmpSignalHandle);
        gSnmpDcpt.snmpSignalHandle = NULL;
    }   
    if(SNMPStatus.Flags.bIsFileOpen != 0U)
    {
        (void)(snmpFileShell->fileClose)(snmpFileShell,snmpFileDescrptr);
        snmpFileDescrptr = (int32_t) SYS_FS_HANDLE_INVALID;
        SNMPStatus.Flags.bIsFileOpen = 0U;
    }

    if(SNMPStatus.Flags.bIsSnmpIntfUp != 0U)
    {
        SNMPStatus.Flags.bIsSnmpIntfUp = 0U;
    }
    if(snmpFileShell != NULL)
    {
        (void)(*snmpFileShell->delete)(snmpFileShell);
        snmpFileShell = NULL;
    }
    
}
#else
#define TCPIP_SNMP_FreeMemory()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

// SNMP enable after stack TCPIP_STACK_ACTION_IF_UP  where all the UDP sockets are closed.
static void F_SnmpEnable(void)
{
    // Open a SNMP agent socket
     if(gSnmpDcpt.skt == INVALID_UDP_SOCKET)
     {
        gSnmpDcpt.skt = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_ANY, SNMP_AGENT_PORT, NULL);
        (void)TCPIP_UDP_SignalHandlerRegister(gSnmpDcpt.skt, TCPIP_UDP_SIGNAL_RX_DATA, &F_SNMPSocketRxSignalHandler, NULL);
     }   
    
// SNMP Trap socket initialization
    if(gSnmpDcpt.trapEnable)
    {
        F_SNMP_Trap_Initialize();
    }

}

bool TCPIP_SNMP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackData, const void* initData)
{   
    if(stackData->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   
        // interface restart
        F_SnmpEnable();
        return true;
    }   

    if(snmpInitCount == 0)
    { 
        if(initData == NULL)
        {
            return false;
        }
        const TCPIP_SNMP_MODULE_CONFIG* snmpData = (const TCPIP_SNMP_MODULE_CONFIG*)initData;
        SnmpStackMemH = stackData->memH;
        if(TCPIP_SNMP_MemoryAllocate(stackData) == false)
        {
            return false;
        }
        SNMPStatus.Val = 0;

        gSnmpDcpt.readFromSnmpBuf = false;
        gSnmpDcpt.skt = INVALID_UDP_SOCKET;
        gSnmpDcpt.trapEnable = snmpData->trapEnable;
        gSnmpDcpt.snmp_trapv2_use = snmpData->snmp_trapv2_use;
        gSnmpDcpt.snmpv3_trapv1v2_use = snmpData->snmpv3_trapv1v2_use;
        gSnmpDcpt.pSnmpIf = NULL;
        
// SNMP initialization
        (void)F_SNMP_CommunityConfiguration(snmpData);
        if(gSnmpDcpt.trapEnable)
        {
            F_SNMP_Trap_Initialize();
        }
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
        TCPIP_SNMPv3_Initialize();
        if(F_SNMPV3_Stack_parameterInit(snmpData) != true)
        {
            TCPIP_SNMP_FreeMemory();
            return false;
        }
        if((gSnmpDcpt.trapEnable) && (gSnmpDcpt.snmpv3_trapv1v2_use) && (gSnmpDcpt.snmp_trapv2_use))
        {
            if(F_SNMPV3_TRAP_Initialize(snmpData) != true)
            {
                TCPIP_SNMP_FreeMemory();
                return false;
            }
        }
#endif      
        if(gSnmpDcpt.snmpSignalHandle == NULL)
        {
        // once per service
            gSnmpDcpt.snmpSignalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_SNMP_Task, TCPIP_SNMP_TASK_PROCESS_RATE);
            if(gSnmpDcpt.snmpSignalHandle == NULL)
            {
                TCPIP_SNMP_FreeMemory();
                return false;
            }
        }
        // Open a SNMP agent socket
        gSnmpDcpt.skt = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_ANY, SNMP_AGENT_PORT, NULL);
        if(gSnmpDcpt.skt == INVALID_UDP_SOCKET)
        {
            return false;
        }
        // create the SYS_FS shell
        SYS_FS_SHELL_RES shellRes;
        if(snmpData->mountPath != NULL)
        {
            snmpMountPath = snmpData->mountPath;
        }
        snmpFileShell = (const SYS_FS_SHELL_OBJ*)SYS_FS_Shell_Create(snmpMountPath,SYS_FS_SHELL_FLAG_NONE,NULL,NULL,&shellRes);
        if(snmpFileShell == NULL)
        {
            SYS_ERROR(SYS_ERROR_ERROR, " SNMP: Wrapper object failure : %d",shellRes);
            TCPIP_SNMP_FreeMemory();
            return false;
        }          
        (void)TCPIP_UDP_SignalHandlerRegister(gSnmpDcpt.skt, TCPIP_UDP_SIGNAL_RX_DATA, &F_SNMPSocketRxSignalHandler, NULL);
        // As we process SNMP variables, we will prepare response on-the-fly
        // creating full duplex transfer.
        // Current MAC layer does not support full duplex transfer, so
        // SNMP needs to manage its own full duplex connection.
        // Prepare for full duplex transfer.
        (void)TCPIP_SNMP_PDUProcessDuplexInit(gSnmpDcpt.skt);
#if defined (TCPIP_STACK_USE_IPV6)
        gSnmpDcpt.snmpIPV6Handler = TCPIP_IPV6_HandlerRegister(NULL, &SNMP_IPV6_Notify, NULL);
        if (gSnmpDcpt.snmpIPV6Handler == NULL)
        {
            TCPIP_SNMP_FreeMemory();
            return false;
        }
        gSnmpDcpt.ipv6EventType = IPV6_EVENT_ADDRESS_REMOVED;
#endif
// Initilaize file descriptor -
        SNMPStatus.Flags.bIsFileOpen = 0U;
        SNMPStatus.Flags.bIsSnmpIntfUp = 0U;
    }
    snmpInitCount++;
    return true;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_SNMP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData)
{
    TCPIP_SNMP_DCPT *pDcpt;
    pDcpt = &gSnmpDcpt;

    if(snmpInitCount > 0)
    {
        if (pDcpt->skt != INVALID_UDP_SOCKET)
        {
            (void)TCPIP_UDP_Close(pDcpt->skt);
            pDcpt->skt = INVALID_UDP_SOCKET;
        }
        if(gSnmpDcpt.trapEnable)
        {
            if(SnmpStackDcptMemStubPtr->SNMPNotifyInfo.sktv4 != INVALID_UDP_SOCKET)
            {
                (void)TCPIP_UDP_Close(SnmpStackDcptMemStubPtr->SNMPNotifyInfo.sktv4);
                SnmpStackDcptMemStubPtr->SNMPNotifyInfo.sktv4 = INVALID_UDP_SOCKET;
            }
#ifdef TCPIP_STACK_USE_IPV6
            if(SnmpStackDcptMemStubPtr->SNMPNotifyInfo.socketv6 != INVALID_UDP_SOCKET)
            {
                (void)TCPIP_UDP_Close(SnmpStackDcptMemStubPtr->SNMPNotifyInfo.socketv6);
                SnmpStackDcptMemStubPtr->SNMPNotifyInfo.socketv6 = INVALID_UDP_SOCKET;
            }                
#endif
        }
    }
    if(stackData->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
    {   // whole stack is going down
        if(snmpInitCount > 0)
        {   // we're up and running
            if(--snmpInitCount == 0)
            {   // all closed
                // release resources
                TCPIP_SNMP_FreeMemory();
            }
            
#if defined (TCPIP_STACK_USE_IPV6)
            (void)TCPIP_IPV6_HandlerDeregister(pDcpt->snmpIPV6Handler);
#endif
           
        }
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static bool TCPIP_SNMP_MemoryAllocate(const TCPIP_STACK_MODULE_CTRL* const stackData)
{
    SnmpStackMemH=stackData->memH;

#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
    SnmpStackDcptMemStubPtr = (SNMP_STACK_DCPT_STUB *)TCPIP_HEAP_Calloc(SnmpStackMemH, 1, (sizeof(SNMP_STACK_DCPT_STUB) + sizeof(SNMPV3_STACK_DCPT_STUB)));
#else
    SnmpStackDcptMemStubPtr = (SNMP_STACK_DCPT_STUB *)TCPIP_HEAP_Calloc(SnmpStackMemH, 1, (sizeof(SNMP_STACK_DCPT_STUB)));
#endif
    if(SnmpStackDcptMemStubPtr == NULL)
    {
        return false;
    }
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
    Snmpv3StkStubPtr = (SNMPV3_STACK_DCPT_STUB*) (((unsigned long int)(SnmpStackDcptMemStubPtr)) + sizeof(SNMP_STACK_DCPT_STUB));
#endif

    return true;

}

void TCPIP_SNMP_PacketProcStubPtrsGet( SNMP_PROCESSING_MEM_INFO_PTRS * dynMemInfoPtr)
{
    if(SnmpStackDcptMemStubPtr == NULL)
    {
        return;
    }
    dynMemInfoPtr->snmpHeapMemHandler=SnmpStackMemH;
    dynMemInfoPtr->snmpStkDynMemStubPtr=SnmpStackDcptMemStubPtr;
    dynMemInfoPtr->snmpDcptPtr=&gSnmpDcpt;
}

#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
void TCPIP_SNMPV3_PacketProcStubPtrsGet( SNMPV3_PROCESSING_MEM_INFO_PTRS * dynMemInfoPtr)
{
    dynMemInfoPtr->snmpHeapMemHandler=SnmpStackMemH;
    dynMemInfoPtr->snmpv3StkProcessingDynMemStubPtr=Snmpv3StkStubPtr;
}

static bool F_SNMPV3_TRAP_Initialize(const TCPIP_SNMP_MODULE_CONFIG *snmpDataConfig)
{
    TCPIP_SNMPV3_TARGET_ENTRY_CONFIG *trap_target_config=NULL;
    int usmTrapIndex=0;
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=NULL;
    uint8_t *ptr=NULL;
    uint16_t snmpv3Headerlength=0;
    uint16_t msgDataLen=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);
    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;
    trap_target_config = snmpDataConfig->trap_target_config;
    if(trap_target_config == NULL)
    {
        return false;
    }
    for(;usmTrapIndex<TCPIP_SNMPV3_USM_MAX_USER;usmTrapIndex++)
    {
        trap_target_config = &snmpDataConfig->trap_target_config[usmTrapIndex];
        (void)memcpy(snmpv3EngnDcptMemoryStubPtr->Snmpv3TrapConfigData[usmTrapIndex].userSecurityName, (const uint8_t*)trap_target_config->secname
                                                        ,strlen(trap_target_config->secname));
        snmpv3EngnDcptMemoryStubPtr->Snmpv3TrapConfigData[usmTrapIndex].messageProcessingModelType = (STD_BASED_SNMP_MESSAGE_PROCESSING_MODEL)trap_target_config->mp_model;
        snmpv3EngnDcptMemoryStubPtr->Snmpv3TrapConfigData[usmTrapIndex].securityModelType = trap_target_config->sec_model;
        snmpv3EngnDcptMemoryStubPtr->Snmpv3TrapConfigData[usmTrapIndex].securityLevelType = trap_target_config->sec_level;
    }
    // SNMPv3 Trap header allocation and construction
    snmpv3Headerlength = F_SNMPv3_Header_Length();
    ptr = snmpv3EngnDcptMemoryStubPtr->TrapMsgHeaderBuf.head =
        (uint8_t *)(TCPIP_HEAP_Calloc(SnmpStackMemH, 1, (size_t)snmpv3Headerlength + 1U));
    if(ptr == NULL)
    {
        return false;
    }
    snmpv3EngnDcptMemoryStubPtr->TrapMsgHeaderBuf.length = 0;
    snmpv3EngnDcptMemoryStubPtr->TrapMsgHeaderBuf.maxlength = snmpv3Headerlength + 1U;
// scoped trap PDU
    msgDataLen = (uint16_t)TCPIP_SNMP_MAX_MSG_SIZE - snmpv3Headerlength;
    ptr = snmpv3EngnDcptMemoryStubPtr->TrapScopdPduRespnsBuf.head =
            (uint8_t*)(TCPIP_HEAP_Calloc(SnmpStackMemH,1,(size_t)msgDataLen+5U));
    if(ptr == NULL)
    {
        return false;
    }
    snmpv3EngnDcptMemoryStubPtr->TrapScopdPduRespnsBuf.length = 0;
    snmpv3EngnDcptMemoryStubPtr->TrapScopdPduRespnsBuf.maxlength = msgDataLen;
    
    return true;
}

static void F_SNMPv3_CreatePasswordLocalizationKey(const TCPIP_NET_IF* pNetIf)
{
    uint8_t userDBIndex=0U;

    if(pNetIf == NULL)
    {
        pNetIf = (const TCPIP_NET_IF *)TCPIP_STACK_NetDefaultGet();
    }
    
    if(gSnmpDcpt.pSnmpIf == pNetIf)
    {
        return;
    }

    if(!F_SNMP_ValidatePktReceivedIntf(pNetIf))
    {
        return;
    }
    gSnmpDcpt.pSnmpIf = pNetIf;
    F_SNMPv3_EngnIDFormulate((uint8_t)MAC_ADDR_ENGN_ID, pNetIf);
    for(;userDBIndex < (uint8_t)TCPIP_SNMPV3_USM_MAX_USER; userDBIndex++)
    {
        //  Authentication and privacy password localization
        SNMPv3USMAuthPrivPswdLocalization(userDBIndex);
        SNMPv3ComputeHMACIpadOpadForAuthLoclzedKey(userDBIndex);
    }
    SNMPStatus.Flags.bIsSnmpIntfUp = 1U;
}

static bool F_SNMPV3_Stack_parameterInit(const TCPIP_SNMP_MODULE_CONFIG *snmpDataConfig)
{
    uint8_t userDBIndex=0;
    const char *usmSecName=NULL;
    const char *userAuthPasswd=NULL;
    const char *userPrivPasswd=NULL;
    uint8_t userAuthType;
    uint8_t userPrivType;
    uint8_t userSecLevel;
    uint8_t strLen=0;

    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=NULL;
    TCPIP_SNMPV3_USM_USER_CONFIG *usmConfig=NULL;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);
    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;
    usmConfig = snmpDataConfig->usm_config;
    if(usmConfig == NULL)
    {
        return false;
    }
    for(;userDBIndex< (uint8_t)TCPIP_SNMPV3_USM_MAX_USER; userDBIndex++)
    {
        usmConfig = &snmpDataConfig->usm_config[userDBIndex];
        usmSecName = usmConfig->username;
        userAuthPasswd = usmConfig->usm_auth_password;
        userPrivPasswd = usmConfig->usm_priv_password;
        userAuthType = (uint8_t)usmConfig->usm_auth_proto;
        userPrivType = (uint8_t)usmConfig->usm_priv_proto;
        userSecLevel = (uint8_t)usmConfig->security_level;

        if(usmSecName == NULL)
        {
            continue;
        }

        strLen = (uint8_t)strlen((const char*)usmSecName);
        if(strLen > (uint8_t)TCPIP_SNMPV3_USER_SECURITY_NAME_LEN)
        {
            continue;
        }
        (void)memcpy(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userName, (const uint8_t*)usmSecName, strLen);
        snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userNameLength = strLen;
        // Auth parameter
        if(userAuthPasswd != NULL)
        {
            strLen = (uint8_t)strlen(userAuthPasswd);
            if(strLen > (uint8_t)TCPIP_SNMPV3_PRIVAUTH_PASSWORD_LEN)
            {
                strLen = (uint8_t)TCPIP_SNMPV3_PRIVAUTH_PASSWORD_LEN;
            }
            (void)memcpy(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userAuthPswd, (const uint8_t*)userAuthPasswd, strLen);
            snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userAuthPswdLen = strLen;
        }
        //Privacy parameter
        if(userPrivPasswd != NULL)
        {
            strLen = (uint8_t)strlen(userPrivPasswd);
            if(strLen > (uint8_t)TCPIP_SNMPV3_PRIVAUTH_PASSWORD_LEN)
            {
                strLen = (uint8_t)TCPIP_SNMPV3_PRIVAUTH_PASSWORD_LEN;
            }
            (void)memcpy(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userPrivPswd, (const uint8_t*)userPrivPasswd, strLen);
            snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userPrivPswdLen = strLen;
        }
        snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userHashType=userAuthType;
        snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userPrivType=userPrivType;
        snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].secLevel = (STD_BASED_SNMPV3_SECURITY_LEVEL)userSecLevel;
        snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userDBIndex=userDBIndex;
    }

    snmpv3EngnDcptMemoryStubPtr->SnmpInMsgAuthParamLen=12;
    snmpv3EngnDcptMemoryStubPtr->SnmpInMsgPrivParmLen=8;
    snmpv3EngnDcptMemoryStubPtr->SnmpOutMsgAuthParmLen=12;
    snmpv3EngnDcptMemoryStubPtr->SnmpOutMsgPrivParmLen=8;

    return true;
}

#endif

static bool F_SNMP_CommunityConfiguration(const TCPIP_SNMP_MODULE_CONFIG *snmpDataConfig)
{
    uint8_t i;
    const char * strCommunity = NULL;
    TCPIP_SNMP_COMMUNITY_CONFIG *readCommunity=NULL,*writeCommunity=NULL;

    // read community
    readCommunity = snmpDataConfig->read_community_config;

    if(readCommunity != NULL)
    {
        for(i=0;i<(uint8_t)TCPIP_SNMP_MAX_COMMUNITY_SUPPORT;i++)
        {
            // read community
            readCommunity = &snmpDataConfig->read_community_config[i];
            strCommunity = NULL;
            if(readCommunity->communityName == NULL)
            {  // Get a pointer to the next community string
               //strCommunity = cReadCommunities[i];
                continue;
            }
            else
            {  // Get a pointer to the next community string
                strCommunity = readCommunity->communityName;
            }
           // Ensure we don't buffer overflow.  If your code gets stuck here,
           // it means your TCPIP_SNMP_COMMUNITY_MAX_LEN definition
           // is either too small or one of your community string lengths
           // (SNMP_READ_DEFAULT_COMMUNITIES) are too large.  Fix either.
            if(strCommunity == NULL)
            {
                strCommunity="";
            }
            if(strlen(strCommunity) > (size_t)TCPIP_SNMP_COMMUNITY_MAX_LEN)
            {
                strCommunity="";
            }
            (void)strcpy((char*)SnmpStackDcptMemStubPtr->snmpNetConfig.readCommunity[i], strCommunity);
        }
    }

    // write Community
    writeCommunity = snmpDataConfig->write_community_config;
    if(writeCommunity != NULL)
    {        
        for(i=0;i<(uint8_t)TCPIP_SNMP_MAX_COMMUNITY_SUPPORT;i++)
        {
            writeCommunity = &snmpDataConfig->write_community_config[i];
            strCommunity = NULL;
            // write community
            if(writeCommunity->communityName == NULL)
            {  // Get a pointer to the next community string
               //strCommunity = cWriteCommunities[i];
                continue;
            }
            else
            {  // Get a pointer to the next community string
                strCommunity = writeCommunity->communityName;
            }
            if(strCommunity == NULL)
            {
                strCommunity="";
            }
           // Ensure we don't buffer overflow.  If your code gets stuck here,
           // it means your TCPIP_SNMP_COMMUNITY_MAX_LEN definition
           // is either too small or one of your community string lengths
           // (SNMP_READ_DEFAULT_COMMUNITIES) are too large.  Fix either.
            if(strlen(strCommunity)> (size_t)TCPIP_SNMP_COMMUNITY_MAX_LEN)
            {
                strCommunity="";
            }
            (void)strcpy((char*)SnmpStackDcptMemStubPtr->snmpNetConfig.writeCommunity[i], strCommunity);
        }
    }
    return true;
}


static void F_SNMP_Trap_Initialize(void)
{
    SnmpStackDcptMemStubPtr->gSendTrapFlag=false;//global flag to send Trap
    SnmpStackDcptMemStubPtr->gOIDCorrespondingSnmpMibID=(uint8_t)MICROCHIP;
    SnmpStackDcptMemStubPtr->gGenericTrapNotification=(uint8_t)ENTERPRISE_SPECIFIC;
    SnmpStackDcptMemStubPtr->gSpecificTrapNotification=(uint8_t)VENDOR_TRAP_DEFAULT; // Vendor specific trap code
//#if defined(SNMP_STACK_USE_V2_TRAP) && !defined(SNMP_TRAP_DISABLED)
    //if gSetTrapSendFlag == false then the last varbind variable for
    //multiple varbind variable pdu structure or if there is only varbind variable send.
    // if gSetTrapSendFlag == true, then v2 trap pdu is expecting more varbind variable.
    SnmpStackDcptMemStubPtr->gSetTrapSendFlag = 0U;
    SnmpStackDcptMemStubPtr->SNMPNotifyInfo.sktv4 = INVALID_UDP_SOCKET;
#ifdef TCPIP_STACK_USE_IPV6
    SnmpStackDcptMemStubPtr->SNMPNotifyInfo.socketv6 = INVALID_UDP_SOCKET;
#endif
//#endif SNMP_STACK_USE_V2_TRAP
    TCPIP_SNMP_CreateTrapSocket();
}

bool TCPIP_SNMP_DataCopyToProcessBuffer(uint8_t val ,SNMP_BUFFER_DATA *putbuf)
{
    if(putbuf->length < (uint16_t)TCPIP_SNMP_MAX_MSG_SIZE)
    {
        putbuf->head[putbuf->length++] = (uint8_t)val;
        return true;
    }
    else
    {
        return false;
    }
}


uint8_t TCPIP_SNMP_ProcessBufferDataGet(SNMP_BUFFER_DATA getbuf,uint16_t pos)
{
    return (uint8_t)(getbuf.head[pos]);
}

#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
static bool F_SNMP_ValidatePktReceivedIntf(const TCPIP_NET_IF *pNetIfFromDcpt)
{
    if(TCPIPStackHandleToNetLinked(pNetIfFromDcpt) == NULL)
    {
        return false;
    }
    if(TCPIPStackNetAddress(pNetIfFromDcpt) == 0U)
    {
        return false;
    }

    return true;
}
#endif

static SNMP_STACK_ERROR TCPIP_SNMP_inputPacketProcessAndRespond(void)
{
    SNMP_ERR_STATUS snmpErr =SNMP_NO_ERR;
    const TCPIP_NET_IF* pNetIf=NULL;
    UDP_SOCKET_INFO    udpSockInfo;
    uint16_t    getBufferSize,bufferSize;
    PDU_INFO pduInfoDB; //received pdu information database
    char community[TCPIP_SNMP_COMMUNITY_MAX_LEN+1];
    uint8_t communityLen=0;
    bool lbReturn;
    UDP_SOCKET     s;

    s = gSnmpDcpt.skt;
    
    // Do nothing if no data is waiting
    getBufferSize = TCPIP_UDP_GetIsReady(s);
    if(getBufferSize == 0U)
    {
       return SNMP_PKT_IS_NOTPRESENT;
    }
    if(getBufferSize > (uint16_t)TCPIP_SNMP_MAX_MSG_SIZE)
    {
       getBufferSize = (uint16_t)TCPIP_SNMP_MAX_MSG_SIZE;
    }
    
    (void)memset(getBuffer,0,sizeof(getBuffer));
    (void)memset(&udpSockInfo,0,sizeof(udpSockInfo));
    (void)TCPIP_UDP_SocketInfoGet(s, &udpSockInfo);
    pNetIf = (const TCPIP_NET_IF*)udpSockInfo.hNet;

#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
    F_SNMPv3_CreatePasswordLocalizationKey(pNetIf);
#endif
    gSnmpDcpt.pSnmpIf = pNetIf;
    gSnmpDcpt.udpGetBufferData.head = getBuffer;
    gSnmpDcpt.udpGetBufferData.wrPtr = gSnmpDcpt.udpGetBufferData.head;
    gSnmpDcpt.udpGetBufferData.endPtr = gSnmpDcpt.udpGetBufferData.head+getBufferSize;

    /* Copy the SNMP query from the UDP socket buffer to SNMP specific allocated buffer 'getBuffer' */
    (void)TCPIP_UDP_ArrayGet(s,gSnmpDcpt.udpGetBufferData.head,getBufferSize);
    (void)TCPIP_UDP_Discard(s);
    (void)memset(community,0,sizeof(community));

    /* Initialize buffer offsets. */
    SNMPRxOffset = 0;
    SNMPTxOffset = 0;
    if((*snmpFileShell->fileSeek)(snmpFileShell,snmpFileDescrptr,0,SYS_FS_SEEK_SET) == -1)    
    {
        return SNMP_STACK_FAIL;
    }

    /*Now process the PDU Header from the received SNMP PDU copied to snmp allocated buffer */
    (void)memset(&pduInfoDB, 0, sizeof(pduInfoDB));
    snmpErr = TCPIP_SNMP_ProcessPDUHeader(&pduInfoDB,community,&communityLen);

    if(pduInfoDB.snmpVersion != (uint8_t)SNMP_V3)
    {
        if ( snmpErr != SNMP_NO_ERR)
        {
            return SNMP_STACK_FAIL;
        }
        if ( !TCPIP_SNMP_ProcessHeaderGetSet(&pduInfoDB))
        {
            return SNMP_STACK_FAIL;
        }
    }
    
    bufferSize = TCPIP_UDP_TxPutIsReady(s, (uint16_t)TCPIP_SNMP_MAX_MSG_SIZE + 1U);
    if(bufferSize < (uint16_t)TCPIP_SNMP_MAX_MSG_SIZE)
    {
        void* txSize = FC_Uint2VPtr((uint32_t)TCPIP_SNMP_MAX_MSG_SIZE + 1UL); 
        (void)TCPIP_UDP_OptionsSet(s, UDP_OPTION_TX_BUFF, txSize);
    }
    //this will put the start pointer at the beginning of the TX buffer
    (void)TCPIP_UDP_TxOffsetSet(s,0,false);

    //Get the write pointer:
    SnmpStackDcptMemStubPtr->outPduBufData.head = TCPIP_UDP_TxPointerGet(s);
    if(SnmpStackDcptMemStubPtr->outPduBufData.head == NULL)
    {
       return SNMP_STACK_FAIL;
    }
    SnmpStackDcptMemStubPtr->outPduBufData.length = 0;
    if(pduInfoDB.snmpVersion != (uint8_t)SNMP_V3) // if(SNMP_V1, SNMP_V2C)
    {
        lbReturn = TCPIP_SNMP_ProcessVariables(&pduInfoDB,community, communityLen);
    }
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
    else
    {
        lbReturn = TCPIP_SNMPv3_V3MsgDataProcess(&pduInfoDB,SnmpStackDcptMemStubPtr->outPduBufData.head);
    }
#endif
    
    if(lbReturn == false)
    {
        SnmpStackDcptMemStubPtr->outPduBufData.length = 0;
        SnmpStackDcptMemStubPtr->outPduBufData.head = NULL;
        return SNMP_STACK_FAIL;
    }

    if(SnmpStackDcptMemStubPtr->gSendTrapFlag == false)
    {
        (void)TCPIP_UDP_TxOffsetSet(s,(uint16_t)SnmpStackDcptMemStubPtr->outPduBufData.length, false);
        (void)TCPIP_UDP_Flush(s);
    }
    
    return SNMP_STACK_NO_ERROR;
}

void TCPIP_SNMP_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);


    if(sigPend != TCPIP_MODULE_SIGNAL_NONE)
    { // either TMO or RX signal occurred
        TCPIP_SNMP_Process();
    }

}


// send a signal to the SNMP module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void F_SNMPSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}


static void TCPIP_SNMP_Process(void)
{
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
    const TCPIP_NET_IF* pNetIf=(const TCPIP_NET_IF *)TCPIP_STACK_NetDefaultGet();
#endif
    if(gSnmpDcpt.skt == INVALID_UDP_SOCKET)
    {
        return;
    }
    if(SnmpStackDcptMemStubPtr == NULL)
    {
        return;
    }

#if defined (TCPIP_STACK_USE_IPV6)    
    if(gSnmpDcpt.ipv6EventType != IPV6_EVENT_ADDRESS_ADDED)
    {
        return;
    }
#endif

    if(SNMPStatus.Flags.bIsFileOpen == 0U)
    {
        snmpFileDescrptr = snmpFileShell->fileOpen(snmpFileShell,TCPIP_SNMP_BIB_FILE_NAME,SYS_FS_FILE_OPEN_READ);
        if(snmpFileDescrptr != SYS_FS_HANDLE_INVALID)
        {
           SNMPStatus.Flags.bIsFileOpen = 1U;
        }
        else
        {
            return;
        }
    }
    
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
    if(SNMPStatus.Flags.bIsSnmpIntfUp == 0U)
    {
        F_SNMPv3_CreatePasswordLocalizationKey(pNetIf);
    }
#endif

    while(true)
    {
        if(TCPIP_SNMP_inputPacketProcessAndRespond()== SNMP_PKT_IS_NOTPRESENT)
        {
            // This will put the socket in the initial open state, ready to listen again for either IPv4 or IPv6 transactions, whatever comes first.
            (void)TCPIP_UDP_Disconnect(gSnmpDcpt.skt,false);
            return;
        }
    }
}



static uint32_t  snmpTrapTimer=0;

void TCPIP_SNMP_NotifyPrepare(IP_MULTI_ADDRESS* remoteHost,
                        char* community,
                        uint8_t communityLen,
                        SNMP_ID agentIDVar,
                        uint8_t notificationCode,
                        uint32_t timestamp )
{
    static IP_MULTI_ADDRESS* remHostIpAddrPtr;
    remHostIpAddrPtr = remoteHost;

    if(remHostIpAddrPtr == NULL)
    {
        // do nothing
    }

    (void)strcpy(SnmpStackDcptMemStubPtr->SNMPNotifyInfo.community, community);
    SnmpStackDcptMemStubPtr->SNMPNotifyInfo.communityLen = communityLen;

    SnmpStackDcptMemStubPtr->SNMPNotifyInfo.agentIDVar = agentIDVar;
    SnmpStackDcptMemStubPtr->SNMPNotifyInfo.notificationCode = notificationCode;

    SnmpStackDcptMemStubPtr->SNMPNotifyInfo.timestamp = timestamp;

}

static void TCPIP_SNMP_CreateTrapSocket(void)
{
    TCPIP_SNMP_DCPT *snmpDcpt;
    IP_MULTI_ADDRESS remoteAddress;
    (void)memset(remoteAddress.v6Add.v,0,sizeof(IPV6_ADDR));
    remoteAddress.v4Add.Val = 0;

    snmpDcpt = &gSnmpDcpt;

    if(snmpDcpt->trapEnable)
    {
        if(SnmpStackDcptMemStubPtr->SNMPNotifyInfo.sktv4 == INVALID_UDP_SOCKET)
        {
            SnmpStackDcptMemStubPtr->SNMPNotifyInfo.sktv4 = TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE_IPV4,SNMP_NMS_PORT,&remoteAddress);
        }
#ifdef TCPIP_STACK_USE_IPV6
        if(SnmpStackDcptMemStubPtr->SNMPNotifyInfo.socketv6 == INVALID_UDP_SOCKET)
        {
            SnmpStackDcptMemStubPtr->SNMPNotifyInfo.socketv6 = TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE_IPV6,SNMP_NMS_PORT,&remoteAddress);
        }
#endif
    }
}

bool TCPIP_SNMP_NotifyIsReady(IP_MULTI_ADDRESS* remoteHost,SNMP_TRAP_IP_ADDRESS_TYPE eTrapMultiAddressType)
{
    TCPIP_NET_IF *pNetIf;   
    IP_MULTI_ADDRESS remoteAddress;
    UDP_SOCKET skt=INVALID_UDP_SOCKET;
    bool bindRes;
    
    if(eTrapMultiAddressType == IPV4_SNMP_TRAP)
    {
        remoteAddress.v4Add.Val = remoteHost->v4Add.Val;
    }
    else
    {
        (void)memcpy(&remoteAddress.v6Add,&remoteHost->v6Add,16);
    }

    pNetIf = TCPIPStackHandleToNet(SnmpStackDcptMemStubPtr->SNMPNotifyInfo.snmpTrapInf);
    if(eTrapMultiAddressType == IPV4_SNMP_TRAP)
    {
        if(pNetIf->netIPAddr.Val == 0U)
        {   
            return false;
        }
        skt = SnmpStackDcptMemStubPtr->SNMPNotifyInfo.sktv4;
    }
#ifdef TCPIP_STACK_USE_IPV6
    if(eTrapMultiAddressType == IPV6_SNMP_TRAP)
    {
        if(TCPIP_SNMP_EventNotifyGet(pNetIf) != IPV6_EVENT_ADDRESS_ADDED)
        {   
            return false;
        }
        skt = SnmpStackDcptMemStubPtr->SNMPNotifyInfo.socketv6;
    }
#endif      

    if(skt != INVALID_UDP_SOCKET)
    {
        if(eTrapMultiAddressType == IPV4_SNMP_TRAP)
        {
            //SnmpStackDcptMemStubPtr->SNMPNotifyInfo.sktv4 = TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE_IPV4,SNMP_NMS_PORT,&remoteAddress);
             // set the interface destination server address
            bindRes = TCPIP_UDP_RemoteBind(skt, IP_ADDRESS_TYPE_IPV4, SNMP_NMS_PORT,&remoteAddress);
        }
#ifdef TCPIP_STACK_USE_IPV6
        else
        {
            //SnmpStackDcptMemStubPtr->SNMPNotifyInfo.sktv4 = TCPIP_UDP_ClientOpen(IP_ADDRESS_TYPE_IPV6,SNMP_NMS_PORT,&remoteAddress);
            bindRes = TCPIP_UDP_RemoteBind(skt, IP_ADDRESS_TYPE_IPV6, SNMP_NMS_PORT,&remoteAddress);
        }
#endif
        if(bindRes == true)
        {
            (void)TCPIP_UDP_SocketNetSet(skt,pNetIf);
        }
        else
        {
            return false;
        }
        snmpTrapTimer = SYS_TMR_TickCountGet();      
    }
    else
    {
        return false;
    }

    if(TCPIP_UDP_IsOpened(skt)== true)
    {
        snmpTrapTimer = SYS_TMR_TickCountGet();
    }
    else
    {
        return false;
    }

    return true;
}

uint32_t TCPIP_SNMP_TrapTimeGet(void)
{
    return snmpTrapTimer;
}

uint8_t *TCPIP_SNMP_GenericTrapCodeToTrapOID(uint8_t generic_trap_code,uint8_t *len)
{
    static  uint8_t gen_trap_oid[] = {0x2bU,6,1,6,3,1,1,5,1};
/*  
    static  uint8_t cold_trap_oid[] = {0x2b,6,1,6,3,1,1,5,1};
    static  uint8_t warm_start_oid = {0x2b,6,1,6,3,1,1,5,2};
    static  uint8_t auth_fail_oid  = {0x2b,6,1,6,3,1,1,5,5};
    static  uint8_t linkdown_oid   = {0x2b,6,1,6,3,1,1,5,3};
    static  uint8_t linkup_oid     = {0x2b,6,1,6,3,1,1,5,4};
*/
    static uint8_t snmptrap_oids[]  = {0x2bU,6,1,6,3,1,1,4,1 };

    *len = (uint8_t)sizeof(gen_trap_oid);
    uint8_t* retPtr = gen_trap_oid;
    switch (generic_trap_code) 
    {
        case (uint8_t)COLD_START:
            gen_trap_oid[*len-1U] = 1;
            break;

        case (uint8_t)WARM_START:
            gen_trap_oid[*len-1U] = 2;
            break;

        case (uint8_t)LINK_UP:
            gen_trap_oid[*len-1U] = 4;
            break;

        case (uint8_t)LINK_DOWN:
            gen_trap_oid[*len-1U] = 3;
            break;

        case (uint8_t)AUTH_FAILURE:
            gen_trap_oid[*len-1U] = 5;
            break;

        case (uint8_t)ENTERPRISE_SPECIFIC:
            *len = (uint8_t)sizeof(snmptrap_oids);
            retPtr = snmptrap_oids;
            break;

        default:
            // fail
            retPtr = NULL;
            break;

    } /* switch (generic_trap_code) */

    return retPtr;

} /* end getSnmpV2TrapOid() */


bool TCPIP_SNMP_TRAPv2Notify(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index,SNMP_TRAP_IP_ADDRESS_TYPE eTrapMultiAddressType)
{
    char* pCommunity;
    uint8_t len;
    uint8_t OIDValue[TCPIP_SNMP_OID_MAX_LEN];
    uint8_t OIDLen;
    static uint32_t varbindlen = 0;
    uint8_t agentIDLen;
    uint8_t* pOIDValue;
    static uint16_t packetStructLenOffset = 0;
    static uint16_t pduStructLenOffset = 0;
    static uint16_t varBindStructLenOffset = 0;
    static uint16_t varPairStructLenOffset = 0;
    uint16_t tempOffset = 0;
    OID_INFO rec;
    SNMP_DATA_TYPE_INFO dataTypeInfo;
    uint8_t snmptrap_oids[]  = {0x2bU,6,1,6,3,1,1,4,1 }; /* len=10 */
    uint8_t sysUpTime_oids[] = {0x2bU,6,1,2,1,1,3}; /* len = 8 */
    TCPIP_UINT16_VAL trapVarBindLen={0};
    uint8_t i=0;
    SNMP_BUFFER_DATA *snmpTrapPutData=NULL;
    UDP_SOCKET skt = INVALID_UDP_SOCKET;
    
    if(snmpFileDescrptr == SYS_FS_HANDLE_INVALID)
    {
        return false;
    }

    snmpTrapFileDescriptr = snmpFileDescrptr;
    // set the file position to the begining
    if((*snmpFileShell->fileSeek)(snmpFileShell,snmpTrapFileDescriptr,0,SYS_FS_SEEK_SET) == -1)
    {
        return false;
    }

#ifdef TCPIP_STACK_USE_IPV4
    if(eTrapMultiAddressType == IPV4_SNMP_TRAP)
    {
        skt  = SnmpStackDcptMemStubPtr->SNMPNotifyInfo.sktv4;
    }
#endif
#ifdef TCPIP_STACK_USE_IPV6
    if(eTrapMultiAddressType == IPV6_SNMP_TRAP)
    {
        skt = SnmpStackDcptMemStubPtr->SNMPNotifyInfo.socketv6;
    }
#endif
    
    if(skt == INVALID_UDP_SOCKET)
    {
        return false;
    }

    if((packetStructLenOffset == 0U)&&(pduStructLenOffset==0U))
    {
        if(!TCPIP_SNMP_PDUProcessDuplexInit(skt))
        {
            return false;
        }
        //this will put the start pointer at the beginning of the TX buffer
        (void)TCPIP_UDP_TxOffsetSet(skt,0,false);

        //Get the write pointer:
        SnmpStackDcptMemStubPtr->trapPduOutBufData.head = TCPIP_UDP_TxPointerGet(skt);
        if(SnmpStackDcptMemStubPtr->trapPduOutBufData.head == NULL)
        {
           return false;
        }
        SnmpStackDcptMemStubPtr->trapPduOutBufData.length = 0;

        snmpTrapPutData = &SnmpStackDcptMemStubPtr->trapPduOutBufData;

        len = SnmpStackDcptMemStubPtr->SNMPNotifyInfo.communityLen;
        pCommunity = SnmpStackDcptMemStubPtr->SNMPNotifyInfo.community;

        (void)TCPIP_SNMP_DataCopyToProcessBuffer(STRUCTURE,snmpTrapPutData);          // First item is packet structure
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x82U,snmpTrapPutData);
        packetStructLenOffset = snmpTrapPutData->length;
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);

        // Put SNMP version info.
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_INT,snmpTrapPutData);                // Int type.
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(1,snmpTrapPutData);                  // One byte long value.
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(SNMP_V2C,snmpTrapPutData);         // v2

        //len = strlen(community);  // Save community length for later use.
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(OCTET_STRING,snmpTrapPutData);       // Octet string type.
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(len,snmpTrapPutData);                    // community string length
        while( len-- != 0U)              // Copy entire string.
        {
            (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)*(pCommunity++),snmpTrapPutData);
        }

        //TRAP Version type.
        (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)SNMP_V2_TRAP,snmpTrapPutData);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x82U,snmpTrapPutData);
        pduStructLenOffset = snmpTrapPutData->length;
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0U,snmpTrapPutData);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0U,snmpTrapPutData);

        //put Request ID for the trapv2 as 1
        (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)ASN_INT,snmpTrapPutData);    // Int type.
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(4,snmpTrapPutData);      // To simplify logic, always use 4 byte long requestID
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(1,snmpTrapPutData);

        // Put error status.
        (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)ASN_INT,snmpTrapPutData);                // Int type
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(1,snmpTrapPutData);                  // One byte long.
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);                  // Placeholder.

        // Similarly put error index.
        (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)ASN_INT,snmpTrapPutData);                // Int type
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(1,snmpTrapPutData);                  // One byte long
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);                  // Placeholder.

        // Variable binding structure header
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x30U,snmpTrapPutData);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x82U,snmpTrapPutData);
        varBindStructLenOffset = snmpTrapPutData->length; //SNMPTxOffset;
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);

        // Create variable name-pair structure
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x30U,snmpTrapPutData);
        varPairStructLenOffset = snmpTrapPutData->length; //SNMPTxOffset;
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);

        // Set 1st varbind object i,e sysUpTime.0 time stamp for the snmpv2 trap
        // Get complete notification variable OID string.

        (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)ASN_OID,snmpTrapPutData);
        OIDLen = (uint8_t)sizeof(sysUpTime_oids);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)(OIDLen)+1U,snmpTrapPutData);
        pOIDValue = sysUpTime_oids;
        while( OIDLen-- != 0U)
        {
            (void)TCPIP_SNMP_DataCopyToProcessBuffer(*pOIDValue++,snmpTrapPutData);
        }

        //1st varbind    and this is a scalar object so index = 0
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);

        // Time stamp
        (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)SNMP_TIME_TICKS,snmpTrapPutData);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(4,snmpTrapPutData);

        TCPIP_UINT32_VAL tStamp;
        tStamp.Val = SnmpStackDcptMemStubPtr->SNMPNotifyInfo.timestamp;
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(tStamp.v[3],snmpTrapPutData);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(tStamp.v[2],snmpTrapPutData);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(tStamp.v[1],snmpTrapPutData);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(tStamp.v[0],snmpTrapPutData);

        tempOffset = snmpTrapPutData->length;
        //set the snmp time varbind trap offset
        snmpTrapPutData->length = varPairStructLenOffset;

        // SNMP time stamp varbind length
        OIDLen = 2U                          // 1st varbind header
           + (uint8_t)sizeof(sysUpTime_oids)
           + 1U                         // index byte
           + 6U ;                        // time stamp

        (void)TCPIP_SNMP_DataCopyToProcessBuffer(OIDLen,snmpTrapPutData);
        //set the previous TX offset
        snmpTrapPutData->length = tempOffset;
        varbindlen += (uint32_t)OIDLen // varbind length
                  + 2U;  // varbind type(30) and length of individual varbind pdu

        // Set 2nd varbind object i,e snmpTrapOID.0 for the snmpv2 trap
        // Get complete notification variable OID string.

        // Create variable name-pair structure
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x30U,snmpTrapPutData);
        varPairStructLenOffset = snmpTrapPutData->length; //SNMPTxOffset;
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);

        // Copy OID string into PDU.
        (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)ASN_OID,snmpTrapPutData);
        OIDLen = (uint8_t)sizeof(snmptrap_oids);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)(OIDLen)+1U,snmpTrapPutData);

        pOIDValue = snmptrap_oids;
        while( OIDLen-- != 0U)
        {
            (void)TCPIP_SNMP_DataCopyToProcessBuffer(*pOIDValue++,snmpTrapPutData);
        }

        //2nd varbind  and this is a scalar object so index = 0
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);
        if ( !TCPIP_SNMP_OIDStringFindByID(snmpTrapFileDescriptr,SnmpStackDcptMemStubPtr->SNMPNotifyInfo.trapIDVar, &rec, OIDValue, &OIDLen) )
        {
            return false;
        }
        (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)ASN_OID,snmpTrapPutData);
        agentIDLen = OIDLen;
        len  = OIDLen;
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(agentIDLen,snmpTrapPutData);
        for(i=0;i<len;i++)
        {
            (void)TCPIP_SNMP_DataCopyToProcessBuffer(OIDValue[i],snmpTrapPutData);
        }
        tempOffset = snmpTrapPutData->length;
        //set the snmp varbind trap offset
        snmpTrapPutData->length = varPairStructLenOffset;
        // Snmp trap varbind length
        OIDLen = 2U                   // Agent ID header bytes
                + (uint8_t)sizeof(snmptrap_oids)
                + 1U                        // index byte
                + 2U                      // header
                + agentIDLen;                // Agent ID bytes
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(OIDLen,snmpTrapPutData);

        //set the previous TX offset
        snmpTrapPutData->length = tempOffset;
        varbindlen += (uint32_t)OIDLen // varbind length
                   + 2UL;  // varbind type(30) and length of individual varbind pdu   
    }
    else
    { // collect the last varbind offset value.
        snmpTrapPutData = &SnmpStackDcptMemStubPtr->trapPduOutBufData;
        snmpTrapPutData->length = varPairStructLenOffset;
    }

    // Create variable name-pair structure
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x30U,snmpTrapPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x82U,snmpTrapPutData);
    varPairStructLenOffset = snmpTrapPutData->length;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);
    /* to send generic trap */
    if(SnmpStackDcptMemStubPtr->gGenericTrapNotification != (uint8_t)ENTERPRISE_SPECIFIC)
    {
        pOIDValue = TCPIP_SNMP_GenericTrapCodeToTrapOID(SnmpStackDcptMemStubPtr->gGenericTrapNotification,&OIDLen);
        if(pOIDValue == NULL)
        {
            return false;
        }
        // Copy OID string into PDU.
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_OID,snmpTrapPutData);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)(OIDLen)+1U,snmpTrapPutData);
        while( OIDLen-- != 0U)
        {
            (void)TCPIP_SNMP_DataCopyToProcessBuffer(*pOIDValue++,snmpTrapPutData);
        }

        //2nd varbind  and this is a scalar object so index = 0
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);
        // for microchip , SnmpStackDcptMemStubPtr->SNMPNotifyInfo.agentIDVar == MICROCHIP
        if ( !TCPIP_SNMP_OIDStringFindByID(snmpTrapFileDescriptr,SnmpStackDcptMemStubPtr->SNMPNotifyInfo.agentIDVar, &rec, OIDValue, &OIDLen) )
        {
            return false;
        }
        if ( rec.nodeInfo.Flags.bIsAgentID == 0U)
        {
            return false;
        }

        if((*snmpFileShell->fileSeek)(snmpFileShell,snmpTrapFileDescriptr,rec.hData,SYS_FS_SEEK_SET) == -1)
        {
            return false;
        }
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_OID,snmpTrapPutData);
        if((*snmpFileShell->fileRead)(snmpFileShell,snmpTrapFileDescriptr,(uint8_t*)&len,1U) == 0xFFFFFFFFU)        
        {
            return false;
        }

        agentIDLen = len;
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(agentIDLen,snmpTrapPutData);
        while( len-- != 0U)
        {
            uint8_t c;
            (void)(*snmpFileShell->fileRead)(snmpFileShell,snmpTrapFileDescriptr,(uint8_t*)&c,1U);
            (void)TCPIP_SNMP_DataCopyToProcessBuffer(c,snmpTrapPutData);
        }
        tempOffset = snmpTrapPutData->length;
        //set the snmp varbind trap offset
        snmpTrapPutData->length = varPairStructLenOffset;
        // Snmp trap varbind length
        trapVarBindLen.Val = (uint16_t)(2U                   // Agent ID header bytes
                + (uint16_t)sizeof(snmptrap_oids)
                + 1U                        // index byte
                + 2U                      // header
                + (uint16_t)agentIDLen);                // Agent ID bytes
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(trapVarBindLen.v[1],snmpTrapPutData);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(trapVarBindLen.v[0],snmpTrapPutData);
        len = (uint8_t)trapVarBindLen.Val;
    }
    else
    {
        // Get complete notification variable OID string.
        if (!TCPIP_SNMP_OIDStringFindByID(snmpTrapFileDescriptr,var, &rec, OIDValue, &OIDLen) )
        {
            return false;
        }

        pOIDValue = OIDValue;

        // Copy OID string into packet.
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_OID,snmpTrapPutData);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)(OIDLen+1U),snmpTrapPutData);
        len = OIDLen;
        while( len-- != 0U)
        {
            (void)TCPIP_SNMP_DataCopyToProcessBuffer(*pOIDValue++,snmpTrapPutData);
        }
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(index,snmpTrapPutData);

        // Encode and Copy actual data bytes
        if ( !TCPIP_SNMP_DataTypeInfoGet(rec.dataType, &dataTypeInfo) )
        {
            return false;
        }
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(dataTypeInfo.asnType,snmpTrapPutData);
         //Modified to Send trap even for  dataTypeInfo.asnType= ASCII_STRING,
        //where dataTypeInfo.asnLen=0xff
        if ( dataTypeInfo.asnLen == 0xffU )
        {
            uint8_t *asciiStr= (uint8_t *)val.dword;
            uint8_t k=0;
            dataTypeInfo.asnLen= (uint8_t)strlen((char *)asciiStr);
            len = dataTypeInfo.asnLen;
            (void)TCPIP_SNMP_DataCopyToProcessBuffer(len,snmpTrapPutData);
            for(k=0;k<len;k++)
            {
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(asciiStr[k],snmpTrapPutData);
            }
        }
        else
        {
            len = dataTypeInfo.asnLen;
            (void)TCPIP_SNMP_DataCopyToProcessBuffer(len,snmpTrapPutData);
            while( len-- != 0U)
            {
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(val.v[len],snmpTrapPutData);
            }
        }

        trapVarBindLen.Val = (uint16_t)((uint16_t)dataTypeInfo.asnLen    // data bytes count
                 + 1U                    // Length byte
                 + 1U                    // Data type byte
                 + (uint16_t)OIDLen               // OID bytes
                 + 2U                    // OID header bytes
                 + 1U);                   // index byte
        tempOffset = snmpTrapPutData->length;
        snmpTrapPutData->length = varPairStructLenOffset;
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(trapVarBindLen.v[1],snmpTrapPutData);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(trapVarBindLen.v[0],snmpTrapPutData);
    }
    //set the previous TX offset
    snmpTrapPutData->length = tempOffset;
    varPairStructLenOffset = tempOffset;

    varbindlen += (uint32_t)trapVarBindLen.Val // length of varbind
                            + 4UL; // varbind type(30) and 0x82 , lenght1 and length2 of individual varbind pdu
    if(SnmpStackDcptMemStubPtr->gSetTrapSendFlag == 1U)
    {
        return true;
    }
    trapVarBindLen.Val = (uint16_t)varbindlen;
    snmpTrapPutData->length = varBindStructLenOffset;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(trapVarBindLen.v[1],snmpTrapPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(trapVarBindLen.v[0],snmpTrapPutData);
    trapVarBindLen.Val = (uint16_t)varbindlen
    + 4U                    //  Variable Binding structure header(0x30,0x82,length1,length2)
    + 12U;                   // req , error and error status for SNMPv2

    snmpTrapPutData->length = pduStructLenOffset;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(trapVarBindLen.v[1],snmpTrapPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(trapVarBindLen.v[0],snmpTrapPutData);


    trapVarBindLen.Val = (uint16_t)trapVarBindLen.Val                           // PDU struct length
    + 4U                             // PDU trap header
    + SnmpStackDcptMemStubPtr->SNMPNotifyInfo.communityLen           // Community string bytes
    + 2U                             // Community header bytes
    + 3U;                            // SNMP version bytes


    snmpTrapPutData->length = packetStructLenOffset;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(trapVarBindLen.v[1],snmpTrapPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(trapVarBindLen.v[0],snmpTrapPutData);

    snmpTrapPutData->length = tempOffset;

// after setting all the offset values, initialize all static variables to 0.
    packetStructLenOffset = 0;
    pduStructLenOffset = 0;
    varBindStructLenOffset = 0;
    varPairStructLenOffset = 0;
    varbindlen = 0;

    (void)TCPIP_UDP_TxOffsetSet(skt,(uint16_t)snmpTrapPutData->length, false);
    (void)TCPIP_UDP_Flush(skt);
    snmpTrapPutData = NULL;
    return true;
}

bool TCPIP_SNMP_TRAPv1Notify(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index,SNMP_TRAP_IP_ADDRESS_TYPE eTrapMultiAddressType)
{
    char* pCommunity;
    uint8_t len;
    uint8_t OIDValue[TCPIP_SNMP_OID_MAX_LEN];
    uint8_t OIDLen;
    uint8_t agentIDLen;
    uint8_t* pOIDValue;
    uint16_t packetStructLenOffset;
    uint16_t pduStructLenOffset;
    uint16_t varBindStructLenOffset;
    uint16_t varPairStructLenOffset;
    uint16_t prevOffset;
    OID_INFO rec;
    SNMP_DATA_TYPE_INFO dataTypeInfo;
    UDP_SOCKET skt = INVALID_UDP_SOCKET;
    SNMP_BUFFER_DATA *snmpTrapPutData=NULL;

    if(snmpFileDescrptr== SYS_FS_HANDLE_INVALID)
    {
        return false;
    }
    snmpTrapFileDescriptr = snmpFileDescrptr;
    // set the file position to the begining
    if((*snmpFileShell->fileSeek)(snmpFileShell,snmpTrapFileDescriptr,0,SYS_FS_SEEK_SET) == -1)
    {
        return false;
    }
#ifdef TCPIP_STACK_USE_IPV4
    if(eTrapMultiAddressType == IPV4_SNMP_TRAP)
    {
        skt  = SnmpStackDcptMemStubPtr->SNMPNotifyInfo.sktv4;
    }
#endif
#ifdef TCPIP_STACK_USE_IPV6
    if(eTrapMultiAddressType == IPV6_SNMP_TRAP)
    {
        skt = SnmpStackDcptMemStubPtr->SNMPNotifyInfo.socketv6;
    }
#endif

    if(skt == INVALID_UDP_SOCKET)
    {
        return false;
    }

    if(!TCPIP_SNMP_PDUProcessDuplexInit(skt))
    {
        return false;
    }

    len = SnmpStackDcptMemStubPtr->SNMPNotifyInfo.communityLen;
    pCommunity = SnmpStackDcptMemStubPtr->SNMPNotifyInfo.community;
    
     //this will put the start pointer at the beginning of the TX buffer
    (void)TCPIP_UDP_TxOffsetSet(skt,0,false);

    //Get the write pointer:
    SnmpStackDcptMemStubPtr->trapPduOutBufData.head = TCPIP_UDP_TxPointerGet(skt);
    if(SnmpStackDcptMemStubPtr->trapPduOutBufData.head == NULL)
    {
       return false;
    }
    SnmpStackDcptMemStubPtr->trapPduOutBufData.length = 0;
    snmpTrapPutData = &SnmpStackDcptMemStubPtr->trapPduOutBufData;
    
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(STRUCTURE,snmpTrapPutData);            // First item is packet structure
    packetStructLenOffset = snmpTrapPutData->length;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);

    // Put SNMP version info.
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_INT,snmpTrapPutData);              // Int type.
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(1,snmpTrapPutData);                    // One byte long value.

    //Application has to decide which snmp version has to be
    //updated to the notification pdu.
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(SNMP_V1,snmpTrapPutData);              // v1.
    

    //len = strlen(community);  // Save community length for later use.
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(OCTET_STRING,snmpTrapPutData);         // Octet string type.
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(len,snmpTrapPutData);                  // community string length
    while( len-- != 0U)                  // Copy entire string.
    {
        (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)*(pCommunity++),snmpTrapPutData);
    }

    // Put PDU type.  SNMP agent's response is always GET RESPONSE
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(TRAP,snmpTrapPutData);
    pduStructLenOffset = snmpTrapPutData->length;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);

    // Get complete OID string from file snmp.bib.
    if ( !TCPIP_SNMP_OIDStringFindByID(snmpTrapFileDescriptr,SnmpStackDcptMemStubPtr->SNMPNotifyInfo.agentIDVar, &rec, OIDValue, &agentIDLen) )
    {
        return false;
    }

    if ( rec.nodeInfo.Flags.bIsAgentID == 0U)
    {
        return false;
    }

    (void)(*snmpFileShell->fileSeek)(snmpFileShell,snmpTrapFileDescriptr,rec.hData,SYS_FS_SEEK_SET);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_OID,snmpTrapPutData);
    (void)(*snmpFileShell->fileRead)(snmpFileShell,snmpTrapFileDescriptr,(uint8_t*)&len,1U);
    agentIDLen = len;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(len,snmpTrapPutData);
    while( len-- != 0U)
    {
        uint8_t c;
        (void)(*snmpFileShell->fileRead)(snmpFileShell,snmpTrapFileDescriptr,&c,1U);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(c,snmpTrapPutData);
    }
    
    // This agent's IP address.
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(SNMP_IP_ADDR,snmpTrapPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(4,snmpTrapPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(((const TCPIP_NET_IF*)(SnmpStackDcptMemStubPtr->SNMPNotifyInfo.snmpTrapInf))->netIPAddr.v[0],snmpTrapPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(((const TCPIP_NET_IF*)(SnmpStackDcptMemStubPtr->SNMPNotifyInfo.snmpTrapInf))->netIPAddr.v[1],snmpTrapPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(((const TCPIP_NET_IF*)(SnmpStackDcptMemStubPtr->SNMPNotifyInfo.snmpTrapInf))->netIPAddr.v[2],snmpTrapPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(((const TCPIP_NET_IF*)(SnmpStackDcptMemStubPtr->SNMPNotifyInfo.snmpTrapInf))->netIPAddr.v[3],snmpTrapPutData);

    // Geberic/Enterprise Trap code
     (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_INT,snmpTrapPutData);
     (void)TCPIP_SNMP_DataCopyToProcessBuffer(1,snmpTrapPutData);
     (void)TCPIP_SNMP_DataCopyToProcessBuffer(SnmpStackDcptMemStubPtr->gGenericTrapNotification,snmpTrapPutData);

    // Specific Trap code
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_INT,snmpTrapPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(1,snmpTrapPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(SnmpStackDcptMemStubPtr->SNMPNotifyInfo.notificationCode,snmpTrapPutData);

    // Time stamp
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(SNMP_TIME_TICKS,snmpTrapPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(4,snmpTrapPutData);
    TCPIP_UINT32_VAL tStamp;
    tStamp.Val = SnmpStackDcptMemStubPtr->SNMPNotifyInfo.timestamp;

    (void)TCPIP_SNMP_DataCopyToProcessBuffer(tStamp.v[3],snmpTrapPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(tStamp.v[2],snmpTrapPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(tStamp.v[1],snmpTrapPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(tStamp.v[0],snmpTrapPutData);

    // Variable binding structure header
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x30U,snmpTrapPutData);
    varBindStructLenOffset = snmpTrapPutData->length;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);

    // Create variable name-pair structure
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x30U,snmpTrapPutData);
    varPairStructLenOffset = snmpTrapPutData->length;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpTrapPutData);
     
    // Get complete notification variable OID string.
    if ( !TCPIP_SNMP_OIDStringFindByID(snmpTrapFileDescriptr,var, &rec, OIDValue, &OIDLen) )
    {
        return false;
    }

    // Copy OID string into packet.
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_OID,snmpTrapPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)(OIDLen+1U),snmpTrapPutData);
    len = OIDLen;
    pOIDValue = OIDValue;
    while( len-- != 0U)
    {
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(*pOIDValue++,snmpTrapPutData);
    }
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(index,snmpTrapPutData);

    // Encode and Copy actual data bytes
    if ( !TCPIP_SNMP_DataTypeInfoGet(rec.dataType, &dataTypeInfo) )
    {
        return false;
    }

    (void)TCPIP_SNMP_DataCopyToProcessBuffer(dataTypeInfo.asnType,snmpTrapPutData);

    //Modified to Send trap even for  dataTypeInfo.asnType= ASCII_STRING,
    //where dataTypeInfo.asnLen=0xff
    if ( dataTypeInfo.asnLen == 0xffU )
    {
        dataTypeInfo.asnLen=0x4U;
        val.dword=0;
    }

    len = dataTypeInfo.asnLen;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(len,snmpTrapPutData);
    while( len-- != 0U)
    {
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(val.v[len],snmpTrapPutData);
    }

    len = (uint8_t)dataTypeInfo.asnLen           // data bytes count
         + 1U                            // Length byte
         + 1U                            // Data type byte
         + OIDLen                       // OID bytes
         + 2U                            // OID header bytes
         + 1U;                           // index byte

    prevOffset = snmpTrapPutData->length;
    snmpTrapPutData->length = varPairStructLenOffset;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(len,snmpTrapPutData);

    len += 2U;                           // Variable Binding structure header
    snmpTrapPutData->length = varBindStructLenOffset;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(len,snmpTrapPutData);

    len = len
        + 2U                             // Var bind struct header
        + 6U                             // 6 bytes of timestamp
        + 3U                             // 3 bytes of trap code
        + 3U                             // 3 bytes of notification code
        + 6U                             // 6 bytes of agnent IP address
        + agentIDLen                    // Agent ID bytes
        + 2U;                                // Agent ID header bytes
    snmpTrapPutData->length = pduStructLenOffset;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(len,snmpTrapPutData);

    len = len                           // PDU struct length
        + 2U                             // PDU header
        + SnmpStackDcptMemStubPtr->SNMPNotifyInfo.communityLen            // Community string bytes
        + 2U                             // Community header bytes
        + 3U;                            // SNMP version bytes
    snmpTrapPutData->length = packetStructLenOffset;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(len,snmpTrapPutData);

    snmpTrapPutData->length = prevOffset;

    (void)TCPIP_UDP_TxOffsetSet(skt,(uint16_t)snmpTrapPutData->length, false);
    (void)TCPIP_UDP_Flush(skt);
    return true;
}

bool TCPIP_SNMP_ValidateTrapIntf(TCPIP_NET_HANDLE pIf)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNet(pIf);
    if(TCPIPStackHandleToNetLinked(pNetIf) != NULL)
    {
        // check for a valid address
        if(!TCPIPStackIsConfig(pNetIf))
        {
            return true;
        }
    }
    return false;
}

TCPIP_NET_HANDLE TCPIP_SNMP_ClientGetNet(size_t* netIx,TCPIP_NET_HANDLE hNet)
{
    const TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNet(hNet);
    if(*netIx >= TCPIP_STACK_NumberOfNetworksGet())
    {
        return NULL;
    }

    if(pNetIf == NULL)
    {
        pNetIf = (const TCPIP_NET_IF*)TCPIP_STACK_NetDefaultGet();
        *netIx = (size_t)TCPIP_STACK_NetIxGet(pNetIf);
        return pNetIf;
    }

    pNetIf++;
    *netIx= (size_t)TCPIP_STACK_NetIxGet(pNetIf);

    return pNetIf;
}

bool TCPIP_SNMP_TRAPTypeGet(void)
{
    if(!TCPIP_SNMP_IsTrapEnabled())
    {
        return false;
    }
    return gSnmpDcpt.snmp_trapv2_use;
}

bool TCPIP_SNMPV3_TrapTypeGet(void)
{
    if(!TCPIP_SNMP_IsTrapEnabled())
    {
        return false;
    }
    return gSnmpDcpt.snmpv3_trapv1v2_use;
}

bool TCPIP_SNMP_IsTrapEnabled(void)
{
    return gSnmpDcpt.trapEnable;
}

uint8_t TCPIP_SNMP_IsValidCommunity(uint8_t * community)
{
    int i;
    uint8_t communityStr[TCPIP_SNMP_COMMUNITY_MAX_LEN+1];

    if(TCPIP_SNMP_IsTrapEnabled())
    {
        TCPIP_SNMP_AuthTrapFlagSet(false);
        TCPIP_SNMP_TrapSpecificNotificationSet((uint8_t)VENDOR_TRAP_DEFAULT,(uint8_t)ENTERPRISE_SPECIFIC,SNMP_DEMO_TRAP);
    }
    /*
    If the community name is encrypted in the request from the Manager,
    agent required to decrypt it to match with the community it is
    configured for. The response from the agent should contain encrypted community
    name using the same encryption algorithm which Manager used while
    making the request.
    */

    // Validate that community string is a legal size
    if(strlen((char*)community) <= (size_t)TCPIP_SNMP_COMMUNITY_MAX_LEN)
    {
        // Search to see if this is a write community.  This is done before
        // searching read communities so that full read/write access is
        // granted if a read and write community name happen to be the same.
        for(i = 0; i < TCPIP_SNMP_MAX_COMMUNITY_SUPPORT; i++)
        {
            (void)memset(communityStr,0,sizeof(communityStr));
            if(TCPIP_SNMP_WriteCommunityGet(i,TCPIP_SNMP_COMMUNITY_MAX_LEN,communityStr)!= true)
            {
                continue;
            }
            if(strncmp((char*)community, (char*)communityStr, TCPIP_SNMP_COMMUNITY_MAX_LEN) == 0)
            {
                return (uint8_t)WRITE_COMMUNITY;
            }
        }

        // Did not find in write communities, search read communities
        for(i = 0; i < TCPIP_SNMP_MAX_COMMUNITY_SUPPORT; i++)
        {
            (void)memset(communityStr,0,sizeof(communityStr));
            if(TCPIP_SNMP_ReadCommunityGet(i,TCPIP_SNMP_COMMUNITY_MAX_LEN,communityStr)!= true)
            {
                continue;
            }
            if(strncmp((char*)community, (char*)communityStr, TCPIP_SNMP_COMMUNITY_MAX_LEN) == 0)
            {
                return (uint8_t)READ_COMMUNITY;
            }
        }

    }
    if(TCPIP_SNMP_IsTrapEnabled())
    {
        // Could not find any matching community, set up to send a trap
        TCPIP_SNMP_TrapSpecificNotificationSet((uint8_t)VENDOR_TRAP_DEFAULT,(uint8_t)AUTH_FAILURE,SNMP_DEMO_TRAP);
        TCPIP_SNMP_AuthTrapFlagSet(true);
    }
    return (uint8_t)INVALID_COMMUNITY;

}

/****************************************************************************
Function:
    SNMP_ERR_STATUS TCPIP_SNMP_ProcessPDUHeader(PDU_INFO* pduDbPtr,
                                                      char* community, uint8_t* len)

Summary:
    Validates the received udp packet Snmp header.

Description:
    Collects PDU_INFO (SNMP pdu information database),community name,
    community length and length of data payload.
    This function validates the received udp packet for these different
    variables of snmp pdu. The sequence in which these elements
    are received is important. The validation is done for the agent
    processing capabilities and the max UDP packet length as UDP packets
    can not be fragmented.

Precondition:
    TCPIP_UDP_GetIsReady(SNMPAgentSocket) is called in TCPIP_SNMP_Task(),
    it check if there is any packet on SNMP Agent socket,
should return true.

Parameters:
     pduDbPtr  - Pointer to received pdu information database
     community - Pointer to var storing, community string in rxed pdu
     len       - Pointer to var storing, community string length rxed in pdu

Return Values:
    SNMP_ACTION - Snmp request pdu type.

Remarks:
    The received pdu will be processed only if this routine returns the
    pdu type else the pdu is discarded as not Snmp pdu.
 ***************************************************************************/
static SNMP_ERR_STATUS TCPIP_SNMP_ProcessPDUHeader(PDU_INFO* pduDbPtr,char* community, uint8_t *len)
{
    uint32_t tempLen;
    uint16_t temp16;
    SNMP_ACTION pdu = SNMP_ACTION_UNKNOWN;
    SNMP_ERR_STATUS snmpErr = SNMP_NO_ERR;

#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=NULL;
    
    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);
    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;
#endif

    // set the readFromSnmpBuf flag to true when we are reading from UDP SNMP socket buffer
    gSnmpDcpt.readFromSnmpBuf = false;
    //Get complete StructureOF var binds info
    if(TCPIP_SNMP_StructureIsValid(&temp16) == 0U)
    {
        return SNMP_WRONG_LENGTH;
    }

    //Get snmp version info ASN_INT (1 Byte) + Length (1 Byte)+ snmp Version 1 Byte

    tempLen = (uint32_t)temp16;
     if (!TCPIP_SNMP_VarDataTypeIsValidInteger(&tempLen))
     {
        return SNMP_WRONG_TYPE;
     }

        
    pduDbPtr->snmpVersion= (uint8_t)tempLen;
    if ( (tempLen != (uint8_t)SNMP_V1) && ( tempLen != (uint8_t)SNMP_V2C )&&( tempLen != (uint8_t)SNMP_V3 ))
    {
        return SNMP_WRONG_TYPE;
    }

#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
    //Valid snmp packet in the SNMP UDP Socket received
    if(pduDbPtr->snmpVersion == (uint8_t)SNMP_V3)
    {       
        snmpErr=TCPIP_SNMPv3_MsgProcessingModelProcessPDU(SNMP_REQUEST_PDU);
        snmpErr=TCPIP_SNMPv3_UserSecurityModelProcessPDU(SNMP_REQUEST_PDU);
        snmpErr=TCPIP_SNMPv3_ScopedPDUProcessing(SNMP_REQUEST_PDU);


        //Complete SNMPv3 data payload (Encrypted or as plain text) is received

         //Check if received SNMPv3 message is Authenticated
        if((snmpv3EngnDcptMemoryStubPtr->SnmpSecurityLevel & (uint8_t)NO_REPORT_NO_PRIVACY_BUT_AUTH_PROVIDED) == (uint8_t)NO_REPORT_NO_PRIVACY_BUT_AUTH_PROVIDED)
        {
            //Message is authenticated
            if(SNMPv3AuthenticateRxedPduForDataIntegrity() != (uint8_t)SNMPV3_MSG_AUTH_PASS)
            {
                return SNMP_AUTH_ERROR;
            }
        }

        //Check if received SNMPv3 message is Encrypted.
        if((snmpv3EngnDcptMemoryStubPtr->SnmpSecurityLevel & (uint8_t)NO_REPORT_PRIVACY_AND_AUTH_PROVIDED) == (uint8_t)NO_REPORT_PRIVACY_AND_AUTH_PROVIDED)
        {
           //Message is encrypted. Decrypt the message for processing
            if(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx].userPrivType == (uint8_t)SNMPV3_AES_PRIV)
            {   //user privacy protocol is AES
                if(SNMPv3AESDecryptRxedScopedPdu() != (uint8_t)SNMPV3_MSG_PRIV_PASS)
                {
                    return SNMP_PRIVACY_ERROR;
                }
            }
            else if(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx].userPrivType == (uint8_t)SNMPV3_DES_PRIV)
            {   //user privacy protocol is DES
                if(SNMPv3DESDecryptRxedScopedPdu() != (uint8_t)SNMPV3_MSG_PRIV_PASS)
                {
                    return SNMP_PRIVACY_ERROR;
                }
            }
            else
            {
                return SNMP_PRIVACY_ERROR;
            }
        }
    }

    else
#endif  
    {
       // set the readFromSnmpBuf flag to true when we are reading from UDP SNMP socket buffer
        if((tempLen == (uint8_t)SNMP_V1)||(tempLen == (uint8_t)SNMP_V2C))
        {
            // This function populates response as it processes community string.
            if (!TCPIP_SNMP_CommunityStringIsValid(community, len))
            {
                return SNMP_WRONG_VALUE;
            }

        // Fetch and validate pdu type.
            if ( !TCPIP_SNMP_PDUIsValid(&pdu) )
            {
                return SNMP_WRONG_TYPE;
            }

            pduDbPtr->pduType = (uint8_t)pdu;

            //Get_Bulk_Request is not defined in SNMP V1, hence discard udp request packet
            if(pduDbPtr->snmpVersion==(uint8_t)SNMP_V1 && pduDbPtr->pduType == (uint8_t)GET_BULK_REQUEST)
            {
                    return SNMP_WRONG_TYPE;
            }

            // Ask main application to verify community name against requested pdu type.
            if(TCPIP_SNMP_IsValidCommunity((uint8_t *)community)==(uint8_t)INVALID_COMMUNITY)
            {
                return SNMP_WRONG_VALUE;
            }
        }
    }
    return snmpErr;
}

/****************************************************************************
Function:
    bool SNMPProcessGetSetHeader(PDU_INFO* pduDbPtr)

Summary:
    Validates the received udp packet Get/Set request header.

Description:
    All the variables of snmp pdu request header are validated for their
    data types. Collects request_id for the snmp request pdu. Fetch,validates
    error status,error index and discard as they are need not to be processed
    as received in request pdu. Collects non repeaters and max repeaters
    values in case of Get_Bulk request.

Precondition:
    TCPIP_SNMP_ProcessPDUHeader() is called and returns pdu type and do not returns
    SNMP_ACTION_UNKNOWN

Parameters:
    pduDbPtr  - Pointer to received pdu information database.

Return Values:
    true  - If the received request header is validated and passed.
    false - If rxed request header is not valid.

Remarks:
    The request pdu will be processed only if this routine returns true
 ***************************************************************************/
static bool TCPIP_SNMP_ProcessHeaderGetSet(PDU_INFO* pduDbPtr)
{
    uint32_t tempData;

    // Fetch and save request ID.
    if ( TCPIP_SNMP_VarDataTypeIsValidInteger(&tempData) )
    {
         pduDbPtr->requestID = tempData;
    }
    else
    {
        return false;
    }

    if((pduDbPtr->snmpVersion == (uint8_t)SNMP_V1 || pduDbPtr->snmpVersion == (uint8_t)SNMP_V2C /*|| pduDbPtr->snmpVersion == (uint8_t)SNMP_V3*/) &&(pduDbPtr->pduType != (uint8_t)GET_BULK_REQUEST))
    {
        // Fetch and discard error status
        if ( !TCPIP_SNMP_VarDataTypeIsValidInteger(&tempData) )
        {
            return false;
        }

        // Fetch and disacard error index
        return TCPIP_SNMP_VarDataTypeIsValidInteger(&tempData);
    }
    else if((pduDbPtr->snmpVersion == (uint8_t)SNMP_V2C /*|| pduDbPtr->snmpVersion == (uint8_t)SNMP_V3*/ )&& pduDbPtr->pduType == (uint8_t)GET_BULK_REQUEST )
    {
        // Fetch non-repeaters value
        if ( TCPIP_SNMP_VarDataTypeIsValidInteger(&tempData) )
        {
             pduDbPtr->nonRepeators=(uint8_t)tempData&0xFFU;
        }
        else
        {
            return false;
        }

        // Fetch Max-repetitions value
        if(TCPIP_SNMP_VarDataTypeIsValidInteger(&tempData))
        {
             pduDbPtr->maxRepetitions=(uint8_t)tempData&0xFFU;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    
    return true;
}


/****************************************************************************
Function:
    bool TCPIP_SNMP_ProcessVariables(PDU_INFO* pduDbPtr,uint8_t* community, uint8_t len)

Summary:
    This routine processes the snmp request and parallely creates the
    response pdu.

Description:
    Once the received pdu is validated as Snmp pdu, it is forwarded for
    processing to this routine. This rotuine handles Get, Get_Next, Get_Bulk,
    Set request and creates appropriate response as Get_Response.
    This routine will decide on whether the request pdu should be processed
    or be discarded.

Precondition:
    The received udp packet is varified as SNMP request.
    TCPIP_SNMP_ProcessPDUHeader() and SNMPProcessGetSetHeader() returns but false.

Parameters:
    pduDbPtr  - Pointer to received pdu information database
community - Pointer to var, storing community string in rxed pdu
    len       - Pointer to var, storing community string length rxed in pdu

Return Values:
    true    - If the snmp request processing is successful.
    false   - If the processing failed else the processing is not completed.

Remarks:
    None
 ***************************************************************************/
static bool TCPIP_SNMP_ProcessVariables(PDU_INFO* pduDbPtr,char* community, uint8_t len)
{   
    bool getbulkOverFlowFlag = false;
    uint8_t temp =0;
    uint8_t OIDValue[TCPIP_SNMP_OID_MAX_LEN] = {0};
    uint8_t OIDLen=0;
    uint8_t varIndex =0;
    uint8_t communityLen=0,commRetVal=0;
    uint8_t noOfOIDsInReq=0,tempNonRepeators=0,noOfVarToBeInResponse=0;
    uint8_t repeatCntr,varBindCntr;
    uint8_t Getbulk_N=0,Getbulk_M=0,Getbulk_R=0;/*Refer RFC 3416 Section "4.2.3" GetBulkRequest-PDU*/
    uint8_t oidLookUpRet=0;
    uint8_t templen=0;
    uint8_t successor=0;// 'I'th lexicographic successor
    uint8_t *ptemp;
    uint8_t *ptroid;
    uint8_t *rxedCommunityName;
    uint16_t varBindStructOffset=0;
    uint16_t tempTxOffset=0;
    uint16_t prevOffset=0;
    uint16_t packetStructLenOffset=0;
    uint16_t pduLenOffset=0;
    uint16_t errorStatusOffset=0;
    uint16_t errorIndexOffset=0;    
    uint16_t varStructLenOffset=0;  
    uint16_t prevSnmpRxOffset=0;
    TCPIP_UINT16_VAL varBindingLen;
    varBindingLen.Val = 0;
    TCPIP_UINT16_VAL tempLen={0};
    TCPIP_UINT16_VAL varPairLen;
    varPairLen.Val = 0;
    static TCPIP_UINT16_VAL varBindLen={0};
    OID_INFO OIDInfo;
    (void)memset(&OIDInfo, 0, sizeof(OID_INFO));
    SNMP_ERR_STATUS errorStatus;
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
    uint8_t agentIDLen;
    OID_INFO rec;
#endif
    bool bSnmpGenError = false;
    SNMP_BUFFER_DATA *snmpPutData=NULL;
    static int smSnmp = (int)SM_PKT_STRUCT_LEN_OFFSET;

    snmpReqVarErrStatus.noSuchInstanceErr=0x0000U;
    snmpReqVarErrStatus.noSuchNameErr=0x0000U;
    snmpReqVarErrStatus.noSuchObjectErr=0x0000U;
    snmpReqVarErrStatus.endOfMibViewErr=0x0000U;

    rxedCommunityName=(uint8_t *)community;
    tempTxOffset = SNMPTxOffset;
    OIDLen = 0;
    varBindLen.Val=0x0000U;
    SnmpStackDcptMemStubPtr->getZeroInstance = false;


    bool doRet = false;

    while(true)
    {
        switch(smSnmp)
        {

            // Before each variables are processed, prepare necessary header.

            case (int)SM_PKT_STRUCT_LEN_OFFSET:
                snmpPutData = &SnmpStackDcptMemStubPtr->outPduBufData;

                varPairLen.Val=0x0000U;

                (void)TCPIP_SNMP_DataCopyToProcessBuffer(STRUCTURE,snmpPutData);  // first item in snmp packet
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x82U,snmpPutData);

                // Since we do not know length of structure at this point, use
                // placeholder bytes that will be replaced with actual value.

                packetStructLenOffset = snmpPutData->length;
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpPutData);
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpPutData);

                // Put SNMP version info - only v1.0 is supported.
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_INT,snmpPutData);              // Int type.
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(1,snmpPutData);                    // One byte long value.
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(pduDbPtr->snmpVersion,snmpPutData);              // v1.0.

                // Put community string
                communityLen = len;             // Save community length for later use.
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(OCTET_STRING,snmpPutData);         // Octet string type.
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(len,snmpPutData);                  // community string length
                while( len-- != 0U)                  // Copy entire string.
                {
                    (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)*community++,snmpPutData);
                }

                smSnmp++;
                break;

            case (int)SM_RESPONSE_PDU_LEN_OFFSET:

                // Put PDU type.  SNMP agent's response is always GET RESPONSE
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(GET_RESPONSE,snmpPutData);

                // Since we don't know length of this response, use placeholders until
                // we know for sure...
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x82U,snmpPutData);
                pduLenOffset = snmpPutData->length;
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpPutData); // Be prepared for 2 byte-long length
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpPutData);

                // Put original request back.
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_INT,snmpPutData);    // Int type.
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(4,snmpPutData);      // To simplify logic, always use 4 byte long requestID
                (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)((pduDbPtr->requestID>>24)&0xFFU),snmpPutData); // Start MSB
                (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)((pduDbPtr->requestID>>16)&0xFFU),snmpPutData);
                (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)((pduDbPtr->requestID>>8)&0xFFU),snmpPutData);
                (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)(pduDbPtr->requestID&0xFFU),snmpPutData);

                smSnmp++;
                break;
            
            case (int)SM_ERROR_STATUS_OFFSET :

                // Put error status.
                // Since we do not know error status, put place holder until we know it...
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_INT,snmpPutData);              // Int type
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(1,snmpPutData);                    // One byte long.
                errorStatusOffset = snmpPutData->length;
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpPutData);                    // Placeholder.
                smSnmp++;
                break;

            case (int)SM_ERROR_INDEX_OFFSET :

                // Similarly put error index.
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_INT,snmpPutData);              // Int type
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(1,snmpPutData);                    // One byte long
                errorIndexOffset = snmpPutData->length;
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpPutData);                    // Placeholder.

                varIndex    = 0;
                errorStatus = SNMP_NO_ERR;

                smSnmp++;
                break;

            case (int)SM_FIND_NO_OF_REQUESTED_VARBINDS:

                // Decode variable binding structure
                if ( TCPIP_SNMP_StructureIsValid(&varBindingLen.Val) == 0U)
                {
                    doRet = true;
                    break;
                }

                //Find number of OIDs/varbinds's data requested in received PDU.
                noOfOIDsInReq=TCPIP_SNMP_OIDsCountGet(varBindingLen.Val);

                smSnmp++;
                break;

            case (int)SM_FIND_NO_OF_RESPONSE_VARBINDS:

                //Calulate number of variables to be responded for the received request
                Getbulk_N = noOfOIDsInReq; Getbulk_M=0; Getbulk_R=0;
                if(((pduDbPtr->snmpVersion == (uint8_t)SNMP_V2C)|| (pduDbPtr->snmpVersion == (uint8_t)SNMP_V3)) && (pduDbPtr->pduType == (uint8_t)GET_BULK_REQUEST))
                {
                    if((pduDbPtr->nonRepeators) <= noOfOIDsInReq)
                    {
                        Getbulk_N = pduDbPtr->nonRepeators;
                    }

                    Getbulk_M = pduDbPtr->maxRepetitions;

                    if( noOfOIDsInReq >= Getbulk_N )
                    {
                        Getbulk_R = noOfOIDsInReq-Getbulk_N;
                    }
                }

                tempNonRepeators = Getbulk_N;

                noOfVarToBeInResponse = Getbulk_N + (Getbulk_M * Getbulk_R);//Refer RFC 3416

                smSnmp++;
                break;

            case (int)SM_VARBIND_STRUCT_OFFSET:

                // Put variable binding response structure
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(STRUCTURE,snmpPutData);
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x82U,snmpPutData);

                // Since we do not know data payload length, put place holder until we know it...
                varBindStructOffset = snmpPutData->length;
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpPutData);
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpPutData);

                varBindLen.Val = 0;

                smSnmp++;
                break;

            case (int)SM_VARSTRUCT_LEN_OFFSET:

                /*  If the getbulk request is received with zero non-repeaters, process
                        variable State Machine jumps to SM_MAX_REPETITIONS. Modify the Rx
                        and Tx offset accordigly. */
                if(Getbulk_N==0u)
                {
                #ifdef TCPIP_STACK_USE_SNMPV3_SERVER
                    if((pduDbPtr->snmpVersion == (uint8_t)SNMP_V3))
                    {
                            // Get complete OID string from snmp.bib file.
                        (void)TCPIP_SNMP_OIDStringFindByID(snmpFileDescrptr,MICROCHIP,&rec, OIDValue, &agentIDLen);

                        if ( rec.nodeInfo.Flags.bIsAgentID != 0U)
                        {
                            (void)(*snmpFileShell->fileSeek)(snmpFileShell,snmpFileDescrptr, rec.hData,SYS_FS_SEEK_SET);
                        }
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_OID,snmpPutData);
                        (void)(*snmpFileShell->fileRead)(snmpFileShell,snmpFileDescrptr,&len,1U);

                        agentIDLen = len;
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(len,snmpPutData);
                        while( len-- != 0U)
                        {
                            uint8_t c;

                            (void)(*snmpFileShell->fileRead)(snmpFileShell,snmpFileDescrptr,&c,1U);

                            (void)TCPIP_SNMP_DataCopyToProcessBuffer(c,snmpPutData);
                        }

                    }
                    else
                    #endif
                    {
                        prevSnmpRxOffset=SNMPRxOffset;
                        smSnmp=(int)SM_MAX_REPETITIONS;
                        varStructLenOffset = snmpPutData->length;
                        snmpPutData->length=snmpPutData->length+4U;
                        break;
                    }
                }
            
/*
    Need to know what variable we are processing, so that in case
    if there is problem for that variable, we can put it in
    errorIndex location of SNMP packet.
*/
                varIndex++;

                // Decode variable length structure
                temp = TCPIP_SNMP_StructureIsValid(&tempLen.Val);
                if ( temp == 0U )
                {
                    TCPIP_SNMP_ErrorStatusSet(errorStatusOffset,errorIndexOffset,SNMP_GEN_ERR,varIndex,snmpPutData);
                    bSnmpGenError = true;
                    break;
                }

                varBindingLen.Val -= tempLen.Val;
                varBindingLen.Val -= temp;

                varStructLenOffset = snmpPutData->length;

                if(pduDbPtr->pduType == (uint8_t)GET_BULK_REQUEST )
                {
                    snmpPutData->length=snmpPutData->length+4U;
                }
                smSnmp++;
                break;

            case (int)SM_POPULATE_REQ_OID:

            /*  Populate received pdu for the requested OIDs and also create the
                response pdu on the go.*/
                // Decode next object
                if ( !TCPIP_SNMP_OIDIsValid(OIDValue, &OIDLen) )
                {
                    TCPIP_SNMP_ErrorStatusSet(errorStatusOffset,errorIndexOffset,SNMP_GEN_ERR,varIndex,snmpPutData);
                    bSnmpGenError = true;
                    break;
                }
                // For Get & Get-Next, value must be NULL.
                if ( pduDbPtr->pduType != (uint8_t)SET_REQUEST )
                {
                    if ( !TCPIP_SNMP_DataIsASNNull() )
                    {
                        doRet = true;
                        break;
                    }
                }
                if(pduDbPtr->pduType != (uint8_t)GET_BULK_REQUEST )
                {
                    // Prepare response - original variable
                    (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_OID,snmpPutData);
                    (void)TCPIP_SNMP_DataCopyToProcessBuffer(OIDLen,snmpPutData);
                    ptemp = OIDValue;
                    temp = OIDLen;
                    while( temp-- != 0U)
                    {
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(*ptemp++,snmpPutData);
                    }
                }

                /*
                   Match "rxedCommunityName" to "readCommunity" to authorize access
                   to private MIB objects.
                   As we start supporting the secured encrypted community transaction,
                   rxed community string can be an encrypted string which the agent
                   need to decrypt and validate to authorize access.
                   The agent should respond with encrypted community name.
                */
                if((pduDbPtr->snmpVersion != (uint8_t)SNMP_V3))
                {
                    commRetVal=TCPIP_SNMP_IsValidCommunity(rxedCommunityName);

                    smSnmp=(int)SM_PKT_STRUCT_LEN_OFFSET;    // Start out assuming commRetVal == INVALID_COMMUNITY
                    if(pduDbPtr->pduType == (uint8_t)SET_REQUEST)
                    {
                        if(commRetVal==(uint8_t)WRITE_COMMUNITY)
                        {   //If SET request, then "community==WRITE_COMMUNITY" is must.
                            smSnmp=(int)SM_FIND_OID_IN_MIB;
                        }
                    }
                    else
                    {
                        if(commRetVal!=(uint8_t)INVALID_COMMUNITY)
                        { //If any GET request, then "community!=INVALID_COMMUNITY" is must (community is WRITE_COMMUNITY or READ_COMMUNITY).
                            smSnmp=(int)SM_FIND_OID_IN_MIB;
                        }
                    }
                }
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
                else
                {
                    smSnmp=(int)SM_FIND_OID_IN_MIB;
                }
#endif
                //Verify if trying to access the private object
                //Application has to decide on what community name should allowed to
                //read write the private mib objects.

                if(TCPIP_SNMP_PvtMIBObjIsRequested(OIDValue) && (smSnmp==(int)SM_PKT_STRUCT_LEN_OFFSET) )
                {
                    //If private mib object is requested and community do not match,
                    //generate authentication failure TRAP

                    Getbulk_N=0;
                    noOfVarToBeInResponse=0;
                    smSnmp=(int)SM_PKT_STRUCT_LEN_OFFSET;

                    //Searching the requested OID in the MIB database
                    oidLookUpRet = TCPIP_SNMP_OIDFindInMgmtInfoBase(snmpFileDescrptr,pduDbPtr,OIDValue, OIDLen, &OIDInfo);
                    SnmpStackDcptMemStubPtr->gOIDCorrespondingSnmpMibID=(uint8_t)OIDInfo.id;

                    SNMPTxOffset=packetStructLenOffset-2U;
                    SnmpStackDcptMemStubPtr->gSpecificTrapNotification=(uint8_t)VENDOR_TRAP_DEFAULT;
                    SnmpStackDcptMemStubPtr->gGenericTrapNotification=(uint8_t)AUTH_FAILURE;
                    SnmpStackDcptMemStubPtr->gSendTrapFlag=true;
                }
                break;
            
            case (int)SM_FIND_OID_IN_MIB:

                /* Search for the requested OID in the MIB database with the agent.*/

                if(Getbulk_N!= 0u)
                {
                    Getbulk_N--;
                }

                if(Getbulk_N==0u)
                {
                    prevSnmpRxOffset=SNMPRxOffset;
                }

                noOfVarToBeInResponse--;

                //Searching the requested OID in the MIB database
                oidLookUpRet = TCPIP_SNMP_OIDFindInMgmtInfoBase(snmpFileDescrptr,pduDbPtr,OIDValue, OIDLen, &OIDInfo);

                if(oidLookUpRet != (uint8_t)true /*&& (pduDbPtr->pduType != (uint8_t)GET_NEXT_REQUEST) */&& (pduDbPtr->pduType != (uint8_t)GET_BULK_REQUEST))
                {
                    snmpPutData->length = varStructLenOffset;

                    // Put corresponding variable response structure
                    (void)TCPIP_SNMP_DataCopyToProcessBuffer(STRUCTURE,snmpPutData);
                    (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x82U,snmpPutData);

                    varStructLenOffset= snmpPutData->length;
                    (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x00U,snmpPutData);//Place holder
                    (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x00U,snmpPutData);

                    // ASN OID data type
                    templen=OIDLen;
                    ptroid=OIDValue;
                    (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_OID,snmpPutData);

                    if(SnmpStackDcptMemStubPtr->appendZeroToOID != 0U)
                    {
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(OIDLen+1U, snmpPutData);//for appending "0"
                    }
                    else
                    {
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(OIDLen,snmpPutData);//do not append "0"
                    }

                    //Put OID
                    while( templen--!= 0U)
                    {
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(*ptroid++,snmpPutData);
                    }

                    if(SnmpStackDcptMemStubPtr->appendZeroToOID != 0U)
                    {
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x00U,snmpPutData);//Appending '0' to OID in response
                        varPairLen.Val += (uint16_t)OIDLen+1U+2U; //Modify the response length
                    }
                    else
                    {
                        varPairLen.Val += (uint16_t)OIDLen+2U;
                    }

                    //update and send the error status and the error index.
                    if(pduDbPtr->snmpVersion == (uint8_t)SNMP_V1)
                    {
                        errorStatus = SNMP_NO_SUCH_NAME;
                        TCPIP_SNMP_ErrorStatusSet(errorStatusOffset,errorIndexOffset,SNMP_NO_SUCH_NAME,varIndex,snmpPutData);
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_NULL,snmpPutData);
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpPutData);
                    }
                    else if(((pduDbPtr->snmpVersion == (uint8_t)SNMP_V2C) ||( pduDbPtr->snmpVersion == (uint8_t)SNMP_V3)) && pduDbPtr->pduType != (uint8_t)SET_REQUEST)
                    {
                        if(pduDbPtr->pduType == (uint8_t)SNMP_GET)
                        {
                            (void)TCPIP_SNMP_DataCopyToProcessBuffer(oidLookUpRet,snmpPutData);
                            (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x00U,snmpPutData);
                            if(oidLookUpRet == (uint8_t)SNMP_NO_SUCH_OBJ)
                            {
                                snmpReqVarErrStatus.noSuchObjectErr |= ((uint16_t)0x0001U << (uint16_t)varIndex);
                            }
                            else if(oidLookUpRet == (uint8_t)SNMP_NO_SUCH_INSTANCE)
                            {
                                snmpReqVarErrStatus.noSuchInstanceErr |= ((uint16_t)0x0001U << (uint16_t)varIndex);
                            }
                            else
                            {
                                // do nothing
                            }
                        }
                        else if((pduDbPtr->pduType == (uint8_t)SNMP_GET_NEXT) && (oidLookUpRet == (uint8_t)SNMP_END_OF_MIB_VIEW))
                        {
                            (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)SNMP_END_OF_MIB_VIEW,snmpPutData);
                            (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpPutData);
                        }
                        else
                        {
                            // do nothing
                        }
                    }
                    else
                    {
                        // do nothing
                    }

                    if(pduDbPtr->snmpVersion !=(uint8_t)SNMP_V3)
                    {
                        varPairLen.Val +=2U;
                    }

                    varBindLen.Val += 4U // Variable Pair STRUCTURE byte + 1 length byte.
                    + varPairLen.Val;

                    //Now update the place holder for var pair length
                    prevOffset = snmpPutData->length;
                    snmpPutData->length = varStructLenOffset;

                    (void)TCPIP_SNMP_DataCopyToProcessBuffer(varPairLen.v[1],snmpPutData);
                    (void)TCPIP_SNMP_DataCopyToProcessBuffer(varPairLen.v[0],snmpPutData);

                    snmpPutData->length = prevOffset;
                    varPairLen.Val=0x00U;

                    //Reset to state machine to access the next oid in request
                    smSnmp=(int)SM_VARSTRUCT_LEN_OFFSET;
                    break;
                }
                smSnmp++;
                break;

            case (int)SM_NON_REPETITIONS:

                /*  Variables in get,get_next,set and get_bulk ( non repetition variables)
                        of snmp request are processed in this part of the state machine.*/

                //Save SnmpTxOffsetfor future uses.
                prevOffset = snmpPutData->length;
                snmpPutData->length = varStructLenOffset;

                //Start response variable binding with ASN STRUCTURE type.
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(STRUCTURE,snmpPutData);
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x82U,snmpPutData);

                varStructLenOffset= snmpPutData->length;
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x00U,snmpPutData); //place holder
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x00U,snmpPutData);

                (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_OID,snmpPutData);

                if(pduDbPtr->pduType == (uint8_t)SNMP_SET)
                {
                    templen=OIDLen;
                    ptroid=OIDValue;
                    //to validate the REC ID is present or not

                    if(TCPIP_SNMP_RecordIDValidation(pduDbPtr->snmpVersion,OIDInfo.nodeInfo.Flags.bIsIDPresent != 0U, (uint16_t)OIDInfo.id, OIDValue,OIDLen) != true)
                    {

                             /*if the variable binding's name specifies a
                         * variable which does not exist and could not ever be
                         * created, then the value of the Response-PDU's error-
                         * status field is set to `noCreation', and the value of its
                         * error-index field is set to the index of the failed
                         * variable binding.
                         */
                        errorStatus = SNMP_NO_CREATION;
                        smSnmp=(int)SM_PKT_STRUCT_LEN_OFFSET;
                        doRet = true;
                        break;
                    }

                    if(SnmpStackDcptMemStubPtr->appendZeroToOID != 0U)
                    {
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(OIDLen+1U,snmpPutData);//for appending "0"
                    }
                    else
                    {
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(OIDLen,snmpPutData);//do not append "0"
                    }

                    //Put OID
                    while( templen-- != 0U )
                    {
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(*ptroid++,snmpPutData);
                    }

                    if(SnmpStackDcptMemStubPtr->appendZeroToOID != 0U)
                    {
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x00U,snmpPutData);//Appending '0' to OID in response
                    }

                    // Now process the SET command
                    temp = TCPIP_SNMP_ProcessSetVar(pduDbPtr,&OIDInfo, &errorStatus);

                    if ( errorStatus != SNMP_NO_ERR )
                    {
                        //SET var command failed. Update the error status.
                        TCPIP_SNMP_ErrorStatusSet(errorStatusOffset,
                        errorIndexOffset,
                        errorStatus,
                        varIndex,snmpPutData);
                    }

                    if(SnmpStackDcptMemStubPtr->appendZeroToOID != 0U)
                    {
                        varPairLen.Val = ((uint16_t)OIDLen + 1U + 2U + (uint16_t)temp);   // OID name + header bytes + value bytes as put by SetVar
                    }
                    else
                    {
                        varPairLen.Val = ((uint16_t)OIDLen+2U+(uint16_t)temp);
                    }

                }
                else if((pduDbPtr->pduType == (uint8_t)SNMP_GET)  || ((pduDbPtr->pduType == (uint8_t)SNMP_V2C_GET_BULK) && (oidLookUpRet != 0U)))
                {
                    //to validate the REC ID is present or not
                    if(TCPIP_SNMP_RecordIDValidation(pduDbPtr->snmpVersion,OIDInfo.nodeInfo.Flags.bIsIDPresent != 0U,(uint16_t)OIDInfo.id,OIDValue,OIDLen) != true)
                    {
                        smSnmp=(int)SM_PKT_STRUCT_LEN_OFFSET;
                        doRet = true;
                        break;
                    }
                    templen=OIDLen;
                    ptroid=OIDValue;

                    if(SnmpStackDcptMemStubPtr->appendZeroToOID != 0U)
                    {
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(OIDLen+1U,snmpPutData);//for appending "0"
                    }
                    else
                    {
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(OIDLen,snmpPutData);//do not append "0"
                    }

                    //Put OID
                    while( templen-- != 0U )
                    {
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(*ptroid++,snmpPutData);
                    }

                    if(SnmpStackDcptMemStubPtr->appendZeroToOID != 0U)
                    {
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x00U,snmpPutData);//Appending '0' to OID in response
                        varPairLen.Val = ((uint16_t)OIDLen + 2U+1U);
                    }
                    else
                    {
                        varPairLen.Val = ((uint16_t)OIDLen +2U);
                    }

                    //Now process the GET command
                    temp=TCPIP_SNMP_ProcessGetVar(&OIDInfo,false,pduDbPtr);

                }
                else if((pduDbPtr->pduType == (uint8_t)SNMP_GET_NEXT)|| ((pduDbPtr->pduType == (uint8_t)SNMP_V2C_GET_BULK) && (oidLookUpRet == 0U)))
                {
                    temp=TCPIP_SNMP_ProcessGetNextVar(&OIDInfo,pduDbPtr);

                    //If Get Next command failed
                    if(temp==0u)
                    {
                        templen=OIDLen;
                        ptroid=OIDValue;

                        if(SnmpStackDcptMemStubPtr->appendZeroToOID != 0U)
                        {
                            (void)TCPIP_SNMP_DataCopyToProcessBuffer(OIDLen+1U,snmpPutData);//for appending "0"
                        }
                        else
                        {
                            (void)TCPIP_SNMP_DataCopyToProcessBuffer(OIDLen,snmpPutData);//do not append "0"
                        }

                        //Put OID
                        while( templen-- != 0U )
                        {
                            (void)TCPIP_SNMP_DataCopyToProcessBuffer(*ptroid++,snmpPutData);
                        }

                        if(SnmpStackDcptMemStubPtr->appendZeroToOID != 0U)
                        {
                            (void)TCPIP_SNMP_DataCopyToProcessBuffer(0x00U,snmpPutData);//Appending '0' to OID in response
                        }
                    }
                }
                else
                {
                    // do nothing
                }


                /*  If the request command processing is failed, update
                        the error status, index accordingly and response pdu.*/
                if(temp == 0u &&(pduDbPtr->pduType != (uint8_t)SNMP_SET))
                {
                    if(pduDbPtr->snmpVersion == (uint8_t)SNMP_V1)
                    {
                        errorStatus = SNMP_NO_SUCH_NAME;
                        TCPIP_SNMP_ErrorStatusSet(errorStatusOffset,errorIndexOffset,SNMP_NO_SUCH_NAME,
                                               varIndex,snmpPutData);
                    }

                    (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_NULL,snmpPutData);
                    (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpPutData);

                    if((pduDbPtr->pduType == (uint8_t)SNMP_GET_NEXT|| pduDbPtr->pduType == (uint8_t)SNMP_V2C_GET_BULK)&&pduDbPtr->snmpVersion == (uint8_t)SNMP_V2C)
                    {
                        snmpPutData->length=snmpPutData->length-2U;
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)SNMP_END_OF_MIB_VIEW,snmpPutData);
                        (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpPutData);
                    }

                    if((pduDbPtr->pduType == (uint8_t)SNMP_GET) || ((pduDbPtr->pduType == (uint8_t)SNMP_V2C_GET_BULK) && (oidLookUpRet != 0U)))
                    {
                        temp = 2;
                    }
                    else if((pduDbPtr->pduType == (uint8_t)SNMP_GET_NEXT)|| ((pduDbPtr->pduType == (uint8_t)SNMP_V2C_GET_BULK) && (oidLookUpRet == 0U)))
                    {
                        varPairLen.Val = ((uint16_t)OIDLen + 1U + 2U + 2U); // as put by GetNextVar() + 2 OID header + 2 END_OF_MIB_VIEW bytes
                    }
                    else
                    {
                        // do nothing
                    }


                    /*  Applications can make use of the below information
                            to find the error status for the given variable and to
                            build the logic arround. */
                    snmpReqVarErrStatus.noSuchNameErr    |= ((uint16_t)0x0001U << (uint16_t)varIndex);
                    snmpReqVarErrStatus.noSuchObjectErr  |= ((uint16_t)0x0001U << (uint16_t)varIndex);
                    snmpReqVarErrStatus.noSuchInstanceErr|= ((uint16_t)0x0001U << (uint16_t)varIndex);
                    snmpReqVarErrStatus.endOfMibViewErr  |= ((uint16_t)0x0001U << (uint16_t)varIndex);

                }
                else if((pduDbPtr->pduType == (uint8_t)SNMP_GET_NEXT)|| ((pduDbPtr->pduType == (uint8_t)SNMP_V2C_GET_BULK) && (oidLookUpRet == 0U)))
                {
                    if(SnmpStackDcptMemStubPtr->getZeroInstance)
                    {
                        varPairLen.Val += ((uint16_t)temp+2U);
                    }
                    else
                    {
                        varPairLen.Val = ((uint16_t)temp + 2U);
                    }
                }
                else
                {
                    // do nothing
                }

                if((pduDbPtr->pduType == (uint8_t)SNMP_GET) || ((pduDbPtr->pduType == (uint8_t)SNMP_V2C_GET_BULK) && (oidLookUpRet != 0U)))
                {
                    varPairLen.Val += temp;
                }

                varBindLen.Val += 4U // Variable Pair STRUCTURE byte + 1 length byte.
                + varPairLen.Val;

                //Update place holder
                prevOffset =  snmpPutData->length;
                snmpPutData->length = varStructLenOffset;
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(varPairLen.v[1],snmpPutData);
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(varPairLen.v[0],snmpPutData);

                snmpPutData->length = prevOffset;
                varStructLenOffset = snmpPutData->length;

                /*  Decide on the number of Non repetition variables remained to
                        be processed, decide the course of state machine.*/

                if((pduDbPtr->pduType==(uint8_t)GET_BULK_REQUEST) && ((pduDbPtr->snmpVersion == (uint8_t)SNMP_V2C)|| (pduDbPtr->snmpVersion == (uint8_t)SNMP_V3))&&( Getbulk_N == 0u))
                {
                    if((varStructLenOffset - tempTxOffset) >= (uint16_t)TCPIP_SNMP_MAX_MSG_SIZE)
                    {
                        getbulkOverFlowFlag = true;
                        break;
                    }
                    else
                    {
                        smSnmp=(int)SM_MAX_REPETITIONS;
                    }
                }
                else
                {
                    smSnmp=(int)SM_VARSTRUCT_LEN_OFFSET;
                }
                varPairLen.Val=0x00U;
                SnmpStackDcptMemStubPtr->getZeroInstance = false;

                /* check length*/
                break;

            case (int)SM_MAX_REPETITIONS:

                /*Process each variable in request as Get_Next for
                  Getbulk_M (Max_repetition) times */
                for(repeatCntr=0;repeatCntr<Getbulk_M;repeatCntr++)
                {
                    SNMPRxOffset=prevSnmpRxOffset;

                    //Process every veriable in the request.
                    for(varBindCntr=0;varBindCntr<Getbulk_R;varBindCntr++)
                    {
                        if(varBindCntr==0u)
                        {
                            varIndex=(noOfOIDsInReq-Getbulk_R);
                        }
                        varIndex++;
                        if(((snmpReqVarErrStatus.endOfMibViewErr >> (tempNonRepeators+varBindCntr+1U))&0x0001U) != 0U)
                        {
                            noOfVarToBeInResponse--;
                            temp = TCPIP_SNMP_StructureIsValid(&tempLen.Val);

                            if(varBindCntr!=Getbulk_R)
                            {
                                SNMPRxOffset=SNMPRxOffset+tempLen.Val;//2+OIDLen+2;
                            }
                            continue;
                        }

                        if(noOfVarToBeInResponse != 0U)
                        {
                            noOfVarToBeInResponse--;
                        }
                        varPairLen.Val = 0;
                        prevOffset = snmpPutData->length;
                        snmpPutData->length = varStructLenOffset;
                        if(TCPIP_SNMP_DataCopyToProcessBuffer(STRUCTURE,snmpPutData)!= true)
                        {
                            getbulkOverFlowFlag = true;
                            break;
                        }
                        if(TCPIP_SNMP_DataCopyToProcessBuffer(0x82U,snmpPutData)!= true)
                        {
                            getbulkOverFlowFlag = true;
                            break;
                        }
                        varStructLenOffset= snmpPutData->length;
                        if(TCPIP_SNMP_DataCopyToProcessBuffer(0x00U,snmpPutData)!= true)
                        {
                            getbulkOverFlowFlag = true;
                            break;
                        }
                        if(TCPIP_SNMP_DataCopyToProcessBuffer(0x00U,snmpPutData)!= true)
                        {
                            getbulkOverFlowFlag = true;
                            break;
                        }
                        successor=repeatCntr;
                        // Decode variable length structure
                        temp = TCPIP_SNMP_StructureIsValid(&tempLen.Val);
                        if ( temp != 0U )
                        {
                            break;
                        }

                        // Decode next object
                        if ( !TCPIP_SNMP_OIDIsValid(OIDValue, &OIDLen) )
                        {
                            TCPIP_SNMP_ErrorStatusSet(errorStatusOffset,errorIndexOffset,SNMP_GEN_ERR,varIndex,snmpPutData);
                            bSnmpGenError = true;
                            break;
                        }
                        templen=OIDLen;
                        ptroid=OIDValue;

                        // For Get & Get-Next, value must be NULL.
                        if ( pduDbPtr->pduType != (uint8_t)SET_REQUEST )
                        {
                            if (!TCPIP_SNMP_DataIsASNNull())
                            {
                                break;
                            }
                        }

                        oidLookUpRet = TCPIP_SNMP_OIDFindInMgmtInfoBase(snmpFileDescrptr,pduDbPtr,OIDValue, OIDLen, &OIDInfo);
                        if(oidLookUpRet == (uint8_t)SNMP_END_OF_MIB_VIEW)
                        {
                            temp = (uint8_t)TCPIP_SNMP_NextLeafGet(snmpFileDescrptr,&OIDInfo);
                        }
                        if(oidLookUpRet == 0U)
                        {
                            templen=OIDLen;
                            ptroid=OIDValue;
                            if(TCPIP_SNMP_DataCopyToProcessBuffer(ASN_OID,snmpPutData)!= true)
                            {
                                getbulkOverFlowFlag = true;
                                break;
                            }

                            if(SnmpStackDcptMemStubPtr->appendZeroToOID != 0U)
                            {
                                if(TCPIP_SNMP_DataCopyToProcessBuffer(OIDLen+1U,snmpPutData)!= true)//for appending "0"
                                {
                                    getbulkOverFlowFlag = true;
                                    break;
                                }
                                OIDLen += 1U;
                            }
                            else
                            {
                                if(TCPIP_SNMP_DataCopyToProcessBuffer(OIDLen,snmpPutData)!= true)
                                {
                                    getbulkOverFlowFlag = true;
                                    break;
                                }
                            }

                            //Put OID
                            while( templen--  != 0U)
                            {
                                if(TCPIP_SNMP_DataCopyToProcessBuffer(*ptroid++,snmpPutData)!= true)
                                {
                                    getbulkOverFlowFlag = true;
                                    break;
                                }
                            }

                            if(SnmpStackDcptMemStubPtr->appendZeroToOID != 0U)
                            {
                                if(TCPIP_SNMP_DataCopyToProcessBuffer(0x00U,snmpPutData)!= true)
                                {
                                    getbulkOverFlowFlag = true;
                                    break;
                                }
                            }

                            if(TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)SNMP_END_OF_MIB_VIEW,snmpPutData)!= true)
                            {
                                getbulkOverFlowFlag = true;
                                break;
                            }
                            if(TCPIP_SNMP_DataCopyToProcessBuffer(0x00U,snmpPutData)!= true)
                            {
                                getbulkOverFlowFlag = true;
                                break;
                            }

                            //Start counting total number of bytes in this structure.
                            varPairLen.Val = (uint16_t)OIDLen // as put by GetNextVar()
                             +2U       // OID header
                             +2U;      // endOfMibView bytes

                            snmpReqVarErrStatus.endOfMibViewErr  |= ((uint16_t)0x0001U << (uint16_t)varIndex);
                        }
                        else if(temp != 0U)//if(oidLookUpRet != SNMP_END_OF_MIB_VIEW)
                        {
                            temp = TCPIP_SNMP_ProcessGetBulkVar(&OIDInfo, &OIDValue[0],&OIDLen,&successor,pduDbPtr);
                        }
                        else
                        {
                            // do nothing
                        }

                        if ( temp == 0u )
                        {
                            templen=OIDLen;
                            ptroid=OIDValue;
                            if(TCPIP_SNMP_DataCopyToProcessBuffer(ASN_OID,snmpPutData)!=true)
                            {
                                getbulkOverFlowFlag = true;
                                break;
                            }
                            if(TCPIP_SNMP_DataCopyToProcessBuffer(OIDLen,snmpPutData)!= true)
                            {
                                getbulkOverFlowFlag = true;
                                break;
                            }

                            //Put OID
                            while( templen--  != 0U)
                            {
                                if(TCPIP_SNMP_DataCopyToProcessBuffer(*ptroid++,snmpPutData)!= true)
                                {
                                    getbulkOverFlowFlag = true;
                                    break;
                                }
                            }

                            /*Do send back the Same OID if get_next is EndOfMibView. Do not
                              append zero to this OID*/

                            if(TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)SNMP_END_OF_MIB_VIEW,snmpPutData)!=true)
                            {
                                getbulkOverFlowFlag = true;
                                break;
                            }
                            if(TCPIP_SNMP_DataCopyToProcessBuffer(0x00U,snmpPutData)!= true)
                            {
                                getbulkOverFlowFlag = true;
                                break;
                            }

                            snmpReqVarErrStatus.endOfMibViewErr  |= ((uint16_t)0x0001U << (uint16_t)varIndex);

                            //Start counting total number of bytes in this structure.
                            varPairLen.Val = (uint16_t)OIDLen  // as put by GetNextVar()
                                 + 2U     // OID header
                                 + 2U;    // endOfMibView byte.
                        }
                        else
                        {
                            varPairLen.Val = ((uint16_t)temp + 2U);        // + OID headerbytes
                        }

                        varBindLen.Val += 4U // Variable Pair STRUCTURE byte + 1 length byte.
                        + varPairLen.Val;

                        prevOffset = snmpPutData->length;
                        snmpPutData->length = varStructLenOffset;
                        if(TCPIP_SNMP_DataCopyToProcessBuffer(varPairLen.v[1],snmpPutData)!=true)
                        {
                            getbulkOverFlowFlag = true;
                            break;
                        }
                        if(TCPIP_SNMP_DataCopyToProcessBuffer(varPairLen.v[0],snmpPutData)!=true)
                        {
                            getbulkOverFlowFlag = true;
                            break;
                        }

                        snmpPutData->length = prevOffset;
                        varStructLenOffset = snmpPutData->length;
                        if((varStructLenOffset - tempTxOffset) > (uint16_t)TCPIP_SNMP_MAX_MSG_SIZE)
                        {
                            getbulkOverFlowFlag = true;
                            break;
                        }

                    }//for(varBindCntr=0;varBindCntr<Getbulk_R;varBindCntr++)

                }//for(repeatCntr=0;repeatCntr<Getbulk_M;repeatCntr++)
            
                break;

            default:
                // do nothing
                break;
        }//end of switch(smSnmp)

        if(doRet)
        {
            return false;
        }

        /*If all the variables are processed and the repsonse pdu is updated with
          the number of variable responses ought to be in the response; you are done
          with the request pdu processing. Else continue to processing.*/
        if((Getbulk_N==0u && noOfVarToBeInResponse==0u)||(bSnmpGenError))
        {
            smSnmp=(int)SM_PKT_STRUCT_LEN_OFFSET;
            break;
        }

    }//end of while(1)      

    // Update the place holders with respective values.
        
    
    /* As per RFC 3416 - GET bULK Response - 4.2.3
    If the size of the message encapsulating the Response-PDU containing the 
    requested number of variable bindings would be greater than either a local
    constraint or the maximum message size of the originator, then the response
    is generated with a lesser number of variable bindings. This lesser number is
    the ordered set of variable bindings with some of the variable bindings at the
    end of the set removed, such that the size of the message encapsulating the
    Response-PDU is approximately equal to but no greater than either a local
    constraint or the maximum message size of the originator. Note that the 
    number of variable bindings removed has no relationship to the values of N, M, or R.*/
    if(getbulkOverFlowFlag && (pduDbPtr->pduType==(uint8_t)GET_BULK_REQUEST))
    {
            snmpPutData->length = prevOffset;
            varBindLen.Val = varBindLen.Val ;
            (void)TCPIP_UDP_TxOffsetSet(gSnmpDcpt.skt, snmpPutData->length, true);
    }

    prevOffset = snmpPutData->length;
    /* GetRequest-PDU (As per RFC 3416 - SECTION - 4.2.1)
    During the process of any OID,variable binding fails due to invalid OID
    or invalid OID type or invalid OID length etc, i,e other than "noSuchObject"
    or " noSuchInstance", then the Response-PDU is re-formatted with the same
    values in its request-id and variable-bindings fields as the received
GetRequest-PDU , with the value of its error-status field set to "genErr", 

    GetNextRequest-PDU (As per RFC 3416 - SECTION - 4.2.2)
    During the process of any OID,variable binding fails due to invalid OID
    or invalid OID type or invalid OID length etc, other than "endOfMibView" ,
    then the Response-PDU is re-formatted with the same values in its request-id and
    variable-bindings fields as the received GetNextRequest-PDU,with the value of
    its error-status field set to "genErr", and the value of its error-index
    field is set to the index of the failed variable binding.

    The generated Response-PDU is then encapsulated into a message. If the size of the resultant
    message is less than or equal to maximum message size of the originator, it is transmitted
    to the originator of the GetNextRequest-PDU.

    Otherwise, an alternate Response-PDU is generated. This alternate Response-PDU is formatted
    with the same values in its request-id field as the received GetNextRequest-PDU, with the value
    of its error-status field set to "tooBig", the value of its error-index field set to zero, and an empty
    variable-bindings field.

    */
    //calculate the number of bytes are the part of RESPONSE PDU
    if(bSnmpGenError)
    {
        if(((prevOffset - tempTxOffset) > (uint16_t)TCPIP_SNMP_MAX_MSG_SIZE) && (pduDbPtr->pduType!=(uint8_t)GET_BULK_REQUEST))
        {
            /* for snmpv2 (or snmpv3) by rfc3416 we return special
            *   tooBig(1) response with empty variable-bindings field.
            * error status  = toobig(1) and error_index set to 0.
            */

            TCPIP_SNMP_ErrorStatusSet(errorStatusOffset,errorIndexOffset,SNMP_TOO_BIG,0,snmpPutData);
            varBindLen.Val =  ((uint16_t)6U                         // Request ID bytes (4+2)
                            + 3U                         // Error status     (2+1)
                            + 3U);                        // Error index      (2+1)
            snmpPutData->length = pduLenOffset;
            (void)TCPIP_SNMP_DataCopyToProcessBuffer(varBindLen.v[1],snmpPutData);
            (void)TCPIP_SNMP_DataCopyToProcessBuffer(varBindLen.v[0],snmpPutData);

            // varBindLen is reused as "packetLen".
            varBindLen.Val = (uint16_t)(3U                      // SNMP Version bytes
                            + 2U + (uint16_t)communityLen      // community string bytes
                            + 4U                     // PDU structure header bytes.
                            + varBindLen.Val);

            snmpPutData->length  = packetStructLenOffset;
            (void)TCPIP_SNMP_DataCopyToProcessBuffer(varBindLen.v[1],snmpPutData);
            (void)TCPIP_SNMP_DataCopyToProcessBuffer(varBindLen.v[0],snmpPutData);
            snmpPutData->length = varBindStructOffset-2U;
            smSnmp = (int)SM_PKT_STRUCT_LEN_OFFSET;
            return true;
        }
    }
    snmpPutData->length = varBindStructOffset;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(varBindLen.v[1],snmpPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(varBindLen.v[0],snmpPutData);
    snmpPutData->length = prevOffset;

    // varBindLen is reused as "pduLen"
    varBindLen.Val += ((uint16_t)4U       // Variable Binding Strucure length
        + 6U                         // Request ID bytes (4+2)
        + 3U                         // Error status     (2+1)
        + 3U);                        // Error index      (2+1)
    prevOffset = snmpPutData->length;

    snmpPutData->length = pduLenOffset;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(varBindLen.v[1],snmpPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(varBindLen.v[0],snmpPutData);
    snmpPutData->length = prevOffset;

#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
    if(pduDbPtr->snmpVersion == (uint8_t)SNMP_V3)
    {
        prevOffset = snmpPutData->length;
        snmpPutData->length = msgSecrtyParamLenOffset;
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(varBindLen.v[0],snmpPutData);
        snmpPutData->length = prevOffset;
    }
#endif

    // Update the place holders with respective values.
    if(pduDbPtr->snmpVersion != (uint8_t)SNMP_V3)
    {
        // varBindLen is reused as "packetLen".
        varBindLen.Val = (uint16_t)(3U                      // SNMP Version bytes
                + 2U + (uint16_t)communityLen      // community string bytes
                + 4U                     // PDU structure header bytes.
                + varBindLen.Val);
    }
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER        
    else
    {
        varBindLen.Val = (uint16_t)(3U       // SNMP Version bytes
                        + 2U      // PDU structure header bytes.
                        + varBindLen.Val);
    }
#endif

    prevOffset = snmpPutData->length;

    snmpPutData->length = packetStructLenOffset;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(varBindLen.v[1],snmpPutData);
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(varBindLen.v[0],snmpPutData);

    snmpPutData->length = prevOffset;

    smSnmp = (int)SM_PKT_STRUCT_LEN_OFFSET;

    return true;
    
}

/****************************************************************************
Function:
    uint8_t TCPIP_SNMP_ProcessGetNextVar(OID_INFO* rec,PDU_INFO* pduDbPtr)

Summary:
    Retrieves next node from the MIB database.

Description:
    This routine reads into the MIB stored with the agent .
    It will search for the first lexicographic successor of the variable
    binding's name in the incoming GetNextRequest-PDU. If found, the
    corresponding variable binding's name and value fields in the Response
    pdu are set to the name and value of the located variable. If the
    lexicographic succesor is not found, the vlaue filed is set to
    "endofMibView" and name field is retained as in request.

Precondition:
    TCPIP_SNMP_ProcessVariables() is called.

Parameters:
    rec - Pointer to SNMP MIB object information for which next node
              to be found

Return Values:
    temp.V[0]- Total number of bytes copied to response packet if succesful.
    false    - If End of MIB is reached or processing is failure.

Remarks:
    None.
 ***************************************************************************/
uint8_t TCPIP_SNMP_ProcessGetNextVar(OID_INFO* rec,PDU_INFO* pduDbPtr)
{
    TCPIP_UINT16_VAL temp;
    uint8_t putBytes=0;
    OID_INFO indexRec = {0};
    uint8_t *pOIDValue;
    uint8_t OIDValue[TCPIP_SNMP_OID_MAX_LEN];
    uint8_t OIDLen;
    SNMP_INDEX_INFO indexInfo;
    MIB_INFO varNodeInfo;
    SNMP_ID varID;
    uint16_t OIDValOffset=0;
    uint16_t prevOffset;
    static uint8_t varDataType;
    static uint8_t indexBytes;
    uint8_t idLen = 1;
    uint8_t dummyRead;

    SNMP_BUFFER_DATA *snmpPutData =  NULL;

#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
    SNMPV3MSGDATA   *dynPduBuf=NULL;

    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=NULL;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;
    dynPduBuf = &snmpv3EngnDcptMemoryStubPtr->ScopedPduRespnsBuf;
#endif

    snmpPutData = &SnmpStackDcptMemStubPtr->outPduBufData;
    temp.v[0] = 0;

    // Get next leaf only if this OID is a parent or a simple leaf node.
    if ( rec->nodeInfo.Flags.bIsParent != 0U || (rec->nodeInfo.Flags.bIsParent == 0U && rec->nodeInfo.Flags.bIsSequence == 0U) )
    {
        if ( !TCPIP_SNMP_NextLeafGet(snmpFileDescrptr,rec))
        {
            return 0U;
        }
    }

    // Get complete OID string from oid record.
    if ( !TCPIP_SNMP_OIDStringGetByAddr(snmpFileDescrptr,rec, OIDValue, &OIDLen))
    {
        return 0U;
    }
    
    //to validate the REC ID is present or not
    // do while loop till find find a valid entry.
    while(true)
    {
        if(TCPIP_SNMP_RecordIDValidation(pduDbPtr->snmpVersion,rec->nodeInfo.Flags.bIsIDPresent != 0U,(uint16_t)rec->id,OIDValue,OIDLen) != true)
        {
            if(!TCPIP_SNMP_NextLeafGet(snmpFileDescrptr,rec))
            {
                return 0U;
            }
            else
            {
                // Get complete OID string from oid record.
                if ( !TCPIP_SNMP_OIDStringGetByAddr(snmpFileDescrptr,rec, OIDValue, &OIDLen))
                {
                    return 0U;
                }
            }
        }
        else
        {
            break;
        }
    }
    varNodeInfo.Val = 0;
    while(true)
    {
        if(rec->nodeInfo.Flags.bIsSequence == 0U)
        {
            break;
        }
        // Need to fetch index information from MIB and prepare complete OID+
        // index response.
        varNodeInfo.Val = rec->nodeInfo.Val;
    
        // In this version, only 7-bit index is supported.
        (void)(*snmpFileShell->fileRead)(snmpFileShell,snmpFileDescrptr,&dummyRead,1U);
        indexBytes = 0;
        (void)(*snmpFileShell->fileRead)(snmpFileShell,snmpFileDescrptr,&indexInfo.Val,1U);
        (void)(*snmpFileShell->fileRead)(snmpFileShell,snmpFileDescrptr,&idLen,1);
        if(idLen == 1U)
        {
            (void)(*snmpFileShell->fileRead)(snmpFileShell,snmpFileDescrptr, &temp.v[0], 1U);
            indexRec.id = ((uint32_t)temp.v[0] & 0xFFU);
        }
        else if(idLen == 2U)
        {
            (void)(*snmpFileShell->fileRead)(snmpFileShell,snmpFileDescrptr, temp.v ,2U);
            indexRec.id = ((uint32_t)temp.v[0] & 0xFFU);
            indexRec.id <<= 8;
            indexRec.id |= ((uint32_t)temp.v[1] & 0xFFU);
        }
        else
        {
            // do nothing
        }
        indexRec.dataType = DATA_TYPE_UNKNOWN;
        (void)(*snmpFileShell->fileRead)(snmpFileShell,snmpFileDescrptr,(uint8_t*)&indexRec.dataType,1U);
        indexRec.index = rec->index;
    
        // Check with application to see if there exists next index
        // for this index id.
        if (!TCPIP_SNMP_NextIndexGet(indexRec.id, &indexRec.index))
        {
            if ( !TCPIP_SNMP_NextLeafGet(snmpFileDescrptr,rec))
            {
                return 0U;
            }
            
            if (!TCPIP_SNMP_OIDStringGetByAddr(snmpFileDescrptr,rec, OIDValue, &OIDLen))
            {   
                return 0U;
            }
            if(TCPIP_SNMP_RecordIDValidation(pduDbPtr->snmpVersion,rec->nodeInfo.Flags.bIsIDPresent != 0U,(uint16_t)rec->id,OIDValue,OIDLen) != true)
            {
                continue;
            }
        }
        else
        {
            break;
        }
    }

    // Copy complete OID string to create response packet.
    pOIDValue = OIDValue;   
    temp.v[0] = OIDLen;
    if(pduDbPtr->snmpVersion != (uint8_t)SNMP_V3)
    {
        OIDValOffset = snmpPutData->length;
        //temp.v[0] = OIDLen;
        snmpPutData->length = OIDValOffset+1U;
        while( temp.v[0]-- != 0U)
        {
            (void)TCPIP_SNMP_DataCopyToProcessBuffer(*pOIDValue++,snmpPutData);
        }
    }
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
    else
    {
        OIDValOffset = dynPduBuf->length;
        //temp.v[0] = OIDLen;
        //dynPduBuf.length = OIDValOffset+1;    // offset for the OID length
        if(TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynPduBuf)!= true)
        {
            return 0U;
        }
        while( temp.v[0]-- != 0U)
        {
            if(TCPIP_SNMPv3_DataCopyToProcessBuff(*pOIDValue++,dynPduBuf) != true)
            {
                return 0U;
            }
        }
    }
#endif

    //Put OID

    // Start counting number of bytes put - OIDLen is already counted.
    temp.v[0] = OIDLen;

    varDataType = (uint8_t)rec->dataType;
    varID = rec->id;

    // If this is a simple OID, handle it as a GetVar command.
    if(rec->nodeInfo.Flags.bIsSequence == 0U)
    {
        if(pduDbPtr->snmpVersion != (uint8_t)SNMP_V3)
        {
            // This is an addition to previously copied OID string.
            // This is index value of '0'.
            (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpPutData);
            temp.v[0]++;

            // Since we added one more byte to previously copied OID
            // string, we need to update OIDLen value.

            prevOffset = snmpPutData->length;
            snmpPutData->length = OIDValOffset;
            (void)TCPIP_SNMP_DataCopyToProcessBuffer(++OIDLen,snmpPutData);
            snmpPutData->length = prevOffset;

            // Now do Get on this simple variable.
            prevOffset = snmpPutData->length;
            putBytes = TCPIP_SNMP_ProcessGetVar(rec, false,pduDbPtr);
            if ( putBytes == 0u )
            {
                snmpPutData->length = prevOffset;
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(ASN_NULL,snmpPutData);
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpPutData);
                putBytes = 2;
            }
        }
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
        else
        {
            // This is index value of '0'.
            if(TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynPduBuf) != true)
            {
                return 0U;
            }
            temp.v[0]++;
            prevOffset = dynPduBuf->length;
            dynPduBuf->length = OIDValOffset;
            // Since we added one more byte to previously copied OID
            // string, we need to update OIDLen value.
            if(TCPIP_SNMPv3_DataCopyToProcessBuff(++OIDLen,dynPduBuf)!= true)
            {
                return 0U;
            }
            dynPduBuf->length = prevOffset;

            // Now do Get on this simple variable.
            prevOffset = dynPduBuf->length;
            putBytes = TCPIP_SNMP_ProcessGetVar(rec, false,pduDbPtr);
            if(dynPduBuf->length >= dynPduBuf->maxlength)
            {
                return 0U;
            }
            if(( putBytes == 0u ) && (pduDbPtr->snmpVersion == (uint8_t)SNMP_V3))
            {
                dynPduBuf->length = prevOffset;
                if(TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_NULL,dynPduBuf)!= true)
                {
                    return 0U;
                }
                if(TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynPduBuf)!= true)
                {
                    return 0U;
                }
                putBytes = 2;
            }
        }
#endif
        temp.v[0] += putBytes; // TCPIP_SNMP_ProcessGetVar(rec, false,pduDbPtr);

         // Return with total number of bytes copied to response packet.
        return temp.v[0];
    }

    // Index is assumed to be dynamic, and leaf node.
    // mib2bib has already ensured that this was the case.
    indexRec.nodeInfo.Flags.bIsConstant = 0;
    indexRec.nodeInfo.Flags.bIsParent = 0;
    indexRec.nodeInfo.Flags.bIsSequence = 1U;

    // Now handle this as simple GetVar.
    // Keep track of number of bytes added to OID.
    indexBytes += TCPIP_SNMP_ProcessGetVar(&indexRec, true,pduDbPtr);

    rec->index = indexRec.index;

    // These are the total number of bytes put so far as a result of this function.
    temp.v[0] += indexBytes;

    // These are the total number of bytes in OID string including index bytes.
    OIDLen += indexBytes;

    if(pduDbPtr->snmpVersion != (uint8_t)SNMP_V3)
    {
        // Since we added index bytes to previously copied OID
        // string, we need to update OIDLen value.
        prevOffset = snmpPutData->length ;
            snmpPutData->length = OIDValOffset;

        (void)TCPIP_SNMP_DataCopyToProcessBuffer(OIDLen,snmpPutData);
        snmpPutData->length = prevOffset;

    }
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
    else
    {
        // Since we added index bytes to previously copied OID
        // string, we need to update OIDLen value.
        prevOffset = dynPduBuf->length;
        dynPduBuf->length = OIDValOffset;
        (void)TCPIP_SNMPv3_DataCopyToProcessBuff(OIDLen,dynPduBuf);
        dynPduBuf->length = prevOffset;
    }
#endif

    // Fetch actual value itself.
    // Need to restore original OID value.
    rec->nodeInfo.Val = varNodeInfo.Val;
    rec->id = varID;
    rec->dataType = (SNMP_DATA_TYPE)varDataType;

    temp.v[0] += TCPIP_SNMP_ProcessGetVar(rec, false,pduDbPtr);
    return temp.v[0];
}


/****************************************************************************
Function:
    uint8_t TCPIP_SNMP_ProcessGetBulkVar(OID_INFO* rec, uint8_t* oidValuePtr,
                                               uint8_t* oidLenPtr,uint8_t* successor)

Summary:
    This routine process the SNMPv2c Get Bulk Request.

Description:
    TCPIP_SNMP_ProcessVariables() processes the received snmp request pdu for each of
    the variable binding in the variable binding list to produce a response
    pdu. Depending on the number of the Max_repetitions for every variable
    in the list for which Bulk information is expected, TCPIP_SNMP_ProcessGetBulkVar()
    is executed. It searches for the next lexicographically ordered
    successor for of the OID received in the request. For each of the
    iterations upto max-repetitions, the next leaf node is searched in the
    MIB to that of the leaf node found in the last iteration, for the
    corresponding variable binding.

Precondition:
    TCPIP_SNMP_ProcessVariables() is called.

Parameters:
    rec         - Pointer to SNMP MIB variable object information OID
    oidValuePtr - Pointer to new node OID found in MIB
    oidLenPtr   - Oid length
    successor   - 'I'th lexicographic successor to be found value

Return Values:
    false     - If no lexicographic successor found
    temp.v[0] - Total number of bytes copied to response packet

Remarks:
    None.
***************************************************************************/
uint8_t TCPIP_SNMP_ProcessGetBulkVar(OID_INFO* rec, uint8_t* oidValuePtr, uint8_t* oidLenPtr,uint8_t* successor,PDU_INFO* pduDbPtr)
{    
    uint8_t putBytes,cntr;
    uint8_t OIDLen;
    static uint8_t varDataType;
    static uint8_t indexBytes;
    uint8_t sequenceCnt=0;
    uint8_t sequenceRepeatCnt=0;
    SNMP_ID varID;
    OID_INFO indexRec  = {0};
    SNMP_INDEX_INFO indexInfo;
    MIB_INFO varNodeInfo;
    uint16_t OIDValOffset;
    uint16_t prevOffset;
    TCPIP_UINT16_VAL temp;
    uint8_t idLen=1;
    uint8_t dummyRead;
    
    
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
    SNMPV3MSGDATA   *dynPduBuf=NULL;
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=NULL;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;
    dynPduBuf = &snmpv3EngnDcptMemoryStubPtr->ScopedPduRespnsBuf;
#endif

    SNMP_BUFFER_DATA *snmpPutData =  NULL;
    snmpPutData = &SnmpStackDcptMemStubPtr->outPduBufData;

    /* intialize the local variables to 0 */
    OIDLen=0;
    sequenceCnt=0;

    temp.Val=0;


    temp.v[0] = 0;
    sequenceRepeatCnt=*successor;

    //Reach to the node for the expected iteration
    for(cntr=0;cntr<=*successor;cntr++)
    {
    // Get next leaf only if this OID is a parent or a simple leaf node.
        if((rec->nodeInfo.Flags.bIsParent != 0U)|| (rec->nodeInfo.Flags.bIsParent == 0U && rec->nodeInfo.Flags.bIsSequence == 0U))
        {   /* to maintain the number of interations */
            sequenceCnt++;
            if(!TCPIP_SNMP_NextLeafGet(snmpFileDescrptr,rec))
            {
                return 0U;
            }
        }
    }

    /* If request OID is a sequence variable, the below for loop retrives the
    expected instance for the OID. SequenceRepeatCnt starts with "0th instance" and
    increments to Max repeatations. Find the exact indexed OID in the request at first.
    If indexed OID is not available, then go for the next index.
    If the next index is not available , then go to the next leaf.
    */
    for(;sequenceCnt<=sequenceRepeatCnt;sequenceCnt++)
    {
        if(rec->nodeInfo.Flags.bIsSequence != 0U)
        {
            (void)TCPIP_SNMP_ExactIndexGet(rec->id,&rec->index);
            if(!TCPIP_SNMP_NextIndexGet(rec->id,&rec->index))
            {
                if(!TCPIP_SNMP_NextLeafGet(snmpFileDescrptr,rec))
                {
                    return 0U;
                }
            }
        }
        else
        {
            if(!TCPIP_SNMP_NextLeafGet(snmpFileDescrptr,rec))
            {
                return 0U;
            }
        }
    }

    // Get complete OID string from oid record.
    if(!TCPIP_SNMP_OIDStringGetByAddr(snmpFileDescrptr,rec, oidValuePtr, &OIDLen))
    {
        return 0U;
    }

    //to validate the REC ID is present or not
    while(true)
    {
        if(TCPIP_SNMP_RecordIDValidation(pduDbPtr->snmpVersion,rec->nodeInfo.Flags.bIsIDPresent != 0U,(uint16_t)rec->id,oidValuePtr,OIDLen) != true)
        {
            if(!TCPIP_SNMP_NextLeafGet(snmpFileDescrptr,rec))
            {
                return 0U;
            }
            else
            {
                    // Get complete OID string from oid record.
                if(!TCPIP_SNMP_OIDStringGetByAddr(snmpFileDescrptr,rec, oidValuePtr, &OIDLen))
                {
                    return 0U;
                }
            }
        }
        else
        {
            break;
        }
    }
    // get exact index value when it is a sequence variable
    varNodeInfo.Val = 0U;
    while(true)
    {
        if(rec->nodeInfo.Flags.bIsSequence == 0U)
        {
            break;
        }
        // Need to fetch index information from MIB and prepare complete OID+
        // index response.
        varNodeInfo.Val = rec->nodeInfo.Val;

        // In this version, only 7-bit index is supported.
        (void)(*snmpFileShell->fileRead)(snmpFileShell,snmpFileDescrptr,&dummyRead,1U);
        indexBytes = 0;
        (void)(*snmpFileShell->fileRead)(snmpFileShell,snmpFileDescrptr,&indexInfo.Val,1U);
        (void)(*snmpFileShell->fileRead)(snmpFileShell,snmpFileDescrptr,&idLen,1U);
        if(idLen == 1U)
        {
            (void)(*snmpFileShell->fileRead)(snmpFileShell,snmpFileDescrptr, &temp.v[0], 1U);
            indexRec.id = ((uint32_t)temp.v[0] & 0xFFUL);
        }
        else if(idLen == 2U)
        {
            (void)(*snmpFileShell->fileRead)(snmpFileShell,snmpFileDescrptr, temp.v, 2U);
            indexRec.id = ((uint32_t)temp.v[0] & 0xFFUL);
            indexRec.id <<= 8;
            indexRec.id |= ((uint32_t)temp.v[1] & 0xFFUL);
        }
        else
        {
            // do nothing
        }
        indexRec.dataType = DATA_TYPE_UNKNOWN;
        (void)(*snmpFileShell->fileRead)(snmpFileShell,snmpFileDescrptr,(uint8_t*)&indexRec.dataType,1U);
        indexRec.index = rec->index;
         // Check with application to see if there exists next index
        // for this index id.
        if (!TCPIP_SNMP_ExactIndexGet(indexRec.id, &indexRec.index))
        {
            if ( !TCPIP_SNMP_NextLeafGet(snmpFileDescrptr,rec))
            {
                return 0U;
            }
            
            if (!TCPIP_SNMP_OIDStringGetByAddr(snmpFileDescrptr,rec, oidValuePtr,&OIDLen))
            {   
                return 0U;
            }
            if(TCPIP_SNMP_RecordIDValidation(pduDbPtr->snmpVersion,rec->nodeInfo.Flags.bIsIDPresent != 0U,(uint16_t)rec->id,oidValuePtr,OIDLen) != true)
            {
                continue;
            }
        }
        else
        {
            break;
        }
    }
    
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
    if(pduDbPtr->snmpVersion == (uint8_t)SNMP_V3)
    {
        if(TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_OID,dynPduBuf) != true)
        {
            return 0U;
        }

        OIDValOffset = dynPduBuf->length;
        temp.v[0] = OIDLen;
        if(TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynPduBuf) != true)
        {
                return 0U;
        }
        //Put OID
        while( temp.v[0]-- != 0U)
        {
            if(TCPIP_SNMPv3_DataCopyToProcessBuff(*oidValuePtr,dynPduBuf) != true)
            {
                return 0U;
            }
            oidValuePtr++;
        }
    }
    else
#endif
    {
        if(TCPIP_SNMP_DataCopyToProcessBuffer(ASN_OID,snmpPutData)!= true)
        {
            return 0U;
        }

        OIDValOffset = snmpPutData->length;
        temp.v[0] = OIDLen;
        snmpPutData->length = OIDValOffset+1U;
        //Put OID
        while( temp.v[0]-- != 0U)
        {
            if(TCPIP_SNMP_DataCopyToProcessBuffer(*oidValuePtr,snmpPutData)!= true)
            {
                return 0U;
            }
            oidValuePtr++;
        }
    }
    // Start counting number of bytes put - OIDLen is already counted.
    temp.v[0] =*oidLenPtr= OIDLen;

    varDataType = (uint8_t)rec->dataType;
    varID = rec->id;

    // If this is a simple OID, handle it as a GetVar command.
    if (rec->nodeInfo.Flags.bIsSequence == 0U)
    {
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
        if(pduDbPtr->snmpVersion == (uint8_t)SNMP_V3)
        {
            // This is an addition to previously copied OID string.
            // This is index value of '0'.
             if(TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynPduBuf) != true)
             {
                return 0U;
             }
            temp.v[0]++;

            // Since we added one more byte to previously copied OID
            // string, we need to update OIDLen value.
            prevOffset = dynPduBuf->length;
            dynPduBuf->length = OIDValOffset;
            if(TCPIP_SNMPv3_DataCopyToProcessBuff(++OIDLen,dynPduBuf) != true)
            {
                return 0U;
            }
            dynPduBuf->length = prevOffset;


            // Now do Get on this simple variable.
            putBytes = TCPIP_SNMP_ProcessGetVar(rec, false,pduDbPtr);
        }
    else
#endif
        {
             // This is an addition to previously copied OID string.
            // This is index value of '0'.
            if(TCPIP_SNMP_DataCopyToProcessBuffer(0,snmpPutData)!= true)
            {
                return 0U;
            }
            temp.v[0]++;

            // Since we added one more byte to previously copied OID
            // string, we need to update OIDLen value.

            prevOffset = snmpPutData->length;
            snmpPutData->length = OIDValOffset;

            if(TCPIP_SNMP_DataCopyToProcessBuffer(++OIDLen,snmpPutData)!= true)
            {
                return 0U;
            }
            snmpPutData->length = prevOffset;
            // Now do Get on this simple variable.
            putBytes = TCPIP_SNMP_ProcessGetVar(rec, false,pduDbPtr);
            if(putBytes == 0U)
            {
                return 0U;
            }
        }
        temp.v[0] += putBytes; // TCPIP_SNMP_ProcessGetVar(rec, false,pduDbPtr);

            // Return with total number of bytes copied to response packet.
        return temp.v[0];
    }
    
    // Index is assumed to be dynamic, and leaf node.
    // mib2bib has already ensured that this was the case.
    indexRec.nodeInfo.Flags.bIsConstant = 0;
    indexRec.nodeInfo.Flags.bIsParent = 0;
    indexRec.nodeInfo.Flags.bIsSequence = 1U;

    // Now handle this as simple GetVar.
    // Keep track of number of bytes added to OID.
    putBytes = TCPIP_SNMP_ProcessGetVar(&indexRec, true,pduDbPtr);
    if(putBytes == 0U)
    {
        return 0U;
    }
    indexBytes += putBytes;

    rec->index = indexRec.index;

    // These are the total number of bytes put so far as a result of this function.
    temp.v[0] += indexBytes;

    // These are the total number of bytes in OID string including index bytes.
    OIDLen += indexBytes;

    if(pduDbPtr->snmpVersion != (uint8_t)SNMP_V3)
    {
        // Since we added index bytes to previously copied OID
        // string, we need to update OIDLen value.
        prevOffset = snmpPutData->length;
        snmpPutData->length = OIDValOffset;
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(OIDLen,snmpPutData);
        snmpPutData->length = prevOffset;
    }
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
    else
    {
        // Since we added index bytes to previously copied OID
        // string, we need to update OIDLen value.
        prevOffset = dynPduBuf->length;
        dynPduBuf->length =OIDValOffset;
        (void)TCPIP_SNMPv3_DataCopyToProcessBuff(OIDLen,dynPduBuf);
        dynPduBuf->length = prevOffset;
    }
#endif
    // Fetch actual value itself.
    // Need to restore original OID value.
    rec->nodeInfo.Val = varNodeInfo.Val;
    rec->id = varID;
    rec->dataType = (SNMP_DATA_TYPE)varDataType;

    temp.v[0] += TCPIP_SNMP_ProcessGetVar(rec, false,pduDbPtr);
    
    return temp.v[0];
}


/****************************************************************************
Function:
    uint8_t TCPIP_SNMP_OIDFindInMgmtInfoBase(SYS_FS_HANDLE fileDescr,PDU_INFO* pduDbPtr,uint8_t* oid, uint8_t oidLen, OID_INFO* rec)

Summary:
    To search and validate whether the requested OID is in the MIB database.

Description:
    The MIB database is stored with the agent in binary mib format.
    This is the binary mib format:
    <oid, nodeInfo, [id], [SiblingOffset], [DistantSibling], [dataType],
    [dataLen], [data], [{IndexCount, <IndexType>, <Index>, ...>]}, ChildNode
    variable bind name is a dotted string of oid. Every oid is a node in the
    MIB tree and have varied information. This routine on reception of the
    snmp request, will search for every oid in the var name. This routine
    will return information whether the requested var name is part of the
    MIB tree data structre of this agent or not.

Precondition:
    Valid snmp request with valid OID format is received.

Parameters:
    fileDescr - A valid File decriptor which is alread opened.
    pduDbPtr    - Pointer to received snmp  pdu elements information
    oid     - Pointer to the string of OID to be searched
    oidLen      - Oid length
    rec     - Pointer to SNMP MIB variable object information

Return Values:
    true    -   If the complete OID string is found in the mib
    false   -   If complete OID do not match.
            Also different erros returned are
            SNMP_END_OF_MIB_VIEW
            SNMP_NO_SUCH_NAME
            SNMP_NO_SUCH_OBJ
            SNMP_NO_SUCH_INSTANCE
Remarks:
    This routine works for the snmp mib storage format. It uses the file system
    APIs to read,search and collect information from the mib database.
***************************************************************************/
uint8_t TCPIP_SNMP_OIDFindInMgmtInfoBase(SYS_FS_HANDLE fileDescr,PDU_INFO* pduDbPtr,uint8_t* oid, uint8_t oidLen, OID_INFO* rec)
{
    uint8_t idLen=1;
    uint8_t savedOID;
    uint8_t matchedCount;
    uint8_t snmpVer;
    uint8_t snmpReqType;
    uint8_t* reqOidPtr;
    bool comapreOidWithSibling=false;
    TCPIP_UINT16_VAL tempData;
    tempData.Val = 0;
    uint32_t hNode;
    bool  bFoundIt=false;

    SnmpStackDcptMemStubPtr->appendZeroToOID=1U;

    snmpVer=pduDbPtr->snmpVersion;
    snmpReqType=pduDbPtr->pduType;

    if(SNMPStatus.Flags.bIsFileOpen == 0U)
    {
        return 0U;
    }
    
    hNode = 0;
    matchedCount = oidLen;

    reqOidPtr=oid;

    while( true )
    {
        (void)(*snmpFileShell->fileSeek)(snmpFileShell,fileDescr, hNode,SYS_FS_SEEK_SET);
        rec->hNode = (uint32_t)(*snmpFileShell->fileTell)(snmpFileShell,fileDescr);
        (void)(*snmpFileShell->fileRead)(snmpFileShell,fileDescr,&savedOID,1U);
        (void)(*snmpFileShell->fileRead)(snmpFileShell,fileDescr,&rec->nodeInfo.Val,1U);
        if(rec->nodeInfo.Flags.bIsIDPresent != 0U)
        {
            (void)(*snmpFileShell->fileRead)(snmpFileShell,fileDescr,&idLen,1U);
            if(idLen == 1U)
            {
                uint8_t temp;
                (void)(*snmpFileShell->fileRead)(snmpFileShell,fileDescr,&temp,1U);
                rec->id = ((uint32_t)temp & 0xFFUL);
            }
            else if(idLen == 2U)
            {
                uint8_t temp[2];
                (void)(*snmpFileShell->fileRead)(snmpFileShell,fileDescr,temp,2U);
                rec->id = ((uint32_t)temp[0] & 0xFFUL);
                rec->id <<= 8;
                rec->id |= ((uint32_t)temp[1] & 0xFFUL);
            }
            else
            {
                // do nothing
            }
        }
        if((rec->nodeInfo.Flags.bIsSibling != 0U) || (rec->nodeInfo.Flags.bIsDistantSibling != 0U))
        {
            (void)(*snmpFileShell->fileRead)(snmpFileShell,fileDescr,&tempData.v[0],1U);
            (void)(*snmpFileShell->fileRead)(snmpFileShell,fileDescr,&tempData.v[1],1U);
            rec->hSibling = tempData.Val;
        }

        if ( savedOID != *reqOidPtr )
        {
            /*if very first OID byte does not match, it may be because it is
            0, 1 or 2.  In that case declare that there is a match.
            The command processor would detect OID type and continue or reject
            this OID as a valid argument.*/
            if(matchedCount == oidLen)
            {
                bFoundIt =  true;
                break;
            }
            
            if(comapreOidWithSibling== true && rec->nodeInfo.Flags.bIsSibling == 0U)
            {          
                bFoundIt =  false;
                break;
            }

            if ( rec->nodeInfo.Flags.bIsSibling  != 0U)
            {
                (void)(*snmpFileShell->fileSeek)(snmpFileShell,fileDescr, tempData.Val,SYS_FS_SEEK_SET);
                hNode = (uint32_t)(*snmpFileShell->fileTell)(snmpFileShell,fileDescr);
                comapreOidWithSibling=true;
            }
            else
            {
                bFoundIt =  false;
                break;
            }
        }
        else
        {
            // One more oid byte matched.
            matchedCount--;
            reqOidPtr++;

            // A node is said to be matched if last matched node is a leaf node
            // or all but last OID string is matched and last byte of OID is '0'.
            // i.e. single index.
            if ( rec->nodeInfo.Flags.bIsParent == 0U)
            {
                rec->dataType = DATA_TYPE_UNKNOWN;
                (void)(*snmpFileShell->fileRead)(snmpFileShell,fileDescr,(uint8_t*)&rec->dataType,1U);
                rec->hData = (uint32_t)(*snmpFileShell->fileTell)(snmpFileShell,fileDescr);
                if(snmpReqType==(uint8_t)SNMP_GET && matchedCount == 0u)
                {
                    SnmpStackDcptMemStubPtr->appendZeroToOID=0U;
                    bFoundIt =  false;
                    break;
                }
                else if(snmpReqType==(uint8_t)SNMP_GET && matchedCount == 1u && *reqOidPtr == 0x00u)
                {
                    SnmpStackDcptMemStubPtr->appendZeroToOID=0U;
                }
                else if(snmpReqType==(uint8_t)SNMP_GET_NEXT && matchedCount == 0u)
                {
                    SnmpStackDcptMemStubPtr->appendZeroToOID=1U;
                    SnmpStackDcptMemStubPtr->getZeroInstance=true;
                }
                else if(snmpReqType==(uint8_t)SNMP_V2C_GET_BULK && matchedCount == 1u )
                {
                    SnmpStackDcptMemStubPtr->appendZeroToOID=0U;
                }
                else
                {
                    // do nothing
                }
                bFoundIt =  true;
                break;
            }
            else if(matchedCount == 1u && *reqOidPtr == 0x00u)
            {
                SnmpStackDcptMemStubPtr->appendZeroToOID=0U;
                if(rec->nodeInfo.Flags.bIsParent != 0U)
                {
                    bFoundIt =  false;
                    break;
                }
            }
            else if(matchedCount == 0u)
            {
                if(rec->nodeInfo.Flags.bIsParent != 0U && snmpReqType==(uint8_t)SNMP_GET)
                {
                    SnmpStackDcptMemStubPtr->appendZeroToOID=0U;
                    bFoundIt =  false;
                    break;
                }
                else
                {
                     bFoundIt =  true;
                     break;
                }
            }
            else
            {
                hNode = (uint32_t)(*snmpFileShell->fileTell)(snmpFileShell,fileDescr);
                // Try to match following child node.
                continue;
            }
        }
    }

    if(bFoundIt == true)
    {
        // Convert index info from OID to regular value format.
        rec->index = savedOID;

        /*To Reach To The Next leaf Node */
        savedOID = *reqOidPtr;
        
        rec->indexLen = 1;

        if(matchedCount ==1u)
        {
            rec->index = *reqOidPtr;
        }
        else if(matchedCount == 0u)
        {
            rec->index = 0;
        }
        else if ( matchedCount > 1u || (savedOID & 0x80U) != 0U /*In this version, we only support 7-bit index*/)
        {   
            // Current instnace spans across more than 7-bit.
            rec->indexLen = 0xffU;

            if(snmpReqType==(uint8_t)SNMP_GET && snmpVer==(uint8_t)SNMP_V1)
            {
                return (uint8_t)SNMP_NO_SUCH_NAME;
            }
            else if(snmpReqType==(uint8_t)SNMP_GET && ((snmpVer==(uint8_t)SNMP_V2C)||(snmpVer==(uint8_t)SNMP_V3)))
            {
                if(matchedCount== oidLen) //No OBJECT IDNETIFIER Prefix match
                {
                    return (uint8_t)SNMP_NO_SUCH_INSTANCE;
                }
                else
                {
                    return (uint8_t)SNMP_NO_SUCH_OBJ;
                }
            }
            else
            {
                // do nothing
            }

            return 0U;
        }
        else
        {
            // do nothing
        }
        
        if(SnmpStackDcptMemStubPtr->getZeroInstance)
        {
            rec->index = SNMP_INDEX_INVALID;
        }

        return 1U;
    }
    else
    {
        if(snmpReqType==(uint8_t)SNMP_GET)
        {
            if(snmpVer==(uint8_t)SNMP_V1)
            {
                return (uint8_t)SNMP_NO_SUCH_NAME;
            }
            else /*if(snmpVer==(uint8_t)SNMP_V2C)*/
            {
                if(matchedCount== oidLen) //No OBJECT IDNETIFIER Prefix match
                {
                    return (uint8_t)SNMP_NO_SUCH_INSTANCE;
                }
                else
                {
                    return (uint8_t)SNMP_NO_SUCH_OBJ;
                }
            }
        }
        else if((snmpReqType==(uint8_t)SNMP_GET_NEXT||snmpReqType==(uint8_t)SNMP_V2C_GET_BULK) && ((snmpVer==(uint8_t)SNMP_V2C) || (snmpVer==(uint8_t)SNMP_V3)))
        {
            if(rec->nodeInfo.Flags.bIsDistantSibling == 0U)
            {
                return (uint8_t)SNMP_END_OF_MIB_VIEW;
            }
            else
            {
                return 1U;
            }
        }       
        else
        {
            // do nothing
        }
    }    
    return 0U;
}   

/****************************************************************************
Function:
    bool TCPIP_SNMP_NextLeafGet(SYS_FS_HANDLE fileDescr,OID_INFO* rec)

Summary:
    Searches for the next leaf node in the MIP tree.

Description:
    This routine searches for the next leaf node from the current node.
    The input to this function is the node from where next leaf node
    is to be located. The next leaf node will be a silbing else distant
    sibling or leaf node of next branch, if any present. The input parameter
    var pointer will be updated with the newly found leaf node OID info.

Precondition:
    TCPIP_SNMP_ProcessGetBulkVar() else TCPIP_SNMP_ProcessGetNextVar() is called.

Parameters:
    fileDescr - A valid File decriptor which is alread opened.
    rec     - Pointer to SNMP MIB variable object information

Return Values:
    true    - If next leaf node is found.
    false   - There is no next leaf node.

Remarks:
    None.
***************************************************************************/
bool TCPIP_SNMP_NextLeafGet(SYS_FS_HANDLE fileDescr, OID_INFO* rec)
{
    TCPIP_UINT16_VAL temp;
    uint8_t idLen=1;

    // If current node is leaf, its next sibling (near or distant) is the next leaf.
    if ( rec->nodeInfo.Flags.bIsParent == 0U)
    {
        // Since this is a leaf node, it must have at least one distant or near
        // sibling to get next sibling.
        if(rec->nodeInfo.Flags.bIsSibling != 0U || rec->nodeInfo.Flags.bIsDistantSibling != 0U )
        {
            // Reposition at sibling.
            (void)(*snmpFileShell->fileSeek)(snmpFileShell,fileDescr,rec->hSibling,SYS_FS_SEEK_SET);
            // Fetch node related information
        }
        // There is no sibling to this leaf.  This must be the very last node on the tree.
        else
        {
            return false;
        }
    }

    while( true )
    {
        // Remember current offset for this node.
        rec->hNode = (uint32_t)(*snmpFileShell->fileTell)(snmpFileShell,fileDescr);

        // Read OID byte.
        
        (void)(*snmpFileShell->fileRead)(snmpFileShell,fileDescr,&rec->oid,1U);
        (void)(*snmpFileShell->fileRead)(snmpFileShell,fileDescr,&rec->nodeInfo.Val,1U);
        if ( rec->nodeInfo.Flags.bIsIDPresent != 0U )
        {
            (void)(*snmpFileShell->fileRead)(snmpFileShell,fileDescr,&idLen,1U);
            if(idLen == 1U)
            {
                (void)(*snmpFileShell->fileRead)(snmpFileShell,fileDescr, &temp.v[0], 1U);
                rec->id = ((uint32_t)temp.v[0] & 0xFFUL);
            }
            else if(idLen == 2U)
            {
                (void)(*snmpFileShell->fileRead)(snmpFileShell,fileDescr, temp.v, 2U);
                rec->id = ((uint32_t)temp.v[0] & 0xFFUL);
                rec->id <<= 8;
                rec->id |= ((uint32_t)temp.v[1] & 0xFFUL);
            }
            else
            {
                // do nothing
            }
        }
        if ( rec->nodeInfo.Flags.bIsSibling != 0U || rec->nodeInfo.Flags.bIsDistantSibling != 0U )
        {
            (void)(*snmpFileShell->fileRead)(snmpFileShell,fileDescr,&temp.v[0],1U);
            (void)(*snmpFileShell->fileRead)(snmpFileShell,fileDescr,&temp.v[1],1U);
            rec->hSibling = temp.Val;
        }
        if ( rec->nodeInfo.Flags.bIsParent != 0U )
        {
            continue;
        }
        rec->dataType = DATA_TYPE_UNKNOWN;
        (void)(*snmpFileShell->fileRead)(snmpFileShell,fileDescr,(uint8_t*)&rec->dataType,1U);
        rec->hData = (uint32_t)(*snmpFileShell->fileTell)(snmpFileShell,fileDescr);

        // Since we just found next leaf in line, it will always have zero index
        // to it.
        rec->indexLen = 1;
        rec->index = 0;

        if (rec->nodeInfo.Flags.bIsSequence != 0U)
        {
            rec->index = SNMP_INDEX_INVALID;
        }

        return true;
    }
}


/****************************************************************************
Function:
    bool TCPIP_SNMP_CommunityStringIsValid(char* community, uint8_t* len)

Summary:
    Verifies for the community string datatype and the max
    community name and length, this agent can process.

Description:
    This routine populates and validates the community datatype, community
    name and length from the received snmp request pdu. Community name is
    used for accessing public and private memebrs of the mib.

Precondition:
    TCPIP_SNMP_ProcessPDUHeader() is called.

Parameters:
    community - Pointer to memory where community string will be stored.
    len       - Pointer to memory where comunity length gets stored.

Return Values:
    true    - If valid community received.
    false   - If community is not valid.

Remarks:
    None.
***************************************************************************/
static bool TCPIP_SNMP_CommunityStringIsValid(char* community, uint8_t* len)
{
    uint8_t tempData;
    uint8_t tempLen;

    tempData = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
    if ( !IS_OCTET_STRING(tempData) )
    {
        return false;
    }

    tempLen = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
    *len    = tempLen;
    if ( tempLen > (uint8_t)TCPIP_SNMP_COMMUNITY_MAX_LEN )
    {
        return false;
    }
    (void)TCPIP_SNMP_GetArrayOfDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData,(int)tempLen,(uint8_t*)community);
    community[tempLen] = '\0';
    return true;
}

/****************************************************************************
Function:
    bool TCPIP_SNMP_VarDataTypeIsValidInteger(uint32_t* val)

Summary:
    Verifies variable datatype as int and retrieves its value.

Description:
    This routine populates and validates the received variable for the
    data type as "ASN_INT" and the data length for max 4 bytes.

Precondition:
    TCPIP_SNMP_ProcessPDUHeader() or SNMPProcessGetSetHeader() is called.

Parameters:
    val - Pointer to memory where int var value will be stored.

ReturnValues:
    true    - If valid integer type and value is received.
    false   - Other than integer data type and value received .

Remarks:
    None.
***************************************************************************/
bool TCPIP_SNMP_VarDataTypeIsValidInteger(uint32_t* val)
{
    TCPIP_UINT32_VAL tempData;
    TCPIP_UINT32_VAL tempLen;

    tempLen.Val = 0;

    // Get variable type

    if ( !IS_ASN_INT(TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData)) )
    {
        return false;
    }


    if ( TCPIP_SNMP_LengthIsValid(&tempLen.w[0]) == 0U)
    {
        return false;
    }

    // Integer length of more than 32-bit is not supported.
    if ( tempLen.Val > 4u )
    {
        return false;
    }

    tempData.Val = 0;
    while( tempLen.v[0]--  != 0U)
    {
        tempData.v[tempLen.v[0]] = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
    }

    *val = tempData.Val;

    return true;
}


/****************************************************************************
Function:
    bool TCPIP_SNMP_PDUIsValid(SNMP_ACTION* pdu)

Summary:
    Verifies for the snmp request type.

Description:
    This routine populates and verifies for the received snmp request
    pdu type.

Precondition:
    TCPIP_SNMP_ProcessPDUHeader() is called.

Parameters:
    val - Pointer to memory where received snmp request type is stored.

Return Values:
    true    - If this snmp request can be processed by the agent.
    false   - If the request can not be processed.

Remarks:
    None.
***************************************************************************/
static bool TCPIP_SNMP_PDUIsValid(SNMP_ACTION* pdu)
{
    uint8_t tempData;
    uint16_t tempLen;

    // Fetch pdu data type
    tempData = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
    if ( !IS_AGENT_PDU(tempData) )
    {
        return false;
    }

    *pdu = (SNMP_ACTION)tempData;


    /* Now fetch pdu length.  We don't need to remember pdu length.
       Do this to proceed to next pdu element of interest*/ 
    return TCPIP_SNMP_LengthIsValid(&tempLen) != 0U;
}

/****************************************************************************
Function:
    uint8_t TCPIP_SNMP_LengthIsValid(uint16_t* len)

Summary:
    Retrieves the packet length and actual pdu length.

Description:
    Checks current packet and returns total length value as well as
    actual length bytes.We do not support any length byte count of more
    than 2 i.e. total length value must not be more than 16-bit.

Precondition:
    None

Parameters:
    len - Pointer to memory where actual length is stored.

Return Values:
    lengthBytes - Total length bytes are 0x80 itself plus tempData.

Remarks:
    None.
***************************************************************************/
uint8_t TCPIP_SNMP_LengthIsValid(uint16_t *len)
{
    uint8_t tempData;
    TCPIP_UINT16_VAL tempLen;
    uint8_t lengthBytes;

    // Initialize length value.
    tempLen.Val = 0;

    tempData = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
    tempLen.v[0] = tempData;
    if ( (tempData & 0x80U)  != 0U)
    {
        tempData &= 0x7FU;

        // We do not support any length byte count of more than 2
        // i.e. total length value must not be more than 16-bit.
        if ( tempData > 2u )
        {
            return 0U;
        }

        // Total length bytes are 0x80 itself plus tempData.
        lengthBytes = tempData + 1U;

        // Get upto 2 bytes of length value.
        while( tempData-- != 0U)
        {
            tempLen.v[tempData] = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
        }
    }
    else
    {
        lengthBytes = 1;
    }

    *len = tempLen.Val;

    return lengthBytes;
}


/****************************************************************************
Function:
    bool TCPIP_SNMP_DataIsASNNull(void)


Summary:
    Verifies the value type as ASN_NULL.

Description:
    For Get,Get_Next,Get_Bulk snmp reuest, the var bind the value data type
    should be ASN_NULL and value field must be NULL and . This routine
    verifies the data type and value fields in the received requests.
    The SET request, the value data type can not be ASN_NULL,
    otherwise the snmp request is not processed.

Precondition:
    None

Parameters:
    None

Returns Values
    true    - If value type is ASN_NULL and value is NULL.
    false   - If data type and value is other than ASN_NULL and NULL resp.

Remarks:
    None.
***************************************************************************/
bool TCPIP_SNMP_DataIsASNNull(void)
{
    uint8_t a;
        
    // Fetch and verify that this is NULL data type.
    a = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);

    if (!IS_ASN_NULL(a))
    {
        return false;
    }

    // Fetch and verify that length value is zero.
    return (TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData) == 0u);
}


/****************************************************************************
Function:
    bool TCPIP_SNMP_OIDIsValid(uint8_t* oid, uint8_t* len)

Summary:
    Populates OID type, length and oid string from the received pdu.

Description:
    In this routine, OID data type "ASN_OID" is verified in the received pdu.
    If the data type is matched, then only var bind is processed. OID length
    and OID is populated. The max OID length can be 15.

Precondition:
    ProcessVariabels() is called.

Parameters:
    oid - Pointer to memory to store the received OID string
    len - Pointer to memory to store OID length

Return Values:
    true    - If value type is ASN_OID and oid length not more than 15.
    false   - Otherwise.

Remarks:
    None.
***************************************************************************/
bool TCPIP_SNMP_OIDIsValid(uint8_t* oid, uint8_t* len)
{
    TCPIP_UINT32_VAL tempLen;
   
    
    // Fetch and verify that this is OID.
    if ( !IS_OID(TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData)) )
    {
        return false;
    }

    // Retrieve OID length
    if ( TCPIP_SNMP_LengthIsValid(&tempLen.w[0]) == 0U)
    {
        return false;
    }

    // Make sure that OID length is within our capability.
    if ( tempLen.w[0] > (uint8_t)TCPIP_SNMP_OID_MAX_LEN )
    {
        return false;
    }

    *len = tempLen.v[0];

    while( tempLen.v[0]-- != 0U)
    {
        *oid++ = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
    }
    *oid=0xffU;
    return true;
}

/****************************************************************************
Function:
    uint8_t TCPIP_SNMP_StructureIsValid(uint16_t* dataLen)

Summary:
    Decode variable length structure.

Description:
    This routine is used  to verify whether the received varbind is of type
    STRUCTURE and to find out the variable binding structure length.

Precondition:
    TCPIP_SNMP_ProcessPDUHeader() is called.

Parameters:
    datalen - Pointer to memory to store OID structure length.

Return Values:
    headrbytes  - Variable binding length.
    false       - If variable data structure is not type STRUCTURE.

Remarks:
    None.
***************************************************************************/
uint8_t TCPIP_SNMP_StructureIsValid(uint16_t* dataLen)
{
    TCPIP_UINT32_VAL tempLen;
    uint8_t headerBytes;

    if ( !IS_STRUCTURE(TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData)) )
    {
        return 0U;
    }


    // Retrieve structure length
    headerBytes = TCPIP_SNMP_LengthIsValid(&tempLen.w[0]);
    if ( headerBytes == 0U || (headerBytes > 3U))
    {
        return 0U;
    }

    headerBytes++;

    // Since we are using UDP as our transport and UDP are not fragmented,
    // this structure length cannot be more than 1500 bytes.
    // As a result, we will only use lower uint16_t of length value.
    *dataLen = tempLen.w[0];

    return headerBytes;
}

/****************************************************************************
Function:
    bool TCPIP_SNMP_PDUProcessDuplexInit(UDP_SOCKET skt)

Summary:
    Prepare for full duplex transfer.

Description:
    As we process SNMP variables, we will prepare response on-the-fly
creating full duplex transfer. SNMP stack manages its software simulated full 
duplex connection. Prepare for full duplex transfer. Set the Tx and Rx 
offset to start of the buffer.

Precondition:
    TCPIP_SNMP_Task() is called.

Parameters:
    skt - An active udp socket for which tx and rx offset to be set.

Returns:
    true if success,
    false otherwise.

Remarks:
    This routine should be called for every new snmp packet received.
***************************************************************************/
bool TCPIP_SNMP_PDUProcessDuplexInit(UDP_SOCKET skt)
{
    uint16_t bufferSize=0;
    // In full duplex transfer, transport protocol must be ready to
    // accept new transmit packet.
    bufferSize = TCPIP_UDP_TxPutIsReady(skt,TCPIP_SNMP_MAX_MSG_SIZE);
    if(bufferSize < (uint16_t)TCPIP_SNMP_MAX_MSG_SIZE)
    {
        void* txSize = FC_Uint2VPtr((uint32_t)TCPIP_SNMP_MAX_MSG_SIZE);         
        (void)TCPIP_UDP_OptionsSet(skt, UDP_OPTION_TX_BUFF, txSize);
    }
    
    // Initialize buffer offsets.
    SNMPRxOffset = 0;
    SNMPTxOffset = 0;
    return true;
}

/****************************************************************************
Function:
    bool TCPIP_SNMP_OIDStringFindByID(SYS_FS_HANDLE fileDescr,SNMP_ID id, OID_INFO* info,
                                              uint8_t* oidString, uint8_t* len)

Summary:
    Get complete notification variable OID string from snmp.bib using var id.

Description:
    This routine is called when a OID string is required to be searched
    from snmp.bib using agent id. The string is saved with agent.
    TRAP pdu is send with this OID corresponding to the SNMP_ID used
    by the agent application to send the pdu.

Precondition:
    TCPIP_SNMP_Notify() is called.

Parameters:
    fileDescr - A valid File decriptor which is alread opened.
    id          -   System ID to use identify this agent.
    info        -   Pointer to SNMP MIB variable object information
    oidString   -   Pointer to store the string of OID serached
    len         -   Oid length

Return Values:
    true    -   If oid string is found for the variable id in snmp.bib.
    FLASE   -   Otherwise.

Remarks:
    This function is used only when TRAP is enabled.
***************************************************************************/
bool TCPIP_SNMP_OIDStringFindByID(SYS_FS_HANDLE fileDescr,SNMP_ID id, OID_INFO* info, uint8_t* oidString, uint8_t* len)
{
    uint32_t hCurrent;

    hCurrent = 0U;

    while (true)
    {
        //Read in the Mib record for the oid info
        TCPIP_SNMP_MIBRecordRead(fileDescr,hCurrent, info);

        if ( info->nodeInfo.Flags.bIsParent == 0U)
        {
            if ( info->nodeInfo.Flags.bIsIDPresent != 0U)
            {
                if ( info->id == id )
                {
                    return TCPIP_SNMP_OIDStringGetByAddr(fileDescr,info, oidString, len);
                }
            }

            if ( info->nodeInfo.Flags.bIsSibling != 0U || info->nodeInfo.Flags.bIsDistantSibling != 0U )
            {
                (void)SYS_FS_FileSeek(fileDescr, (int32_t)info->hSibling, SYS_FS_SEEK_SET);
            }
            else
            {
                break;
            }
        }
        hCurrent = (uint32_t)SYS_FS_FileTell(fileDescr);
    }
    return false;
}



/****************************************************************************
Function:
    bool TCPIP_SNMP_OIDStringGetByAddr(SYS_FS_HANDLE fileDescr,OID_INFO* rec, uint8_t* oidString, uint8_t* len)

Summary:
    Get OID string from snmp.bib using the node address.

Description:
    This routine is called when a OID string is required to be searched
    from snmp.bib using node address.

Precondition:
    None.

Parameters:
    fileDescr - A valid File decriptor which is alread opened.
    rec         -   Pointer to SNMP MIB variable object information
    oidString   -   Pointer to store the string of OID searched
    len         -   Oid length

Return Values:
    true    -   If oid string is found.
    FLASE   -   Otherwise.

Remarks:
    None.
***************************************************************************/
static bool TCPIP_SNMP_OIDStringGetByAddr(SYS_FS_HANDLE fileDescr,OID_INFO* rec, uint8_t* oidString, uint8_t* len)
{
    uint32_t hTarget;
    uint32_t hCurrent;
    uint32_t hNext;
    OID_INFO currentMIB = {0};
    uint8_t index;
    enum { SM_PROBE_SIBLING, SM_PROBE_CHILD } state;

    hCurrent = 0;


    hTarget = rec->hNode;//node address
    state = SM_PROBE_SIBLING;
    index = 0;

    while( true )
    {
        TCPIP_SNMP_MIBRecordRead(fileDescr,hCurrent, &currentMIB);

        oidString[index] = currentMIB.oid;

        if ( hTarget == hCurrent )
        {
            *len = index + 1U;
            return true;
        }

        if(state == SM_PROBE_SIBLING)
        {
            if ( currentMIB.nodeInfo.Flags.bIsSibling == 0U )
            {
                state = SM_PROBE_CHILD;
            }
            else
            {
                hNext = currentMIB.hSibling;

                (void)SYS_FS_FileSeek(fileDescr, (int32_t)hNext, SYS_FS_SEEK_SET);
                hNext = (uint32_t)SYS_FS_FileTell(fileDescr);

                if ( hTarget >= hNext )
                {
                    hCurrent = hNext;
                }
                else
                {
                    state = SM_PROBE_CHILD;
                }
            }
        }
        else
        {   // state == SM_PROBE_CHILD
            if (currentMIB.nodeInfo.Flags.bIsParent == 0U)
            {
                return false;
            }

            index++;

            hCurrent = currentMIB.hChild;
            state = SM_PROBE_SIBLING;
        }
    }
}


/****************************************************************************
Function:
    void TCPIP_SNMP_MIBRecordRead(SYS_FS_HANDLE fileDescr,uint32_t h, OID_INFO* rec)

Summary:
    Get OID string from snmp.bib using the node address.

Description:
    This routine is called when a OID string is required to be searched
    from snmp.bib using node address.

Precondition:
    TCPIP_SNMP_OIDStringFindByID() or TCPIP_SNMP_OIDStringGetByAddr() is called.

Parameters:
    fileDescr - A valid File decriptor which is alread opened.
    h       -   Node adderess whose oid is to be read.
    rec     -   Pointer to store SNMP MIB variable object information

Returns:
    None.

Remarks:
    None.
***************************************************************************/
static void TCPIP_SNMP_MIBRecordRead(SYS_FS_HANDLE fileDescr,uint32_t h, OID_INFO* rec)
{
    MIB_INFO nodeInfo;
    TCPIP_UINT16_VAL tempVal;
    uint8_t idLen=1;

    (void)SYS_FS_FileSeek(fileDescr, (int32_t)h, SYS_FS_SEEK_SET);

    rec->hNode = h;
    (void)SYS_FS_FileRead(fileDescr,&rec->oid,1);
    (void)SYS_FS_FileRead(fileDescr,&rec->nodeInfo.Val,1);
    nodeInfo.Val = rec->nodeInfo.Val;
    if ( nodeInfo.Flags.bIsIDPresent != 0U)
    {
        (void)SYS_FS_FileRead(fileDescr,&idLen,1);
        if(idLen == 1U)
        {
            uint8_t temp=0;
            (void)SYS_FS_FileRead(fileDescr,&temp,1);
            rec->id = ((uint32_t)temp & 0xFFUL);
        }
        else if(idLen == 2U)
        {
            uint8_t temp[2];
            (void)SYS_FS_FileRead(fileDescr,temp,2);
            rec->id = ((uint32_t)temp[0] & 0xFFUL);
            rec->id <<= 8;
            rec->id |= ((uint32_t)temp[1] & 0xFFUL);
        }
        else
        {
            // do nothing
        }
    }
    if ( nodeInfo.Flags.bIsSibling != 0U)
    {
        (void)SYS_FS_FileRead(fileDescr,&tempVal.v[0],1);
        (void)SYS_FS_FileRead(fileDescr,&tempVal.v[1],1);
        rec->hSibling = tempVal.Val;
    }
    if ( nodeInfo.Flags.bIsParent != 0U)
    {
       rec->hChild = (uint32_t)SYS_FS_FileTell(fileDescr);
    }
    else
    {
        if ( nodeInfo.Flags.bIsDistantSibling != 0U)
        {
            (void)SYS_FS_FileRead(fileDescr,&tempVal.v[0],1);
            (void)SYS_FS_FileRead(fileDescr,&tempVal.v[1],1);
            rec->hSibling = tempVal.Val;
        }
        rec->dataType = DATA_TYPE_UNKNOWN;
        (void)SYS_FS_FileRead(fileDescr,(uint8_t*)&rec->dataType,1);
        rec->hData = (uint32_t)SYS_FS_FileTell(fileDescr);
    }
}

bool TCPIP_SNMP_DataTypeInfoGet(SNMP_DATA_TYPE dataType, SNMP_DATA_TYPE_INFO *info )
{
    if ( dataType >= DATA_TYPE_UNKNOWN )
    {
        info->asnType   = 0x00U;
        info->asnLen    = 0x00U;
        return false;
    }

    info->asnType   = dataTypeTable[dataType].asnType;
    info->asnLen    = dataTypeTable[dataType].asnLen;

    return true;
}

/****************************************************************************
Function:
    uint8_t TCPIP_SNMP_ProcessSetVar(PDU_INFO* pduDbPtr,OID_INFO* rec,
                                       SNMP_ERR_STATUS* errorStatus)

Summary:
    Processes snmp Set request pdu.

Description:
    This routine processes the received snmp set request pdu for the
    variable binding in the request and also creates the response pdu.

Precondition:
    TCPIP_SNMP_ProcessVariables() is called.

Parameters:
pduDbPtr    -   Received pdu information database pointer
rec         -   Pointer to SNMP MIB variable object information
errorStatus -   Pointer to update error status info

Return Values:
    copiedBytes - Number of bytes copied by this routine to the
                              snmp pdu tx buffer.

Remarks:
    None.
***************************************************************************/
uint8_t TCPIP_SNMP_ProcessSetVar(PDU_INFO* pduDbPtr,OID_INFO* rec, SNMP_ERR_STATUS* errorStatus)
{
    uint8_t ref;
    uint8_t temp=0;
    uint8_t dataType=0;
    uint8_t dataLen=0;
    uint8_t copiedBytes=0;
    SNMP_ERR_STATUS errorCode;
    SNMP_DATA_TYPE_INFO actualDataTypeInfo;
    SNMP_VAL dataValue,setValue;
    SNMP_BUFFER_DATA *snmpPutData = NULL;

#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
    SNMPV3MSGDATA   *dynPduBuf=NULL;

    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=NULL;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;
    dynPduBuf = &snmpv3EngnDcptMemoryStubPtr->ScopedPduRespnsBuf;
#endif

    snmpPutData = &SnmpStackDcptMemStubPtr->outPduBufData;
    
    // Start with no error.
    errorCode = SNMP_NO_ERR;
    copiedBytes = 0;

    

    // Non-leaf, Constant and ReadOnly node cannot be modified
    if(rec->nodeInfo.Flags.bIsParent != 0U || rec->nodeInfo.Flags.bIsConstant != 0U || rec->nodeInfo.Flags.bIsEditable == 0U )
    {   
        if(pduDbPtr->snmpVersion == (uint8_t)SNMP_V1)
        {
            errorCode = SNMP_NO_SUCH_NAME;
        }
        else if ((pduDbPtr->snmpVersion == (uint8_t)SNMP_V2C) || (pduDbPtr->snmpVersion == (uint8_t)SNMP_V3))
        {
            errorCode = SNMP_NOT_WRITABLE;
        }
        else
        {
            // do nothing
        }
    }

    if(pduDbPtr->snmpVersion != (uint8_t)SNMP_V3)
    {
        dataType = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(dataType,snmpPutData);
    }
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
    else
    {
        dataType = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);//TCPIP_SNMPv3_ProcessBuffDataGet(snmpv3EngnDcptMemoryStubPtr->ScopedPduRequstBuf,++snmpv3EngnDcptMemoryStubPtr->ScopedPduDataPos);
        (void)TCPIP_SNMPv3_DataCopyToProcessBuff(dataType,dynPduBuf);
    }
#endif
    copiedBytes++;

    // Get data type for this node.
    if ( !TCPIP_SNMP_DataTypeInfoGet(rec->dataType, &actualDataTypeInfo) )
    {
        if(pduDbPtr->snmpVersion == (uint8_t)SNMP_V1)
        {
            errorCode = SNMP_BAD_VALUE;
        }
        else if ((pduDbPtr->snmpVersion == (uint8_t)SNMP_V2C) || (pduDbPtr->snmpVersion == (uint8_t) SNMP_V3))
        {
            errorCode = SNMP_WRONG_TYPE;
        }
        else
        {
            // do nothing
        }
    }

    // Make sure that received data type is same as what is declared
    // for this node.
    if ( dataType != actualDataTypeInfo.asnType )
    {
        if(pduDbPtr->snmpVersion == (uint8_t)SNMP_V1)
        {
            errorCode = SNMP_BAD_VALUE;
        }
        else if ((pduDbPtr->snmpVersion == (uint8_t)SNMP_V2C) || (pduDbPtr->snmpVersion == (uint8_t) SNMP_V3))
        {
            errorCode = SNMP_WRONG_TYPE;
        }
        else
        {
            // do nothing
        }
    }
// find the length of the ASN Integer
    dataLen = (uint8_t)F_SNMP_ParseLength();            
    if (dataLen == (uint8_t)0xffU)
    {        
        return (0);
    }
    if(actualDataTypeInfo.asnType != (uint8_t)OCTET_STRING)
    {
        // if the length of a integer type is more than 4 or  less than 0 bytes don't do anything
        if ((dataLen > 4U) || (dataLen == 0U))
        {   
            return (0);
        }
    }
    // Make sure that received data length is within our capability.
    if(pduDbPtr->snmpVersion != (uint8_t) SNMP_V3)
    {
        (void)TCPIP_SNMP_DataCopyToProcessBuffer(dataLen,snmpPutData);
    }
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
    else
    {
        (void)TCPIP_SNMPv3_DataCopyToProcessBuff(dataLen,dynPduBuf);
    }
#endif
    copiedBytes++;

    // Only max data length of 127 is supported.
    if ( dataLen > 0x7fu )
    {
        if(pduDbPtr->snmpVersion == (uint8_t)SNMP_V1)
        {
            errorCode = SNMP_BAD_VALUE;
        }
        else if ((pduDbPtr->snmpVersion == (uint8_t)SNMP_V2C)|| (pduDbPtr->snmpVersion == (uint8_t) SNMP_V3))
        {
            errorCode = SNMP_WRONG_LENGTH;
        }
        else
        {
            // do nothing
        }
    }

    // If this is a Simple variable and given index is other than '0',
    // it is considered bad value
    if ( rec->nodeInfo.Flags.bIsSequence == 0U && rec->index != 0x00u )
    {
        errorCode = SNMP_NO_SUCH_NAME;
    }

    dataValue.dword = 0;
    ref = 0;

    // If data length is within 4 bytes, fetch all at once and pass it
    // to application.
    if ( actualDataTypeInfo.asnLen != 0xffU )
    {
        // According to mib def., this data length for this data type/
        // must be less or equal to 4, if not, we don't know what this
        // is.
        if ( dataLen <= 4u )
        {
            dataValue.dword = F_SNMP_ParseInteger(dataLen,&setValue.dword);      

            while( dataLen-- != 0U)
            {
                temp = (unsigned char)(setValue.v[dataLen] & 0xFFU);
                if(pduDbPtr->snmpVersion != (uint8_t) SNMP_V3)
                {
                    // Copy same byte back to create response...
                    (void)TCPIP_SNMP_DataCopyToProcessBuffer(temp,snmpPutData);
                }
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
                else
                {
                    (void)TCPIP_SNMPv3_DataCopyToProcessBuff(temp,dynPduBuf);
                }
#endif
                copiedBytes++;
            }
            

            // Pass it to application.
            if ( errorCode == SNMP_NO_ERR )
            {
                if(!TCPIP_SNMP_VarbindSet(rec->id, rec->index, ref, dataValue))
                {
                    if(pduDbPtr->snmpVersion == (uint8_t)SNMP_V1)
                    {
                        errorCode = SNMP_BAD_VALUE;
                    }
                    else if ((pduDbPtr->snmpVersion == (uint8_t)SNMP_V2C) || (pduDbPtr->snmpVersion == (uint8_t) SNMP_V3))
                    {
                        errorCode = SNMP_WRONG_VALUE;
                    }
                    else
                    {
                        // do nothing
                    }
                }
            }
        }
        else
        {
            if(pduDbPtr->snmpVersion == (uint8_t)SNMP_V1)
            {
                errorCode = SNMP_BAD_VALUE;
            }
            else if ((pduDbPtr->snmpVersion == (uint8_t)SNMP_V2C)|| (pduDbPtr->snmpVersion == (uint8_t) SNMP_V3))
            {
                if( rec->nodeInfo.Flags.bIsConstant != 0U)
                {
                    errorCode = SNMP_NOT_WRITABLE;
                }
                else
                {
                    errorCode = SNMP_WRONG_LENGTH;
                }
            }
            else
            {
                // do nothing
            }
        }
    }
    else
    {
        // This is a multi-byte Set operation.
        // Check with application to see if these many bytes can be
        // written to current variable.      
        if ( !TCPIP_SNMP_IsValidLength(rec->id, dataLen,rec->index) )
        {
            if(pduDbPtr->snmpVersion == (uint8_t)SNMP_V1)
            {
                errorCode = SNMP_BAD_VALUE;
            }
            else if ((pduDbPtr->snmpVersion == (uint8_t)SNMP_V2C)  || (pduDbPtr->snmpVersion == (uint8_t) SNMP_V3))
            {
                if( rec->nodeInfo.Flags.bIsConstant != 0U)
                {
                    errorCode = SNMP_NOT_WRITABLE;
                }
                else
                {
                    errorCode = SNMP_WRONG_LENGTH;
                }
            }
            else
            {
                // do nothing
            }
        }
        // Even though there may have been error processing this
        // variable, we still need to reply with original data
        // so at least copy those bytes.
        while( dataLen--  != 0U)
        {
            if(pduDbPtr->snmpVersion != (uint8_t) SNMP_V3)
            {
                dataValue.byte = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
                (void)TCPIP_SNMP_DataCopyToProcessBuffer(dataValue.byte,snmpPutData);
            }
            #ifdef TCPIP_STACK_USE_SNMPV3_SERVER
            else
            {
                dataValue.byte = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);// TCPIP_SNMPv3_ProcessBuffDataGet(snmpv3EngnDcptMemoryStubPtr->ScopedPduRequstBuf,++snmpv3EngnDcptMemoryStubPtr->ScopedPduDataPos);
                (void)TCPIP_SNMPv3_DataCopyToProcessBuff(dataValue.byte,dynPduBuf);
            }
            #endif
            copiedBytes++;

            // Ask application to set this variable only if there was
            // no previous error.
            if ( errorCode == SNMP_NO_ERR )
            {
                if ( !TCPIP_SNMP_VarbindSet(rec->id, rec->index, ref++, dataValue) )
                {
                    errorCode = SNMP_BAD_VALUE;
                }
            }
        }
        // Let application know about end of data transfer
        if ( errorCode == SNMP_NO_ERR )
        {
            if(!TCPIP_SNMP_VarbindSet(rec->id, rec->index, (uint8_t)SNMP_END_OF_VAR, dataValue))
            {
                errorCode = SNMP_BAD_VALUE;
            }
        }
    }
    *errorStatus = errorCode;

    return copiedBytes;
}


/****************************************************************************
Function:
    uint8_t TCPIP_SNMP_ProcessGetVar(OID_INFO* rec, bool bAsOID,PDU_INFO* pduDbPtr)

Summary:
    Processes snmp Get request pdu.

Description:
    This routine processes the received snmp Get request pdu for the
    variable binding in the request and also creates the response pdu.

Precondition:
    TCPIP_SNMP_ProcessVariables() is called.

Parameters:
rec      -   Pointer to SNMP MIB variable object information
bAsOID   -   Oid flag.

Return Values:
    varLen  - Number of bytes put in response tx pdu
    false   - If any of the elements of the request pdu validation fails.

Remarks:
    None.
***************************************************************************/
uint8_t TCPIP_SNMP_ProcessGetVar(OID_INFO* rec, bool bAsOID,PDU_INFO* pduDbPtr)
{
    uint8_t ref;
    uint8_t temp;
    uint8_t varLen,tempLen=0;
    uint8_t dataType;
    uint16_t offset;
    uint16_t prevOffset;
    uint16_t initPutDatalengthOffset=0;
    SNMP_VAL v;
    SNMP_DATA_TYPE_INFO dataTypeInfo;
    SNMP_BUFFER_DATA *snmpPutData = NULL;
    
     
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    

    SNMPV3MSGDATA   *dynPduBuf=NULL;
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=NULL;
                         
    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;
    dynPduBuf = &snmpv3EngnDcptMemoryStubPtr->ScopedPduRespnsBuf;
        
#endif
    
    snmpPutData = &SnmpStackDcptMemStubPtr->outPduBufData;
    initPutDatalengthOffset = snmpPutData->length;
    offset = 0; 
    v.dword   = 0;

    // Non-leaf node does not contain any data.
    if ( rec->nodeInfo.Flags.bIsParent  != 0U)
    {
        return 0U;
    }

    // If current OID is Simple variable and index is other than .0
    // we don't Get this variable.
    if ( rec->nodeInfo.Flags.bIsSequence == 0U )
    {
        // index of other than '0' is not invalid.
        if ( rec->index > 0u )
        {
            snmpPutData->length = initPutDatalengthOffset;
            return 0U;
        }
    }

    dataType = (uint8_t)rec->dataType;
    if ( !TCPIP_SNMP_DataTypeInfoGet((SNMP_DATA_TYPE)dataType, &dataTypeInfo) )
    {
        snmpPutData->length = initPutDatalengthOffset;
        return 0U;
    }

    if ( !bAsOID )
    {
        if(pduDbPtr->snmpVersion != (uint8_t) SNMP_V3)
        {
            if(TCPIP_SNMP_DataCopyToProcessBuffer(dataTypeInfo.asnType,snmpPutData)!=true)
            {
                return 0U;
            }

            offset = snmpPutData->length;//SNMPTxOffset;
            if(TCPIP_SNMP_DataCopyToProcessBuffer(dataTypeInfo.asnLen,snmpPutData)!=true)
            {
                return 0U;
            }
        }
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
        else
        {
            if(TCPIP_SNMPv3_DataCopyToProcessBuff(dataTypeInfo.asnType,dynPduBuf) != true)
            {
                return 0U;
            }
            offset = dynPduBuf->length;
            if(TCPIP_SNMPv3_DataCopyToProcessBuff(dataTypeInfo.asnLen,dynPduBuf)!= true)
            {
                return 0U;
            }
        }
#endif
    }

    if ( rec->nodeInfo.Flags.bIsConstant != 0U)
    {
        uint8_t c;

        (void)SYS_FS_FileSeek(snmpFileDescrptr, (int32_t)rec->hData, SYS_FS_SEEK_SET);
        (void)SYS_FS_FileRead(snmpFileDescrptr,&varLen,1);
        temp = varLen;
        while( temp--  != 0U)
        {
            (void)SYS_FS_FileRead(snmpFileDescrptr,&c,1);
            if(pduDbPtr->snmpVersion != (uint8_t) SNMP_V3)
            {
                if(TCPIP_SNMP_DataCopyToProcessBuffer(c,snmpPutData)!= true)
                {
                    return 0U;
                }
            }
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
            else if(TCPIP_SNMPv3_DataCopyToProcessBuff(c,dynPduBuf) != true)
            {
                return 0U;
            }
#endif  
            else
            {
                // do nothing
            }
        }
    }
    else
    {
        ref = SNMP_START_OF_VAR;
        v.dword = 0;
        varLen = 0;

        do
        {
            if ( TCPIP_SNMP_VarbindGet(rec->id, rec->index, &ref, &v) )
            {
                if ( dataTypeInfo.asnLen != 0xffU )
                {
                    if((dataTypeInfo.asnType != (uint8_t)SNMP_IP_ADDR) && (dataTypeInfo.asnType != (uint8_t)SNMP_COUNTER32)&&
                            (dataTypeInfo.asnType != (uint8_t)SNMP_TIME_TICKS) && (dataTypeInfo.asnType != (uint8_t)SNMP_GAUGE32)&&
                            (dataTypeInfo.asnType != (uint8_t)SNMP_OPAQUE) && (dataTypeInfo.asnType != (uint8_t)SNMP_NSAP_ADDR))
                    {
                        tempLen = varLen = (uint8_t)F_SNMP_BuildIntegerLen(v.dword);
                    }
                    else
                    {
                        tempLen = varLen = 4;
                    }
                    v.dword = F_SNMP_BuildIntegerValue(v,varLen);
                    while( tempLen--  != 0U)
                    {
                        temp = v.v[tempLen];
                        if(pduDbPtr->snmpVersion != (uint8_t) SNMP_V3)
                        {
                            if(TCPIP_SNMP_DataCopyToProcessBuffer(temp,snmpPutData)!= true)
                            {
                                return 0U;
                            }
                        }
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
                        else
                        {
                            if(TCPIP_SNMPv3_DataCopyToProcessBuff(temp,dynPduBuf)!=true)
                            {
                                return 0U;
                            }
                        }
#endif
                    }
                    break;
                }
                else
                {
                    if(ref == (uint8_t)SNMP_END_OF_VAR)
                    {
                        break;
                    }
                    varLen++;
                    if(pduDbPtr->snmpVersion != (uint8_t) SNMP_V3)
                    {
                        if(TCPIP_SNMP_DataCopyToProcessBuffer(v.v[0],snmpPutData)!= true)
                        {
                            return 0U;
                        }
                    }
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
                    else if(TCPIP_SNMPv3_DataCopyToProcessBuff(v.v[0],dynPduBuf) != true)
                    {
                        return 0U;
                    }
#endif
                    else
                    {
                        // do nothing
                    }
                }
            }
            else
            {
                snmpPutData->length = initPutDatalengthOffset;
                return 0U;
            }

        } while( true);
    }

    if ( !bAsOID  || (varLen != dataTypeInfo.asnLen))
    {
        if(pduDbPtr->snmpVersion != (uint8_t)SNMP_V3)
        {
            prevOffset = snmpPutData->length;
            snmpPutData->length = offset;
            (void)TCPIP_SNMP_DataCopyToProcessBuffer(varLen,snmpPutData);
            snmpPutData->length = prevOffset;
        }
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER    
        else
        {
            prevOffset = dynPduBuf->length;
            dynPduBuf->length = offset;
            (void)TCPIP_SNMPv3_DataCopyToProcessBuff(varLen,dynPduBuf);
            dynPduBuf->length = prevOffset;
        }
#endif
        varLen++;
        varLen++;
    }
    return varLen;
}

/****************************************************************************
Function:
    void TCPIP_SNMP_ErrorStatusSet(uint16_t errorStatusOffset,
                       uint16_t errorIndexOffset,
                       SNMP_ERR_STATUS errorStatus,
                       uint8_t errorIndex)
Summary:
    Set snmp error status in the response pdu.

Description:
    This routine processes the received snmp Get request pdu for the
    variable binding in the request and also creates the response pdu.

Precondition:
    TCPIP_SNMP_ProcessVariables() is called.

Parameters:
    errorStatusOffset - Offset to update error status in Response Tx pdu
    errorIndexOffset  - Offset to update error index
    errorStatus       - Snmp process error to be updated in response.
    errorIndex        - Index of the request varbind in the var bind list
                                    for which error status is to be updated.

Returns:
    None.

Remarks:
    None.
***************************************************************************/
static void TCPIP_SNMP_ErrorStatusSet(uint16_t errorStatusOffset,
                           uint16_t errorIndexOffset,
                           SNMP_ERR_STATUS errorStatus,
                           uint8_t errorIndex,SNMP_BUFFER_DATA *snmpPutTxData)
{
    uint16_t prevOffset;

    prevOffset = snmpPutTxData->length;
    snmpPutTxData->length = errorStatusOffset;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer((uint8_t)errorStatus,snmpPutTxData);

    
    snmpPutTxData->length = errorIndexOffset;
    (void)TCPIP_SNMP_DataCopyToProcessBuffer(errorIndex,snmpPutTxData);

    snmpPutTxData->length = prevOffset;
}


/****************************************************************************
Function:
    uint8_t TCPIP_SNMP_OIDsCountGet(uint16_t pdulen)

Summary:
    Finds number of varbinds in the varbind list received in a pdu.

Description:
    This routine is used to find the number of OIDs requested in the received
    snmp pdu.

Precondition    :
    TCPIP_SNMP_ProcessVariables() is called.

Parameters:
    pdulen      -   Length of snmp pdu request received.

Return Values:
    varCount    -   Number of OIDs found in a pdu request.

Remarks:
    None.
***************************************************************************/
static uint8_t TCPIP_SNMP_OIDsCountGet(uint16_t pdulen)
{
    uint8_t  structureLen;
    uint8_t varCount=0;
    uint16_t prevUDPRxOffset;
    uint16_t varBindLen;
    uint16_t snmpPduLen;
    
    snmpPduLen=pdulen;

    prevUDPRxOffset=SNMPRxOffset;

    while(snmpPduLen != 0U)
    {
        structureLen = TCPIP_SNMP_StructureIsValid(&varBindLen);
        if(structureLen == 0U)
        {
            return 0U;
        }
    

        SNMPRxOffset=SNMPRxOffset+varBindLen;
        varCount++;
        snmpPduLen=snmpPduLen
                - structureLen // 1 byte for STRUCTURE identifier + 0x82 or ox81+1+1 byte(s) for varbind length
                - varBindLen;
                    //-1 //1 byte for STRUCTURE identifier
                    //-1//1 byte for varbind length 
                //  -varBindLen;
    }

    SNMPRxOffset=prevUDPRxOffset;

    return varCount;
}

bool  TCPIP_SNMP_WriteCommunityGet(int index,int len, uint8_t * dest)
{
    if((dest == NULL) || (len==0)|| (SnmpStackDcptMemStubPtr==NULL))
    {
        return false;
    }

   
    if(index >= TCPIP_SNMP_MAX_COMMUNITY_SUPPORT)
    {
        return false;
    }
    

    /* the parameter "len" will be always less than and equal to the value of TCPIP_SNMP_COMMUNITY_MAX_LEN + 1
    * The value of sizeof(SnmpStackDcptMemStubPtr->snmpNetConfig.writeCommunity[index]) = TCPIP_SNMP_COMMUNITY_MAX_LEN + 1
    */        
    (void)strncpy((char*)dest,(char*)SnmpStackDcptMemStubPtr->snmpNetConfig.writeCommunity[index], (size_t)len - 1U);
    dest[len-1] = 0;
    
    return true;
}

bool  TCPIP_SNMP_ReadCommunityGet(int index,int len, uint8_t *dest)
{

    if((dest == NULL) || (len==0)|| (SnmpStackDcptMemStubPtr==NULL))
    {
        return false;
    }

       
    if(index >= TCPIP_SNMP_MAX_COMMUNITY_SUPPORT)
    {
        return false;
    }
    
    /* the parameter "len" will be always less than and equal to the value of TCPIP_SNMP_COMMUNITY_MAX_LEN + 1
    * The value of sizeof(SnmpStackDcptMemStubPtr->snmpNetConfig.readCommunity[index]) = TCPIP_SNMP_COMMUNITY_MAX_LEN + 1
    */
    (void)strncpy((char*)dest,(char*)SnmpStackDcptMemStubPtr->snmpNetConfig.readCommunity[index],(size_t)len - 1U);
    dest[len-1] = 0;
    
    return true;
}


bool  TCPIP_SNMP_WriteCommunitySet(int index,int len, uint8_t * src)
{

    if((src == NULL) || (len==0)|| (SnmpStackDcptMemStubPtr==NULL))
    {
        return false;
    }

    if(len>TCPIP_SNMP_COMMUNITY_MAX_LEN)
    {
        return false;
    }

    if(index >= TCPIP_SNMP_MAX_COMMUNITY_SUPPORT)
    {
        return false;
    }

        
    /* the parameter "len" will be always less than the value of TCPIP_SNMP_COMMUNITY_MAX_LEN
     * The value of sizeof(SnmpStackDcptMemStubPtr->snmpNetConfig.writeCommunity[index]) = TCPIP_SNMP_COMMUNITY_MAX_LEN + 1
     */
    
    size_t  commLen = sizeof(SnmpStackDcptMemStubPtr->snmpNetConfig.writeCommunity[index]);
    (void)strncpy((char*)SnmpStackDcptMemStubPtr->snmpNetConfig.writeCommunity[index],(char*)src,commLen - 1U);

    SnmpStackDcptMemStubPtr->snmpNetConfig.writeCommunity[index][commLen-1U] = 0;
    return true;
}

bool  TCPIP_SNMP_ReadCommunitySet(int index,int len, uint8_t * src)
{

    if((src == NULL) || (len==0) || (SnmpStackDcptMemStubPtr==NULL))
    {
        return false;
    }

    if(len>TCPIP_SNMP_COMMUNITY_MAX_LEN)
    {
        return false;
    }

    if(index >= TCPIP_SNMP_MAX_COMMUNITY_SUPPORT)
    {
        return false;
    }
    
    
    /* the parameter "en" will be always less than the value of TCPIP_SNMP_COMMUNITY_MAX_LEN
     * The value of sizeof(SnmpStackDcptMemStubPtr->snmpNetConfig.readCommunity[index]) = TCPIP_SNMP_COMMUNITY_MAX_LEN + 1
     */
    size_t commLen = sizeof(SnmpStackDcptMemStubPtr->snmpNetConfig.readCommunity[index]);
    
    (void)strncpy((char*)SnmpStackDcptMemStubPtr->snmpNetConfig.readCommunity[index],(char*)src,commLen-1U);
    SnmpStackDcptMemStubPtr->snmpNetConfig.readCommunity[index][commLen-1U] = 0;
    
    return true;
}


/****************************************************************************
Function:
    bool TCPIP_SNMP_PvtMIBObjIsRequested(uint8_t* OIDValuePtr)

Summary:
    To find whether requested OID is only for private access.

Description:
    This routine is used to find whether requested object belongs
    to the private object group of the mib of agent. If yes, then
    that mib object can be accessed only with private community
    (supported in SNMPv2c).

Precondition    :
    TCPIP_SNMP_ProcessVariables() is called.

Parameters:
    OIDValuePtr -   Pointer to memory stored with received OID.

Return Values:
    true    -   If the requested object is of private branch of the mib.
    FLASE   -   If the requested object is publically accessible.

Remarks:
    None.
***************************************************************************/
static bool TCPIP_SNMP_PvtMIBObjIsRequested(uint8_t* OIDValuePtr)
{
    uint8_t cnt=0;
    static const uint8_t pvtObjIdentifier[4]={0x2bU,0x06U/*dod*/,0x01U/*internet*/,0x04U/*private*/};

    while(cnt < 4u)
    {
        //check whether requested oid is for pvt obj
        if(pvtObjIdentifier[cnt]== OIDValuePtr[cnt])
        {
            cnt++;
        }
        else
        {
            return false;
        }
    }
    return true;
}

/*
* Set the File Descriptor
*/
void TCPIP_SNMP_FileDescrSet(SYS_FS_HANDLE fileDescr)
{
    snmpFileDescrptr = fileDescr;
}

/*
*  Get The File Descriptor
*/
SYS_FS_HANDLE TCPIP_SNMP_FileDescrGet(void)
{
    return snmpFileDescrptr;
}

void TCPIP_SNMP_TrapSpecificNotificationGet(uint8_t *specTrap)
{
    SNMP_PROCESSING_MEM_INFO_PTRS snmpPktProcsMemPtrsInfo;
    SNMP_STACK_DCPT_STUB * snmpStkDcptMemStubPtr=NULL;
    (void)memset(&snmpPktProcsMemPtrsInfo,0,sizeof(snmpPktProcsMemPtrsInfo));
    TCPIP_SNMP_PacketProcStubPtrsGet(&snmpPktProcsMemPtrsInfo);
    if(snmpPktProcsMemPtrsInfo.snmpStkDynMemStubPtr == NULL)
    {
        return;
    }
    snmpStkDcptMemStubPtr=snmpPktProcsMemPtrsInfo.snmpStkDynMemStubPtr;
    *specTrap = snmpStkDcptMemStubPtr->gSpecificTrapNotification;
}

/* MISRA C-2012 Rule 5.1 deviated:1 Deviation record ID -  H3_MISRAC_2012_R_5_1_NET_DR_10 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:1 "MISRA C-2012 Rule 5.1" "H3_MISRAC_2012_R_5_1_NET_DR_10" 
void TCPIP_SNMP_TrapSpecificNotificationSet(uint8_t specTrap,uint8_t genTrap, SNMP_ID trapID)
{
    SNMP_PROCESSING_MEM_INFO_PTRS snmpPktProcsMemPtrsInfo;
    SNMP_STACK_DCPT_STUB * snmpStkDcptMemStubPtr=NULL;

    (void)memset(&snmpPktProcsMemPtrsInfo,0,sizeof(snmpPktProcsMemPtrsInfo));
    TCPIP_SNMP_PacketProcStubPtrsGet(&snmpPktProcsMemPtrsInfo);
    if(snmpPktProcsMemPtrsInfo.snmpStkDynMemStubPtr == NULL)
    {
        return;
    }
    snmpStkDcptMemStubPtr=snmpPktProcsMemPtrsInfo.snmpStkDynMemStubPtr;
    snmpStkDcptMemStubPtr->gSpecificTrapNotification = specTrap;
    snmpStkDcptMemStubPtr->gGenericTrapNotification = genTrap;
    snmpStkDcptMemStubPtr->SNMPNotifyInfo.trapIDVar = trapID;
}
#pragma coverity compliance end_block "MISRA C-2012 Rule 5.1"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */

void TCPIP_SNMP_TrapInterFaceSet(TCPIP_NET_HANDLE netIntf)
{
    if(SnmpStackDcptMemStubPtr == NULL)
    {
        return;
    }
    SnmpStackDcptMemStubPtr->SNMPNotifyInfo.snmpTrapInf = netIntf;
}
void TCPIP_SNMP_AuthTrapFlagSet(bool sendTrap)
{
    if(SnmpStackDcptMemStubPtr == NULL)
    {
        return;
    }
   SnmpStackDcptMemStubPtr->gSendTrapFlag =  sendTrap;
}

void TCPIP_SNMP_AuthTrapFlagGet(bool *authTrap)
{
    if(SnmpStackDcptMemStubPtr == NULL)
    {
        return;
    }
    *authTrap = SnmpStackDcptMemStubPtr->gSendTrapFlag;
}
//#if defined(SNMP_STACK_USE_V2_TRAP)
void TCPIP_SNMP_TrapSendFlagSet(bool trapNotify)
{
    if(SnmpStackDcptMemStubPtr == NULL)
    {
        return;
    }
   SnmpStackDcptMemStubPtr->gSetTrapSendFlag =  trapNotify ? 1U : 0U;
}

void TCPIP_SNMP_TrapSendFlagGet(bool *trapNotify)
{
    if(SnmpStackDcptMemStubPtr == NULL)
    {
        return;
    }
   *trapNotify = SnmpStackDcptMemStubPtr->gSetTrapSendFlag == 0U ? false : true;
}
//#endif
void TCPIP_SNMP_SocketIDGet(UDP_SOCKET *skt)
{
    *skt = gSnmpDcpt.skt;
}

void TCPIP_SNMP_SocketIDSet(UDP_SOCKET skt)
{
    gSnmpDcpt.skt =  skt;
}

void TCPIP_SNMP_TRAPMibIDGet(uint32_t *mibID)
{
    if(SnmpStackDcptMemStubPtr == NULL)
    {
        return;
    }
    *mibID = (uint32_t)SnmpStackDcptMemStubPtr->gOIDCorrespondingSnmpMibID;
}

void TCPIP_SNMP_MibIDSet(uint32_t mibID)
{
    if(SnmpStackDcptMemStubPtr == NULL)
    {
        return;
    }
    SnmpStackDcptMemStubPtr->gOIDCorrespondingSnmpMibID = (uint8_t)mibID;
}

uint32_t TCPIP_SNMP_GetRXOffset(void)
{
    return (uint32_t)SNMPRxOffset;
}

void TCPIP_SNMP_SetRXOffset(uint32_t offset)
{
    SNMPRxOffset = (uint16_t)offset;
}
#if defined (TCPIP_STACK_USE_IPV6)
static void SNMP_IPV6_Notify(TCPIP_NET_HANDLE hNet, IPV6_EVENT_TYPE evType, const void* evParam, const void* usrParam)
{
    gSnmpDcpt.ipv6EventType = evType;
}

/*
* Return IPv6  Notification 
*/

IPV6_EVENT_TYPE TCPIP_SNMP_EventNotifyGet(TCPIP_NET_HANDLE hNet)
{
    return gSnmpDcpt.ipv6EventType;
}

#endif


#endif //#if defined(TCPIP_STACK_USE_SNMP_SERVER)

