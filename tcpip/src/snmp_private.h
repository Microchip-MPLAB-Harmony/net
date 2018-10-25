/*******************************************************************************
  SNMP Stack private APIs

  Company:
    Microchip Technology Inc.
    
  File Name:
    snmp_private.h

  Summary:
    SNMP Stack private API for Microchip TCP/IP Stack
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
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








// DOM-IGNORE-END

#ifndef _SNMP_PRIVATE_H_
#define _SNMP_PRIVATE_H_


/*
 * This macro is used to calculate the LENGTH from a Integer  Signed value.
 */
#define SNMP_MASK_FF8 (0xFF800000)


// Section:  SNMP specific variables
#define STRUCTURE               (0x30u)
#define ASN_INT                 (0x02u)
#define OCTET_STRING            (0x04u)
#define ASN_NULL                (0x05u)
#define ASN_OID                 (0x06u)


#define SNMP_IP_ADDR            (0x40)
#define SNMP_COUNTER32          (0x41)
#define SNMP_GAUGE32            (0x42)
#define SNMP_TIME_TICKS         (0x43)
#define SNMP_OPAQUE             (0x44)
#define SNMP_NSAP_ADDR          (0x45)



// Section:  SNMP v1 and v2c PDU types
#define GET_REQUEST             (0xa0)
#define GET_NEXT_REQUEST        (0xa1)
#define GET_RESPONSE            (0xa2)
#define SET_REQUEST             (0xa3)
#define TRAP                    (0xa4)
#define GET_BULK_REQUEST        (0xa5)
#define REPORT_RESPONSE		(0xa8)


// Section:  SNMP UDP ports
#define SNMP_AGENT_PORT     (161)
#define SNMP_NMS_PORT       (162)
#define AGENT_NOTIFY_PORT   (0xfffe)


// Section:  SNMP specific data validation
#define IS_STRUCTURE(a)         (a==STRUCTURE)
#define IS_ASN_INT(a)           (a==ASN_INT)
#define IS_OCTET_STRING(a)      (a==OCTET_STRING)
#define IS_OID(a)               (a==ASN_OID)
#define IS_ASN_NULL(a)          (a==ASN_NULL)
#define IS_GET_REQUEST(a)       (a==GET_REQUEST)
#define IS_GET_NEXT_REQUEST(a)  (a==GET_NEXT_REQUEST)
#define IS_GET_RESPONSE(a)      (a==GET_RESPONSE)
#define IS_SET_REQUEST(a)       (a==SET_REQUEST)
#define IS_TRAP(a)              (a==TRAP)
#define IS_AGENT_PDU(a)         (a==GET_REQUEST || \
                                 a==GET_NEXT_REQUEST || \
                                 a==SET_REQUEST || \
                                 a==SNMP_V2C_GET_BULK)
#define IS_SNMPV3_AUTH_STRUCTURE(a) (a==SNMPV3_ENCRYPTION)





// Section:  SNMP specific errors
typedef enum
{
    SNMP_NO_ERR = 0,			//SNMP no error
    SNMP_TOO_BIG,			    //Value too big error
    SNMP_NO_SUCH_NAME,			//No such name in MIB error
    SNMP_BAD_VALUE,			    //Not assignable value for the var error
    SNMP_READ_ONLY,			    //Read-only variable, write not allowed err
    SNMP_GEN_ERR,			    //SNMP gen error
    SNMP_NO_ACCESS,			    //Access to modify or read not granted err
    SNMP_WRONG_TYPE,			//Variable data type wrong error
    SNMP_WRONG_LENGTH,			//Wrong data length error
    SNMP_WRONG_ENCODING,		//Wrong encoding error
    SNMP_WRONG_VALUE,			//Wrong value for the var type
    SNMP_NO_CREATION,			//No creating error
    SNMP_INCONSISTENT_VAL,		//Inconsistent value error
    SNMP_RESOURCE_UNAVAILABE,   //Resource unavailable error
    SNMP_COMMIT_FAILED,			//Modification update failed error
    SNMP_UNDO_FAILED,			//Modification undo failed
    SNMP_AUTH_ERROR,			//Authorization failed error
    SNMP_NOT_WRITABLE,			//Variable read only
    SNMP_INCONSISTENT_NAME,		//Inconsistent name
    SNMP_NO_SUCH_OBJ=128,		//No such object error
    SNMP_NO_SUCH_INSTANCE=129,  //No such instance error
    SNMP_END_OF_MIB_VIEW=130,   //Reached to end of MIB error
    SNMP_PRIVACY_ERROR          // SNMP Privacy error
} SNMP_ERR_STATUS;


