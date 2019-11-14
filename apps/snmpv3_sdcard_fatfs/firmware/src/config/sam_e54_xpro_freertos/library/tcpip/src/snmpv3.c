/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_SNMPV3_SERVER

#include "tcpip/src/tcpip_private.h"
#if defined(TCPIP_STACK_USE_SNMPV3_SERVER)
#include "tcpip/src/snmpv3_private.h"
#include "tcpip/src/common/sys_fs_shell.h"
#include "tcpip/snmpv3.h"



static SNMPV3_STACK_DCPT_STUB * Snmpv3StackDcptStubPtr=0;

/*Length of the SNMPv3 msg header(x) = Header length (2 bytes)
+ MSGID size (type(1 byte) + length of value(1 byte)+4 bytes value)
+ msgMAXSIZE(type + length of value +4 bytes value)
+ msg flag(type + length of value +1 byte value)
+ security model type(type + length of value +1 byte value) */
#define MSGGLOBAL_HEADER_LEN(x)	( x= (2 \
                                      +1+1+4 \
                                      +1+1+4 \
                                      +1+1+1  \
                                      +1+1+1)\
                                )
/*Length of SNMPv3 authoratative msg header length =
Header length ( 2 + 2 bytes)  + engineID ( snmpEngnIDLength bytes)
+ engine boot( 4 bytes)+ engine time(4 bytes)
+security name (securityPrimitivesOfIncomingPdu value)
+authentication parameters (snmpOutMsgAuthParamLen value)
+privacy parameters (snmpOutMsgAuthParamLen value)*/
#define MSG_AUTHORITATIVE_HEADER_LEN(x)   ( x=(2+2 \
                                             +1+1+sizeof(Snmpv3StackDcptStubPtr->SnmpEngineID)\
                                             +1+1+4 \
                                             +1+1+4 \
                                             +1+1+TCPIP_SNMPV3_USER_SECURITY_NAME_LEN \
                                             +1+1+SNMPv3_MSG_AUTH_PARAM_STRING_LEN \
                                             +1+1+SNMPv3_MSG_PRIV_PARAM_STRING_LEN) \
                                            )

static int32_t Snmpv3TrapFileDescrptr=(int32_t)SYS_FS_HANDLE_INVALID;

typedef struct 
{
    uint8_t privAndAuthFlag:2;
    uint8_t reportableFlag :1;
}snmpV3MsgFlagsBitWise;

extern TCPIP_SNMP_DCPT gSnmpDcpt;

// file shell object for file access
extern const SYS_FS_SHELL_OBJ*  snmpFileShell;

/*   
SNMP_ENGINE_MAX_MSG_SIZE is determined as the minimum of the max msg size
values supported among all of the transports available to and supported by the engine. 
*/
#define SNMP_ENGINE_MAX_MSG_SIZE	1024 

static uint8_t _SNMPv3_FindOIDsFrmIncmingV3Req(uint16_t pdulen);

static void _SNMPv3_ConstructReportPDU(SNMPV3MSGDATA *dynScopedBufPtr);
static void _SNMPv3_SetErrorStatus(uint16_t errorStatusOffset        ,
                                uint16_t errorIndexOffset,
                                SNMP_ERR_STATUS errorStatus,
                                uint8_t errorIndex                ,
                                SNMPV3MSGDATA *dynScopedPduPutBuf );


static uint8_t _SNMPv3_CheckIfValidAuthStructure(uint16_t* dataLen);
extern void SaveAppConfig(void);
static bool _SNMP_V3ContextNameLengthGet(uint16_t* dataLen);
static SNMPV3_PRIV_PROT_TYPE TCPIP_SNMPV3_PrivProtocolGet(uint8_t *username);
static void TCPIP_SNMPV3_DESEncryptionPadding(SNMPV3MSGDATA *dynTrapScopedPduBuf);


uint16_t _SNMPv3_Header_Length(void)
{
    uint16_t  snmpv3MsgGlobalHeaderlength=0,snmpv3MsgAuthHeaderLength=0;
    uint16_t snmpv3Headerlength=0;
     // SNMPv3 Trap header allocation and construction
    snmpv3Headerlength = MSGGLOBAL_HEADER_LEN(snmpv3MsgGlobalHeaderlength)+
                  MSG_AUTHORITATIVE_HEADER_LEN(snmpv3MsgAuthHeaderLength);

    return snmpv3Headerlength;
}
/****************************************************************************
  Function:
    void SNMPv3Init(TCPIP_NET_IF* netIf)
	
  Summary:
    Initialize the Snmpv3 agent Username and Security Values and SNMPv3 engine ID.

  Description:
    SNMPv3 Engine ID initilization happens with proper interface details.
    So this API is called from TCPIP_SNMP_Task() .
	  	 		 		  	
  Precondition:
    TCPIP_SNMP_Initialize(); is called.
		
  Parameters:
    netIf - Interface  details
  	
  Return Values:
    None

  Remarks:
    None
***************************************************************************/
void TCPIP_SNMPv3_Initialize(void)
{
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    uint32_t        snmpv3Headerlength=0,msgDataLen=0;
    uint8_t         *ptr=NULL;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    Snmpv3StackDcptStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;
    Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.head = NULL;
    Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.wholeMsgHead = NULL;
    Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.head = NULL;
    Snmpv3StackDcptStubPtr->PduHeaderBuf.head = NULL;

    // SNMPv3 Outgoing MSG header allocation and construction
    snmpv3Headerlength = _SNMPv3_Header_Length();
    ptr = Snmpv3StackDcptStubPtr->PduHeaderBuf.head =
            (uint8_t *)(TCPIP_HEAP_Calloc(snmpv3PktProcessingMemPntr.snmpHeapMemHandler,1,(size_t)snmpv3Headerlength+5));
    if(ptr == NULL)
    {
        TCPIP_SNMP_FreeMemory();
        return;
    }
    Snmpv3StackDcptStubPtr->PduHeaderBuf.length = 0;
    Snmpv3StackDcptStubPtr->PduHeaderBuf.maxlength = snmpv3Headerlength+1;
// scoped trap PDU
    msgDataLen = TCPIP_SNMP_MAX_MSG_SIZE - snmpv3Headerlength;
    ptr = Snmpv3StackDcptStubPtr->ScopedPduRespnsBuf.head =
            (uint8_t*)(TCPIP_HEAP_Calloc(snmpv3PktProcessingMemPntr.snmpHeapMemHandler,1,(size_t)msgDataLen+5));
    if(ptr == NULL)
    {
        TCPIP_SNMP_FreeMemory();
        return ;
    }
    Snmpv3StackDcptStubPtr->ScopedPduRespnsBuf.length = 0;
    Snmpv3StackDcptStubPtr->ScopedPduRespnsBuf.maxlength = msgDataLen;

}

