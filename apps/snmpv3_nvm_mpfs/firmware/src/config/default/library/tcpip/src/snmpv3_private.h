
/*******************************************************************************
  SNMPV3 Core Stack APIs

  Company:
    Microchip Technology Inc.
    
  File Name:
    snmpv3_private.h

  Summary:
    SNMPV3 Stack private API for Microchip TCP/IP Stack
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2012 released Microchip Technology Inc.  All rights reserved.

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
// DOM-IGNORE-END

#ifndef _SNMPV3_PRIVATE_H_
#define _SNMPV3_PRIVATE_H_

#include "snmp_private.h"

/*=======================================================================================*/

/* Abstract Service Interfaces and Set of primitives for various sub systems of the SNMP engine */

/*=======================================================================================*/
#define TCPIP_SNMPV3_DES_CRYPTO_KEY_LEN 0x8

#define TCPIP_SNMPV3_DES_CRYPTO_BLOCK_SIZE 0x8

/* ============== */
/* Dispatcher Primitives  */
/* ============== */

/* 
Registering Responsibility for Handling SNMP PDUs 

Applications can register/unregister responsibility for a specific
contextEngineID, for specific pduTypes, with the PDU Dispatcher
according to the following primitives. The list of particular
pduTypes that an application can register for is determined by the
Message Processing Model(s) supported by the SNMP entity that
contains the PDU Dispatcher.

Note that realizations of the registerContextEngineID and
unregisterContextEngineID abstract service interfaces may provide
implementation-specific ways for applications to register/deregister
responsibility for all possible values of the contextEngineID or
pduType parameters.
*/

typedef struct  registerContextEngineID
{
    uint8_t* contextEngineID; //take responsibility for this one
    uint8_t pduType;			//the pduType(s) to be registered
}statusInformation; //success or errorIndication

struct unregisterContextEngineID 
{
    uint8_t* contextEngineID; //give up responsibility for this one
    uint8_t pduType;			//the pduType(s) to be unregistered
};

/* Snmp Message Processing Model */

typedef enum
{
    /*Octet's Least significant three bits: Reportable, PrivFlag, AuthFlag */
    NO_REPORT_NO_PRIVACY_NO_AUTH 			=0x00, /* 00000000b */
    NO_REPORT_NO_PRIVACY_BUT_AUTH_PROVIDED		=0x01, /* 00000001b */
    NO_REPORT_PRIVACY_PROVIDED_BUT_NO_AUTH		=0x02, /* 00000010b Priv without Auth is not allowed*/
    NO_REPORT_PRIVACY_AND_AUTH_PROVIDED			=0x03, /* 00000011b */

    REPORT2REQ_NO_PRIVACY_NO_AUTH			=0x04, /* 00000100b */
    REPORT2REQ_NO_PRIVACY_BUT_AUTH_PROVIDED		=0x05, /* 00000101b */
    REPORT2REQ_PRIVACY_PROVIDED_BUT_NO_AUTH		=0x06, /* 00000110b Priv without Auth is not allowed*/
    REPORT2REQ_PRIVACY_AND_AUTH_PROVIDED		=0x07, /* 00000111b */
    INVALID_MSG						=0xFF

}REPORT_FLAG_AND_SECURITY_LEVEL_FLAGS;


typedef enum
{
    noAuthProtocol = 0x1,
    hmacMD5Auth,
    hmacSHAAuth,
    noPrivProtocol,
    desPrivProtocol = 0x5,
    aesPrivProtocol =0x6
}USM_SECURITY_LEVEL;

typedef enum
{
    SNMPV3_MSG_AUTH_FAIL=0x00,
    SNMPV3_MSG_AUTH_PASS=0x01
}SNMPV3_MSG_AUTH_SEC_PARAM_RESULT;

typedef enum
{
    SNMPV3_MSG_PRIV_FAIL=0x00,
    SNMPV3_MSG_PRIV_PASS=0x01
}SNMPV3_MSG_PRIV_SEC_PARAM_RESULT;