// Section:  SNMP specific data types
typedef enum
{
    INT8_VAL		= 0x00, 	//8 bit integer value
    INT16_VAL		= 0x01, 	//16 bit integer value
    INT32_VAL		= 0x02, 	//32 bit integer value
    BYTE_ARRAY		= 0x03, 	//Array of bytes
    ASCII_STRING	= 0x04, 	//ASCII string type
    IPADDRESS		= 0x05, 	//IP address variable
    COUNTER32		= 0x06, 	//32 bit counter variable
    TIME_TICKS_VAL	= 0x07, 	//Timer value counter variable
    GAUGE32 		= 0x08, 	//32-bit guage variable
    OID_VAL 		= 0x09, 	//Object id value var
    DATA_TYPE_UNKNOWN			//Unknown data type
} SNMP_DATA_TYPE;


// Section:  ASN data type info
typedef struct
{
    uint8_t asnType;	//ASN data type
    uint8_t asnLen;	//ASN data length
} SNMP_DATA_TYPE_INFO;

typedef enum
{
    SNMP_HOME = 0,
    SNMP_LISTEN,
    SNMP_PROCESS,
    SNMP_PACKET_DISCARD,
}TCPIP_SNMP_SM;

// Section:  SNMP PDU information database
typedef struct
{
    uint32_t    requestID;		//SNMP request id
    uint8_t	nonRepeators;	    //# non repeaters in the request
    uint8_t	maxRepetitions;     //# max repeaters in the request
    uint8_t	pduType;		    //SNMP PDU type
    uint8_t	errorStatus;	    //PDU error status
    uint8_t	erroIndex;		    //PDU error Index
    uint8_t	snmpVersion;	    //SNMP version
    uint16_t	pduLength;		//PDU length
} PDU_INFO;

typedef struct
{
    // SNMPv2C Read community names
    // TCPIP_SNMP_COMMUNITY_MAX_LEN (8) + 1 null termination byte
    uint8_t readCommunity[TCPIP_SNMP_MAX_COMMUNITY_SUPPORT][TCPIP_SNMP_COMMUNITY_MAX_LEN+1];

    // SNMPv2C Write community names
    // TCPIP_SNMP_COMMUNITY_MAX_LEN (8) + 1 null termination byte
    uint8_t writeCommunity[TCPIP_SNMP_MAX_COMMUNITY_SUPPORT][TCPIP_SNMP_COMMUNITY_MAX_LEN+1];

    uint32_t SnmpEngineBootRcrd;
}SNMP_NET_CONFIG;



//the only if that runs SNMP
// Section:  SNMP trap notification information for agent
typedef struct
{
    char community[TCPIP_SNMP_NOTIFY_COMMUNITY_LEN]; //Community name array
    uint8_t communityLen;		      //Community name length
    SNMP_ID agentIDVar; 		      //Agent id for trap identification
    uint8_t notificationCode;		  //Trap notification code
    UDP_SOCKET socket;			      //UDP socket number
#ifdef TCPIP_STACK_USE_IPV6
    UDP_SOCKET socketv6;
#endif
    uint32_t    timestamp;		      //Time stamp for trap
//#if defined(SNMP_STACK_USE_V2_TRAP) || defined(SNMP_V1_V2_TRAP_WITH_SNMPV3)
    SNMP_ID trapIDVar;			      // SNMPV2 specific trap
//#endif
    TCPIP_NET_HANDLE snmpTrapInf;     // interface we use for the SNMP TRAP
} SNMP_NOTIFY_INFO;



/****************************************************************************
  Section:
	Data Structures and Enumerations
  ***************************************************************************/