/****************************************************************************
  Function:
    uint8_t SNMPv3ConfigEngnSecrtyModel(uint32_t securityModelInRequest)
	
  Summary:
    Updates the Snmp agent security model value.

  Description:
    Checks for the security model value the in the request pdu against the security
    model value specified in the RFC3411 max range.
    Updates the snmp agent global variable 'SnmpEngnSecurityModel' for storing
    security model value reuqested in the incoming request from the SNMP managers.
	  	 		 		  	
  Precondition:
    TCPIP_SNMP_Initialize(); is called. The agent is recognised in the network as the SNMPv3 node.
    Snmp request is received.
		
  Parameters:
    securityModelInRequest : this value is populated from the incoming request PDUs.
  	
  Return Values:
    true:	If the incoming PDU has valid security model value.
    false:    If the incoming PDU has invalid or un supported security model.

  Remarks:
    'SnmpEngnSecurityModel' is value to uniquely identiy a security model of the
    security sub system whithin the SNMP management architecture. This value is
    used by the SNMP engine to send respond or inform PDUs to the other SNMP nodes.
    'SnmpEngnSecurityModel' value is used for interoperability.
***************************************************************************/
uint8_t TCPIP_SNMPv3_EngnSecrtyModelConfig(uint32_t securityModelInRequest)
{
    if(securityModelInRequest < 0x80000000  )
    {
        if(securityModelInRequest > ANY_SECUTIRY_MODEL &&
           securityModelInRequest <= SNMPV3_USM_SECURITY_MODEL
           /* && User can add the Enterprise specific Security Model if reuired and is implemented */ )
        {
            Snmpv3StackDcptStubPtr->SnmpEngnSecurityModel=securityModelInRequest;
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

/****************************************************************************
  Function:
    uint8_t SNMPv3EngnDecodeMsgFlags(uint8_t msgFlasgInRequest)
	
  Summary:
    Returns the message flag value in the received incoming snmp request pdu.

  Description:
    Compare the message flag value the in the request pdu against the Octet's
    least significant three bits: Reportable, PrivFlag, AuthFlag. Return the message flag
    value to message processing model to take corresponding decision for the
    message processing flow.
		  	 		 		  	
  Precondition:
    TCPIP_SNMP_Initialize(); is called. The agent is recognised in the network as the SNMPv3 node.
    Snmp request is received.

  Parameters:
    'msgFlasgInRequest' is value populated from the incoming request PDUs.
  	
  Return Values:
    INVALID_MSG: If the incoming PDU has undefined message flag value  or
    REPORT_FLAG_AND_SECURITY_LEVEL_FLAGS: if not INVALID_MSG, then
    any other matching value in the enum except INVALID_MSG.

  Remarks:
    'msgFlasgInRequest' is compared to possible combinations of snmp message flags.
    The return value from this routine is utilised by the message processing unit to
    decide on the course of action to be taken while processing the incoming request.
***************************************************************************/

uint8_t TCPIP_SNMPv3_EngnDecodeMsgFlags(uint8_t msgFlasgInRequest)
{
    if(msgFlasgInRequest > REPORT2REQ_PRIVACY_AND_AUTH_PROVIDED) /*if all msgFlags are SET*/
    {
        return INVALID_MSG;
    }
    else if( (msgFlasgInRequest & NO_REPORT_NO_PRIVACY_NO_AUTH) == NO_REPORT_NO_PRIVACY_NO_AUTH)
    {
        return NO_REPORT_NO_PRIVACY_NO_AUTH;
    }
    else if( (msgFlasgInRequest & NO_REPORT_NO_PRIVACY_BUT_AUTH_PROVIDED) == NO_REPORT_NO_PRIVACY_BUT_AUTH_PROVIDED)
    {
        return NO_REPORT_NO_PRIVACY_BUT_AUTH_PROVIDED;
    }
    else if( (msgFlasgInRequest & NO_REPORT_PRIVACY_PROVIDED_BUT_NO_AUTH) == NO_REPORT_PRIVACY_PROVIDED_BUT_NO_AUTH)
    {
        return INVALID_MSG;
    }
    else if( (msgFlasgInRequest & NO_REPORT_PRIVACY_AND_AUTH_PROVIDED) == NO_REPORT_PRIVACY_AND_AUTH_PROVIDED)
    {
        return NO_REPORT_PRIVACY_AND_AUTH_PROVIDED;
    }
    else if( (msgFlasgInRequest & REPORT2REQ_NO_PRIVACY_NO_AUTH) == REPORT2REQ_NO_PRIVACY_NO_AUTH)
    {
        return REPORT2REQ_NO_PRIVACY_NO_AUTH;
    }
    else if( (msgFlasgInRequest & REPORT2REQ_NO_PRIVACY_BUT_AUTH_PROVIDED) == REPORT2REQ_NO_PRIVACY_BUT_AUTH_PROVIDED)
    {
        return REPORT2REQ_NO_PRIVACY_BUT_AUTH_PROVIDED;
    }
    else if( (msgFlasgInRequest & REPORT2REQ_PRIVACY_PROVIDED_BUT_NO_AUTH) == REPORT2REQ_PRIVACY_PROVIDED_BUT_NO_AUTH)
    {
        return INVALID_MSG;
    }
    else if( (msgFlasgInRequest & REPORT2REQ_PRIVACY_AND_AUTH_PROVIDED) == REPORT2REQ_PRIVACY_AND_AUTH_PROVIDED)
    {
        return REPORT2REQ_PRIVACY_AND_AUTH_PROVIDED;
    }
    else
        return INVALID_MSG;
}

/****************************************************************************
  Function:
    uint32_t SNMPv3TrackAuthEngineTimeTick(void)
	
  Summary:
    Returns the internal tick timer value to be used by 'SnmpEngineTime'.
	
  Description:
  	
    This routine reads the internal system tick and converts it to tenths of milliseconds.
  	
  Precondition:
    None

  Parameters:
    None
  	
  Return Values:
    timeStamp : uint32_t value of the timer ticks

  Remarks:
    None
***************************************************************************/
uint32_t TCPIP_SNMPv3_AuthEngineTimeTickGet(void)
{
  
    uint32_t    timeStamp;

    // convert the current system tick to 10 ms units
    timeStamp = (SYS_TMR_TickCountGet() * 100ull)/SYS_TMR_TickCounterFrequencyGet();


    return timeStamp;

}


/****************************************************************************
  Function:
    void SNMPv3FormulateEngnID(uint8_t fifthOctectIdentifier,TCPIP_NET_IF* snmpIntf )
	
  Summary:
    Formulates the SnmpEngineID for the SNMPV3 engine.

  Description:
    Formulates the SnmpEngineID depending on value of  'fifthOctectIdentifier'.
    as MAC_ADDR_ENGN_ID using the application MAC address.
    'fifthOctectIdentifier' defualt set to MAC_ADDR_ENGN_ID as the following octets
    used for the SnmpEngineID are of mac address.

    User can set this octet of their choice to fomulate new SnmpEngineID.
    fifthOctectIdentifier=IPV4_ADDR_ENGN_ID;

    If
    fifthOctectIdentifier=ADMIN_ASSIGNED_TEXT;  or
    fifthOctectIdentifier=ADMIN_ASSIGNED_OCTETS;
    then the following octets should be provided by the administrator through some
    custom application interface mechanism.
    API parameter 'fifthOctectIdentifier' has to be upated in the intefrace API before
    passing through SNMPv3FormulateEngnID().

  Precondition:
    InitAppConfig(); is called.
		
  Parameters:
    fifthOctectIdentifier : Value of the 5th octet in the SnmpEngineID which indicates
    how the rest (6th and following octets) are formatted.
  	
  Return Values:
    None

  Remarks:
    Authentication and encryption keys are generated using corresponding passwords and
    SnmpEngineID. If the SnmpEngineID is newly configured, then the auth and privacy keys
    would also change. Hence while using this API to change the SnmpEngineID dynamically,
    care should be taken to update the new localized keys at the agent as well as at the manager.
***************************************************************************/
void _SNMPv3_EngnIDFormulate(uint8_t fifthOctectIdentifier,TCPIP_NET_IF* snmpIntf )
{
    unsigned int i;

    SNMP_PROCESSING_MEM_INFO_PTRS snmpPktProcsMemPtrsInfo;
    SNMP_STACK_DCPT_STUB * snmpStkDcptMemStubPtr=0;
    //Modify this private enterprise assigned number to your organization number asssigned by IANA
    TCPIP_UINT32_VAL mchpPvtEntpriseAssignedNumber;

    mchpPvtEntpriseAssignedNumber.Val = 0x42C7; //microchip = 17095.

    TCPIP_SNMP_PacketProcStubPtrsGet(&snmpPktProcsMemPtrsInfo);
    snmpStkDcptMemStubPtr=snmpPktProcsMemPtrsInfo.snmpStkDynMemStubPtr;


    //Set the first bit as '1b' .  Refer to RFC3411 section5 Page# 41
    mchpPvtEntpriseAssignedNumber.Val = ((0x80000000) | mchpPvtEntpriseAssignedNumber.Val);

    Snmpv3StackDcptStubPtr->SnmpEngineID[0]=mchpPvtEntpriseAssignedNumber.v[3];
    Snmpv3StackDcptStubPtr->SnmpEngineID[1]=mchpPvtEntpriseAssignedNumber.v[2];
    Snmpv3StackDcptStubPtr->SnmpEngineID[2]=mchpPvtEntpriseAssignedNumber.v[1];
    Snmpv3StackDcptStubPtr->SnmpEngineID[3]=mchpPvtEntpriseAssignedNumber.v[0];

    //Refer to RFC3411 section5 Page# 41
    fifthOctectIdentifier=MAC_ADDR_ENGN_ID;

    Snmpv3StackDcptStubPtr->SnmpEngineID[4]= fifthOctectIdentifier;

    if(fifthOctectIdentifier == MAC_ADDR_ENGN_ID)
    {
        for(i=0;i<6/*sizeof(TCPIP_MAC_ADDR)*/;i++)
        {
            Snmpv3StackDcptStubPtr->SnmpEngineID[5+i]=snmpIntf->netMACAddr.v[i];
        }

        Snmpv3StackDcptStubPtr->SnmpEngineID[5+6/*sizeof(TCPIP_MAC_ADDR)*/]='\0';
        Snmpv3StackDcptStubPtr->SnmpEngnIDLength=4/* 4 Bytes of IANA Pvt Enterprise Assigned Number*/
                                            +1/* 1 Byte for fifthOctectIdentifier*/
                                            +6/*sizeof(TCPIP_MAC_ADDR)*/;
    }
    else if(fifthOctectIdentifier == IPV4_ADDR_ENGN_ID)
    {
        Snmpv3StackDcptStubPtr->SnmpEngineID[5+4/*sizeof(IP_ADDR)*/]='\0';
        Snmpv3StackDcptStubPtr->SnmpEngnIDLength=4/* 4 Bytes of IANA Pvt Enterprise Assigned Number*/
                                            +1/* 1 Byte for fifthOctectIdentifier*/
                                            +4 /*sizeof(IP_ADDR)*/;
    }
    else if((fifthOctectIdentifier == ADMIN_ASSIGNED_TEXT )||(fifthOctectIdentifier == ADMIN_ASSIGNED_OCTETS))
    {
        //Interface API updates the  Snmpv3StackDcptStubPtr->SnmpEngineID[4] = fifthOctectIdentifier
        //and Snmpv3StackDcptStubPtr->SnmpEngineID[5] onwords with the corresponding octet string or value.
        ;
        //Snmpv3StackDcptStubPtr->SnmpEngnIDLength=strlen((const char*) Snmpv3StackDcptStubPtr->SnmpEngineID);
    }
		

    //Increment the SnmpEngineBoots record as Snmpv3StackDcptStubPtr->SnmpEngineID reconfigured
    Snmpv3StackDcptStubPtr->SnmpEngineBoots+=1;

    //Increment the snmpEngineBootRcrd to be stored in the non volatile memory
    snmpStkDcptMemStubPtr->snmpNetConfig.SnmpEngineBootRcrd=Snmpv3StackDcptStubPtr->SnmpEngineBoots;

    //Reset the snmEngineTime as SnmpEngineBoots incremented
    Snmpv3StackDcptStubPtr->SnmpEngineTime=0;

    Snmpv3StackDcptStubPtr->SnmpEngineTimeOffset=TCPIP_SNMPv3_AuthEngineTimeTickGet();
}

/****************************************************************************
  Function:
    void SNMPv3MaintainEngineBootsRcrd(void)
	
  Summary:
    Updates the snmp engine boots counter for the SNMPV3 engine.

  Description:
    Updates the global SnmpEngineBoots counter variable with the boots counter stored in the
    non volatile memory. Also increments and stores the new incremented value  to the non
    volatile memoryafter the SNMP engine initialization at the power cycle.
  	 		 		  	
  Precondition:
    InitAppConfig(); is called.

  Parameters:
    None
  	
  Return Values:
    None

  Remarks:
    Should be called only during tcp/ip stack initialization and only after the InitAppConfig();
    is called.
***************************************************************************/
void TCPIP_SNMPv3_EngineBootsRcrdUpdate(void)
{
    SNMP_PROCESSING_MEM_INFO_PTRS snmpPktProcsMemPtrsInfo;
    SNMP_STACK_DCPT_STUB * snmpStkDcptMemStubPtr=0;

    TCPIP_SNMP_PacketProcStubPtrsGet(&snmpPktProcsMemPtrsInfo);
    snmpStkDcptMemStubPtr=snmpPktProcsMemPtrsInfo.snmpStkDynMemStubPtr;

    //Increment the snmpEngineBootRcrd as due to power cycle snmp egine reinitialization happened
    //to be stored in the non volatile memory
    snmpStkDcptMemStubPtr->snmpNetConfig.SnmpEngineBootRcrd+=1;

    //Assgined this counter value to Global cntr which will track the snmp engine boot in real time.
    //'snmpEnigineBoots' can incremement in case of 'SnmpEngineTime' overflow or adminstrator configuring
    //'SnmpEngineID' with new 'fifthOctectIdentifier' through administrative interface.
    Snmpv3StackDcptStubPtr->SnmpEngineBoots=snmpStkDcptMemStubPtr->snmpNetConfig.SnmpEngineBootRcrd;

}

/****************************************************************************
  Function:
    void SNMPv3GetAuthEngineTime(void)
	
  Summary:
    Updates the snmp engine time variable 'SnmpEngineTime' for the SNMPV3 engine.
	
  Description:
    'SnmpEngineTime' is used for Timeliness checking for Message level security. Snmp
    engine keep updating the ''SnmpEngineTime' variable for checking the time window
    for the requrest and responses/inform etc. This routine also updates SnmpEngineBoots
    in scenarios of internal timer reset or 'SnmpEngineTime' cntr ovrflowed
    the (2^31 -1) value specified in RFC3411.
  	 		 		  	
  Precondition:
    TCPIP_SNMP_Initialize(); is called.

  Parameters:
    None
  	
  Return Values:
    None

  Remarks:
    This routine is called every time the rx/tx PDU processing is handled  by the SNMP agent.
    Updates the 'SnmpEngineTime' and requires frequnet access to internal timer registers.
***************************************************************************/
void TCPIP_SNMPv3_AuthEngineTimeGet(void)
{
    Snmpv3StackDcptStubPtr->SnmpEngineTime = TCPIP_SNMPv3_AuthEngineTimeTickGet()-Snmpv3StackDcptStubPtr->SnmpEngineTimeOffset;

    if((TCPIP_SNMPv3_AuthEngineTimeTickGet() < Snmpv3StackDcptStubPtr->SnmpEngineTimeOffset)/* Internal Timer Reset occured*/
	   ||(Snmpv3StackDcptStubPtr->SnmpEngineTime > 2147483647 /* (2^31 -1) Refer RFC 3411 Section 5 */))
    {
        /*This means the SnmpEngineTime cntr ovrflowed the (2^31 -1) value
            or the internal Tick timer Reset occured*/
        Snmpv3StackDcptStubPtr->SnmpEngineTime=0;
        Snmpv3StackDcptStubPtr->SnmpEngineTimeOffset=0;

        //Increment the SnmpEngineBoots counter
        TCPIP_SNMPv3_EngineBootsRcrdUpdate();
    }
}

/****************************************************************************
  Function:
    uint8_t SNMPv3NegotiateEngnMaxMsgSize(uint32_t maxMsgSizeInRequest)
	
  Summary:
    Snmp engine max pdu message size is updated for the pdu recipient from this
    Snmp agent.
	
  Description:
    This routine defines the maximum size PDU that could be generated or received
    from this SNMP agent. The maximum size is limited to the maximum pdu size the
    recipient can receive and process or originator can sent. For this SNMP engine,
    maximum message size is limited between 484 and 'SNMP_ENGINE_MAX_MSG_SIZE'.
    SNMP_ENGINE_MAX_MSG_SIZE is determined as the minimum of the maximum
    message size values supported among all of the transports available to and supported
    by the engine.
   	
   Precondition:
    The SNMP engine should have received a message from the other Snmp node
    notifying the maximum message size they can receive and process or send.
 

  Parameters:
    maxMsgSizeInRequest: incoming SNMPv3 request max msg size value
  	
  Return Values:
    true:  If the incoming message size is in the predefined range
    false: If the incoming maximum messgae size is less than 484 bytes

  Remarks:
    SNMP_ENGINE_MAX_MSG_SIZE should not be more than 0x80000000 (2^31 -1).
	
***************************************************************************/
uint8_t TCPIP_SNMPv3_EngnMaxMsgSizeNegotiate(uint32_t maxMsgSizeInRequest)
{

    Snmpv3StackDcptStubPtr->SnmpEngnMaxMsgSize=SNMP_ENGINE_MAX_MSG_SIZE;//"The maximum length in octets of an SNMP message ranges 484 to (2^31-1), send or receive and process.RFC3411

    if(maxMsgSizeInRequest > 0x80000000 || maxMsgSizeInRequest< 484)
    {
        return false;
    }
    else if(maxMsgSizeInRequest < SNMP_ENGINE_MAX_MSG_SIZE )
    {
        Snmpv3StackDcptStubPtr->SnmpEngnMaxMsgSize=maxMsgSizeInRequest;
        return true;
    }
    else
    {
        Snmpv3StackDcptStubPtr->SnmpEngnMaxMsgSize=SNMP_ENGINE_MAX_MSG_SIZE;
    }
    return true;
}

/****************************************************************************
  Function:
    bool SNMPv3CopyDataToProcessBuff(uint8_t val ,SNMPV3MSGDATA *putbuf)
	
  Summary:
    Copies uint8_t data to dynamically allocated memory buffer.

  Description:
    The SNMPv3 stack implementation uses dynamically allocated memory buffer for
    processing of request and response packets. This routine copies the uint8_t data to the
    allocated buffer and updates the offset length couter.
		  	 		 		  	
  Precondition:
    The SNMPv3 stack has sucessfully allocated dynamic memory buffer from the Heap
   	
  Parameters:
    val: uint8_t value to be written to the buffer
    putbuf: pointer to the dynamically allocated buffer to which the 'val' to be written
  	
  Return Values:
    true: if successfully write to the buffer
    false: failure in writing to the buffer
	
  Remarks:
    This routine is used by the SNMPv3 stack. If required to be used by the application
    code, valid pointers should be passed to this routine.
  	
***************************************************************************/
bool TCPIP_SNMPv3_DataCopyToProcessBuff(const uint8_t val ,SNMPV3MSGDATA *putbuf)
{
    if(putbuf->maxlength > putbuf->length)
    {
        putbuf->head[putbuf->length] = (uint8_t)val;
        putbuf->length++;
        return true;
    }
    else
    {
        return false;
    }
}

/****************************************************************************
  Function: 
    SNMP_ACTION SNMPv3MsgProcessingModelProcessPDU(uint8_t inOutPdu)

  Summary:
    This routine collects or populates the message processing model infomation
    from the received SNMPv3 request PDU or to the response PDU respectively.
  
  Description:
    The recievd SNMPv3 PDU or the transmit PDU header has message processing
    data bytes infomration. This routine retrievs the messgae processing model
    infomration from the stored pdu or write the appropriate msg proc info to the
    repsonse msg buffer.
		  	 		 		  	
  Precondition:
    Valid SNMPv3 request msg is received.
   	
  Parameters:
    inOutPdu: indicates whether the incomig PDU is to be read for msg proc values
    to be retrieved or the response PDU is to be populated with these values
  	
  Return Values:
    SNMP_NO_CREATION: Failure due to improper msg processing information format in
                      the received PDU or failure in constructing the response PDU.
    SNMP_NO_ERR: The message processing infomration retrieval or response PDU
               fomration is successful

  Remarks:
    The messgae processing model parameters like 'msgID', 'msgMaxSize', 'msgFlags' and
    'msgSecurityModel' decides the SNMPv3 engine processing modalities regarding
    request or response PDU
***************************************************************************/
SNMP_ERR_STATUS TCPIP_SNMPv3_MsgProcessingModelProcessPDU(INOUT_SNMP_PDU inOutPdu)
{
    TCPIP_UINT32_VAL tempLen={0};
    uint8_t tempData=0;
    uint8_t snmpv3MsgGlobalHeaderlength=0;


    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    if(inOutPdu == SNMP_REQUEST_PDU)
    {
        if (!TCPIP_SNMP_StructureIsValid((uint16_t*)&tempLen.Val))
            return SNMP_NO_CREATION;
	
        //Check and collect "msgID"
        if(!TCPIP_SNMP_VarDataTypeIsValidInteger(&tempLen.Val))
            return SNMP_NO_CREATION;
		

        Snmpv3StackDcptStubPtr->IncmngSnmpPduMsgID = tempLen.Val;

        //Check and collect "msgMaxSize"
        if(!TCPIP_SNMP_VarDataTypeIsValidInteger(&tempLen.Val))
            return SNMP_NO_CREATION;
		
        Snmpv3StackDcptStubPtr->incomingPdu.maxSizeResponseScopedPDU = tempLen.Val;

        if(TCPIP_SNMPv3_EngnMaxMsgSizeNegotiate(tempLen.Val)==false)
            return SNMP_NO_CREATION;

        //Check and collect "msgFlags"
        tempData = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
		
        if ( !IS_OCTET_STRING(tempData) )
                return false;

        tempData=TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);//Length byte of "msgFlags"
        tempData=TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);//"msgFlag"

        Snmpv3StackDcptStubPtr->SnmpSecurityLevel=tempData;
        Snmpv3StackDcptStubPtr->incomingPdu.securityLevel=Snmpv3StackDcptStubPtr->SnmpSecurityLevel;
        Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityLevel=Snmpv3StackDcptStubPtr->SnmpSecurityLevel;

        //Check and collect "msgSecurityModel"
        if(!TCPIP_SNMP_VarDataTypeIsValidInteger(&tempLen.Val))
            return SNMP_NO_CREATION;

        if(TCPIP_SNMPv3_EngnSecrtyModelConfig(tempLen.Val))
        {
            Snmpv3StackDcptStubPtr->incomingPdu.securityModel= tempLen.Val;
            Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityModel= (uint8_t)tempLen.Val;
        }
        else
            return SNMP_NO_CREATION;

    }
    else if(inOutPdu == SNMP_RESPONSE_PDU)
    {
        bool retBuf=true;
        if(Snmpv3StackDcptStubPtr->PduHeaderBuf.head == NULL)
        {
            return SNMP_NO_CREATION;
        }
		Snmpv3StackDcptStubPtr->PduHeaderBuf.length = 0;


        //message header
        TCPIP_SNMPv3_DataCopyToProcessBuff(STRUCTURE,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(MSGGLOBAL_HEADER_LEN(snmpv3MsgGlobalHeaderlength)-2,&Snmpv3StackDcptStubPtr->PduHeaderBuf);

        //Put "msgID" type ASN_INT of length 4 bytes
        TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(0x04,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_UINT32_VAL pduMsgId;
        pduMsgId.Val = Snmpv3StackDcptStubPtr->IncmngSnmpPduMsgID;

        TCPIP_SNMPv3_DataCopyToProcessBuff(pduMsgId.v[3],&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(pduMsgId.v[2],&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(pduMsgId.v[1],&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(pduMsgId.v[0],&Snmpv3StackDcptStubPtr->PduHeaderBuf);

        //Put "msgMaxSize"  type ASN_INT of length 4 bytes
        TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(0x04,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        
        TCPIP_UINT32_VAL maxMsgSize;
        maxMsgSize.Val = Snmpv3StackDcptStubPtr->SnmpEngnMaxMsgSize;
        TCPIP_SNMPv3_DataCopyToProcessBuff(maxMsgSize.v[3],&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(maxMsgSize.v[2],&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(maxMsgSize.v[1],&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(maxMsgSize.v[0],&Snmpv3StackDcptStubPtr->PduHeaderBuf);

        //Put "msgFlags"  type octet_string
        TCPIP_SNMPv3_DataCopyToProcessBuff(OCTET_STRING,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(0x01,&Snmpv3StackDcptStubPtr->PduHeaderBuf);

        if((Snmpv3StackDcptStubPtr->SnmpSecurityLevel & 0x03)==0x03) // Rxed pkt is authenticated and encrypted
        {
            Snmpv3StackDcptStubPtr->SnmpRespnsSecrtyFlg=0x03;
            TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpRespnsSecrtyFlg,&Snmpv3StackDcptStubPtr->PduHeaderBuf);//Rsponse "msgFlags" value as Reportable, Encrypted and Authenticated Bits are not set.
        }
        else if ((Snmpv3StackDcptStubPtr->SnmpSecurityLevel & 0x01)==0x01) // Rxed pkt is  authenticated and no priv
        {
            Snmpv3StackDcptStubPtr->SnmpRespnsSecrtyFlg=0x01;
            TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpRespnsSecrtyFlg,&Snmpv3StackDcptStubPtr->PduHeaderBuf);//Rsponse "msgFlags" value as Reportable, Encrypted and Authenticated Bits are not set.
        }
        else
        {
            Snmpv3StackDcptStubPtr->SnmpRespnsSecrtyFlg=0x00;
            TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpRespnsSecrtyFlg,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        }
        //Put "msgSecurityModel"
        TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(0x01,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        retBuf = TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpEngnSecurityModel,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        if(retBuf != true)
        {
            return SNMP_NO_CREATION;
        }
    }
    return SNMP_NO_ERR;
}

bool TCPIP_SNMPV3_EngineUserDataBaseSet(TCPIP_SNMPV3_USERDATABASECONFIG_TYPE userDataBaseType,uint8_t len,uint8_t userIndex,void *val)
{
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet (&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;

    if(userIndex == TCPIP_SNMPV3_USM_MAX_USER)
        return false;
    
    switch(userDataBaseType)
    {
        case SNMPV3_USERNAME_CONFIG_TYPE:
            if(val == 0)
                return false;
            if(strncmp((char*)val,"initial",len)== 0)
                return false;
            snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userNameLength = len;
            memset(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userName,'\0',TCPIP_SNMPV3_USER_SECURITY_NAME_LEN);
            strncpy((char*)snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userName,(char*)val,len);
            snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userNameLength = strlen((char*)snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userName);           
            break;
        case SNMPV3_AUTHPASSWD_CONFIG_TYPE:
            break;
        case SNMPV3_PRIVPASSWD_CONFIG_TYPE:
            break;
        case SNMPV3_AUTHPASSWDLOCALIZEDKEY_CONFIG_TYPE:
            if(len>TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN)
                return false;
            strncpy((char*)snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userAuthPswdLoclizdKey,(char*)val,len);
            SNMPv3ComputeHMACIpadOpadForAuthLoclzedKey(userIndex);
            break;
        case SNMPV3_PRIVPASSWWDLOCALIZEDKEY_CONFIG_TYPE:
            if(len>TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN)
                return false;
            strncpy((char*)snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userPrivPswdLoclizdKey,(char*)val,len);
            break;
        case SNMPV3_HASHTYPE_CONFIG_TYPE:
            if(*((USM_SECURITY_LEVEL*)val)== hmacMD5Auth)
            {
                snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userHashType = SNMPV3_HMAC_MD5;
            }
            else if(*((USM_SECURITY_LEVEL*)val) == hmacSHAAuth)
            {
                snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userHashType = SNMPV3_HMAC_SHA1;
            }
            else if(*((USM_SECURITY_LEVEL*)val) == noAuthProtocol)
            {
                snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userHashType = SNMPV3_NO_HMAC_AUTH;
            }
            else
                return false;

            SNMPv3USMAuthPrivPswdLocalization(userIndex);
            SNMPv3ComputeHMACIpadOpadForAuthLoclzedKey(userIndex);
            break;
        case SNMPV3_PRIVTYPE_CONFIG_TYPE:
            if((snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userHashType == SNMPV3_NO_HMAC_AUTH)
                && (*((USM_SECURITY_LEVEL*)val) != noPrivProtocol))
            {
                return false;
            }
            if(*((USM_SECURITY_LEVEL*)val) == aesPrivProtocol)
            {
                snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userPrivType = SNMPV3_AES_PRIV;
            }
            else if(*((USM_SECURITY_LEVEL*)val) == desPrivProtocol)
            {
                snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userPrivType = SNMPV3_DES_PRIV;
            }
            else if(*((USM_SECURITY_LEVEL*)val) == noPrivProtocol)
            {
                snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userPrivType = SNMPV3_NO_PRIV;
            }
            else
                return false;
            break;
        case SNMPV3_TARGET_SECURITY_LEVEL_TYPE:
            if(TCPIP_SNMPv3_CmprTrapSecNameAndSecLvlWithUSMDb(userIndex,strlen((char*)snmpv3EngnDcptMemoryStubPtr->Snmpv3TrapConfigData[userIndex].userSecurityName),
                snmpv3EngnDcptMemoryStubPtr->Snmpv3TrapConfigData[userIndex].userSecurityName,(*(STD_BASED_SNMPV3_SECURITY_LEVEL*)val))!= true)
                return false;
            snmpv3EngnDcptMemoryStubPtr->Snmpv3TrapConfigData[userIndex].securityModelType = (*(STD_BASED_SNMPV3_SECURITY_LEVEL*)val);
            break;
        case SNMPV3_TARGET_SECURITY_NAME_TYPE:
        {
            uint8_t index = 0;
            
            // restrict the user security name "initial"
            if(strncmp((char*)val,"initial",len)== 0)
                return false;
            // check if the target security name is the part of the user security name table,
            // if target security name is not present in that table then return false.

            for(index=0;index<TCPIP_SNMPV3_USM_MAX_USER;index++)
            {
                if(strncmp((char*)val,(char*)snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[index].userName,len)== 0)
                    break;
            }
            if(index == TCPIP_SNMPV3_USM_MAX_USER)
                return false;

            return true;
        }
        case SNMPV3_TARGET_SECURITY_MODEL_TYPE:
            snmpv3EngnDcptMemoryStubPtr->Snmpv3TrapConfigData[userIndex].securityModelType = *(STD_BASED_SNMP_SECURITY_MODEL*)val;
            break;
        case SNMPV3_TARGET_MP_MODEL_TYPE:
            snmpv3EngnDcptMemoryStubPtr->Snmpv3TrapConfigData[userIndex].messageProcessingModelType = *(STD_BASED_SNMP_MESSAGE_PROCESSING_MODEL*)val;
            break;
        default:
            return false;            
    }
    return true;
}

bool TCPIP_SNMPV3_EngineUserDataBaseGet(TCPIP_SNMPV3_USERDATABASECONFIG_TYPE userDataBaseType,uint8_t len,uint8_t userIndex,void *val)
{
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet (&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;

    if(userIndex == TCPIP_SNMPV3_USM_MAX_USER)
        return false;

    switch(userDataBaseType)
    {
        case SNMPV3_USERNAME_CONFIG_TYPE:
            if ( snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userNameLength == 0u )
            {
                return false;
            }
            if (len == snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userNameLength)
            {
                return false;
            }
            *(uint8_t*)val = snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userName[len];
            break;
        case SNMPV3_AUTHPASSWD_CONFIG_TYPE:
            break;
        case SNMPV3_PRIVPASSWD_CONFIG_TYPE:
            break;
        case SNMPV3_AUTHPASSWDLOCALIZEDKEY_CONFIG_TYPE:
            if(len>=TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN)
                return false;
            *(uint8_t*)val = snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userAuthPswdLoclizdKey[len];            
            break;
        case SNMPV3_PRIVPASSWWDLOCALIZEDKEY_CONFIG_TYPE:
            if(len>=TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN)
                return false;
            *(uint8_t*)val = snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userPrivPswdLoclizdKey[len];
            
            break;
        case SNMPV3_HASHTYPE_CONFIG_TYPE:
            if(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userHashType == SNMPV3_HMAC_MD5)
                *(uint8_t*)val = hmacMD5Auth;
            else if(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userHashType == SNMPV3_HMAC_SHA1)
                *(uint8_t*)val = hmacSHAAuth;
            else
                *(uint8_t*)val = noAuthProtocol;
            break;
        case SNMPV3_PRIVTYPE_CONFIG_TYPE:
           if(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userPrivType == SNMPV3_AES_PRIV)
                *(uint8_t*)val = aesPrivProtocol;
           else if(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userPrivType == SNMPV3_DES_PRIV)
                *(uint8_t*)val = desPrivProtocol;
            else
                *(uint8_t*)val = noPrivProtocol;
            break;
        case SNMPV3_TARGET_SECURITY_LEVEL_TYPE:
            *(uint8_t*)val = snmpv3EngnDcptMemoryStubPtr->Snmpv3TrapConfigData[userIndex].securityLevelType;
            break;
        case SNMPV3_TARGET_SECURITY_NAME_TYPE:
            if (strlen((char*)snmpv3EngnDcptMemoryStubPtr->Snmpv3TrapConfigData[userIndex].userSecurityName) == 0u)
                return false;
            if (len == strlen((char*)snmpv3EngnDcptMemoryStubPtr->Snmpv3TrapConfigData[userIndex].userSecurityName))
                return false;
            *(uint8_t*)val = snmpv3EngnDcptMemoryStubPtr->Snmpv3TrapConfigData[userIndex].userSecurityName[len];
            return true;
            break;
        case SNMPV3_TARGET_SECURITY_MODEL_TYPE:
            *(uint8_t*)val = snmpv3EngnDcptMemoryStubPtr->Snmpv3TrapConfigData[userIndex].securityModelType;            
            break;
        case SNMPV3_TARGET_MP_MODEL_TYPE:
            *(uint8_t*)val = snmpv3EngnDcptMemoryStubPtr->Snmpv3TrapConfigData[userIndex].messageProcessingModelType;
            break;
        case SNMPV3_ENGINE_ID_TYPE:
            if(snmpv3EngnDcptMemoryStubPtr->SnmpEngnIDLength == 0u)
            {
                return false;
            }
            if ( len == snmpv3EngnDcptMemoryStubPtr->SnmpEngnIDLength)
            {
                return false;
            }
            *(uint8_t*)val = snmpv3EngnDcptMemoryStubPtr->SnmpEngineID[len];
            break;
        case SNMPV3_ENGINE_BOOT_TYPE:
            *(uint8_t*)val = (uint32_t)snmpv3EngnDcptMemoryStubPtr->SnmpEngineBoots;
            break;
        case SNMPV3_ENGINE_TIME_TYPE:
            *(uint8_t*)val = (uint32_t)snmpv3EngnDcptMemoryStubPtr->SnmpEngineTime;
            break;
        case SNMPV3_ENGINE_MAX_MSG_TYPE:
            *(uint8_t*)val = (uint16_t)snmpv3EngnDcptMemoryStubPtr->SnmpEngnMaxMsgSize;
            break;
        default:
            return false;
    }
    return true;
}

/****************************************************************************
  Function: 
    SNMP_ERR_STATUS SNMPv3UserSecurityModelProcessPDU(uint8_t inOutPdu)

  Summary:
    This routine collects or populates the security model parametrs infomation
    from the received SNMPv3 request PDU or to the response PDU respectively.
  
  Description:
    The recievd SNMPv3 PDU or the transmit PDU header has message security
    data bytes infomration. This routine retrievs the messgae security parameters
    infomration from the stored incoming pdu or write the appropriate security
    model info to the repsonse msg buffer.
		  	 		 		  	
  Precondition:
    Valid SNMPv3 request msg is received.
   	
  Parameters:
    inOutPdu: indicates whether the incomig PDU is to be read for user security
    model to be retrieved or the response PDU to be populated with these values
  	
  Return Values:
    SNMP_NO_CREATION: Failure due to improper security model processing information
         format in the received PDU or failure in constructing the response PDU.
    SNMP_NO_ERR: The user security model retrieval or response PDU fomration is successful
				   
  Remarks:
    The user security parameter constitute the vital information for the message
    authentication and privacy of the message.
    The user security model parameters header structure
    MsgAuthEngnID+MsgAuthEngnBoots+MsgAuthEngnTime
    +MsgUserName+MsgAuthParam+MsgPrivParam
***************************************************************************/
SNMP_ERR_STATUS TCPIP_SNMPv3_UserSecurityModelProcessPDU(INOUT_SNMP_PDU inOutPdu)
{
    TCPIP_UINT32_VAL tempLen={0};
    uint8_t* ptr=NULL;
    uint8_t engnIdCntr=0;
    uint8_t tempData=0,putCntr=0;
    SNMPV3_PRIV_PROT_TYPE privType=SNMPV3_NO_PRIV;

    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    if(inOutPdu == SNMP_REQUEST_PDU)
    {
        // This is the part of the MEssage security parameters
        // 0x04 [len] 0x30 [len]  .....
        tempData = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
        if ( !IS_OCTET_STRING(tempData) )
            return SNMP_NO_CREATION;
		 
        //Msg security Parameter length
        tempLen.Val = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
			
         //Start collecting the security parameters from the incoming PDU.
         //Check if the security parametrs are binded in ASN structure format
        if (!TCPIP_SNMP_StructureIsValid((uint16_t*)&tempLen.Val))
            return SNMP_NO_CREATION;

        //Collect "msgAuthoritiveEngineID"
        tempData = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
        if ( !IS_OCTET_STRING(tempData) )
        return false;

        tempData = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);

        Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityEngineIDLen=tempData;
        memset(Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityEngineID,0,sizeof(Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityEngineID));
        if(tempData != 0x00)
        {
            TCPIP_SNMP_GetArrayOfDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData,tempData,Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityEngineID);
        }

		//Check and collect "msgAuthoritiveEngineBoots"	
        if(!TCPIP_SNMP_VarDataTypeIsValidInteger(&tempLen.Val))
            return SNMP_NO_CREATION;
        Snmpv3StackDcptStubPtr->AuthoritativeSnmpEngineBoots = tempLen.Val;

		//Check and collect "msgAuthoritiveEngineTime"	
        if(!TCPIP_SNMP_VarDataTypeIsValidInteger(&tempLen.Val))
            return SNMP_NO_CREATION;
        Snmpv3StackDcptStubPtr->AuthoritativeSnmpEngnTime = tempLen.Val;


		//Collect "msgUserName"	
        tempData = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
        if ( !IS_OCTET_STRING(tempData) )
        return false;
		 
        tempData = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
        Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityNameLength=tempData;
        memset(Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityName,0,sizeof(Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityName));
        if(tempData!= 0x00)
        {
            TCPIP_SNMP_GetArrayOfDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData,tempData,Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityName);
        }

        //validate user security name with security level
        if(!SNMPv3ValidateSecNameAndSecLevel())
            return SNMP_NO_CREATION;

        //Validate if the "msgAuthoritiveEngineID" matches to this agent's SNMP Engine ID
        if(!SNMPv3ValidateSnmpEngnId())
            return SNMP_NO_CREATION;

		//Check and collect "msgAuthenticationParameters"	
        tempData = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
        if ( !IS_OCTET_STRING(tempData) )
            return false;

 //(SnmpInMsgAuthParamLen should be 12 bytes if using HAMC-MD5-96 or HMAC-SHA-96)
        Snmpv3StackDcptStubPtr->SnmpInMsgAuthParamLen=tempData= TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);//TCPIP_SNMPv3_WholeMsgBufferDataGet(Snmpv3StackDcptStubPtr->InPduWholeMsgBuf.snmpMsgHead,&tempPos);

        if((Snmpv3StackDcptStubPtr->SnmpSecurityLevel&0x01)==0x01)//If message is authenticated
            if(Snmpv3StackDcptStubPtr->SnmpInMsgAuthParamLen !=12 /* if using HAMC-MD5-96 or HMAC-SHA-96 */)
                return SNMP_NO_CREATION;
		
        if(tempData != 0x00)
        {
            ptr=Snmpv3StackDcptStubPtr->SnmpInMsgAuthParmStrng;
            Snmpv3StackDcptStubPtr->InPduWholeMsgBuf.msgAuthParamOffsetInWholeMsg=(uint16_t)TCPIP_SNMP_GetRXOffset();
            TCPIP_SNMP_GetArrayOfDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData,tempData,ptr);
        }

		
//Check and collect "msgPrivacyParameters"	
        tempData= TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
        if ( !IS_OCTET_STRING(tempData) )
        return false;

//(SnmpInMsgPrivParmLen should be 8 bytes) 
        Snmpv3StackDcptStubPtr->SnmpInMsgPrivParmLen=tempData= TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);//TCPIP_SNMPv3_WholeMsgBufferDataGet(Snmpv3StackDcptStubPtr->InPduWholeMsgBuf.snmpMsgHead,&tempPos);
        if((Snmpv3StackDcptStubPtr->SnmpSecurityLevel&0x02)==0x02)//If message is encrypted
            if(Snmpv3StackDcptStubPtr->SnmpInMsgPrivParmLen !=8)
                return SNMP_NO_CREATION;
		
        if(tempData != 0x00)
        {
            ptr=Snmpv3StackDcptStubPtr->snmpInMsgPrvParamStrng;
            TCPIP_SNMP_GetArrayOfDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData,tempData,ptr);
            //This is a secured request. Compute the AES decryption IV
            SNMPv3UsmAesEncryptDecrptInitVector(SNMP_REQUEST_PDU);
        }

        /* global variable to find out how many times SNMPv3 engine id has been validated*/
        Snmpv3StackDcptStubPtr->UsmStatsEngineID++;
    }
    else if(inOutPdu == SNMP_RESPONSE_PDU)
    {
        uint16_t snmpv3MsgAuthHeaderLength=0;
        bool   retBuf=true;
        uint16_t msgHeaderOffset1=0;
        uint16_t msgHeaderOffset2=0;
        uint16_t tempMsgHeaderOffset=0;

        TCPIP_SNMPv3_DataCopyToProcessBuff(OCTET_STRING,&Snmpv3StackDcptStubPtr->PduHeaderBuf);  //Security Parameter string
        msgHeaderOffset1 = Snmpv3StackDcptStubPtr->PduHeaderBuf.length;
        TCPIP_SNMPv3_DataCopyToProcessBuff(MSG_AUTHORITATIVE_HEADER_LEN(snmpv3MsgAuthHeaderLength)-2,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(STRUCTURE,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        msgHeaderOffset2 = Snmpv3StackDcptStubPtr->PduHeaderBuf.length;
        TCPIP_SNMPv3_DataCopyToProcessBuff(MSG_AUTHORITATIVE_HEADER_LEN(snmpv3MsgAuthHeaderLength)-4,&Snmpv3StackDcptStubPtr->PduHeaderBuf);

                //Put "msgAuthoritiveEngineID"
        TCPIP_SNMPv3_DataCopyToProcessBuff(OCTET_STRING,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpEngnIDLength,&Snmpv3StackDcptStubPtr->PduHeaderBuf); //Integer Length
        tempData=Snmpv3StackDcptStubPtr->SnmpEngnIDLength;
        for(;engnIdCntr<tempData;engnIdCntr++)
        {
            TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpEngineID[engnIdCntr],&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        }

                //Put "msgAuthoritiveEngineBoots"
        TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(0x04,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpEngineBoots>>24,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpEngineBoots>>16,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpEngineBoots>>8,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpEngineBoots,&Snmpv3StackDcptStubPtr->PduHeaderBuf);

            //Put "msgAuthoritiveEngineTime"
        TCPIP_SNMPv3_AuthEngineTimeGet();
        TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(0x04,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_UINT32_VAL engTime;
        engTime.Val = Snmpv3StackDcptStubPtr->SnmpEngineTime;
        TCPIP_SNMPv3_DataCopyToProcessBuff(engTime.v[3],&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(engTime.v[2],&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(engTime.v[1],&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(engTime.v[0],&Snmpv3StackDcptStubPtr->PduHeaderBuf);


            //Put "msgUserName"
        TCPIP_SNMPv3_DataCopyToProcessBuff(OCTET_STRING,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityNameLength,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        tempData=Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityNameLength ;
        if(Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityNameLength != 0)
        {
            ptr= Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityName;
            for(putCntr=0;putCntr<tempData;putCntr++)
            {
                TCPIP_SNMPv3_DataCopyToProcessBuff(*(ptr+putCntr),&Snmpv3StackDcptStubPtr->PduHeaderBuf);
            }
        }

        putCntr = 0;

    //Put "msgAuthenticationParameters"
        TCPIP_SNMPv3_DataCopyToProcessBuff(OCTET_STRING,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        if((Snmpv3StackDcptStubPtr->SnmpSecurityLevel & 0x01) == 0x01)
        {
            TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpOutMsgAuthParmLen,&Snmpv3StackDcptStubPtr->PduHeaderBuf); //Not supported with the Alpha Release.

            Snmpv3StackDcptStubPtr->PduHeaderBuf.msgAuthParamOffset=Snmpv3StackDcptStubPtr->PduHeaderBuf.length;

            //Put 0x00 to msgAuthenticationParameters, Once the response WholeMsg is
            //populated, this offset can be updated with the new msgAuthenticationParam
            for(;putCntr<Snmpv3StackDcptStubPtr->SnmpOutMsgAuthParmLen;putCntr++)
            TCPIP_SNMPv3_DataCopyToProcessBuff(0x00,&Snmpv3StackDcptStubPtr->PduHeaderBuf);//RFC3414 Section 6.3.2 Page#56 Step3
        }
        else
        {
            TCPIP_SNMPv3_DataCopyToProcessBuff(0x00,&Snmpv3StackDcptStubPtr->PduHeaderBuf); 
        }
        putCntr = 0;
        privType = TCPIP_SNMPV3_PrivProtocolGet(Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityName);
        SNMPv3USMOutMsgPrivParam(privType);

            //Put "msgPrivacyParameters"
        TCPIP_SNMPv3_DataCopyToProcessBuff(OCTET_STRING,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        if((Snmpv3StackDcptStubPtr->SnmpSecurityLevel & 0x02) == 0x02)
        {
            TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpOutMsgPrivParmLen,&Snmpv3StackDcptStubPtr->PduHeaderBuf); 
            for(;putCntr<Snmpv3StackDcptStubPtr->SnmpOutMsgPrivParmLen;putCntr++)
            {
            	retBuf = TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpOutMsgPrvParmStrng[putCntr],&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        	}
        }
        else
        {
            TCPIP_SNMPv3_DataCopyToProcessBuff(0x00,&Snmpv3StackDcptStubPtr->PduHeaderBuf); 
        }
		
        if(retBuf != true)
        {
            return SNMP_NO_CREATION;
        }
        tempMsgHeaderOffset = Snmpv3StackDcptStubPtr->PduHeaderBuf.length;
        Snmpv3StackDcptStubPtr->PduHeaderBuf.length = msgHeaderOffset2;
        TCPIP_SNMPv3_DataCopyToProcessBuff((tempMsgHeaderOffset-msgHeaderOffset2)-1,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        Snmpv3StackDcptStubPtr->PduHeaderBuf.length = tempMsgHeaderOffset;

        tempMsgHeaderOffset = Snmpv3StackDcptStubPtr->PduHeaderBuf.length;
        Snmpv3StackDcptStubPtr->PduHeaderBuf.length = msgHeaderOffset1;
        TCPIP_SNMPv3_DataCopyToProcessBuff((tempMsgHeaderOffset-msgHeaderOffset1)-1,&Snmpv3StackDcptStubPtr->PduHeaderBuf);
        Snmpv3StackDcptStubPtr->PduHeaderBuf.length = tempMsgHeaderOffset;
    }
    return SNMP_NO_ERR;
}

/****************************************************************************
  Function: 
    SNMP_ERR_STATUS SNMPv3ScopedPduProcessing(uint8_t inOutPdu)

  Summary:
    This routine collects  the scoped pdu header information from the
    received SNMPv3 request PDU or populates to the response PDU respectively.
  
  Description:
    The recievd SNMPv3 PDU or the transmit PDU header has scoped pdu parameters
    like 'contextEngineID' 'context name' etc. This routine retrievs these parameters
    infomration from the stored incoming pdu or write the appropriate dynamically
    allocated memory for the transmit response PDU.
		  	 		 		  	
  Precondition:
    Valid SNMPv3 request msg is received.
   	
  Parameters:
    inOutPdu: indicates whether the incomig PDU is to be read for scoped pdu
    paraemters to be retrieved or the response PDU to be populated with these values
  	
  Return Values:
    SNMP_NO_CREATION: Failure due to improper scoped pdu information format in the
                         PDU or failure in constructing the response PDU.
    SNMP_NO_ERR: The scoped parameters retrieval or response PDU fomration
				   is successful
				   
  Remarks:
    The scoped pDu parameters
    msg data : - <contextEngineID><context name>[<data> == <pdutype><request id>
    <error status><error index><varbinds>
***************************************************************************/
SNMP_ERR_STATUS TCPIP_SNMPv3_ScopedPDUProcessing(INOUT_SNMP_PDU inOutPdu)
{
    TCPIP_UINT32_VAL tempLen={0};
    uint16_t dataLen=0;
    SNMPV3MSGDATA scopedPtr={NULL,0,0,0};

    uint16_t 	contextIDlen=0;
    uint16_t	contextNameLength=0;


    if(inOutPdu == SNMP_REQUEST_PDU)
    {
        if ( !_SNMPv3_CheckIfValidAuthStructure((uint16_t*)&tempLen) )
        {
            return SNMP_NO_CREATION;
        }
        Snmpv3StackDcptStubPtr->InPduWholeMsgBuf.scopedPduOffset=(uint16_t)TCPIP_SNMP_GetRXOffset();
    }
    else if(inOutPdu == SNMP_RESPONSE_PDU)
    {
        if(Snmpv3StackDcptStubPtr->ScopedPduRespnsBuf.head == NULL)
        {
            return SNMP_NO_CREATION;
        }
        Snmpv3StackDcptStubPtr->ScopedPduRespnsBuf.length = 0;
        Snmpv3StackDcptStubPtr->ScopedPduDataPos = 0;


    	//Start collecting the plaint text Scoped PDU data byte from the WholeMsg buffer
        //Check if the plain text scoped pdu data bytes are binded in ASN structure format
        scopedPtr = Snmpv3StackDcptStubPtr->ScopedPduRespnsBuf;
        TCPIP_SNMPv3_DataCopyToProcessBuff(STRUCTURE,&scopedPtr); // First item to Response buffer is packet structure
        TCPIP_SNMPv3_DataCopyToProcessBuff(0x82,&scopedPtr);
        TCPIP_SNMPv3_DataCopyToProcessBuff(0,&scopedPtr);
        TCPIP_SNMPv3_DataCopyToProcessBuff(0,&scopedPtr);

        if((Snmpv3StackDcptStubPtr->SnmpSecurityLevel & 0x02)==0x02)
        {
            contextIDlen=TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData) ;
            contextIDlen=TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData) ;
            if(contextIDlen == 0x81)
            {
                TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
            }
            else if(contextIDlen == 0x82)
            {
                TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
                TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
            }           
        }

        contextIDlen = _SNMP_V3ContextNameLengthGet(&dataLen);
        if(contextIDlen != true)
        {
            return SNMP_NO_CREATION;
        }
//        //Collect context engine id
        TCPIP_SNMPv3_DataCopyToProcessBuff(OCTET_STRING,&scopedPtr);
        if(dataLen == 0)
        {
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,&scopedPtr);
        }
        else
        {
			//copy context engine id from a local buffer			
            TCPIP_SNMPv3_DataCopyToProcessBuff(dataLen,&scopedPtr);
            while(dataLen!=0)
            {
                TCPIP_SNMPv3_DataCopyToProcessBuff(TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData),&scopedPtr);
                dataLen-=1;
            }
        }

		//Check and collect "contextName"
        contextNameLength = _SNMP_V3ContextNameLengthGet(&dataLen);
        if(contextNameLength != true)
        {
            return SNMP_NO_CREATION;
        }
        TCPIP_SNMPv3_DataCopyToProcessBuff(OCTET_STRING,&scopedPtr);
        if(dataLen == 0x00)
        {
            TCPIP_SNMPv3_DataCopyToProcessBuff(0x00,&scopedPtr);
        }
        else
        {
            TCPIP_SNMPv3_DataCopyToProcessBuff(dataLen,&scopedPtr);
            while(dataLen!=0x00)
            {
                TCPIP_SNMPv3_DataCopyToProcessBuff(TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData),&scopedPtr);
                dataLen-=1;
            }
        }

        Snmpv3StackDcptStubPtr->ScopedPduRespnsBuf.length = scopedPtr.length;
    }

    return SNMP_NO_ERR;
}

/****************************************************************************
  Function:
    bool SNMPv3ProcessV3MsgData(PDU_INFO* pduDbPtr)
	
  Summary:
    This routine processes the snmpv3 request and parallely creates the response pdu.
	
  Description:
    Once the received pdu is validated as Snmpv3 pdu, it is forwarded for 
    processing to this routine. This rotuine handles Get, Get_Next, Get_Bulk,
    Set request and creates appropriate response as Get_Response. 
    This routine will decide on whether the request pdu should be processed
    or be discarded. 
  	
  Precondition:
    The received udp packet is varified as valid SNMPv3 request.
		
  Parameters:
    pduDbPtr  - Pointer to received pdu information database
  	
  Return Values:
    true 	- If the snmp request processing is successful.
    false	- If the processing failed else the processing is not completed.
	
  Remarks:
    None
 ***************************************************************************/
bool TCPIP_SNMPv3_V3MsgDataProcess(PDU_INFO* pduDbPtr,uint8_t * headWrPtr)
{
    uint8_t                 Getbulk_N=0,Getbulk_M=0,Getbulk_R=0;/*Refer RFC 3416 Section "4.2.3" GetBulkRequest-PDU*/
    uint8_t                 OIDValue[TCPIP_SNMP_OID_MAX_LEN];
    uint8_t                 OIDlen=0;
    uint8_t                 oidLookUpRet=0;
    uint8_t                 noOfVarbindreq=0xFF;
    uint8_t                 tempBuf[4];
    tempBuf[0] = 0U;
    tempBuf[1] = 0U;
    tempBuf[2] = 0U;
    tempBuf[3] = 0U;
    uint8_t                 tempCntr=0,desPaddingCntr=0; // desPaddingCntr is used for DES padding.
    uint8_t*                tempPtr=NULL;
    uint8_t*                outBufPtr=NULL;
    uint8_t                 varIndex=0;
    uint8_t                 repeatCntr=0,varBindCntr=0;
    uint8_t                 succesor=0,tempRet=0xFF;
    uint16_t                pduLenOffset=0;
    uint16_t                pduLength=0;
    uint16_t                errorStatusOffset=0;
    uint16_t                errorIndexOffset=0;
    uint16_t                varBindHeaderLen=0;
    uint16_t                varBindHeaderOffset=0;
    uint16_t                varBindHeaderOffset_3=0;
    uint16_t                contextNameOffset=0;	
    uint16_t                varStructLenOffset=0;
    uint16_t                maxRepeatationOffset=0;
    uint16_t                tempOffset=0;						
    TCPIP_UINT32_VAL        tempVal={0};
    static SNMPV3MSGDATA	*dynScopedBufPtr=NULL;
    OID_INFO                OIDInfo;  
    memset(&OIDInfo, 0, sizeof(OID_INFO));
    SNMP_ERR_STATUS         errorStatus;
    bool                    bSnmpV3GetBulkResError = false;
    enum 
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
    }smSnmp=SM_PKT_STRUCT_LEN_OFFSET;

	
    SNMP_PROCESSING_MEM_INFO_PTRS snmpPktProcsMemPtrsInfo; 
    SNMP_STACK_DCPT_STUB * snmpStkDcptMemStubPtr=0;		

    TCPIP_SNMP_PacketProcStubPtrsGet(&snmpPktProcsMemPtrsInfo);
    snmpStkDcptMemStubPtr=snmpPktProcsMemPtrsInfo.snmpStkDynMemStubPtr;

    while(1)
    {
        switch(smSnmp)
        {
            // Before each variables are processed, prepare necessary header.
            case SM_PKT_STRUCT_LEN_OFFSET:
	
                if(TCPIP_SNMPv3_MsgProcessingModelProcessPDU(SNMP_RESPONSE_PDU)!= SNMP_NO_ERR)
                {
                    return false;
                }
                if(TCPIP_SNMPv3_UserSecurityModelProcessPDU(SNMP_RESPONSE_PDU)!= SNMP_NO_ERR)
                {
                    return false;
                }
                contextNameOffset = 0;
                if(TCPIP_SNMPv3_ScopedPDUProcessing(SNMP_RESPONSE_PDU)!= SNMP_NO_ERR)
                {
                    return false;
                }
				
                dynScopedBufPtr = &Snmpv3StackDcptStubPtr->ScopedPduRespnsBuf;					
                dynScopedBufPtr->head = Snmpv3StackDcptStubPtr->ScopedPduRespnsBuf.head;
                dynScopedBufPtr->length = Snmpv3StackDcptStubPtr->ScopedPduRespnsBuf.length;
				
                pduDbPtr->pduType = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
                pduLength = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
                if(pduLength == 0x81)
                {
                    pduLength = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
                }
                else if(pduLength == 0x82)
                {
                    pduLength = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
                    pduLength = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
                }					
			
                varBindHeaderOffset = dynScopedBufPtr->length;
                smSnmp++;
            case SM_RESPONSE_PDU_LEN_OFFSET:
                // The exact Reponse will be updated with varBindHeaderOffset
                // After reading no of varbinds from SNMPv3FindOIDsFrmIncmingV3Req
                if(!noOfVarbindreq)
                    TCPIP_SNMPv3_DataCopyToProcessBuff(REPORT_RESPONSE,dynScopedBufPtr);
                else
                    TCPIP_SNMPv3_DataCopyToProcessBuff(GET_RESPONSE,dynScopedBufPtr);
                // Since we don't know length of this response, use placeholders until
                pduLenOffset = dynScopedBufPtr->length;
                TCPIP_SNMPv3_DataCopyToProcessBuff(0x82,dynScopedBufPtr);
                TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynScopedBufPtr);
                TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynScopedBufPtr);
			
                if(TCPIP_SNMP_VarDataTypeIsValidInteger(&tempVal.Val) != true)
                {					
                    return false;
                }
                else
                {
                    // Put original request back.
                    TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,dynScopedBufPtr);   // Int type.
                    TCPIP_SNMPv3_DataCopyToProcessBuff(4,dynScopedBufPtr);     // To simplify logic, always use 4 byte long requestID
                    TCPIP_SNMPv3_DataCopyToProcessBuff(tempVal.v[3],dynScopedBufPtr); // Start MSB
                    TCPIP_SNMPv3_DataCopyToProcessBuff(tempVal.v[2],dynScopedBufPtr);
                    TCPIP_SNMPv3_DataCopyToProcessBuff(tempVal.v[1],dynScopedBufPtr);
                    TCPIP_SNMPv3_DataCopyToProcessBuff(tempVal.v[0],dynScopedBufPtr);
                }

                smSnmp++;
            case SM_ERROR_STATUS_OFFSET :
                /*update pduDbPtr structure for error index and eroor status 
                and non repeators and max repeators */
                if(pduDbPtr->pduType != GET_BULK_REQUEST)
                {
                    // ignore error index and error status but update pduDBptr
                    tempVal.Val = 0;
                    if(! TCPIP_SNMP_VarDataTypeIsValidInteger(&tempVal.Val))
                        return false;
                    pduDbPtr->errorStatus = tempVal.Val;
                    tempVal.Val = 0;
                    if(! TCPIP_SNMP_VarDataTypeIsValidInteger(&tempVal.Val))
                        return false;
                    pduDbPtr->erroIndex = tempVal.Val;
                }
                else
                {
                    // update max repeators and non repeators
                    tempVal.Val = 0;
                    if(TCPIP_SNMP_VarDataTypeIsValidInteger(&tempVal.Val)==true)
                    {
                       pduDbPtr->nonRepeators = tempVal.Val;
                    }
                    else
                    {						
                        return false;
                    }
                    tempVal.Val = 0;
                    if(TCPIP_SNMP_VarDataTypeIsValidInteger(&tempVal.Val)==true)
                    {
                       pduDbPtr->maxRepetitions = tempVal.Val;
                    }
                    else
                    {						
                        return false;
                    }
                }
				
                // Put error status.
                // Since we do not know error status, put place holder until we know it...
                TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,dynScopedBufPtr);               // Int type
                TCPIP_SNMPv3_DataCopyToProcessBuff(1,dynScopedBufPtr);                 // One byte long.
                errorStatusOffset = dynScopedBufPtr->length;
                TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynScopedBufPtr);                 // Placeholder.
                smSnmp++;
	
            case SM_ERROR_INDEX_OFFSET :
	
                // Similarly put error index.
                TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,dynScopedBufPtr);               // Int type
                TCPIP_SNMPv3_DataCopyToProcessBuff(1,dynScopedBufPtr);                 // One byte long
                errorIndexOffset = dynScopedBufPtr->length;
                TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynScopedBufPtr);                 // Placeholder.
	
                smSnmp++;
	
            case SM_FIND_NO_OF_REQUESTED_VARBINDS:

                // Decode variable binding structure
                if ( TCPIP_SNMP_StructureIsValid(&varBindHeaderLen) == false)
                {
                    noOfVarbindreq = 0;						
                }
                else	//Find number of OIDs/varbinds's data requested in received PDU.
                {
                    noOfVarbindreq = _SNMPv3_FindOIDsFrmIncmingV3Req(varBindHeaderLen);
                }
				
                tempOffset = dynScopedBufPtr->length;
                dynScopedBufPtr->length = varBindHeaderOffset;
                if(!noOfVarbindreq)
                {
                    TCPIP_SNMPv3_DataCopyToProcessBuff(REPORT_RESPONSE,dynScopedBufPtr);
                }
                else
                {
                    TCPIP_SNMPv3_DataCopyToProcessBuff(GET_RESPONSE,dynScopedBufPtr);
                }				
                dynScopedBufPtr->length = tempOffset;
								
                TCPIP_SNMPv3_DataCopyToProcessBuff(STRUCTURE,dynScopedBufPtr);
                varBindHeaderOffset_3 = dynScopedBufPtr->length;
                TCPIP_SNMPv3_DataCopyToProcessBuff(0x82,dynScopedBufPtr);
                TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynScopedBufPtr);
                TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynScopedBufPtr);
                if(noOfVarbindreq == 0)
                {
                    _SNMPv3_ConstructReportPDU(dynScopedBufPtr);
                    break;
                }
                smSnmp++;
	
            case SM_FIND_NO_OF_RESPONSE_VARBINDS:
                //Calculate number of variables to be responded for the received request
                Getbulk_N = noOfVarbindreq; Getbulk_M=0; Getbulk_R=0;
                if((pduDbPtr->snmpVersion == (uint8_t)SNMP_V3) &&
                        (pduDbPtr->pduType == GET_BULK_REQUEST))
                {
                    if((pduDbPtr->nonRepeators) <= noOfVarbindreq)
                    {
                        Getbulk_N = pduDbPtr->nonRepeators;
                    }
                    Getbulk_M = pduDbPtr->maxRepetitions;

                    if((noOfVarbindreq - Getbulk_N)>=0u)
                            Getbulk_R = noOfVarbindreq-Getbulk_N;
					
                    noOfVarbindreq = Getbulk_N + (Getbulk_M * Getbulk_R);//Refer RFC 3416

                    if(Getbulk_N == 0)
                    {
                        smSnmp=SM_MAX_REPETITIONS;
                        break;
                    }
                }
                //noOfVarbindreq = Getbulk_N + (Getbulk_M * Getbulk_R);//Refer RFC 3416

                smSnmp++;
            case SM_VARSTRUCT_LEN_OFFSET:
                if(noOfVarbindreq == 0)
                    break;
				
                if(Getbulk_N!= 0u) // decreament non repeators.
                    Getbulk_N--;
                else if(Getbulk_M > 0) // jump to max repeatations
                {
                    smSnmp = SM_MAX_REPETITIONS;
                    break;
                }
				
                varIndex++;
                if(TCPIP_SNMP_StructureIsValid(&varBindHeaderLen) ==  false)
                {
                    _SNMPv3_SetErrorStatus(errorStatusOffset,errorIndexOffset,SNMP_GEN_ERR,varIndex,dynScopedBufPtr);
                    break;
                }
                smSnmp++;
            case SM_POPULATE_REQ_OID:
                for(OIDlen=0;OIDlen<sizeof(OIDValue);OIDlen++)
                {
                    OIDValue[OIDlen]=0;
                }
                OIDlen=0;
                if(TCPIP_SNMP_OIDIsValid(OIDValue,&OIDlen) == false)
                {
                    _SNMPv3_SetErrorStatus(errorStatusOffset,errorIndexOffset,SNMP_GEN_ERR,varIndex,dynScopedBufPtr);
                    break;
                }
				
                // For Get & Get-Next, value must be NULL.
                if ( pduDbPtr->pduType != (uint8_t)SET_REQUEST )
                {
                    if(!TCPIP_SNMP_DataIsASNNull())
                    {
                        _SNMPv3_SetErrorStatus(errorStatusOffset,errorIndexOffset,SNMP_GEN_ERR,varIndex,dynScopedBufPtr);
                        break;
                    }
                }
                noOfVarbindreq--;
                smSnmp++;
			
            case SM_FIND_OID_IN_MIB:
			
                /* Search for the requested OID in the MIB database with the agent.*/
                //Searching the requested OID in the MIB database
                oidLookUpRet = TCPIP_SNMP_OIDFindInMgmtInfoBase(TCPIP_SNMP_FileDescrGet(),pduDbPtr,OIDValue, OIDlen, &OIDInfo);
                if(TCPIP_SNMPv3_DataCopyToProcessBuff(STRUCTURE,dynScopedBufPtr) != true)
                {    
                    bSnmpV3GetBulkResError = true;
                    break;
                }
                varStructLenOffset= dynScopedBufPtr->length;
                if(TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynScopedBufPtr)!= true)
                {    
                    bSnmpV3GetBulkResError = true;
                    break;
                }
		
                // ASN OID data type
                if(TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_OID,dynScopedBufPtr)!= true)
                {    
                    bSnmpV3GetBulkResError = true;
                    break;
                }
		
                /* send the error code for SNMPv3 version for GET request and SET - request.
                As the follwing code is only for the get and set response, so SNMPv3CopyDataToProcessBuff is not
                under the buffer over flow check.
                */
                if(oidLookUpRet != (uint8_t)true /*&& (pduDbPtr->pduType != GET_NEXT_REQUEST)*/ &&
                        (pduDbPtr->pduType != GET_BULK_REQUEST))
                {
                    if(snmpStkDcptMemStubPtr->appendZeroToOID)
                    {
                        TCPIP_SNMPv3_DataCopyToProcessBuff(OIDlen+1,dynScopedBufPtr);//for appending "0"
                    }
                    else
                    {
                        TCPIP_SNMPv3_DataCopyToProcessBuff(OIDlen,dynScopedBufPtr);//do not append "0"
                    }
                    pduLength = 0;
					//Put OID
                    while( OIDlen-- )
                    {
                        TCPIP_SNMPv3_DataCopyToProcessBuff(OIDValue[pduLength++],dynScopedBufPtr);//do not append "0"
                    }
					
                    if(snmpStkDcptMemStubPtr->appendZeroToOID)
                    {
                        TCPIP_SNMPv3_DataCopyToProcessBuff(0x00,dynScopedBufPtr);//Appending '0' to OID in response
                    }
                    if(( pduDbPtr->snmpVersion == (uint8_t)SNMP_V3)
                                    && (pduDbPtr->pduType == SNMP_GET))
                    {
                        TCPIP_SNMPv3_DataCopyToProcessBuff(oidLookUpRet,dynScopedBufPtr);//Appending '0' to OID in response
                        TCPIP_SNMPv3_DataCopyToProcessBuff(0x0,dynScopedBufPtr);//Appending '0' to OID in response
                    }
                    else if((pduDbPtr->pduType == SNMP_GET_NEXT) && (oidLookUpRet == SNMP_END_OF_MIB_VIEW))
                    {
                        TCPIP_SNMPv3_DataCopyToProcessBuff(SNMP_END_OF_MIB_VIEW,dynScopedBufPtr);
                        TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynScopedBufPtr);
                    }
                    tempOffset = dynScopedBufPtr->length;
                    pduLength = dynScopedBufPtr->length-(varStructLenOffset+1);
                    dynScopedBufPtr->length = varStructLenOffset;
                    TCPIP_SNMPv3_DataCopyToProcessBuff(pduLength,dynScopedBufPtr);
                    dynScopedBufPtr->length = tempOffset;
                    //Reset to state machine to access the next oid in request
                    smSnmp=SM_VARSTRUCT_LEN_OFFSET;
                    break;
                }
                smSnmp++;

            //return false;

            case SM_NON_REPETITIONS:

                /*	Variables in get,get_next,set and get_bulk ( non repetition variables)
                        of snmp request are processed in this part of the state machine.*/

                if(pduDbPtr->pduType == SNMP_SET)
                {
                    uint8_t templen=OIDlen;
                    uint8_t *ptroid=OIDValue;
                    //to validate the REC ID is present or not

                    if(TCPIP_SNMP_RecordIDValidation(pduDbPtr->snmpVersion,OIDInfo.nodeInfo.Flags.bIsIDPresent,OIDInfo.id,OIDValue,OIDlen) != true)
                    {
                        /*if(pduDbPtr->snmpVersion == (uint8_t)SNMP_V1)
                        *errorStatus = SNMP_NO_SUCH_NAME;
                        else if ((pduDbPtr->snmpVersion == (uint8_t)SNMP_V2C) ||
                                                        (pduDbPtr->snmpVersion == (uint8_t)SNMP_V3))*/

                         /*if the variable binding's name specifies a
                         * variable which does not exist and could not ever be
                         * created, then the value of the Response-PDU's error-
                         * status field is set to `noCreation', and the value of its
                         * error-index field is set to the index of the failed
                         * variable binding.
                         */
                        errorStatus = SNMP_NO_CREATION;
                        return false;
                    }
						
                    if(snmpStkDcptMemStubPtr->appendZeroToOID)
                        TCPIP_SNMPv3_DataCopyToProcessBuff(OIDlen+1,dynScopedBufPtr);//for appending "0"
                    else
                        TCPIP_SNMPv3_DataCopyToProcessBuff(OIDlen,dynScopedBufPtr);//do not append "0"
						
                    //Put OID
                    while( templen-- )
                        TCPIP_SNMPv3_DataCopyToProcessBuff(*ptroid++,dynScopedBufPtr);//do not append "0"
						
                    if(snmpStkDcptMemStubPtr->appendZeroToOID)
                    {
                        TCPIP_SNMPv3_DataCopyToProcessBuff(0x00,dynScopedBufPtr);//Appending '0' to OID in response
                    }
                    //Now process the SET command
                    tempRet = TCPIP_SNMP_ProcessSetVar(pduDbPtr,&OIDInfo, &errorStatus);
				
                    if ( errorStatus != SNMP_NO_ERR )
                    {
                        //SET var command failed. Update the error status.
                           _SNMPv3_SetErrorStatus(errorStatusOffset, \
                           errorIndexOffset, \
                           errorStatus, \
                           varIndex,dynScopedBufPtr); \
				
                    }
						
                }/*Get-Next-rquest also calls the TCPIP_SNMP_ProcessGetVar for 0the instance.  */
                else if((pduDbPtr->pduType == SNMP_GET) ||
                        ((pduDbPtr->pduType == SNMP_V2C_GET_BULK) && (oidLookUpRet == true)))
                {
                    uint8_t templen=OIDlen;
                    uint8_t *ptroid=OIDValue;

                        //to validate the REC ID is present or not
                        if(TCPIP_SNMP_RecordIDValidation(pduDbPtr->snmpVersion,OIDInfo.nodeInfo.Flags.bIsIDPresent,OIDInfo.id,OIDValue,OIDlen) != true)
                        {
                            return false;
                        }
                        if(snmpStkDcptMemStubPtr->appendZeroToOID)
                        {
                            if(TCPIP_SNMPv3_DataCopyToProcessBuff(OIDlen+1,dynScopedBufPtr)!= true) //for appending "0"
                            {    
                                bSnmpV3GetBulkResError = true;
                                break;
                            }
                        }
                        else
                        {
                            if(TCPIP_SNMPv3_DataCopyToProcessBuff(OIDlen,dynScopedBufPtr) != true)//do not append "0"
                            {    
                                bSnmpV3GetBulkResError = true;
                                break;
                            }
                        }
						
                        //Put OID
                        while( templen-- )
                        {
                            if(TCPIP_SNMPv3_DataCopyToProcessBuff(*ptroid++,dynScopedBufPtr)!= true)//do not append "0"
                            {    
                                bSnmpV3GetBulkResError = true;
                                break;
                            }
                        }

                        if(snmpStkDcptMemStubPtr->appendZeroToOID)
                        {
                            if(TCPIP_SNMPv3_DataCopyToProcessBuff(0x00,dynScopedBufPtr)!= true)//Appending '0' to OID in response
                            {    
                                bSnmpV3GetBulkResError = true;
                                break;
                            }
                        }

                        tempRet = TCPIP_SNMP_ProcessGetVar(&OIDInfo,false,pduDbPtr);
                    }
                    else if((pduDbPtr->pduType == SNMP_GET_NEXT)||
                            ((pduDbPtr->pduType == SNMP_V2C_GET_BULK) && (oidLookUpRet != true)))
                    {
                        tempRet = TCPIP_SNMP_ProcessGetNextVar(&OIDInfo,pduDbPtr);
                        if(tempRet ==0)
                        {
                            uint8_t templen=OIDlen;
                            uint8_t *ptroid=OIDValue;
							
                            if(snmpStkDcptMemStubPtr->appendZeroToOID)
                            {
                                if(TCPIP_SNMPv3_DataCopyToProcessBuff(OIDlen+1,dynScopedBufPtr) != true)//for appending "0"
                                {    
                                    bSnmpV3GetBulkResError = true;
                                    break;
                                }
                            }
                            else
                            {
                                if(TCPIP_SNMPv3_DataCopyToProcessBuff(OIDlen,dynScopedBufPtr)!= true)//do not append "0"
                                {    
                                    bSnmpV3GetBulkResError = true;
                                    break;
                                }
                            }
							
                            //Put OID
                            while( templen-- )
                            {
                                if(TCPIP_SNMPv3_DataCopyToProcessBuff(*ptroid++,dynScopedBufPtr)!= true)//do not append "0"
                                {    
                                    bSnmpV3GetBulkResError = true;
                                    break;
                                }
                            }
							
                            if(snmpStkDcptMemStubPtr->appendZeroToOID)
                            {
                                if(TCPIP_SNMPv3_DataCopyToProcessBuff(0x00,dynScopedBufPtr)!= true)//Appending '0' to OID in response
                                {    
                                    bSnmpV3GetBulkResError = true;
                                    break;
                                }
                            }
                        }
                    }
                    /*	If the request command processing is failed, update
                            the error status, index accordingly and response pdu.*/
                    if(tempRet == 0u &&(pduDbPtr->pduType != SNMP_SET))
                    {
                        if(dynScopedBufPtr->length >= dynScopedBufPtr->maxlength)
                        {    
                            bSnmpV3GetBulkResError = true;
                            break;
                        }
                        if(((pduDbPtr->pduType == SNMP_GET_NEXT)|| (pduDbPtr->pduType == SNMP_V2C_GET_BULK))&&pduDbPtr->snmpVersion == (uint8_t)SNMP_V3)
                        {
                            if(TCPIP_SNMPv3_DataCopyToProcessBuff(SNMP_END_OF_MIB_VIEW,dynScopedBufPtr)!= true)
                            {    
                                bSnmpV3GetBulkResError = true;
                                break;
                            }
                            if(TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynScopedBufPtr)!= true)
                            {    
                                bSnmpV3GetBulkResError = true;
                                break;
                            }
                            //if get bulk response reaches END of mIB view break from the loop.
                            noOfVarbindreq = 0;
                            Getbulk_N = 0u;
                        }
				
                    }
                    dynScopedBufPtr->length = Snmpv3StackDcptStubPtr->ScopedPduRespnsBuf.length;
                    tempOffset = dynScopedBufPtr->length;
                    pduLength = dynScopedBufPtr->length-(varStructLenOffset+1);
                    dynScopedBufPtr->length = varStructLenOffset;
                    TCPIP_SNMPv3_DataCopyToProcessBuff(pduLength,dynScopedBufPtr);
                    dynScopedBufPtr->length = tempOffset;

                    /* to avoid Dynamic out buffer crash   we need to calculate buffer
                    availability .Approximatly the next variable bind length should be less than 30.*/

                    if(dynScopedBufPtr->length>= dynScopedBufPtr->maxlength)
                    {
                        noOfVarbindreq = 0;
                        Getbulk_N = 0u;
                        break;
                    }
                    /*	Decide on the number of Non repetition variables remained to
                            be processed, decide the course of state machine.*/
                    if((pduDbPtr->pduType==GET_BULK_REQUEST) && (pduDbPtr->snmpVersion == (uint8_t)SNMP_V3)
                                                                            &&( Getbulk_N == 0u))
                    {
                        smSnmp=SM_MAX_REPETITIONS;
                    }
                    else
                    {
                        smSnmp=SM_VARSTRUCT_LEN_OFFSET;
                    }
				
                    snmpStkDcptMemStubPtr->getZeroInstance = false;
                    break;
                case SM_MAX_REPETITIONS:
                    maxRepeatationOffset = (uint32_t)TCPIP_SNMP_GetRXOffset();
                    /*Process each variable in request as Get_Next for
                      Getbulk_M (Max_repetition) times */
                    for(repeatCntr=0;repeatCntr<Getbulk_M;repeatCntr++)
                    {
                        TCPIP_SNMP_SetRXOffset((uint32_t)maxRepeatationOffset);

                        //Process every veriable in the request.
                        for(varBindCntr=0;varBindCntr<Getbulk_R;varBindCntr++)
                        {
                            if(noOfVarbindreq != 0)
                            {
                                noOfVarbindreq--;
                            }
                            else
                            {
                                break;
                            }
							
                            if(varBindCntr==0u)
                            {
                                varIndex=(noOfVarbindreq - Getbulk_R);
                            }
				
                            varIndex++;

                            if(TCPIP_SNMPv3_DataCopyToProcessBuff(STRUCTURE,dynScopedBufPtr)!= true)
                            {    
                                bSnmpV3GetBulkResError = true;
                                break;
                            }
                            varStructLenOffset= dynScopedBufPtr->length;
                            if(TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynScopedBufPtr)!= true)
                            {    
                                bSnmpV3GetBulkResError = true;
                                break;
                            }
                            succesor=repeatCntr;
				
                            // Decode variable length structure
                            if(TCPIP_SNMP_StructureIsValid(&varBindHeaderLen)==false)
                            {
                                _SNMPv3_SetErrorStatus(errorStatusOffset,errorIndexOffset,SNMP_GEN_ERR,varIndex,dynScopedBufPtr);
                                break;
                            }
                            // Decode next object
                            if(!TCPIP_SNMP_OIDIsValid(OIDValue, &OIDlen))
                            {
                                _SNMPv3_SetErrorStatus(errorStatusOffset,errorIndexOffset,SNMP_GEN_ERR,varIndex,dynScopedBufPtr);
                                break;
                            }
							
                            // For Get & Get-Next, value must be NULL.
                            if ( pduDbPtr->pduType != (uint8_t)SET_REQUEST )
                            {
                                if(!TCPIP_SNMP_DataIsASNNull())
                                    break;
                            }
				
                            oidLookUpRet = TCPIP_SNMP_OIDFindInMgmtInfoBase(TCPIP_SNMP_FileDescrGet(),pduDbPtr,OIDValue, OIDlen, &OIDInfo);
                            if(oidLookUpRet == SNMP_END_OF_MIB_VIEW)
                            {
                                tempRet = TCPIP_SNMP_NextLeafGet(TCPIP_SNMP_FileDescrGet(),&OIDInfo);
                            }
                            if(oidLookUpRet == false)
                            {
                                uint8_t templen=OIDlen;
                                uint8_t *ptroid=OIDValue;
								
                                if(TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_OID,dynScopedBufPtr)!= true)
                                {    
                                    bSnmpV3GetBulkResError = true;
                                    break;
                                }
                                if(snmpStkDcptMemStubPtr->appendZeroToOID)
                                {
                                    if(TCPIP_SNMPv3_DataCopyToProcessBuff(OIDlen+1,dynScopedBufPtr)!= true)
                                    {    
                                        bSnmpV3GetBulkResError = true;
                                        break;
                                    }
                                    OIDlen += 1;
                                }
                                else if(TCPIP_SNMPv3_DataCopyToProcessBuff(OIDlen,dynScopedBufPtr) != true)
                                {
                                    bSnmpV3GetBulkResError = true;
                                    break;
                                }
				
                                //Put OID
                                while( templen-- )
                                {
	                                if(TCPIP_SNMPv3_DataCopyToProcessBuff(*ptroid++,dynScopedBufPtr) != true)
	                                {
	                                    bSnmpV3GetBulkResError = true;
	                                    break;
	                                }
                                }
                                if(snmpStkDcptMemStubPtr->appendZeroToOID)
                                {
                                    if(TCPIP_SNMPv3_DataCopyToProcessBuff(0x0,dynScopedBufPtr) != true)
                                    {
                                        bSnmpV3GetBulkResError = true;
                                        break;
                                    }
                                }
                                if(TCPIP_SNMPv3_DataCopyToProcessBuff(SNMP_END_OF_MIB_VIEW,dynScopedBufPtr) != true)
                                {    
                                    bSnmpV3GetBulkResError = true;
                                    break;
                                }
                                if(TCPIP_SNMPv3_DataCopyToProcessBuff(0x0,dynScopedBufPtr)!= true)
                                {    
                                    bSnmpV3GetBulkResError = true;
                                    break;
                                }

                                noOfVarbindreq = 0;

                            }
                            else if(tempRet != 0)//if(oidLookUpRet != SNMP_END_OF_MIB_VIEW)
                            {
                                tempRet = TCPIP_SNMP_ProcessGetBulkVar(&OIDInfo, &OIDValue[0],&OIDlen,&succesor,pduDbPtr);
                            }
                            if ( tempRet == 0u )
                            {
                                uint8_t templen=OIDlen;
                                uint8_t *ptroid=OIDValue;
                                if(TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_OID,dynScopedBufPtr) != true)
                                {    
                                    bSnmpV3GetBulkResError = true;
                                    break;
                                }
                                if(snmpStkDcptMemStubPtr->appendZeroToOID)
                                {
                                    if(TCPIP_SNMPv3_DataCopyToProcessBuff(OIDlen+1,dynScopedBufPtr)!= true) //for appending "0"
                                    {    
                                        bSnmpV3GetBulkResError = true;
                                        break;
                                    }
                                    OIDlen += 1;
                                }
                                else if(TCPIP_SNMPv3_DataCopyToProcessBuff(OIDlen,dynScopedBufPtr)!=  true)
                                {
                                    bSnmpV3GetBulkResError = true;
                                    break;
                                }				
                                //Put OID
                                while( templen-- )
                                {
                                    if(TCPIP_SNMPv3_DataCopyToProcessBuff(*ptroid++,dynScopedBufPtr) != true)
                                    {    
                                        bSnmpV3GetBulkResError = true;
                                        break;
                                    }
                                }
				
                                /*Do send back the Same OID if get_next is EndOfMibView. Do not
                                  append zero to this OID*/
								
                                if(TCPIP_SNMPv3_DataCopyToProcessBuff(SNMP_END_OF_MIB_VIEW,dynScopedBufPtr) != true)
                                {    
                                    bSnmpV3GetBulkResError = true;
                                    break;
                                }
                                if(TCPIP_SNMPv3_DataCopyToProcessBuff(0x0,dynScopedBufPtr) != true)
                                {    
                                    bSnmpV3GetBulkResError = true;
                                    break;
                                }
				
                                if(snmpStkDcptMemStubPtr->appendZeroToOID)
                                {
                                    if(TCPIP_SNMPv3_DataCopyToProcessBuff(0x0,dynScopedBufPtr) != true)
                                    {    
                                        bSnmpV3GetBulkResError = true;
                                        break;
                                    }
                                }
                                noOfVarbindreq = 0;
								
                            }

                            dynScopedBufPtr = &Snmpv3StackDcptStubPtr->ScopedPduRespnsBuf;
                            tempOffset = dynScopedBufPtr->length;
                            pduLength = dynScopedBufPtr->length -(varStructLenOffset+1);
                            dynScopedBufPtr->length = varStructLenOffset;
                            TCPIP_SNMPv3_DataCopyToProcessBuff(pduLength,dynScopedBufPtr);
                            dynScopedBufPtr->length = tempOffset;

                            /* if length dynamic buffer length increases more than the allocated memory
                            then break from the loop.*/
                            if(dynScopedBufPtr->length>= dynScopedBufPtr->maxlength)
                            {
                                noOfVarbindreq = 0;
                                Getbulk_N = 0u;
                                bSnmpV3GetBulkResError = true;
                                break;
                            }
                            tempRet = 0xFF;
                        }//for(varBindCntr=0;varBindCntr<Getbulk_R;varBindCntr++)
                    }//for(repeatCntr=0;repeatCntr<Getbulk_M;repeatCntr++)
                    /* check length*/
                    break;

                default:
                    return false;
            }
            if(noOfVarbindreq == 0)
            {
                break;
            }
	}

	if(bSnmpV3GetBulkResError && (dynScopedBufPtr->length >= dynScopedBufPtr->maxlength))
	{
            if(pduDbPtr->pduType == SNMP_V2C_GET_BULK)
            {
                pduLength = dynScopedBufPtr->length - (varStructLenOffset-1);
                dynScopedBufPtr->length = dynScopedBufPtr->length - pduLength;
            }
            else
            {
                dynScopedBufPtr->length = varBindHeaderOffset_3;
                TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynScopedBufPtr);
                _SNMPv3_SetErrorStatus(errorStatusOffset,errorIndexOffset,SNMP_TOO_BIG,varIndex,dynScopedBufPtr);
            }
	}


    /* pass the data to wire*/
    {
        uint16_t            scopedpduHeaderOffset = 0;
        TCPIP_UINT16_VAL 	totalPdulength = {0};
        TCPIP_UINT16_VAL	scoped_pdu_len_1 = {0}; // context data length
        TCPIP_UINT16_VAL	scoped_pdu_len_2 = {0}; // PDU response length
        TCPIP_UINT16_VAL	scoped_pdu_len_3 = {0}; //variable binding header with varbinds
        uint16_t 		i=0;
        SNMPV3MSGDATA tempScopedData;
        SNMPV3_PRIV_PROT_TYPE   configPrivType;

        scopedpduHeaderOffset = dynScopedBufPtr->length;
        tempScopedData = Snmpv3StackDcptStubPtr->ScopedPduRespnsBuf;

        // update length for variable binds
        scoped_pdu_len_3.Val = (dynScopedBufPtr->length-3)-varBindHeaderOffset_3;
        dynScopedBufPtr->length = varBindHeaderOffset_3+1;
        TCPIP_SNMPv3_DataCopyToProcessBuff(scoped_pdu_len_3.v[1],dynScopedBufPtr);
        TCPIP_SNMPv3_DataCopyToProcessBuff(scoped_pdu_len_3.v[0],dynScopedBufPtr);
        dynScopedBufPtr->length = scopedpduHeaderOffset;

        //update the length for get response pduLenOffset
        scoped_pdu_len_2.Val = (dynScopedBufPtr->length-3)-pduLenOffset;
        dynScopedBufPtr->length = pduLenOffset+1;
        TCPIP_SNMPv3_DataCopyToProcessBuff(scoped_pdu_len_2.v[1],dynScopedBufPtr);
        TCPIP_SNMPv3_DataCopyToProcessBuff(scoped_pdu_len_2.v[0],dynScopedBufPtr);
        dynScopedBufPtr->length = scopedpduHeaderOffset;

        scoped_pdu_len_1.Val = dynScopedBufPtr->length-4;
        if((scoped_pdu_len_1.Val >= 0x80) && (scoped_pdu_len_1.Val <= 0xFF))
        {// total scoped pdu length decreamented by 1
            dynScopedBufPtr->length = contextNameOffset+1;
            TCPIP_SNMPv3_DataCopyToProcessBuff(0x30,dynScopedBufPtr);
            TCPIP_SNMPv3_DataCopyToProcessBuff(0x81,dynScopedBufPtr);
            TCPIP_SNMPv3_DataCopyToProcessBuff(scoped_pdu_len_1.Val,dynScopedBufPtr);
            dynScopedBufPtr->length = scopedpduHeaderOffset;
            tempScopedData.head++;
            tempScopedData.length--;
        }
        else if((scoped_pdu_len_1.Val > 0xFF) && (scoped_pdu_len_1.Val < 0xFFFF))
        {
            dynScopedBufPtr->length = contextNameOffset;
            TCPIP_SNMPv3_DataCopyToProcessBuff(0x30,dynScopedBufPtr);
            TCPIP_SNMPv3_DataCopyToProcessBuff(0x82,dynScopedBufPtr);
            TCPIP_SNMPv3_DataCopyToProcessBuff(scoped_pdu_len_1.v[1],dynScopedBufPtr);
            TCPIP_SNMPv3_DataCopyToProcessBuff(scoped_pdu_len_1.v[0],dynScopedBufPtr);
            dynScopedBufPtr->length = scopedpduHeaderOffset;
        }
        else
        {// total scoped PDU length decremented by 2
            dynScopedBufPtr->length = contextNameOffset+2;
            TCPIP_SNMPv3_DataCopyToProcessBuff(0x30,dynScopedBufPtr);
            TCPIP_SNMPv3_DataCopyToProcessBuff(scoped_pdu_len_1.Val,dynScopedBufPtr);
            dynScopedBufPtr->length = scopedpduHeaderOffset;
            tempScopedData.head=tempScopedData.head+2;
            tempScopedData.length=tempScopedData.length-2;
        }

        configPrivType = TCPIP_SNMPV3_PrivProtocolGet(Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityName);
        //DES padding
        if(configPrivType == SNMPV3_DES_PRIV)
        {
            if((desPaddingCntr=tempScopedData.length %8)!=0)
            {
                desPaddingCntr = 8-desPaddingCntr;
                tempScopedData.length += desPaddingCntr;
            }
        }

        totalPdulength.Val = tempScopedData.length + \
                     Snmpv3StackDcptStubPtr->PduHeaderBuf.length + \
                     3; // asn_int+len+version

        if((Snmpv3StackDcptStubPtr->SnmpSecurityLevel & 0x02)==0x02)
        {
            tempPtr=tempBuf;
            *tempPtr++=0X04;
            if((tempScopedData.length >= 0x80) && (tempScopedData.length <= 0xFF))
            {
                *tempPtr++=0x81;
                *tempPtr=tempScopedData.length;
                tempCntr=3; //0x04(encrypted pkt),0x81,len
            }
            else if((tempScopedData.length > 0xFF) && (tempScopedData.length < 0xFFFF))
            {
                *tempPtr++=0x82;
                *tempPtr++=tempScopedData.length>>8;
                *tempPtr=tempScopedData.length;
                tempCntr=4; //0x04(encrypted pkt),0x81,len_1,len_0
            }
            else
            {
                *tempPtr=tempScopedData.length;
                tempCntr=2; //0x04(encrypted pkt),len
            }
        }

          
        Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.wholeMsgHead= 0x00;
        Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.wholeMsgLen = 0x00;
        Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.snmpMsgHead = NULL;
        Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.msgAuthParamOffsetOutWholeMsg = NULL;
        Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.scopedPduOffset =  NULL;

        if(configPrivType == SNMPV3_DES_PRIV)
        {
            Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.wholeMsgLen=(totalPdulength.Val+tempCntr/*0x04,0x82,len_1,len_0*/ + desPaddingCntr);
        }
        else
        {
            Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.wholeMsgLen = totalPdulength.Val+tempCntr/*0x04,0x82,len_1,len_0*/;
        }
        Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.wholeMsgHead=headWrPtr;
        if(Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.wholeMsgHead == NULL)
        {
           return false;
        }
        outBufPtr=Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.wholeMsgHead;

        //Start Writing to the outPut Buffer
        *outBufPtr++=STRUCTURE;

        totalPdulength.Val+=tempCntr;

        if((totalPdulength.Val >= 0x80) && (totalPdulength.Val <= 0xFF))
        {
            *outBufPtr++=0x81;
            *outBufPtr++=totalPdulength.Val;
        }
        else if((totalPdulength.Val > 0xFF) && (totalPdulength.Val < 0xFFFF))
        {
            *outBufPtr++=0x82;
            *outBufPtr++=totalPdulength.v[1];
            *outBufPtr++=totalPdulength.v[0];
        }
        else
        {
            *outBufPtr++=totalPdulength.Val;
        }

        *outBufPtr++=ASN_INT;
        *outBufPtr++=0x1;
        *outBufPtr++=SNMP_V3;

        Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.msgAuthParamOffsetOutWholeMsg=(uint8_t*)(outBufPtr+Snmpv3StackDcptStubPtr->PduHeaderBuf.msgAuthParamOffset);
        //put global snmpv3 msg header
        for(i=0;i<Snmpv3StackDcptStubPtr->PduHeaderBuf.length;i++)
        {
            *outBufPtr++=Snmpv3StackDcptStubPtr->PduHeaderBuf.head[i];
        }

        //Copy Scoped PDU to the Out Buffer
        if((Snmpv3StackDcptStubPtr->SnmpSecurityLevel & 0x02)==0x02) //Encrypted message
        {
            //Copy Packet Auth indicator, length
            for(i=0;i<tempCntr;i++)
            {
                *outBufPtr++=tempBuf[i];
            }
        }
        Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.scopedPduOffset=outBufPtr;
        if(configPrivType == SNMPV3_DES_PRIV)
        {
            Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.scopedPduStructLen=tempScopedData.length+desPaddingCntr;
        }
        else
        {
        	Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.scopedPduStructLen=tempScopedData.length;
        }
        i=0;
        *outBufPtr++=tempScopedData.head[i++];//0x30

        if(tempScopedData.head[1] == 0x81)
        {
            *outBufPtr++=tempScopedData.head[i++];//0x81
            *outBufPtr++=tempScopedData.head[i++];//len_0
        }
        else if(tempScopedData.head[1] == 0x82)
        {
            *outBufPtr++=tempScopedData.head[i++]; //0x82
            *outBufPtr++=tempScopedData.head[i++]; //len_1
            *outBufPtr++=tempScopedData.head[i++]; //len_0
        }
        else
		{
            *outBufPtr++=tempScopedData.head[i++];//len_0
		}

        // send context id and context name and the get response
        // Authentication and privacy data packet will be sent from here onwards
        for(;i<(tempScopedData.length-desPaddingCntr);i++)
        {
            *outBufPtr++=tempScopedData.head[i];
        }

        /*Encrypt the Response to the messgae originator*/
        if((Snmpv3StackDcptStubPtr->SnmpSecurityLevel & NO_REPORT_PRIVACY_AND_AUTH_PROVIDED)
			==NO_REPORT_PRIVACY_AND_AUTH_PROVIDED) //Encrypted message
        {
             /*Rxed SNMPv3 message is encrypted. Hence Response should be encrypted*/
             Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.wholeMsgLen = outBufPtr-Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.wholeMsgHead;

             /*If user privacy protocol is AES*/
             if(configPrivType == SNMPV3_AES_PRIV)  //user privacy protocol is AES
             {
                if(SNMPv3AESEncryptResponseScopedPdu(&Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf) != SNMPV3_MSG_PRIV_PASS)
                {
                   return SNMPV3_MSG_PRIV_FAIL;
                }
             }
             else if(configPrivType == SNMPV3_DES_PRIV)  //user privacy protocol is DES
             {
                //DES PADDING
                for(i=0;i<desPaddingCntr;i++)
                {
                    *outBufPtr++=0x00;
                }
                if(configPrivType == SNMPV3_DES_PRIV)
                {
                    if(SNMPv3DESEncryptResponseScopedPdu(&Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf) != SNMPV3_MSG_PRIV_PASS)
                    {
                        return SNMPV3_MSG_PRIV_FAIL;
                    }
                }               
            }
            /*If user privacy Protocol is DES*/
            //snmpV3DESDecryptRxedScopedPdu();
        }

        /* Authenticate the whole message to be transmitted*/
        if((Snmpv3StackDcptStubPtr->SnmpSecurityLevel & NO_REPORT_NO_PRIVACY_BUT_AUTH_PROVIDED)
			==NO_REPORT_NO_PRIVACY_BUT_AUTH_PROVIDED) //Authenticated message
        {
             /*Rxed SNMPv3 message is Authenticated.Send authentication parameters for the Response*/
            Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.wholeMsgLen = outBufPtr-Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.wholeMsgHead;
             /*If user authentication is HAMC-MD5-96*/
            if(SNMPv3AuthenticateTxPduForDataIntegrity(&Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf)!=SNMPV3_MSG_AUTH_PASS)
            {
                return SNMPV3_MSG_AUTH_FAIL;
            }

            tempPtr = outBufPtr;
            outBufPtr=Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.msgAuthParamOffsetOutWholeMsg;
            for(i=0;i<Snmpv3StackDcptStubPtr->SnmpOutMsgAuthParmLen;i++)
            {
                *outBufPtr++=Snmpv3StackDcptStubPtr->SnmpOutMsgAuthParaStrng[i];
            }
            outBufPtr = tempPtr;
        }

        Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.wholeMsgLen = outBufPtr-Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.wholeMsgHead;

        snmpStkDcptMemStubPtr->outPduBufData.length  = Snmpv3StackDcptStubPtr->OUTPduWholeMsgBuf.wholeMsgLen;

    }
    return true;
}