#define SNMPv3_MSG_AUTH_PARAM_STRING_LEN 12
#define SNMPv3_MSG_PRIV_PARAM_STRING_LEN 8

/*
Generate Outgoing Request or Notification 

statusInformation =  -- sendPduHandle if success
				   -- errorIndication if failure
*/

struct dispatcherStatusInfo
{
    uint8_t transportDomain;		//transport domain to be used
    uint32_t transportAddress;	//transport address to be used
    uint8_t messageProcessingModel;//typically, SNMP version
    uint8_t securityModel; 		//Security Model to use
    uint8_t* securityName; 		//on behalf of this principal
    uint8_t securityLevel;			//Level of Security requested
    uint8_t* contextEngineID; 	//data from/at this entity
    uint8_t* contextName;			//data from/in this context
    uint8_t pduVersion; 			//the version of the PDU
    uint8_t* PDU; 					//SNMP Protocol Data Unit
    bool expectResponse; 		//true or false
};

/*
Generate Outgoing Response

The PDU Dispatcher provides the following primitive for an
application to return an SNMP Response PDU to the PDU Dispatcher:

result = SUCCESS or FAILURE
*/

struct dispathcerReturnResponsePdu
{
    uint8_t messageProcessingModel;	//typically, SNMP version
    uint8_t securityModel;				//Security Model in use
    uint8_t* securityName;			//on behalf of this principal
    uint8_t securityLevel;				//same as on incoming request
    uint8_t* contextEngineID;			//data from/at this SNMP entity
    uint8_t* contextName;				//data from/in this context
    uint8_t pduVersion;				//the version of the PDU
    uint8_t* PDU;						//SNMP Protocol Data Unit
    uint32_t maxSizeResponseScopedPDU;//maximum size sender can accept
    uint32_t stateReference;			//reference to state information as presented with the request
    statusInformation statInfo;		//success or errorIndication, error counter OID/value if error
};


/*
Process Incoming Response PDU

The PDU Dispatcher provides the following primitive to pass an
incoming SNMP Response PDU to an application:

*/

struct processResponsePdu //process Response PDU
{ 
    uint8_t messageProcessingModel;	//typically, SNMP version
    uint8_t securityModel;				//Security Model in use
    uint8_t* securityName;			//on behalf of this principal
    uint8_t securityLevel;				//Level of Security
    uint8_t* contextEngineID;			//data from/at this SNMP entity
    uint8_t* contextName;				//data from/in this context
    uint8_t pduVersion;				//the version of the PDU
    uint8_t* PDU;						//SNMP Protocol Data Unit
    statusInformation statInfo;		//success or errorIndication
};			

/*=======================================================================================*/

/* =========================== */
/* Message Processing Subsystem Primitives  */
/* =========================== */

/*
The Dispatcher interacts with a Message Processing Model to process a
specific version of an SNMP Message. Below are the
primitives provided by the Message Processing Subsystem.
*/

/* 
Prepare Outgoing SNMP Request or Notification Message

The Message Processing Subsystem provides this service primitive for
preparing an outgoing SNMP Request or Notification Message
*/

struct MsgProcModPrepareOutgoingMessage
{
    uint8_t transportDomain;		//transport domain to be used
    uint32_t transportAddress;	//transport address to be used
    uint8_t messageProcessingModel;//typically, SNMP version
    uint8_t securityModel;			//Security Model to use
    uint8_t* securityName;		//on behalf of this principal
    uint8_t securityLevel;			//Level of Security requested
    uint8_t* contextEngineID;		//data from/at this entity
    uint8_t* contextName;			//data from/in this context
    uint8_t pduVersion;			//the version of the PDU
    uint8_t* PDU;					//SNMP Protocol Data Unit
    bool expectResponse;		//true or false
    uint8_t destTransportDomain;	//destination transport domain
    uint32_t destTransportAddress;//destination transport address
    uint8_t* outgoingMessage;		//the message to send
    uint32_t outgoingMessageLength; //its length
};