// Section:  SNMP object information
typedef union
{
    struct
    {
        unsigned int bIsDistantSibling : 1; //Object has distant sibling node
        unsigned int bIsConstant : 1;		//Object is constant
        unsigned int bIsSequence : 1;		//Object is sequence
        unsigned int bIsSibling : 1;		//Sibling node flag

        unsigned int bIsParent : 1; 		//Node is parent flag
        unsigned int bIsEditable : 1;		//Node is editable flag
        unsigned int bIsAgentID : 1;		//Node has agent id flag
        unsigned int bIsIDPresent : 1;		//Id present flag
    } Flags;
    uint8_t Val;    //MIB Obj info as byte value
} MIB_INFO;


// Section:  SNMP specific MIB file access information
typedef union
{
    struct
    {
        unsigned char bIsFileOpen : 1;  //MIB file access int flag
        unsigned char bIsSnmpIntfUp : 1;  // SNMP Interface is UP and this flag is used to initialize SNMPv3
    } Flags;
    uint8_t Val;
} SNMP_STATUS;


// Section:  SNMP OID index information
typedef union 
{
    struct
    {
        unsigned int bIsOID:1;	//value is OID/index int flag
    } Flags;
    uint8_t Val;					//value is OID/index byte flag
} SNMP_INDEX_INFO;



// Section:  SNMP requested variable list error status information.
//Max variable in a request supported 15
typedef struct 
{
    uint16_t noSuchObjectErr;		//Var list no such obj errors flags
    uint16_t noSuchNameErr; 		//Var list no such name error
    uint16_t noSuchInstanceErr; 	//Var list no such instance error
    uint16_t endOfMibViewErr;		//Var list end of MIB view error
}reqVarErrStatus;  

//This is the list of SNMP action a remote NMS can perform.
//This information is passed to application via
//callback TCPIP_SNMP_IsValidCommunity().
//Application should validate the action for given community
//string.
typedef enum
{
    SNMP_GET            = 0xa0,	//SNMP GET identifier
    SNMP_GET_NEXT       = 0xa1, //SNMP GET_NEXT identifier
    SNMP_GET_RESPONSE   = 0xa2,	//SNMP GET_RESPONSE identifier
    SNMP_SET            = 0xa3,	//SNMP SET identifier
    SNMP_TRAP           = 0xa4,	//SNMP TRAP identifier
    SNMP_V2C_GET_BULK	= 0xa5,	//SNMP GET_BULK identifier
    SNMP_V2_TRAP		= 0xa7, //SNMP v2 Trap Identifier
    SNMPV3_ENCRYPTION	= 0x04,
    SNMP_ACTION_UNKNOWN = 0	//SNMP requested action unknown
} SNMP_ACTION;


typedef enum
{
    SNMP_RESPONSE_PDU=0x01,
    SNMP_REQUEST_PDU=0x02,
}INOUT_SNMP_PDU;

typedef struct 
{	
    uint8_t* wholeMsgHead;
    uint8_t* snmpMsgHead;
    uint16_t wholeMsgLen;
    uint16_t snmpMsgLen;
}SNMPV1V2_REQUEST_WHOLEMSG;

// This one is uesd for UDPGET and UDPPUT
typedef struct
{
    uint8_t *head;      // probably this is the allocated structure
    uint8_t* wrPtr;     // current write pointer, initialized to head when buffer allocated
    uint8_t* endPtr;    // end pointer, initialized to head + len when buffer allocated
}TCPIP_SNMP_DATABUF;
// Section:  SNMP MIB variable object information
typedef struct
{
    uint32_t		hNode;		//Node location in the MIB
    uint8_t		oid;		//Object Id
    MIB_INFO		nodeInfo;	//Node info
    SNMP_DATA_TYPE	dataType;	//Data type
    SNMP_ID 		id; 		//SNMP Id
    uint16_t    	dataLen;	//Data length
    uint32_t		hData;		//Data
    uint32_t		hSibling;	//Sibling info
    uint32_t		hChild; 	//Child info
    uint8_t		index;		//Index of object
    uint8_t		indexLen;	//Index length
} OID_INFO;