static bool _SNMP_V3ContextNameLengthGet(uint16_t *len)
{
    TCPIP_UINT32_VAL tempLen;
    uint8_t retLen = 0;

    *len =0;

    if (!IS_OCTET_STRING(TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData)))
        return false;

    // Retrieve structure length
    retLen= TCPIP_SNMP_LengthIsValid((uint16_t *)&tempLen.Val);
    if (!retLen)
        return false;
    *len = tempLen.Val;
    return true;
}



/****************************************************************************
Function:
    static uint8_t SNMPv3FindOIDsFrmIncmingV3Req(uint16_t pdulen)

Summary:
    Finds number of varbinds in the varbind list received in a SNMPv3 pdu.

Description:
    This routine is used to find the number of OIDs requested in the received
    snmp pdu.

Precondition	:
    TCPIP_SNMP_ProcessVariables() is called.

Parameters:
    pdulen		-	Length of snmp pdu request received.

Return Values:
    varCount	-	Number of OIDs found in a pdu request.

Remarks:
    None.
  	
***************************************************************************/
static uint8_t _SNMPv3_FindOIDsFrmIncmingV3Req(uint16_t pdulen)
{
    uint8_t varCount=0;
    uint16_t prevUDPRxOffset;
    uint16_t varBindLen;
    uint16_t snmpPduLen;
    uint32_t   tempPos=0;
 	
    snmpPduLen=pdulen;

    prevUDPRxOffset=(uint16_t)TCPIP_SNMP_GetRXOffset();
    while(snmpPduLen)
    {
        if(!TCPIP_SNMP_StructureIsValid(&varBindLen))
            return false;

        varCount++;
        snmpPduLen=snmpPduLen
                                -1      //  1   byte for STRUCTURE identifier
                                -1  //  1  byte for varbind length
                                -varBindLen;
        tempPos = TCPIP_SNMP_GetRXOffset()+varBindLen;
        TCPIP_SNMP_SetRXOffset((uint32_t)tempPos);
    }

    TCPIP_SNMP_SetRXOffset((uint32_t)prevUDPRxOffset);

    return varCount;
}