/*
Prepare an Outgoing SNMP Response Message

The Message Processing Subsystem provides this service primitive for
preparing an outgoing SNMP Response Message:
result = -- SUCCESS or FAILURE

*/
struct MsgProcModPrepareResponseMessage
{
    uint8_t messageProcessingModel;	//typically, SNMP version
    uint8_t securityModel;  			//same as on incoming request
    uint8_t* securityName;  			//same as on incoming request
    uint8_t securityLevel;  			//same as on incoming request
    uint8_t* contextEngineID;  		//data from/at this SNMP entity
    uint8_t* contextName;  			//data from/in this context
    uint8_t pduVersion;  				//the version of the PDU
    uint8_t* PDU;  					//SNMP Protocol Data Unit
    uint32_t maxSizeResponseScopedPDU;//maximum size able to accept
    uint32_t stateReference;  		//reference to state information as presented with the request
    statusInformation statInfo;//success or errorIndication, error counter OID/value if error
    uint8_t destTransportDomain;  		//destination transport domain
    uint32_t destTransportAddress;  	//destination transport address
    uint8_t* outgoingMessage;  		//the message to send
    uint32_t outgoingMessageLength;  	//its length
};



/*
Prepare Data Elements from an Incoming SNMP Message

The Message Processing Subsystem provides this service primitive for
preparing the abstract data elements from an incoming SNMP message:
result = -- SUCCESS or errorIndication

*/
struct MsgProcModPrepareDataElements
{
    uint8_t transportDomain;		//origin transport domain
    uint32_t transportAddress; 	//origin transport address
    uint8_t* wholeMsg; 			//as received from the network
    uint32_t wholeMsgLength; 		//as received from the network
    uint8_t messageProcessingModel;//typically, SNMP version
    uint8_t securityModel; 		//Security Model to use
    uint8_t* securityName; 		//on behalf of this principal
    uint8_t securityLevel; 		//Level of Security requested
    uint8_t* contextEngineID; 	//data from/at this entity
    uint8_t* contextName; 		//data from/in this context
    uint8_t pduVersion; 			//the version of the PDU
    uint8_t* PDU ; 				//SNMP Protocol Data Unit
    uint8_t pduType ; 				//SNMP PDU type
    uint32_t maxSizeResponseScopedPDU; //maximum size sender can accept
    statusInformation statInfo; 	//success or errorIndication error counter OID/value if error
    uint32_t stateReference; 		//reference to state information to be used for possible Response
};



/*=======================================================================================*/

/* ======================== */
/* Access Control Subsystem Primitives  */
/* ======================== */

/*
Applications are the typical clients of the service(s) of the Access
Control Subsystem. The following primitive is provided by the Access 
Control Subsystem to check if access is allowed:

statusInformation = -- success or errorIndication
*/

struct AccessCtrlSubSysIsAccessAllowed
{
    uint8_t securityModel; 	//Security Model in use
    uint8_t* securityName;	//principal who wants to access
    uint8_t securityLevel;	 	//Level of Security
    uint8_t viewType; 			//read, write, or notify view
    uint8_t* contextName; 	//context containing variableName
    uint8_t* variableName; 	//OID for the managed object
};


/*=======================================================================================*/

/* ==================== */
/* Security Subsystem Primitives  */
/* ==================== */

/*
The Message Processing Subsystem is the typical client of the services of the Security Subsystem.
*/


/*
Process Incoming Message

The Security Subsystem provides the following primitive to process an
incoming message:
statusInformation = -- errorIndication or success error counter OID/value if error

*/
typedef enum
{
    RESERVED=0x0,
    IPV4_ADDR_ENGN_ID=0x01,//4octets
    IPV6_ADDR_ENGN_ID=0x02,//16 octets
    MAC_ADDR_ENGN_ID=0x03,//6 octets
    ADMIN_ASSIGNED_TEXT=0x04,
    ADMIN_ASSIGNED_OCTETS=0x05,
    RESERVED_UNUSED=0x06, //6 to 127 are reserved and unused
    ENTERPRISE_DEFINED=128 //128 to 255 as defined by the enterprise maximum remaining length
}SNMP_ENGNID_OCTET_IDENTIFIER_VAL;	//The fifth octet indicates how the rest (6th and following octets) are formatted. Refer to RFC3411 section5 Page# 41	