typedef struct
{
    TCPIP_SNMP_SM   sm;     // current status
    UDP_SOCKET      skt;    // associated socket
    bool readFromSnmpBuf;
    TCPIP_SNMP_DATABUF udpGetBufferData; // this is assigned by Local buffer
    tcpipSignalHandle snmpSignalHandle;
#if defined (TCPIP_STACK_USE_IPV6)
    IPV6_HANDLE snmpIPV6Handler;
    IPV6_EVENT_TYPE ipv6EventType;
#endif
    bool trapEnable;    /* true = agent can send the trap, flase= agent shouldn't send the trap */
    bool snmp_trapv2_use; /* true = agent uses Trap version v2 and false = uses Tarp version 1*/
    bool snmpv3_trapv1v2_use; /* SNMPv3 trap should be true , only if SNMP version is 3 */
    TCPIP_NET_IF* pSnmpIf;
}TCPIP_SNMP_DCPT;


// SNMPv1/v2c
typedef struct
{
    uint8_t *head;
    uint16_t length;
}SNMP_BUFFER_DATA;

/****************************************************************************
  Section:
	Global Variables
  ***************************************************************************/

typedef struct
{
    uint8_t gOIDCorrespondingSnmpMibID;

//#if defined(SNMP_STACK_USE_V2_TRAP)
    uint8_t	gSetTrapSendFlag;
//#endif

    bool getZeroInstance;
    uint8_t appendZeroToOID;

    bool gSendTrapFlag;
    uint8_t gGenericTrapNotification;
    uint8_t gSpecificTrapNotification;
    SNMP_NOTIFY_INFO SNMPNotifyInfo; //notify info for trap
    SNMP_BUFFER_DATA outPduBufData;

    SNMP_BUFFER_DATA trapPduOutBufData;
    SNMP_NET_CONFIG snmpNetConfig;

}SNMP_STACK_DCPT_STUB;


typedef struct
{
    SNMP_STACK_DCPT_STUB* snmpStkDynMemStubPtr;
    const void* snmpHeapMemHandler;
    TCPIP_SNMP_DCPT* snmpDcptPtr;
}SNMP_PROCESSING_MEM_INFO_PTRS;

uint8_t *TCPIP_SNMP_GenericTrapCodeToTrapOID(uint8_t generic_trap_code,uint8_t *len);

bool TCPIP_SNMP_VarDataTypeIsValidInteger(uint32_t* val);
bool TCPIP_SNMP_NextLeafGet(int32_t fileDescr,OID_INFO* rec);
bool TCPIP_SNMP_OIDStringFindByID(int32_t fileDescr,SNMP_ID id, OID_INFO* info, uint8_t* oidString, uint8_t* len);
bool TCPIP_SNMP_PDUProcessDuplexInit(UDP_SOCKET socket);
bool TCPIP_SNMP_DataTypeInfoGet(SNMP_DATA_TYPE dataType, SNMP_DATA_TYPE_INFO *info );


uint8_t TCPIP_SNMP_ProcessGetVar(OID_INFO* rec, bool bAsOID,PDU_INFO* pduDbPtr);
uint8_t TCPIP_SNMP_ProcessGetNextVar(OID_INFO* rec,PDU_INFO* pduDbPtr);
uint8_t TCPIP_SNMP_ProcessSetVar(PDU_INFO* pduDbPtr,OID_INFO* rec, SNMP_ERR_STATUS* errorStatus);
uint8_t TCPIP_SNMP_ProcessGetBulkVar(OID_INFO* rec, uint8_t* oidValuePtr, uint8_t* oidLenPtr,uint8_t* successor,PDU_INFO* pduDbPtr);
uint8_t TCPIP_SNMP_ReadByte(UDP_SOCKET skt);
uint8_t TCPIP_SNMP_LengthIsValid(uint16_t *len);
uint8_t TCPIP_SNMP_StructureIsValid(uint16_t* dataLen);
uint8_t TCPIP_SNMP_OIDFindInMgmtInfoBase(int32_t fileDescr,PDU_INFO* pduDbPtr,uint8_t* oid, uint8_t oidLen, OID_INFO* rec);


void TCPIP_SNMP_PutByte(UDP_SOCKET socket,uint8_t v);

/*
* Set the File Descriptor
*/
void TCPIP_SNMP_FileDescrSet(int32_t fileDescr);

/*
*  Get The File Descriptor
*/
int32_t TCPIP_SNMP_FileDescrGet(void);