/****************************************************************************
Function:
    static void SNMPv3ConstructReportPdu(SNMPV3MSGDATA *dynScopedBufPtr)

Summary:
    Constructs the report pdu infomration for the Report Pdu.

Description:
    The SNMPv3 PDU exchange starts with the agent sending a report pdu on
    reception of any Get_Request PDU for SNMPv3 request.
    This routine froms the report pdu for response to the requesting entity.

Precondition	:
    TCPIP_SNMP_ProcessVariables() is called and a valid SNMPv3 request is received.

Parameters:
    dynScopedBufPtr	- pointer to the response buffer memory where the 'report' response
                                       to be savced for transmission.

Return Values:
    None

Remarks:
    None.
  	
***************************************************************************/
static void _SNMPv3_ConstructReportPDU(SNMPV3MSGDATA *dynScopedBufPtr)
{
    static const uint8_t	usmStatEngineIds[]={43,6,1,6,3,15,1,1,4,0};
    uint8_t	reportPduLenOffset=0;
    uint8_t	usmLen=0,i=0;
    uint16_t	varbindPairOffset1=0;
	

    TCPIP_SNMPv3_DataCopyToProcessBuff(STRUCTURE,dynScopedBufPtr);
    varbindPairOffset1 = dynScopedBufPtr->length;
    TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynScopedBufPtr);