struct SecuritySysGenerateRequestMsg
{
    uint8_t messageProcessingModel; 	//typically, SNMP version
    uint8_t* globalData; 				//message header, admin data
    uint32_t maxMessageSize; 			//of the sending SNMP entity
    uint8_t securityModel; 			//for the outgoing message
    uint8_t* securityEngineID ; 		//authoritative SNMP entity
    uint8_t* securityName;			//on behalf of this principal
    uint8_t securityLevel; 			//Level of Security requested
    uint8_t* scopedPDU; 				//message (plaintext) payload
    //OUT securityParameters; 		//filled in by Security Module
    uint8_t* wholeMsg ; 				//complete generated message
    uint32_t wholeMsgLength; 			//length of the generated message
};

/*
Generate a Response Message

The Security Subsystem provides the following primitive to generate a
Response message:

*/ 

struct SecuritySysGenerateResponseMsg
{
    uint8_t messageProcessingModel; 	//typically, SNMP version
    uint8_t* globalData; 				//message header, admin data
    uint32_t maxMessageSize; 			//of the sending SNMP entity
    uint8_t securityModel; 			//for the outgoing message
    uint8_t* securityEngineID; 		//authoritative SNMP entity
    uint8_t* securityName;			//on behalf of this principal
    uint8_t securityLevel; 			//for the outgoing message
    uint8_t* scopedPDU; 				//message (plaintext) payload
    uint8_t* wholeMsg;	 			//complete generated message
    uint32_t wholeMsgLength; 		//length of the generated message
};

/* 
Generate a Request or Notification Message

The Security Subsystem provides the following primitive to generate a
Request or Notification message:

*/

/*=======================================================================================*/

/* ============== */
/*  Common Primitives   */
/* ============== */

/*
These primitive(s) are provided by multiple Subsystems.
*/

/*
Release State Reference Information

All Subsystems which pass stateReference information also provide a
primitive to release the memory that holds the referenced state
information

*/

struct StateRelease
{
    uint32_t stateReference; 	//handle of reference to be released
};

// SNMPv3
typedef struct
{
    uint8_t *head;
    uint16_t length;
    uint16_t maxlength;
    uint16_t msgAuthParamOffset;
}SNMPV3MSGDATA;

typedef struct
{
    uint8_t* wholeMsgHead;
    uint8_t* snmpMsgHead;
    uint16_t wholeMsgLen;
    uint16_t snmpMsgLen;
    uint16_t msgAuthParamOffsetInWholeMsg;
    uint16_t scopedPduOffset;
    uint8_t scopedPduAuthStructVal;
    uint16_t scopedPduStructLen;
}SNMPV3_REQUEST_WHOLEMSG;

typedef struct
{
    uint8_t* wholeMsgHead;
    uint8_t* snmpMsgHead;
    uint16_t wholeMsgLen;
    uint16_t snmpMsgLen;
    uint8_t* msgAuthParamOffsetOutWholeMsg;
    uint8_t* scopedPduOffset;
    uint16_t scopedPduStructLen;
    uint8_t scopedPduAuthStructVal;
}SNMPV3_RESPONSE_WHOLEMSG;

/*
 snmpv3 target configuration with respect to trap.
*/
typedef struct
{
    uint8_t userSecurityName[TCPIP_SNMPV3_USER_SECURITY_NAME_LEN];
    STD_BASED_SNMP_MESSAGE_PROCESSING_MODEL messageProcessingModelType;
    STD_BASED_SNMP_SECURITY_MODEL securityModelType;
    STD_BASED_SNMPV3_SECURITY_LEVEL securityLevelType;
}snmpV3TrapConfigDataBase;

/*
Process Incoming Request or Notification PDU

Dispatcher provides the following primitive to pass an incoming SNMP PDU to an application.
*/