/****************************************************************************
  Function:
    void TCPIP_SNMP_PacketProcStubPtrsGet( SNMP_PROCESSING_MEM_INFO_PTRS * dynMemInfoPtr)

  Summary:
    Get SNMP packet processing memory pointer.

  Description:
    This function is used to get dynamic memory allocation pointer details which is used for SNMP
    packet processing.

  Precondition:
    TCPIP_SNMP_Initialize() is already called.

  Parameters:
    dynMemInfoPtr  - Dynamic memory pointer for packet processing

  Return Values:
    None.

  Remarks:
    None.
*/
void TCPIP_SNMP_PacketProcStubPtrsGet( SNMP_PROCESSING_MEM_INFO_PTRS * dynMemInfoPtr);

/****************************************************************************
  Function:
    uint8_t TCPIP_SNMP_ProcessBufferDataGet(SNMP_BUFFER_DATA getbuf,uint16_t pos)

  Summary:
    Reads uint8_t data from dynamically allocated memory buffer.

  Description:
    The SNMPstack implementation uses dynamically allocated memory buffer for
    processing of request and response packets. This routine reads the uint8_t data from
    the allocated buffer at the positions (offset) provided.

  Precondition:
    The SNMPv1 and v2c stack has successfully allocated dynamic memory buffer from the Heap

  Parameters:
    getbuf: Structure from where to read the data byte.
    pos: position in the buffer from which the data to be read

  Return Values:
    uint8_t: 1 byte value read

  Remarks:
    The read position offset is required to be provided every time the routine is called.
    This API do not increment the buffer read offset automatically, every time it is called.

 */
uint8_t TCPIP_SNMP_ProcessBufferDataGet(SNMP_BUFFER_DATA getbuf,uint16_t pos);


/****************************************************************************
  Function:
    bool TCPIP_SNMP_DataCopyToProcessBuffer(uint8_t val ,SNMP_BUFFER_DATA *putbuf)

  Summary:
    Copies uint8_t data to dynamically allocated memory buffer.

  Description:
    The SNMPv1 and v2c stack implementation uses dynamically allocated memory buffer for
    processing of request and response packets. This routine copies the uint8_t data to the
    allocated buffer and updates the offset length counter.

  Precondition:
    The SNMPv1 and v2c stack has successfully allocated dynamic memory buffer from the Heap

  Parameters:
    val: uint8_t value to be written to the buffer
    putbuf: pointer to the dynamically allocated buffer to which the 'val' to be written

  Return Values:
    true: if successfully write to the buffer
    false: failure in writing to the buffer

  Remarks:
    This routine is used by the SNMP stack. If required to be used by the application
    code, valid pointers should be passed to this routine.

 */
bool TCPIP_SNMP_DataCopyToProcessBuffer(uint8_t val ,SNMP_BUFFER_DATA *putbuf);

//****************************************************************************
/*
  Function:
    IPV6_EVENT_TYPE TCPIP_SNMP_EventNotifyGet(TCPIP_NET_HANDLE hNet);
	
  Summary:
    To Get the IPv6 DHCP event notification.
  	
  Description:
    This routine is used to get the DHCP event with IPv6 notification if the IPv6 unicast address
    is updated.
  	
  Precondition	:
    TCPIP_SNMP_Initialize() is called.
		
  Parameters:
    hNet - Interface    
  Return Values:
    IPV6_EVENT_TYPE
	
  Remarks:
  	None.
***************************************************************************/

IPV6_EVENT_TYPE TCPIP_SNMP_EventNotifyGet(TCPIP_NET_HANDLE hNet);

uint8_t TCPIP_SNMP_GetDataFromUDPBuff(TCPIP_SNMP_DATABUF *getbuf);

int TCPIP_SNMP_GetArrayOfDataFromUDPBuff(TCPIP_SNMP_DATABUF *getbuf,int bytes,uint8_t *buf);
bool TCPIP_SNMP_OIDIsValid(uint8_t* oid, uint8_t* len);
bool TCPIP_SNMP_DataIsASNNull(void);
void TCPIP_SNMP_FreeMemory(void);
uint32_t TCPIP_SNMP_GetRXOffset(void);
void TCPIP_SNMP_SetRXOffset(uint32_t offset);
void TCPIP_SNMP_CopyOfDataToINUDPBuff(TCPIP_SNMP_DATABUF *getbuf,int val);
#endif 