/* put  usm OID */
    TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_OID,dynScopedBufPtr);
    usmLen = sizeof(usmStatEngineIds);
    TCPIP_SNMPv3_DataCopyToProcessBuff(usmLen,dynScopedBufPtr);
    while(usmLen--)
        TCPIP_SNMPv3_DataCopyToProcessBuff(usmStatEngineIds[i++],dynScopedBufPtr);

/* put engine ID stat value */	
    TCPIP_SNMPv3_DataCopyToProcessBuff(SNMP_COUNTER32,dynScopedBufPtr);
    TCPIP_SNMPv3_DataCopyToProcessBuff(4,dynScopedBufPtr);
    TCPIP_UINT32_VAL engId;
    engId.Val = Snmpv3StackDcptStubPtr->UsmStatsEngineID;
    TCPIP_SNMPv3_DataCopyToProcessBuff(engId.v[3],dynScopedBufPtr);
    TCPIP_SNMPv3_DataCopyToProcessBuff(engId.v[2],dynScopedBufPtr);
    TCPIP_SNMPv3_DataCopyToProcessBuff(engId.v[1],dynScopedBufPtr);
    TCPIP_SNMPv3_DataCopyToProcessBuff(engId.v[0],dynScopedBufPtr);
	
    reportPduLenOffset = dynScopedBufPtr->length;

    usmLen = dynScopedBufPtr->length - (varbindPairOffset1+1) ;
    dynScopedBufPtr->length = varbindPairOffset1;
    TCPIP_SNMPv3_DataCopyToProcessBuff(usmLen,dynScopedBufPtr);
	
    dynScopedBufPtr->length = reportPduLenOffset;
}