typedef struct
{
    uint8_t messageProcessingModel;	//typically, SNMP version
    uint8_t securityModel;				//Security Model in use
    uint8_t* securityName;			//on behalf of this principal
    uint8_t securityLevel;				//Level of Security
    uint8_t* contextEngineID;			//data from/at this SNMP entity
    uint8_t* contextName;				//data from/in this context
    uint8_t pduVersion;				//the version of the PDU
    uint8_t* PDU;						//SNMP Protocol Data Unit
    uint32_t maxSizeResponseScopedPDU;// maximum size of the Response PDU
    uint32_t stateReference;			//reference to state information needed when sending a response
}dispatcherProcessPdu;//process Request/Notification PDU

typedef struct
{
    uint32_t maxMessageSize; 			//of the sending SNMP entity
    uint32_t wholeMsgLength; 			//length as received on the wire
    uint8_t* wholeMsg; 				//as received on the wire
    uint8_t securityEngineID[32];	 		//authoritative SNMP entity
    uint8_t securityName[TCPIP_SNMPV3_USER_SECURITY_NAME_LEN]; 			//identification of the principal
    uint8_t* scopedPDU; 				//message (plaintext) payload
    uint32_t maxSizeResponseScopedPDU;//maximum size sender can handle
    uint8_t messageProcessingModel; 	//typically, SNMP version
    uint8_t securityModel; 			//for the received message
    uint8_t securityLevel; 			//Level of Security
    uint8_t securityEngineIDLen;	 		//authoritative SNMP entity
    uint8_t securityNameLength;
}SecuritySysProcessIncomingMsg;



typedef struct
{
    uint8_t userName[TCPIP_SNMPV3_USER_SECURITY_NAME_LEN];
    uint8_t userAuthPswd[TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN]; //RFC specifies not to save password with the managed nodes instead store pswd ipad and opad values.
    uint8_t userPrivPswd[TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN];
    uint8_t userAuthPswdLoclizdKey[TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN];
    uint8_t userPrivPswdLoclizdKey[TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN];
    uint8_t userAuthLocalKeyHmacIpad[64];
    uint8_t userAuthLocalKeyHmacOpad[64];
    uint8_t userDBIndex;
    uint8_t userHashType;
    uint8_t userNameLength;
    uint8_t userAuthPswdLen;
    uint8_t userPrivPswdLen;
    uint8_t userPrivType;
    STD_BASED_SNMPV3_SECURITY_LEVEL secLevel;
}snmpV3EngnUserDataBase;

// SNMPv3 Descriptor Structure Typedef
typedef struct
{
    uint16_t UserInfoDataBaseIndx;

    uint8_t SnmpEngineID[32]; //Reserving 32 bytes for the snmpEngineID as the octet string length can vary form 5 to 32 //**
    uint8_t SnmpEngnIDLength;//**

    uint16_t SnmpMsgBufSeekPos;
    uint16_t ScopedPduDataPos;//**

    uint32_t SnmpEngineTimeOffset;//**
    uint32_t SnmpEngineBoots;//The number of times that the SNMP engine has (re-)initialized itself since snmpEngineID was last configured.//**

    uint32_t    UsmStatsEngineID;  //**
    uint32_t    AuthoritativeSnmpEngineBoots;//**
    uint32_t    AuthoritativeSnmpEngnTime;//**
    uint32_t    IncmngSnmpPduMsgID;//**
    uint32_t    SnmpEngineTime;//The number of seconds since the value of the SnmpEngineBoots object last changed//**
    uint32_t    SnmpEngnMaxMsgSize;//**

    SNMPV3_REQUEST_WHOLEMSG InPduWholeMsgBuf;//**
    SNMPV3_RESPONSE_WHOLEMSG OUTPduWholeMsgBuf;//**
    SNMPV3_RESPONSE_WHOLEMSG TrapOUTPduWholeMsgBuf;

    //snmv3 global database for trap table
    snmpV3TrapConfigDataBase Snmpv3TrapConfigData[TCPIP_SNMPV3_USM_MAX_USER];

    //SNMPV3MSGDATA ScopedPduRequstBuf;
    SNMPV3MSGDATA ScopedPduRespnsBuf;
    SNMPV3MSGDATA PduHeaderBuf;
    SNMPV3MSGDATA TrapMsgHeaderBuf;
    SNMPV3MSGDATA TrapScopdPduRespnsBuf;

    dispatcherProcessPdu incomingPdu;

    uint8_t  SnmpSecurityLevel;
    uint8_t  SnmpRespnsSecrtyFlg;

    uint8_t SnmpInMsgAuthParmStrng[SNMPv3_MSG_AUTH_PARAM_STRING_LEN+1];
    uint8_t SnmpInMsgAuthParamLen;
    uint8_t snmpInMsgPrvParamStrng[SNMPv3_MSG_PRIV_PARAM_STRING_LEN+1];
    uint8_t SnmpInMsgPrivParmLen;

    uint8_t SnmpOutMsgAuthParaStrng[SNMPv3_MSG_AUTH_PARAM_STRING_LEN+1];
    uint8_t SnmpOutMsgAuthParmLen;
    uint8_t SnmpOutMsgPrvParmStrng[SNMPv3_MSG_PRIV_PARAM_STRING_LEN+1];
    uint8_t SnmpOutMsgPrivParmLen;

    uint32_t SnmpEngnSecurityModel;//Maximum range (2^31-1), RFC3411
    uint32_t SnmpEngnMsgProcessModel;//Maximum range (2^31-1), RFC3411

    SecuritySysProcessIncomingMsg SecurtyPrimtvesOfIncmngPdu;//**

    snmpV3EngnUserDataBase UserInfoDataBase[TCPIP_SNMPV3_USM_MAX_USER];//**

} SNMPV3_STACK_DCPT_STUB;


// SNMPv3 Processing Memory Pointers
typedef struct
{
    SNMPV3_STACK_DCPT_STUB * snmpv3StkProcessingDynMemStubPtr;
    const void* snmpHeapMemHandler;

} SNMPV3_PROCESSING_MEM_INFO_PTRS;


/* ================= */
/*  SNMPv3 Core Stack APIs   */
/* ================= */




void SNMPv3USMOutMsgPrivParam(SNMPV3_PRIV_PROT_TYPE privType);
void SNMPv3InitializeUserDataBase(void);
void TCPIP_SNMPv3_DynAllocMemFree(void);
void TCPIP_SNMPv3_AuthEngineTimeGet(void);
void SNMPv3UsmAesEncryptDecrptInitVector(uint8_t inOutPdu);
void SNMPv3UsmOutMsgAuthParam(uint8_t hashType);

uint8_t SNMPv3AESDecryptRxedScopedPdu(void);
uint8_t TCPIP_SNMPv3_ProcessBuffDataGet(SNMPV3MSGDATA getbuf,uint16_t pos);
bool TCPIP_SNMPv3_TrapScopedPDU(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index,uint8_t targetIndex);
uint8_t TCPIP_SNMPv3_TrapSecurityLevelGet(STD_BASED_SNMPV3_SECURITY_LEVEL securityLevel);
uint8_t SNMPv3GetSecurityLevel(uint8_t userIndex);
uint8_t SNMPv3AESEncryptResponseScopedPdu(SNMPV3_RESPONSE_WHOLEMSG* plain_text);
uint8_t SNMPv3DESDecryptRxedScopedPdu(void);
uint8_t SNMPv3DESEncryptResponseScopedPdu(SNMPV3_RESPONSE_WHOLEMSG* plain_text);
uint8_t SNMPv3AuthenticateTxPduForDataIntegrity(SNMPV3_RESPONSE_WHOLEMSG* txDataPtr);
uint8_t SNMPv3AuthenticateRxedPduForDataIntegrity(void);
 