/****************************************************************************
Function:
    static void SNMPv3SetErrorStatus(uint16_t errorStatusOffset,
                       uint16_t errorIndexOffset,
                       SNMP_ERR_STATUS errorStatus,
                       uint8_t errorIndex,SNMPV3MSGDATA *dynScopedPduPutBuf)
Summary:
    Set snmpv3 error status in the response pdu.

Description:
    This routine processes the received snmp Get request pdu for the
    variable binding in the request and also creates the response pdu.

Precondition:
    TCPIP_SNMP_ProcessVariables() is called.

Parameters:
    errorStatusOffset - Offset to update error status in Response Tx pdu
    errorIndexOffset  - Offset to update error index
    errorStatus		  - Snmp process error to be updated in response.
    errorIndex		  - Index of the request varbind in the var bind list
                                        for which error status is to be updated.
    dynScopedPduPutBuf -  dynamic snmpv3 scoped pdu buffer
Returns:
    None.

Remarks:
    None.
***************************************************************************/
static void _SNMPv3_SetErrorStatus(uint16_t errorStatusOffset,
                           uint16_t errorIndexOffset,
                           SNMP_ERR_STATUS errorStatus,
                           uint8_t errorIndex,SNMPV3MSGDATA *dynScopedPduPutBuf)
{
    uint16_t prevOffset;

    prevOffset = dynScopedPduPutBuf->length;
    dynScopedPduPutBuf->length = errorStatusOffset;
    TCPIP_SNMPv3_DataCopyToProcessBuff(errorStatus,dynScopedPduPutBuf);

    
    dynScopedPduPutBuf->length = errorIndexOffset;
    TCPIP_SNMPv3_DataCopyToProcessBuff(errorIndex,dynScopedPduPutBuf);

    dynScopedPduPutBuf->length = prevOffset;
}

/****************************************************************************
Function:
    static uint8_t SNMPv3CheckIfValidAuthStructure(uint16_t* dataLen)

Summary:
    Decode variable length structure.

Description:
    This routine is used  to verify whether the received varbind is of type
    STRUCTURE and to find out the variable binding structure length.
    This rotuine only refers to the incoming snmpv3 request dynamically
    allocated   memory buffer 'InPduWholeMsgBuf' .

Precondition:
    TCPIP_SNMP_ProcessPDUHeader() is called.

Parameters:
    datalen	- Pointer to memory to store OID structure length.

Return Values:
    headrbytes	- Variable binding length.
    false		- If variable data structure is not type STRUCTURE.

Remarks:
    None.
***************************************************************************/
static uint8_t _SNMPv3_CheckIfValidAuthStructure(uint16_t* dataLen)
{
    TCPIP_UINT16_VAL tempLen;
    uint8_t headerBytes;
    uint8_t authStructure=0;
    uint8_t tempData;
	
    authStructure = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
	

    if ( !IS_STRUCTURE(authStructure) && !IS_SNMPV3_AUTH_STRUCTURE(authStructure) )
        return false;

	
    // Initialize length value.
    tempLen.Val = 0;
    headerBytes = 0;

    tempData= TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
    tempLen.v[0] = tempData;
    if ( tempData & 0x80 )
    {
        tempData &= 0x7F;

        // We do not support any length byte count of more than 2
        // i.e. total length value must not be more than 16-bit.
        if ( tempData > 2u )
            return false;

        // Total length bytes are 0x80 itself plus tempData.
        headerBytes = tempData + 1;

        // Get upto 2 bytes of length value.
        while( tempData-- )
        {
         //   tempLen.v[tempData] = TCPIP_SNMPv3_WholeMsgBufferDataGet(Snmpv3StackDcptStubPtr->InPduWholeMsgBuf.snmpMsgHead,&tempPos);
            tempLen.v[tempData] = TCPIP_SNMP_GetDataFromUDPBuff(&gSnmpDcpt.udpGetBufferData);
        }
    }
    else
        headerBytes = 1;

    if ( !headerBytes )
        return false;

    Snmpv3StackDcptStubPtr->InPduWholeMsgBuf.scopedPduAuthStructVal=authStructure;
    Snmpv3StackDcptStubPtr->InPduWholeMsgBuf.scopedPduStructLen=tempLen.Val;
	
    headerBytes++;

    // Since we are using UDP as our transport and UDP are not fragmented,
    // this structure length cannot be more than 1500 bytes.
    // As a result, we will only use lower uint16_t of length value.
    *dataLen = tempLen.Val;

    return headerBytes;
}

static SNMPV3_PRIV_PROT_TYPE TCPIP_SNMPV3_PrivProtocolGet(uint8_t *username)
{
    uint8_t *userSecurityName=NULL;
    uint8_t i=0;

    for(i=0;i<TCPIP_SNMPV3_USM_MAX_USER;i++)
    {
        userSecurityName = Snmpv3StackDcptStubPtr->UserInfoDataBase[i].userName;
        if(strcmp((char *)username,(char *)userSecurityName) == 0)
        {
            return (SNMPV3_PRIV_PROT_TYPE)Snmpv3StackDcptStubPtr->UserInfoDataBase[i].userPrivType;
        }
    }

    return SNMPV3_NO_PRIV;
}

static uint8_t gSNMPV3TrapSecurityLevel = NO_REPORT_NO_PRIVACY_NO_AUTH;
#define INVALID_INDEX 0xFF

/****************************************************************************
Function:
    uint8_t SNMPv3GetUserIndxFromUsmUserDB(uint8_t targetIndex)

Summary:
    Routine to find the index of the user name in the user data base table.

Description:
    There are two different data base tables defined with SNMPv3 stack,
    like 'UserInfoDataBase' and 'Snmpv3TrapConfigData'.
    It returns the index of the user name which matches to the trap target
    user name within the user data base.

Precondition:
    Trap notification event is triggred and the trap send flag is enabled.

Parameters:
    targetIndex -index of the 'Snmpv3TrapConfigData' table to match the
                         'userSecurityName' with the user data base

Return Values:
    INVALID_INDEX - if the trap target user name does not match.
    uint8_t - Byte value fo the index matched

Remarks:
    None.
***************************************************************************/
uint8_t TCPIP_SNMPv3_UserIndxGetFromUsmUserDB(uint8_t targetIndex)
{
    uint8_t *userSecurityName=NULL;
    uint8_t userDBsecurityLevel=0;
    uint8_t trapSecurityLevel=0;
    uint8_t userTrapSecLen=0;
    uint8_t *userTrapSecurityName=NULL;
    uint8_t i=0;

    trapSecurityLevel = TCPIP_SNMPv3_TrapSecurityLevelGet((STD_BASED_SNMPV3_SECURITY_LEVEL)Snmpv3StackDcptStubPtr->Snmpv3TrapConfigData[targetIndex].securityLevelType);
    if(trapSecurityLevel == INVALID_MSG)
        return INVALID_INDEX;

    for(i=0;i<TCPIP_SNMPV3_USM_MAX_USER;i++)
    {
        userSecurityName = Snmpv3StackDcptStubPtr->UserInfoDataBase[i].userName;
        userDBsecurityLevel = SNMPv3GetSecurityLevel(i);
        userTrapSecLen = strlen((char *)Snmpv3StackDcptStubPtr->Snmpv3TrapConfigData[targetIndex].userSecurityName);
        userTrapSecurityName = Snmpv3StackDcptStubPtr->Snmpv3TrapConfigData[targetIndex].userSecurityName;

        if(userTrapSecLen != Snmpv3StackDcptStubPtr->UserInfoDataBase[i].userNameLength)
                continue;
        if(strncmp((char *)userTrapSecurityName,(char *)userSecurityName,userTrapSecLen) == 0)
        {
            if(trapSecurityLevel == userDBsecurityLevel)
                return i;
            else
                continue;
        }
    }

    return INVALID_INDEX;
}


/****************************************************************************
Function:
    bool SNMPv3CmprTrapSecNameAndSecLvlWithUSMDb(
                            uint8_t tragetIndex,
                            uint8_t userTrapSecLen,
                            uint8_t *userTrapSecurityName,
                            STD_BASED_SNMPV3_SECURITY_LEVEL securityLevel)

Summary:
    Routine to find the index of the user name in the user data base table.

Description:
    There are two different data base tables defined with SNMPv3 stack,
    like 'UserInfoDataBase' and 'Snmpv3TrapConfigData'.
    This routine is used to validte the trap user security level setting with
    SET request.


Precondition:
    SET operation would be allowed if the USM security conditions and
    user security name in the request is matched to one of the user security
    name stored in the usm user database.

Parameters:
    targetIndex -index of the 'Snmpv3TrapConfigData' table to match the
                         'userSecurityName' with the user data base
    userTrapSecLen - user sec name length in the SET request
    userTrapSecurityName - pointer to user sec name in the SET request
    securityLevel - trap security level to be SET on the agent

Return Values:
    true - if the trap target user sec level setting is successful
    FLASE - If the SET failed due to non matching of the security parameters

Remarks:
    None.
***************************************************************************/
bool TCPIP_SNMPv3_CmprTrapSecNameAndSecLvlWithUSMDb(uint8_t tragetIndex,
                                                    uint8_t userTrapSecLen,
                                                    uint8_t *userTrapSecurityName,
                                                    STD_BASED_SNMPV3_SECURITY_LEVEL securityLevel)
{
    uint8_t *userSecurityName=NULL;
    uint8_t userDBsecurityLevel=0;
    uint8_t trapSecurityLevel=0;
    uint8_t i=0;

    trapSecurityLevel = TCPIP_SNMPv3_TrapSecurityLevelGet(securityLevel);
    if(trapSecurityLevel == INVALID_MSG)
        return false;

    for(i=0;i<TCPIP_SNMPV3_USM_MAX_USER;i++)
    {
        userSecurityName = Snmpv3StackDcptStubPtr->UserInfoDataBase[i].userName;
        userDBsecurityLevel = SNMPv3GetSecurityLevel(i);
        if(userTrapSecLen != Snmpv3StackDcptStubPtr->UserInfoDataBase[i].userNameLength)
                continue;
        if(strncmp((char *)userTrapSecurityName,(char *)userSecurityName,userTrapSecLen) == 0)
        {
            if(trapSecurityLevel == userDBsecurityLevel)
                return true;
            else
                continue;
        }
    }

    return false;
}


/****************************************************************************
Function:
    uint8_t SNMPv3GetTrapSecurityLevel(STD_BASED_SNMPV3_SECURITY_LEVEL securityLevel)

Summary:
    Routine to find the report, auth and privacy flags settings in the TRAP.

Description:
    This routine to find the report, auth and privacy flags setting for the trap to be
    generated. The message flags octet's least significant three bits:
    Reportable, PrivFlag, AuthFlag forms different secuity level combinations.


Precondition:
    None

Parameters:
    securityLevel -trap security level to be compared for getting the agent's security
                            level settings
Return Values:
    NO_REPORT_NO_PRIVACY_NO_AUTH - No authentication, no encryption
    NO_REPORT_NO_PRIVACY_BUT_AUTH_PROVIDED - authentication but no encryption
    NO_REPORT_PRIVACY_AND_AUTH_PROVIDED - authentication and encryption
    INVALID_MSG - if security level doesn't match any of the above

Remarks:
    None.
***************************************************************************/
uint8_t TCPIP_SNMPv3_TrapSecurityLevelGet(STD_BASED_SNMPV3_SECURITY_LEVEL securityLevel)
{
    uint8_t tempSecurityLevel=0xFF;

    switch(securityLevel)
    {
        case NO_AUTH_NO_PRIV:
            tempSecurityLevel =  NO_REPORT_NO_PRIVACY_NO_AUTH;
            break;
        case AUTH_NO_PRIV:
            tempSecurityLevel = NO_REPORT_NO_PRIVACY_BUT_AUTH_PROVIDED;
            break;
        case AUTH_PRIV:
            tempSecurityLevel = NO_REPORT_PRIVACY_AND_AUTH_PROVIDED;
            break;
        default:
            return INVALID_MSG;
    }
    return tempSecurityLevel;
}