SNMP_ERR_STATUS TCPIP_SNMPv3_MsgProcessingModelProcessPDU(INOUT_SNMP_PDU inOutPdu);
SNMP_ERR_STATUS TCPIP_SNMPv3_UserSecurityModelProcessPDU(INOUT_SNMP_PDU inOutPdu);
SNMP_ERR_STATUS TCPIP_SNMPv3_ScopedPDUProcessing(INOUT_SNMP_PDU inOutPdu);

void _SNMPv3_EngnIDFormulate(uint8_t fifthOctectIdentifier,TCPIP_NET_IF* snmpIntf);
bool SNMPv3ValidateSecNameAndSecLevel(void);
bool SNMPv3ValidateUserSecurityName(void);
bool SNMPv3ValidateSnmpEngnId(void);
bool TCPIP_SNMPv3_V3MsgDataProcess(PDU_INFO* pduDbPtr,uint8_t * headWrPtr);
bool TCPIP_SNMPv3_DataCopyToProcessBuff(const uint8_t val ,SNMPV3MSGDATA *putbuf);
/****************************************************************************
  Function:
    bool TCPIP_SNMPv3_CmprTrapSecNameAndSecLvlWithUSMDb(
                uint8_t tragetIndex,
                uint8_t userTrapSecLen,
                uint8_t *userTrapSecurityName,
                STD_BASED_SNMPV3_SECURITY_LEVEL securityLevel)

  Summary:
    Routine to find the index of the user name in the user data base table.

  Description:
    There are two different data base tables defined with SNMPv3 stack,
    like 'UserInfoDataBase' and 'Snmpv3TrapConfigData'.
    This routine is used to validate the trap user security level setting with
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
 */
bool TCPIP_SNMPv3_CmprTrapSecNameAndSecLvlWithUSMDb(uint8_t tragetIndex,
                                                    uint8_t userTrapSecLen,
                                                    uint8_t *userTrapSecurityName,
                                                    STD_BASED_SNMPV3_SECURITY_LEVEL securityLevel);

/****************************************************************************
Function:
    void TCPIP_SNMPV3_PacketProcStubPtrsGet( SNMPV3_PROCESSING_MEM_INFO_PTRS * dynMemInfoPtr);

Summary:
    Get SNMPv3 packet processing memory pointer.

Description:
    This function is used to get dynamic memory allocation pointer details which is used for
    SNMPv3 packet processing.

Precondition:
    TCPIP_SNMP_Initialize() is already called.

Parameters:
    dynMemInfoPtr  - Dynamic memory pointer for packet processing

Return Values:
    None.

Remarks:
    The source code for this routine is found in snmp.c, not snmpv3.c.
*/
void TCPIP_SNMPV3_PacketProcStubPtrsGet( SNMPV3_PROCESSING_MEM_INFO_PTRS * dynMemInfoPtr);

/****************************************************************************
  Function:
    void SNMPv3ComputeHMACIpadOpadForAuthLoclzedKey(uint8_t userDBIndex)

  Summary:
    Compute HMAC inner and outer pad for authorization localized key.

  Description:
    This routine computes HMAC inner and outer pad strings for authorization localized key.
    RFC - 2104.

  Precondition:
    TCPIP_SNMPv3_Initialize() and ProcessVariabels() are called.

  Parameters:
    userDBIndex -  password storage pointer

  Return Values:
    None

  Remarks:
    None
 */
void SNMPv3ComputeHMACIpadOpadForAuthLoclzedKey(uint8_t userDBIndex);




/****************************************************************************
  Function:
    void SNMPv3USMAuthPrivPswdLocalization(uint8_t userDBIndex)

  Summary:
    Convert Auth and Priv  password to the localized Key using SNMPEngineID.

  Description:
    This routine converts MD5 or SHA1 and AES privacy password key
    to localized key using snmpSngineID(RFC- 3414 - section 6).

  Precondition:
    TCPIP_SNMPv3_Initialize() and ProcessVariabels() are called.

  Parameters:
    userDBIndex  - authentication protocol type

  Return Values:
    None

  Remarks:
    None
 */
void SNMPv3USMAuthPrivPswdLocalization(uint8_t userDBIndex);

uint16_t _SNMPv3_Header_Length(void);
#endif 