/****************************************************************************
Function:
    bool SNMPv3TrapMsgHeaderPDU(unsigned int targetIndex)

Summary:
    TRAP PDU message header construction.

Description:
    This routine forms the message header for the SNMPv3 trap PDU
    to be originated from this agent.

Precondition:
    TRAP event is triggered.

Parameters:
    targetIndex -index of the 'Snmpv3TrapConfigData' table's security user name
                         for which the TRAP PDU message header to constructed.

Return Values:
    INVALID_INDEX - if the 'targetIndex' does not match to any of the users configured
                                    with the agent in 'Snmpv3TrapConfigData'.
    true - The trap message header generation is successful.
    false -The trap message header generation failed.

Remarks:
    None.
***************************************************************************/
bool TCPIP_SNMPv3_TrapMsgHeaderPDU(unsigned int targetIndex)
{
    uint8_t putCntr=0;
    uint8_t *ptr=NULL;
    bool retBuf=true;
    uint8_t snmpv3MsgGlobalHeaderlength=0;
    uint16_t snmpv3MsgAuthHeaderLength=0;
    uint8_t tempData=0;
    uint16_t msgHeaderOffset1=0;
    uint16_t msgHeaderOffset2=0;
    uint16_t tempMsgHeaderOffset=0;
    uint8_t  USM_Index=0,temp_index=0;
    SNMPV3_PRIV_PROT_TYPE privType=SNMPV3_NO_PRIV;
	
    SNMP_PROCESSING_MEM_INFO_PTRS snmpPktProcsMemPtrsInfo;
    TCPIP_SNMP_PacketProcStubPtrsGet(&snmpPktProcsMemPtrsInfo);

    if(Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.head == NULL)
    {
        return false;
    }
    temp_index = Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityNameLength ;
    USM_Index = TCPIP_SNMPv3_UserIndxGetFromUsmUserDB(targetIndex);
    if(USM_Index != INVALID_INDEX)
    {
        Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityNameLength = strlen((char *)Snmpv3StackDcptStubPtr->Snmpv3TrapConfigData[targetIndex].userSecurityName);
    }

    // update the IN pdu trap security name size
    Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityNameLength = temp_index;

    /*Msg Processing Model PDU header */
    /*ID + Msg Size + Msg Flag + Security Model */
    //message header
    TCPIP_SNMPv3_DataCopyToProcessBuff(STRUCTURE,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(MSGGLOBAL_HEADER_LEN(snmpv3MsgGlobalHeaderlength)-2,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);

	//Put "msgID" type ASN_INT of length 4 bytes	
    TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(0x04,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_UINT32_VAL pduMsgId;
    pduMsgId.Val = Snmpv3StackDcptStubPtr->IncmngSnmpPduMsgID;

    TCPIP_SNMPv3_DataCopyToProcessBuff(pduMsgId.v[3],&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(pduMsgId.v[2],&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(pduMsgId.v[1],&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(pduMsgId.v[0],&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);

	//Put "msgMaxSize"  type ASN_INT of length 4 bytes
    TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(0x04,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_UINT32_VAL maxMsgSize;
    maxMsgSize.Val = Snmpv3StackDcptStubPtr->SnmpEngnMaxMsgSize;
    
    TCPIP_SNMPv3_DataCopyToProcessBuff(maxMsgSize.v[3],&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(maxMsgSize.v[2],&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(maxMsgSize.v[1],&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(maxMsgSize.v[0],&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);

	//Put "msgFlags"  type octet_string 
    TCPIP_SNMPv3_DataCopyToProcessBuff(OCTET_STRING,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(0x01,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(gSNMPV3TrapSecurityLevel&0x03,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);//Rsponse "msgFlags" value as Reportable, Encrypted and Authenticated Bits are not set.

	//Put "msgSecurityModel"	
    TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(0x01,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->Snmpv3TrapConfigData[targetIndex].securityModelType,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);

    /*User Security Module pdu header
    Authoritative Engin ID + Authoritative Boots + Authoritative Engine Time+
    User name + Authentication parameters + Privacy Parameter
    */
    TCPIP_SNMPv3_DataCopyToProcessBuff(OCTET_STRING,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);  //Security Parameter string
    msgHeaderOffset1 = Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.length;
    TCPIP_SNMPv3_DataCopyToProcessBuff(MSG_AUTHORITATIVE_HEADER_LEN(snmpv3MsgAuthHeaderLength)-2,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(STRUCTURE,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    msgHeaderOffset2 = Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.length;
    TCPIP_SNMPv3_DataCopyToProcessBuff(MSG_AUTHORITATIVE_HEADER_LEN(snmpv3MsgAuthHeaderLength)-4,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
	
	//Put "msgAuthoritiveEngineID"	
    TCPIP_SNMPv3_DataCopyToProcessBuff(OCTET_STRING,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpEngnIDLength,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf); //Integer Length
    putCntr = 0;
    for(;putCntr<Snmpv3StackDcptStubPtr->SnmpEngnIDLength;putCntr++)
    {
        TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpEngineID[putCntr],&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    }

	//Put "msgAuthoritiveEngineBoots" 
    TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(0x04,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpEngineBoots>>24,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpEngineBoots>>16,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpEngineBoots>>8,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpEngineBoots,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
	
	//Put "msgAuthoritiveEngineTime" 
    TCPIP_SNMPv3_AuthEngineTimeGet();
    TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(0x04,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_UINT32_VAL engTime;
    engTime.Val = Snmpv3StackDcptStubPtr->SnmpEngineTime;
    
    TCPIP_SNMPv3_DataCopyToProcessBuff(engTime.v[3],&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(engTime.v[2],&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(engTime.v[1],&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(engTime.v[0],&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
	
    putCntr = 0;
    // warning removal for retBuf
    if(retBuf == 0)
    {
        
    }

	//Put "msgUserName" 
    TCPIP_SNMPv3_DataCopyToProcessBuff(OCTET_STRING,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    tempData = strlen((char *)Snmpv3StackDcptStubPtr->Snmpv3TrapConfigData[targetIndex].userSecurityName);
    TCPIP_SNMPv3_DataCopyToProcessBuff(tempData,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    if(tempData != 0)
    {
        ptr= Snmpv3StackDcptStubPtr->Snmpv3TrapConfigData[targetIndex].userSecurityName;

        for(;putCntr<tempData;putCntr++)
        {
            TCPIP_SNMPv3_DataCopyToProcessBuff(ptr[putCntr],&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
        }
    }

    putCntr = 0;

    SNMPv3UsmOutMsgAuthParam(Snmpv3StackDcptStubPtr->UserInfoDataBase[USM_Index].userHashType);

    //Put "msgAuthenticationParameters"
    TCPIP_SNMPv3_DataCopyToProcessBuff(OCTET_STRING,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    if((gSNMPV3TrapSecurityLevel &0x01) == 0x01)
    {
        TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpOutMsgAuthParmLen,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf); //Not supported with the Alpha Release.

        Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.msgAuthParamOffset=Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.length;

        for(;putCntr<Snmpv3StackDcptStubPtr->SnmpOutMsgAuthParmLen;putCntr++)
            TCPIP_SNMPv3_DataCopyToProcessBuff(0x0,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    }
    else
    {
        TCPIP_SNMPv3_DataCopyToProcessBuff(0x0,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf); //Not supported with the Alpha Release.
    }
	
    putCntr = 0;
    SNMPv3USMOutMsgPrivParam(privType);

	//Put "msgPrivacyParameters" 
    TCPIP_SNMPv3_DataCopyToProcessBuff(OCTET_STRING,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    if((gSNMPV3TrapSecurityLevel&0x02) == 0x02)
    {
        SNMPv3USMOutMsgPrivParam(privType);
        TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpOutMsgPrivParmLen,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf); //Not supported with the Alpha Release
        for(;putCntr<Snmpv3StackDcptStubPtr->SnmpOutMsgPrivParmLen;putCntr++)
            retBuf = TCPIP_SNMPv3_DataCopyToProcessBuff(Snmpv3StackDcptStubPtr->SnmpOutMsgPrvParmStrng[putCntr],&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    }
    else
    {
        TCPIP_SNMPv3_DataCopyToProcessBuff(0x0,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf); //Not supported with the Alpha Release.
    }


    tempMsgHeaderOffset = Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.length;
    Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.length = msgHeaderOffset2;
    TCPIP_SNMPv3_DataCopyToProcessBuff((tempMsgHeaderOffset-msgHeaderOffset2)-1,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.length = tempMsgHeaderOffset;
	
    tempMsgHeaderOffset = Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.length;
    Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.length = msgHeaderOffset1;
    TCPIP_SNMPv3_DataCopyToProcessBuff((tempMsgHeaderOffset-msgHeaderOffset1)-1,&Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf);
    Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.length = tempMsgHeaderOffset;

    return true;
}


/****************************************************************************
Function:
    bool SNMPv3TrapScopedpdu(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index,
                                                       uint8_t targetIndex)

Summary:
    TRAP PDU scoped pdu header construction.

Description:
    This routine forms the trap scoped pdu header for the SNMPv3 trap PDU
    to be originated from this agent. Scoped pdu comprises of
    msg data : - <contextEngineID><context name>[<data> == <pdutype><request id>
    <error status><error index><varbinds>

Precondition:
    TRAP event is triggered.

Parameters:
    var - var id of the variable whose value to be sent in the trap pdu
    val - value of the variable
    index - index of the variable in the multiple variable bind scenario
    targetIndex -index of the 'Snmpv3TrapConfigData' table's security user name
                         for which the TRAP PDU message header to constructed.

Return Values:
    true - The trap scoped pdu header generation is successful.
    false -The trap scoped pdu header generation failed.

Remarks:
    None.
***************************************************************************/
bool TCPIP_SNMPv3_TrapScopedPDU(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index,uint8_t targetIndex)
{	
    uint8_t 			*ptr=NULL;
    uint8_t			contextName[]="";
    uint8_t			contextEngId[]="";
    uint8_t			count=0;
    uint8_t			OIDLen=0; 
    uint8_t 		len=0;
    uint8_t 		OIDValue[TCPIP_SNMP_OID_MAX_LEN];
    uint8_t 			snmptrap_oids[]  = {0x2b,6,1,6,3,1,1,4,1 }; /* len=10 */
    uint8_t			sysUpTime_oids[] = {0x2b,6,1,2,1,1,3}; /* len = 8 */
    int 			i=0;
    uint16_t			contextIDlen=0;
    uint16_t			contextNameLength=0;
    SNMPV3MSGDATA	*dynTrapScopedPduBuf;
    static uint16_t		pduStructLenOffset=0;
    uint16_t			varPairStructLenOffset=0;
    static uint16_t		varBindStructLenOffset=0;
    uint16_t			tempOffset=0;
    OID_INFO		rec;
    SNMP_DATA_TYPE_INFO	dataTypeInfo;
    TCPIP_UINT16_VAL		varBindLen = {0};
    uint8_t  			USM_Index=0,temp_index=0;
    SNMPV3_PRIV_PROT_TYPE       usmPrivType = SNMPV3_NO_PRIV;

    SNMP_PROCESSING_MEM_INFO_PTRS snmpPktProcsMemPtrsInfo; 
    SNMP_STACK_DCPT_STUB * snmpStkDcptMemStubPtr=0;		

    TCPIP_SNMP_PacketProcStubPtrsGet(&snmpPktProcsMemPtrsInfo);
    snmpStkDcptMemStubPtr=snmpPktProcsMemPtrsInfo.snmpStkDynMemStubPtr;
	
    if(Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.head == NULL)
    {
        return false;
    }
    if(Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.length == 0)
    {
        temp_index = Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityNameLength ;
        USM_Index = TCPIP_SNMPv3_UserIndxGetFromUsmUserDB(targetIndex);
        if(USM_Index != INVALID_INDEX)
        {
            Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityNameLength = strlen((char *)Snmpv3StackDcptStubPtr->Snmpv3TrapConfigData[targetIndex].userSecurityName);
        }
//                                                                  MSG_AUTHORITATIVE_HEADER_LEN(snmpv3MsgAuthHeaderLength);
        // update the IN pdu trap security name size
        Snmpv3StackDcptStubPtr->SecurtyPrimtvesOfIncmngPdu.securityNameLength = temp_index;

        //Start collecting the plaint text Scoped PDU data byte from the incoming PDU.
        //Check if the plain text scoped pdu data bytes are binded in ASN structure format 

        dynTrapScopedPduBuf = &Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf;
        TCPIP_SNMPv3_DataCopyToProcessBuff(STRUCTURE,dynTrapScopedPduBuf); // First item to Response buffer is packet structure
        TCPIP_SNMPv3_DataCopyToProcessBuff(0x82,dynTrapScopedPduBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);

        //Collect context engine id
        TCPIP_SNMPv3_DataCopyToProcessBuff(OCTET_STRING,dynTrapScopedPduBuf);
        // populate context Engine id to contextEngId
        contextIDlen = strlen((char*)contextEngId);
        if(contextIDlen == 0)
        {			
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);
        }
        else
        {
                //copy context engine id from a local buffer			
            TCPIP_SNMPv3_DataCopyToProcessBuff(contextIDlen,dynTrapScopedPduBuf);
            while(contextIDlen--)
            {
                TCPIP_SNMPv3_DataCopyToProcessBuff(contextEngId[count++],dynTrapScopedPduBuf);
            }
        }
		//Check and collect "contextName" 
        TCPIP_SNMPv3_DataCopyToProcessBuff(OCTET_STRING,dynTrapScopedPduBuf);
        contextNameLength = strlen((char*)contextName);
        count = 0;
        if(contextNameLength == 0)
        {
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);
        }
        else
        {
            TCPIP_SNMPv3_DataCopyToProcessBuff(contextNameLength,dynTrapScopedPduBuf);
            while(contextNameLength--)
            {
                TCPIP_SNMPv3_DataCopyToProcessBuff(contextName[count++],dynTrapScopedPduBuf);
            }
        }	
// Trap V2 PDU update
        if(TCPIP_SNMP_TRAPTypeGet()==true)
        {
            //TRAP Version type.
            TCPIP_SNMPv3_DataCopyToProcessBuff(SNMP_V2_TRAP,dynTrapScopedPduBuf);
            pduStructLenOffset = dynTrapScopedPduBuf->length;
            TCPIP_SNMPv3_DataCopyToProcessBuff(0x82,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);

            //put Request ID for the trapv2 as 1
            TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,dynTrapScopedPduBuf);// To simplify logic, always use 4 byte long requestID
            TCPIP_SNMPv3_DataCopyToProcessBuff(4,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(1,dynTrapScopedPduBuf);

            // Put error status.
            TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,dynTrapScopedPduBuf);// Int type
            TCPIP_SNMPv3_DataCopyToProcessBuff(1,dynTrapScopedPduBuf); // One byte long.
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf); // Placeholder.

            // Similarly put error index.
            TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,dynTrapScopedPduBuf);// Int type
            TCPIP_SNMPv3_DataCopyToProcessBuff(1,dynTrapScopedPduBuf); // One byte long.
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf); // Placeholder.

            // Variable binding structure header
            TCPIP_SNMPv3_DataCopyToProcessBuff(STRUCTURE,dynTrapScopedPduBuf);
            varBindStructLenOffset = dynTrapScopedPduBuf->length;
            TCPIP_SNMPv3_DataCopyToProcessBuff(0x82,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);

            // Create variable name-pair structure
            TCPIP_SNMPv3_DataCopyToProcessBuff(STRUCTURE,dynTrapScopedPduBuf);
            varPairStructLenOffset = dynTrapScopedPduBuf->length;
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);

            // Set 1st varbind object i,e sysUpTime.0 time stamp for the snmpv2 trap
            // Get complete notification variable OID string.

            TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_OID,dynTrapScopedPduBuf);
            OIDLen = (uint8_t)sizeof(sysUpTime_oids);
            TCPIP_SNMPv3_DataCopyToProcessBuff((uint8_t)(OIDLen)+1,dynTrapScopedPduBuf);
            ptr = sysUpTime_oids;
            while( OIDLen-- )
            {
                TCPIP_SNMPv3_DataCopyToProcessBuff(*ptr++,dynTrapScopedPduBuf);
            }

            //1st varbind	 and this is a scalar object so index = 0
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);

            // Time stamp
            TCPIP_SNMPv3_DataCopyToProcessBuff(SNMP_TIME_TICKS,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(4,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff((snmpStkDcptMemStubPtr->SNMPNotifyInfo.timestamp>>24)&0xFF,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff((snmpStkDcptMemStubPtr->SNMPNotifyInfo.timestamp>>16)&0xFF,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff((snmpStkDcptMemStubPtr->SNMPNotifyInfo.timestamp>>8)&0xFF,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(snmpStkDcptMemStubPtr->SNMPNotifyInfo.timestamp&0xFF,dynTrapScopedPduBuf);

            tempOffset = dynTrapScopedPduBuf->length;
            //set the snmp time varbind trap offset
            dynTrapScopedPduBuf->length = varPairStructLenOffset;

            /*// SNMP time stamp varbind length
            OIDLen = 2							// 1st varbind header
               + (uint8_t)sizeof(sysUpTime_oids)
               + 1						   // index byte
               + 6 ;						// time stamp */

            TCPIP_SNMPv3_DataCopyToProcessBuff((tempOffset-varPairStructLenOffset)-1,dynTrapScopedPduBuf);
            //set the previous TX offset
            dynTrapScopedPduBuf->length = tempOffset;

            // Set 2nd varbind object i,e snmpTrapOID.0 for the snmpv2 trap
            // Get complete notification variable OID string.

            // Create variable name-pair structure
            TCPIP_SNMPv3_DataCopyToProcessBuff(STRUCTURE,dynTrapScopedPduBuf);
            varPairStructLenOffset = dynTrapScopedPduBuf->length;
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);

                    // Copy OID string into PDU.
            TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_OID,dynTrapScopedPduBuf);
            OIDLen = (uint8_t)sizeof(snmptrap_oids);
            TCPIP_SNMPv3_DataCopyToProcessBuff((uint8_t)(OIDLen)+1,dynTrapScopedPduBuf);

            ptr = snmptrap_oids;
            while( OIDLen-- )
            {
                TCPIP_SNMPv3_DataCopyToProcessBuff(*ptr++,dynTrapScopedPduBuf);
            }

                    //2nd varbind  and this is a scalar object so index = 0
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);

            if (!TCPIP_SNMP_OIDStringFindByID(Snmpv3TrapFileDescrptr,snmpStkDcptMemStubPtr->SNMPNotifyInfo.trapIDVar, &rec, OIDValue, &OIDLen) )
            {
                return false;
            }
            TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_OID,dynTrapScopedPduBuf);
            len = OIDLen;
            TCPIP_SNMPv3_DataCopyToProcessBuff(OIDLen,dynTrapScopedPduBuf);
            for(i=0;i<len;i++)
            {
                TCPIP_SNMPv3_DataCopyToProcessBuff(OIDValue[i],dynTrapScopedPduBuf);
            }
            tempOffset = dynTrapScopedPduBuf->length;
            //set the snmp varbind trap offset
            dynTrapScopedPduBuf->length = varPairStructLenOffset;
            // Snmp trap varbind length
            /*OIDLen = 2					 // Agent ID header bytes
                    + (uint8_t)sizeof(snmptrap_oids)
                    + 1 					   // index byte
                    + 2 					 // header
                    + agentIDLen;				 // Agent ID bytes				  */
            TCPIP_SNMPv3_DataCopyToProcessBuff((tempOffset-varPairStructLenOffset)-1,dynTrapScopedPduBuf);
            //set the previous TX offset
            dynTrapScopedPduBuf->length = tempOffset;
        }
        else
        {
            // Put PDU type.  SNMP agent's response is always GET RESPONSE
            TCPIP_SNMPv3_DataCopyToProcessBuff(TRAP,dynTrapScopedPduBuf);
            pduStructLenOffset = dynTrapScopedPduBuf->length;
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);

            // Get complete OID string from snmp.bib.
            if(!TCPIP_SNMP_OIDStringFindByID(Snmpv3TrapFileDescrptr,snmpStkDcptMemStubPtr->SNMPNotifyInfo.agentIDVar,&rec, OIDValue, &OIDLen))
            {
                return false;
            }

            if(!rec.nodeInfo.Flags.bIsAgentID )
            {
                return false;
            }
            (*snmpFileShell->fileSeek)(snmpFileShell,Snmpv3TrapFileDescrptr,rec.hData,SYS_FS_SEEK_SET);
            TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_OID,dynTrapScopedPduBuf);
            (*snmpFileShell->fileRead)(snmpFileShell,Snmpv3TrapFileDescrptr,(uint8_t*)&len,1);
            OIDLen = len;
            TCPIP_SNMPv3_DataCopyToProcessBuff(len,dynTrapScopedPduBuf);
            while( len-- )
            {
                uint8_t c;
                (*snmpFileShell->fileRead)(snmpFileShell,Snmpv3TrapFileDescrptr,&c,1);
                TCPIP_SNMPv3_DataCopyToProcessBuff(c,dynTrapScopedPduBuf);
            }

                    // This agent's IP address.
            TCPIP_SNMPv3_DataCopyToProcessBuff(SNMP_IP_ADDR,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(4,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(((TCPIP_NET_IF*)(snmpStkDcptMemStubPtr->SNMPNotifyInfo.snmpTrapInf))->netIPAddr.v[0],dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(((TCPIP_NET_IF*)(snmpStkDcptMemStubPtr->SNMPNotifyInfo.snmpTrapInf))->netIPAddr.v[1],dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(((TCPIP_NET_IF*)(snmpStkDcptMemStubPtr->SNMPNotifyInfo.snmpTrapInf))->netIPAddr.v[2],dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(((TCPIP_NET_IF*)(snmpStkDcptMemStubPtr->SNMPNotifyInfo.snmpTrapInf))->netIPAddr.v[3],dynTrapScopedPduBuf);

            // Geberic/Enterprise Trap code
            TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(1,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(snmpStkDcptMemStubPtr->gGenericTrapNotification,dynTrapScopedPduBuf);

            // Specific Trap code
            TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_INT,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(1,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(snmpStkDcptMemStubPtr->SNMPNotifyInfo.notificationCode,dynTrapScopedPduBuf);

            // Time stamp
            TCPIP_SNMPv3_DataCopyToProcessBuff(SNMP_TIME_TICKS,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(4,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff((snmpStkDcptMemStubPtr->SNMPNotifyInfo.timestamp>>24)&0xFF,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff((snmpStkDcptMemStubPtr->SNMPNotifyInfo.timestamp>>16)&0xFF,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff((snmpStkDcptMemStubPtr->SNMPNotifyInfo.timestamp>>8)&0xFF,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff((snmpStkDcptMemStubPtr->SNMPNotifyInfo.timestamp)&0xFF,dynTrapScopedPduBuf);

            // Variable binding structure header
            TCPIP_SNMPv3_DataCopyToProcessBuff(0x30,dynTrapScopedPduBuf);
            varBindStructLenOffset = dynTrapScopedPduBuf->length;
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);

            // Create variable name-pair structure
            TCPIP_SNMPv3_DataCopyToProcessBuff(0x30,dynTrapScopedPduBuf);
            varPairStructLenOffset = dynTrapScopedPduBuf->length;
            TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);

            // Get complete notification variable OID string.
            if ( !TCPIP_SNMP_OIDStringFindByID(Snmpv3TrapFileDescrptr,var, &rec, OIDValue, &OIDLen) )
            {
               return false;
            }

            // Copy OID string into packet.
            TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_OID,dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff((uint8_t)(OIDLen+1),dynTrapScopedPduBuf);
            len = OIDLen;
            ptr = OIDValue;
            while( len-- )
            {
                TCPIP_SNMPv3_DataCopyToProcessBuff(*ptr++,dynTrapScopedPduBuf);
            }
            TCPIP_SNMPv3_DataCopyToProcessBuff(index,dynTrapScopedPduBuf);

            // Encode and Copy actual data bytes
            if ( !TCPIP_SNMP_DataTypeInfoGet(rec.dataType, &dataTypeInfo) )
            {
                return false;
            }

            TCPIP_SNMPv3_DataCopyToProcessBuff(dataTypeInfo.asnType,dynTrapScopedPduBuf);

            //Modified to Send trap even for  dataTypeInfo.asnType= ASCII_STRING,
            //where dataTypeInfo.asnLen=0xff
            if ( dataTypeInfo.asnLen == 0xff )
            {
                dataTypeInfo.asnLen=0x4;
                val.dword=0;
            }
            len = dataTypeInfo.asnLen;
            TCPIP_SNMPv3_DataCopyToProcessBuff(len,dynTrapScopedPduBuf);
            while( len-- )
            {
                TCPIP_SNMPv3_DataCopyToProcessBuff(val.v[len],dynTrapScopedPduBuf);
            }

            tempOffset = dynTrapScopedPduBuf->length;
            dynTrapScopedPduBuf->length = varPairStructLenOffset;
            varBindLen.Val = (tempOffset - varPairStructLenOffset)-1;
            TCPIP_SNMPv3_DataCopyToProcessBuff(varBindLen.v[0],dynTrapScopedPduBuf);

            dynTrapScopedPduBuf->length = varBindStructLenOffset;
            varBindLen.Val = (tempOffset - varBindStructLenOffset)-1;
            TCPIP_SNMPv3_DataCopyToProcessBuff(varBindLen.v[0],dynTrapScopedPduBuf);

            dynTrapScopedPduBuf->length = pduStructLenOffset;
            varBindLen.Val = (tempOffset - pduStructLenOffset)-1;
            TCPIP_SNMPv3_DataCopyToProcessBuff(varBindLen.v[0],dynTrapScopedPduBuf);

            dynTrapScopedPduBuf->length = 1;
            TCPIP_SNMPv3_DataCopyToProcessBuff(0x82,dynTrapScopedPduBuf);
            varBindLen.Val = tempOffset - 4; // equal to tempOffset - dynTrapScopedPduBuf->length
            TCPIP_SNMPv3_DataCopyToProcessBuff(varBindLen.v[1],dynTrapScopedPduBuf);
            TCPIP_SNMPv3_DataCopyToProcessBuff(varBindLen.v[0],dynTrapScopedPduBuf);

            dynTrapScopedPduBuf->length = tempOffset;

            pduStructLenOffset = 0;
            varBindStructLenOffset = 0;
            return true;
	
        }
    }
    else
    {
        dynTrapScopedPduBuf = &Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf;
    }
	
    // Create variable name-pair structure
    TCPIP_SNMPv3_DataCopyToProcessBuff(STRUCTURE,dynTrapScopedPduBuf);
    varPairStructLenOffset = dynTrapScopedPduBuf->length;
    TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);
    /* to send generic trap trap */
    if(snmpStkDcptMemStubPtr->gGenericTrapNotification != ENTERPRISE_SPECIFIC)
    {
        ptr = (uint8_t*)TCPIP_SNMP_GenericTrapCodeToTrapOID(snmpStkDcptMemStubPtr->gGenericTrapNotification,&OIDLen);
        if(ptr == NULL)
        {
            return false;
        }
		// Copy OID string into PDU.
        TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_OID,dynTrapScopedPduBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff((uint8_t)(OIDLen)+1,dynTrapScopedPduBuf);
        while( OIDLen-- )
        {
            TCPIP_SNMPv3_DataCopyToProcessBuff(*ptr++,dynTrapScopedPduBuf);
        }

//2nd varbind  and this is a scalar object so index = 0
        TCPIP_SNMPv3_DataCopyToProcessBuff(0,dynTrapScopedPduBuf);
        if ( !TCPIP_SNMP_OIDStringFindByID(Snmpv3TrapFileDescrptr,snmpStkDcptMemStubPtr->SNMPNotifyInfo.agentIDVar, &rec, OIDValue, &OIDLen) )
        {
            (*snmpFileShell->fileClose)(snmpFileShell,Snmpv3TrapFileDescrptr);
            
            Snmpv3TrapFileDescrptr = SYS_FS_HANDLE_INVALID;
            return false;
        }
        if ( !rec.nodeInfo.Flags.bIsAgentID )
        {
            return false;
        }

        (*snmpFileShell->fileSeek)(snmpFileShell,Snmpv3TrapFileDescrptr, rec.hData, SYS_FS_SEEK_SET);		
        TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_OID,dynTrapScopedPduBuf);
		
        (*snmpFileShell->fileRead)(snmpFileShell,Snmpv3TrapFileDescrptr,&len,1);		
        OIDLen = len;
        TCPIP_SNMPv3_DataCopyToProcessBuff(OIDLen,dynTrapScopedPduBuf);
        while( OIDLen-- )
        {
            uint8_t c;			
            (*snmpFileShell->fileRead)(snmpFileShell,Snmpv3TrapFileDescrptr,&c,1);
			
            TCPIP_SNMPv3_DataCopyToProcessBuff(c,dynTrapScopedPduBuf);
        }
        tempOffset = dynTrapScopedPduBuf->length;
        //set the snmp varbind trap offset
        dynTrapScopedPduBuf->length = varPairStructLenOffset;
        /*OIDLen = 2 					 // Agent ID header bytes
                + (uint8_t)sizeof(snmptrap_oids)
                + 1 					   // index byte
                + 2 					 // header
                + agentIDLen;				 // Agent ID bytes				  */
		
        TCPIP_SNMPv3_DataCopyToProcessBuff((tempOffset-varPairStructLenOffset)-1,dynTrapScopedPduBuf);
        //set the previous TX offset
        dynTrapScopedPduBuf->length = tempOffset;
    }
    else
    {
        // Get complete notification variable OID string.
        if ( !TCPIP_SNMP_OIDStringFindByID(Snmpv3TrapFileDescrptr,var, &rec, OIDValue, &OIDLen) )
        {
            return false;
        }
        ptr = OIDValue;

        // Copy OID string into packet.
        TCPIP_SNMPv3_DataCopyToProcessBuff(ASN_OID,dynTrapScopedPduBuf);
        TCPIP_SNMPv3_DataCopyToProcessBuff((uint8_t)(OIDLen+1),dynTrapScopedPduBuf);
        len = OIDLen;
        while( len-- )
        {
            TCPIP_SNMPv3_DataCopyToProcessBuff(*ptr++,dynTrapScopedPduBuf);
        }
        TCPIP_SNMPv3_DataCopyToProcessBuff(index,dynTrapScopedPduBuf);

        // Encode and Copy actual data bytes
        if ( !TCPIP_SNMP_DataTypeInfoGet(rec.dataType, &dataTypeInfo) )
        {
            return false;
        }
        TCPIP_SNMPv3_DataCopyToProcessBuff(dataTypeInfo.asnType,dynTrapScopedPduBuf);
     //Modified to Send trap even for  dataTypeInfo.asnType= ASCII_STRING,
        //where dataTypeInfo.asnLen=0xff
        if ( dataTypeInfo.asnLen == 0xff )
        {
            uint8_t *asciiStr= (uint8_t *)val.dword;
            int k=0;
            dataTypeInfo.asnLen=strlen((char *)asciiStr);
            len = dataTypeInfo.asnLen;
            //val.dword=0;
            TCPIP_SNMPv3_DataCopyToProcessBuff(len,dynTrapScopedPduBuf);
            for(k=0;k<len;k++)
            {
                TCPIP_SNMPv3_DataCopyToProcessBuff(asciiStr[k],dynTrapScopedPduBuf);
            }
        }
        else
        {
            len = dataTypeInfo.asnLen;
            TCPIP_SNMPv3_DataCopyToProcessBuff(len,dynTrapScopedPduBuf);
            while( len-- )
            {
                TCPIP_SNMPv3_DataCopyToProcessBuff(val.v[len],dynTrapScopedPduBuf);
            }
        }
	  
        /*len	 = dataTypeInfo.asnLen	// data bytes count
                 + 1                    // Length byte
                 + 1                    // Data type byte
                 + OIDLen               // OID bytes
                 + 2                    // OID header bytes
                 + 1;		            // index byte */
        tempOffset = dynTrapScopedPduBuf->length;
        dynTrapScopedPduBuf->length = varPairStructLenOffset;
        TCPIP_SNMPv3_DataCopyToProcessBuff((tempOffset-varPairStructLenOffset)-1,dynTrapScopedPduBuf);
        dynTrapScopedPduBuf->length = tempOffset;
    }
    //set the previous TX offset

    if((TCPIP_SNMP_TRAPTypeGet()== true)&& (snmpStkDcptMemStubPtr->gSetTrapSendFlag == true))
    {
        return true;
    }
    usmPrivType = TCPIP_SNMPV3_PrivProtocolGet(Snmpv3StackDcptStubPtr->Snmpv3TrapConfigData[USM_Index].userSecurityName);
    if(usmPrivType == SNMPV3_DES_PRIV)
    {
        TCPIP_SNMPV3_DESEncryptionPadding(dynTrapScopedPduBuf);
    }
    tempOffset = dynTrapScopedPduBuf->length;
    dynTrapScopedPduBuf->length = varBindStructLenOffset;
    TCPIP_SNMPv3_DataCopyToProcessBuff(0x82,dynTrapScopedPduBuf);
    varBindLen.Val = (tempOffset - varBindStructLenOffset)-3;
    TCPIP_SNMPv3_DataCopyToProcessBuff(varBindLen.v[1],dynTrapScopedPduBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(varBindLen.v[0],dynTrapScopedPduBuf);

    dynTrapScopedPduBuf->length = pduStructLenOffset;
    TCPIP_SNMPv3_DataCopyToProcessBuff(0x82,dynTrapScopedPduBuf);
    varBindLen.Val = (tempOffset - pduStructLenOffset)-3;
    TCPIP_SNMPv3_DataCopyToProcessBuff(varBindLen.v[1],dynTrapScopedPduBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(varBindLen.v[0],dynTrapScopedPduBuf);

    dynTrapScopedPduBuf->length = 1;
    TCPIP_SNMPv3_DataCopyToProcessBuff(0x82,dynTrapScopedPduBuf);
    varBindLen.Val = tempOffset - 4; // equal to tempOffset - dynTrapScopedPduBuf->length
    TCPIP_SNMPv3_DataCopyToProcessBuff(varBindLen.v[1],dynTrapScopedPduBuf);
    TCPIP_SNMPv3_DataCopyToProcessBuff(varBindLen.v[0],dynTrapScopedPduBuf);

    dynTrapScopedPduBuf->length = tempOffset;
    pduStructLenOffset = 0;
    varBindStructLenOffset = 0;
    return true;
}

/****************************************************************************
Function:
    bool SNMPv3Notify(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index,uint8_t targetIndex,SNMP_TRAP_IP_ADDRESS_TYPE eTrapMultiAddressType)

Summary:
    Creates and Sends SNMPv3 TRAP pdu.

Description:
    This function creates SNMPv3 trap PDU and sends it to previously specified
    remoteHost.

Precondition:
    TRAP event is triggered.

Parameters:
    var     - SNMP var ID that is to be used in notification
    val     - Value of var. Only value of uint8_t, uint16_t or uint32_t can be sent.
    index   - Index of var. If this var is a single,index would be 0, or else
                      if this var Is a sequence, index could be any value
                      from 0 to 127
    targetIndex -index of the 'Snmpv3TrapConfigData' table's security user name
                         for which the TRAP PDU message header to constructed.
    eTrapMultiAddressType - trap type

Return Values:
    true	-	if SNMP notification was successful sent.
                            This does not guarantee that remoteHost recieved it.
    false	-	Notification sent failed.
                        This would fail under following contions:
                        1) Given SNMP_BIB_FILE does not exist in file system
                        2) Given var does not exist.
                        3) Previously given agentID does not exist
                        4) Data type of given var is unknown - only
                           possible if file system itself was corrupted.
    SNMPV3_MSG_PRIV_FAIL -encryption of the trap msg failed
    SNMPV3_MSG_AUTH_FAIL - HAMC of the trap msg failed

Remarks:
    None
 ***************************************************************************/ 
bool TCPIP_SNMPv3_Notify(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index,uint8_t targetIndex,SNMP_TRAP_IP_ADDRESS_TYPE eTrapMultiAddressType)
{
    TCPIP_UINT16_VAL	totaltrapLen={0};
    uint16_t		i=0;
    uint8_t		USM_Index=0;
    SNMP_PROCESSING_MEM_INFO_PTRS snmpPktProcsMemPtrsInfo; 
    SNMP_STACK_DCPT_STUB * snmpStkDcptMemStubPtr=0; 	
    uint8_t tempBuf[4] = {0U, 0U, 0U, 0U};
    uint8_t tempCntr=0;
    uint8_t* tempPtr=NULL;
    uint8_t* outBufPtr=NULL;
    UDP_SOCKET skt = INVALID_UDP_SOCKET;
						
    TCPIP_SNMP_PacketProcStubPtrsGet(&snmpPktProcsMemPtrsInfo);
    snmpStkDcptMemStubPtr=snmpPktProcsMemPtrsInfo.snmpStkDynMemStubPtr;

    //validate the trap user security name , message processing model 
    //, security model and the security level
    gSNMPV3TrapSecurityLevel = TCPIP_SNMPv3_TrapSecurityLevelGet(Snmpv3StackDcptStubPtr->Snmpv3TrapConfigData[targetIndex].securityLevelType);
    if( gSNMPV3TrapSecurityLevel == INVALID_MSG)
    {
        return false;
    }

    Snmpv3TrapFileDescrptr = TCPIP_SNMP_FileDescrGet();
    if(Snmpv3TrapFileDescrptr == SYS_FS_HANDLE_INVALID)
    {
        return false;
    }
    // set the file position to the begining
    if((*snmpFileShell->fileSeek)(snmpFileShell,Snmpv3TrapFileDescrptr,0,SYS_FS_SEEK_SET) == -1)
    {
        return false;
    }
    
    if(Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.length == 0)
    {
        if(TCPIP_SNMPv3_TrapMsgHeaderPDU(targetIndex)!= true)
        {		
            return false;
        }
    }
    if(TCPIP_SNMPv3_TrapScopedPDU(var,val,index,targetIndex) != true)
    {
        return false;
    }

    if((TCPIP_SNMP_TRAPTypeGet()== true)&&(snmpStkDcptMemStubPtr->gSetTrapSendFlag == true))
    {	
        return true;
    }

    USM_Index = TCPIP_SNMPv3_UserIndxGetFromUsmUserDB(targetIndex);
    if(USM_Index == INVALID_INDEX)
    {
        return false;
    }

    totaltrapLen.Val = Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.length + Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.length+3;
    //tempScopedPduLen = Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.length-4; // 4 == STRUCTURE+0x82+len1+len2
    if((gSNMPV3TrapSecurityLevel & 0x02)==0x02) //Encrypted message	
    {
        tempPtr=tempBuf;
        *tempPtr++=0X04;
        if((Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.length >= 0x80) && (Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.length <= 0xFF))
        {
            *tempPtr++=0x81;
            *tempPtr=Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.length;
            tempCntr=3; //0x04(encrypted pkt),0x81,len
        }
        else if((Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.length > 0xFF) && (Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.length < 0xFFFF))
        {			
            *tempPtr++=0x82;
            *tempPtr++=Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.length>>8;
            *tempPtr=Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.length;
            tempCntr=4; //0x04(encrypted pkt),0x81,len_1,len_0
        }
        else
        {
            *tempPtr=Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.length;
            tempCntr=2; //0x04(encrypted pkt),len
        }
    }
    if(eTrapMultiAddressType == IPV4_SNMP_TRAP)
    {
        skt  = snmpStkDcptMemStubPtr->SNMPNotifyInfo.socket;
    }
#ifdef TCPIP_STACK_USE_IPV6
    else
    {
        skt = snmpStkDcptMemStubPtr->SNMPNotifyInfo.socketv6;
    }
#endif
    TCPIP_SNMP_PDUProcessDuplexInit(skt);
    //this will put the start pointer at the beginning of the TX buffer
    TCPIP_UDP_TxOffsetSet(skt,0,false);
    Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf.wholeMsgLen =(totaltrapLen.Val+tempCntr/*0x04,0x82,len_1,len_0*/);
    Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf.wholeMsgHead= TCPIP_UDP_TxPointerGet(skt);
    if(Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf.wholeMsgHead == NULL)
    {
        return false;
    }
    outBufPtr=Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf.wholeMsgHead;	
    //Start Writing to the outPut Buffer

    *outBufPtr++=STRUCTURE;
    totaltrapLen.Val+=tempCntr;
    if((totaltrapLen.Val >= 0x80) && (totaltrapLen.Val <= 0xFF))
    {
        *outBufPtr++=0x81;
        *outBufPtr++=totaltrapLen.Val;
    }
    else if((totaltrapLen.Val > 0xFF) && (totaltrapLen.Val < 0xFFFF))
    {			
        *outBufPtr++=0x82;
        *outBufPtr++=totaltrapLen.v[1];
        *outBufPtr++=totaltrapLen.v[0];
    }
    else
        *outBufPtr++=totaltrapLen.Val;

    *outBufPtr++=ASN_INT;
    *outBufPtr++=0x1;		
    *outBufPtr++=SNMP_V3;

    Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf.msgAuthParamOffsetOutWholeMsg=(uint8_t*)(outBufPtr+Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.msgAuthParamOffset);
    //put global snmpv3 msg header 
    for(i=0;i<Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.length;i++)
    {
        *outBufPtr++=Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.head[i];
    }

    //Copy Scoped PDU to the Out Buffer
    if((gSNMPV3TrapSecurityLevel & 0x02)==0x02) //Encrypted message	
    {	//Copy Packet Auth indicator, length
        for(i=0;i<tempCntr;i++) 
        {
            *outBufPtr++=tempBuf[i];
        }
    }
    Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf.scopedPduOffset=outBufPtr;
    Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf.scopedPduStructLen=Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.length;

    i=0;
    *outBufPtr++=Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.head[i++];//0x30

    if(Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.head[1] == 0x81)
    {
        *outBufPtr++=Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.head[i++];//0x81
        *outBufPtr++=Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.head[i++];//len_0
    }
    else if(Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.head[1] == 0x82)
    {
        *outBufPtr++=Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.head[i++]; //0x82
        *outBufPtr++=Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.head[i++]; //len_1
        *outBufPtr++=Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.head[i++]; //len_0
    }
    else
        *outBufPtr++=Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.head[i++];//len_o

    // send context id and context name and the get response 
    // Authentication and privacy data packet will be sent from here onwards
    for(;i<(Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.length);i++)
    {
        *outBufPtr++=Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.head[i];
    }
	/*Encrypt the Response to the messgae originator*/
    if((gSNMPV3TrapSecurityLevel & 0x02)==0x02) //Encrypted message
    {
        uint8_t temp_usm_index = 0;
        /*Rxed SNMPv3 message is encrypted. Hence Response should be encrypted*/

        /*If user privacy protocol is AES*/
        temp_usm_index = Snmpv3StackDcptStubPtr->UserInfoDataBaseIndx;
        Snmpv3StackDcptStubPtr->UserInfoDataBaseIndx = USM_Index;
        if(Snmpv3StackDcptStubPtr->UserInfoDataBase[Snmpv3StackDcptStubPtr->UserInfoDataBaseIndx].userPrivType
                    == SNMPV3_AES_PRIV)  //user privacy protocol is AES
        {
	        if(SNMPv3AESEncryptResponseScopedPdu(&Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf) != SNMPV3_MSG_PRIV_PASS)
	        {
	            Snmpv3StackDcptStubPtr->UserInfoDataBaseIndx = temp_usm_index;
	            return SNMPV3_MSG_PRIV_FAIL;
	        }
        }
 		else if(Snmpv3StackDcptStubPtr->UserInfoDataBase[Snmpv3StackDcptStubPtr->UserInfoDataBaseIndx].userPrivType
                    == SNMPV3_DES_PRIV)  //user privacy protocol is DES
		{
			if(SNMPv3DESEncryptResponseScopedPdu(&Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf) != SNMPV3_MSG_PRIV_PASS)
			{
			    return SNMPV3_MSG_PRIV_FAIL;
			}
		}
        Snmpv3StackDcptStubPtr->UserInfoDataBaseIndx = temp_usm_index;
        /*If user privacy Protocol is DES*/
        //snmpV3DESDecryptRxedScopedPdu();
    }

    /* Authenticate the whole message to be transmitted*/
    if((gSNMPV3TrapSecurityLevel & 0x01)==0x01) //Authenticatd message
    {
        uint8_t temp_usm_index = 0;
        /*Rxed SNMPv3 message is Authenticated.Send authenticatin parameters for the Response*/
        Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf.wholeMsgLen = outBufPtr - Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf.wholeMsgHead;
        /*If user authentication is HAMC-MD5-96*/
        temp_usm_index = Snmpv3StackDcptStubPtr->UserInfoDataBaseIndx;
        Snmpv3StackDcptStubPtr->UserInfoDataBaseIndx = USM_Index;
        if(SNMPv3AuthenticateTxPduForDataIntegrity(&Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf)!=SNMPV3_MSG_AUTH_PASS)
        {
            Snmpv3StackDcptStubPtr->UserInfoDataBaseIndx = temp_usm_index;
            return SNMPV3_MSG_AUTH_FAIL;
        }
        Snmpv3StackDcptStubPtr->UserInfoDataBaseIndx = temp_usm_index;
        tempPtr = outBufPtr;
        outBufPtr=Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf.msgAuthParamOffsetOutWholeMsg;
        for(i=0;i<Snmpv3StackDcptStubPtr->SnmpOutMsgAuthParmLen;i++)
        {
            *outBufPtr++=Snmpv3StackDcptStubPtr->SnmpOutMsgAuthParaStrng[i];
        }
        outBufPtr = tempPtr;
    }	
    
    TCPIP_UDP_TxOffsetSet(skt,(uint16_t)(outBufPtr-Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf.wholeMsgHead), false);
    TCPIP_UDP_Flush(skt);
    
    if(Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf.wholeMsgHead != NULL)
    {
        Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf.wholeMsgHead=0x00;
        Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf.wholeMsgLen=0x00;
        Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf.snmpMsgHead = NULL;
        Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf.msgAuthParamOffsetOutWholeMsg = NULL;
        Snmpv3StackDcptStubPtr->TrapOUTPduWholeMsgBuf.scopedPduOffset =	NULL;
    }
    Snmpv3StackDcptStubPtr->TrapScopdPduRespnsBuf.length = 0;
    Snmpv3StackDcptStubPtr->TrapMsgHeaderBuf.length = 0;
		
    return true;
}

void TCPIP_SNMPv3_TrapConfigDataGet(uint8_t userIndex,uint8_t *msgModelType,uint8_t *securityModelType)
{
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet (&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;
    *msgModelType = snmpv3EngnDcptMemoryStubPtr->Snmpv3TrapConfigData[userIndex].messageProcessingModelType;
    *securityModelType  = snmpv3EngnDcptMemoryStubPtr->Snmpv3TrapConfigData[userIndex].securityModelType;
}


static void TCPIP_SNMPV3_DESEncryptionPadding(SNMPV3MSGDATA *dynTrapScopedPduBuf)
{
    uint8_t  desPaddingLen=0;
    uint8_t  i=0;
/*
 DES expects the data to be encrypted in multiples of 8 bytes.
*/
    if((desPaddingLen=dynTrapScopedPduBuf->length %8)!=0)
    {
        desPaddingLen = 8-desPaddingLen;
    }
    for(;i<desPaddingLen;i++)
    {
        // adding 0 padding for the remaining bytes
        TCPIP_SNMPv3_DataCopyToProcessBuff(0x0,dynTrapScopedPduBuf);
    }
}
#endif // #if defined(TCPIP_STACK_USE_SNMPV3_SERVER)
