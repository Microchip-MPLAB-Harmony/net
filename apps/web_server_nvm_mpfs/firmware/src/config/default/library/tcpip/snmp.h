/*******************************************************************************
  SNMP Definitions for the Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    snmp.h

  Summary:
    Simple Network Management Protocol(SNMP) v1/v2c API header file.
    
  Description:
    Simple Network Management Protocol (SNMP) is one of the key components
	of a Network Management System (NMS). SNMP is an application layer protocol
	that facilitates the exchange of management information among network devices.
	It is a part of the TCP/IP protocol suite. The SNMP Manager uses the UDP
	Port Number 161 to send requests to the agent. The agent uses the UDP Port
	Number 162 to send Trap(s) or notification events to the manager.
	- Supports SNMP Version V1 and V2c over User Datagram Protocol (UDP)
	- Supports Get, Get_Bulk, Get_Next, Set and Trap Protocol Data Units (PDUs)
	- Supports up to 255 dynamic OIDs and unlimited constant OIDs
	- mib2bib.jar utility is Microchip MIB compiler. It accepts Microchip MIB script
	 in ASCII format and generates two output files: The binary information file,
	 snmp.bib and the C header file , mib.h.
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2012-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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

#ifndef __SNMP_H
#define __SNMP_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: SNMP Agent Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************

/* This Macro is used for both SNMP SET and GET Variable processing to indicate 
   the start of SNMP variable processing.
   For multi byte data request, the first byte will be always SNMP_START_OF_VAR.  */
#define SNMP_START_OF_VAR       (0)

/* This Macro is used for both SNMP SET and GET Variable processing to indicate 
   the end of SNMP variable processing.
   For multi byte data request, the end byte will be always SNMP_END_OF_VAR.  */
#define SNMP_END_OF_VAR         (0xff)

/* This Macro is used for both SNMP SET and GET Sequence Variable processing.
   SNMP starts processing the start of sequence variable with Invalid index.
   TCPIP_SNMP_ExactIndexGet and TCPIP_SNMP_NextIndexGet returns a valid index as 
   per SNMP_INDEX_INVALID. */
#define SNMP_INDEX_INVALID      (0xff)

/* This macro is used for SNMP version 1 */
#define SNMP_V1                 (0)

/* This macro is used for SNMP version 2 with community */
#define SNMP_V2C		(1)

/*This macro is used for SNMP version 3 with authentication and privacy  */
#define SNMP_V3			(3)


// *****************************************************************************
/*
  Type:
    SNMP_ID

  Summary:
    SNMP dynamic variable ID.

  Description:
    Only dynamic and AgentID variables can contain a dyanmic ID.  MIB2BIB utility 
	enforces this rule when BIB was generated. All the dynamic IDs are are listed 
	in mib.h. The maximum value of a dynamic ID is 1023.

  Remarks:
    mib2bib.jar utility generates mib.h  and snmp.bib from Microchip MIB script. 
	DynamicVar - This command declares defined OID variable as dynamic.
	Syntax - $DynamicVar(oidName, id).
*/
typedef uint32_t 	SNMP_ID;

// *****************************************************************************
/*
  Type:
    SNMP_INDEX

  Summary:
    SNMP sequence variable index.

  Description:
    The current version limits the size of the index to 7 bits wide, 
	meaning that such arrays can contain up to 127 entries. 

  Remarks:
    SequenceVar - This command is part of Microchip MIB script declares a previously 
	defined OID variable as a sequence variable and assigns an index to it. A sequence 
	variable can contain an array of values and any instance of its values can be 
	referenced by an index. More than one sequence variable may share a single index, 
	creating multi dimensional arrays. 
	
	More than one index variable is not supported by mib2bib.jar compiler. 
*/
typedef uint8_t 	SNMP_INDEX;

// *****************************************************************************
/*
  Enumeration:
    SNMP_TRAP_IP_ADDRESS_TYPE

  Summary:
    Definition of the supported address types for SNMP trap.

  Description:
    SNMP agent supports both IPv4 and IPv6  trap address type and is able to transmit 
	traps to both IPv4 and IPv6 Host receiver address.
	
  Remarks:
    None.
*/
typedef enum
{
    IPV4_SNMP_TRAP=1,
    IPV6_SNMP_TRAP,
}SNMP_TRAP_IP_ADDRESS_TYPE;

// *****************************************************************************
/*
  Union:
    SNMP_VAL

  Summary:
    Definition to represent SNMP OID object values.

  Description:
    SNMP agent processes different variable types.
	
  Remarks:
    None.
*/
typedef union
{
    uint32_t dword;					//double word value
    uint16_t word;					//word value
    uint8_t  byte;					//byte value
    uint8_t  v[sizeof(uint32_t)];	//byte array
} SNMP_VAL;


// *****************************************************************************
/*
  Enumeration:
    SNMP_COMMUNITY_TYPE

  Summary:
    Definition to represent different type of SNMP communities.

  Description:
    List of different SNMP community types.
	
  Remarks:
    SNMP agent use these community types  for both TRAP and SNMP agent request.
*/
typedef enum
{
    READ_COMMUNITY=1,		//Read only community
    WRITE_COMMUNITY=2,		//Read write community
    INVALID_COMMUNITY=3		//Community invalid
}SNMP_COMMUNITY_TYPE;

// *****************************************************************************
/*
  Enumeration:
    SNMP_GENERIC_TRAP_NOTIFICATION_TYPE

  Summary:
    Definition to represent different SNMP generic trap types.

  Description:
    List of different SNMP specific Notification types.
	
  Remarks:
    ENTERPRISE_SPECIFIC and AUTH_FAILURE are used while sending specific trap.  
*/
typedef enum
{
	/*Controls the sending of SNMP coldstart notifications. A coldStart(0) trap 
	signifies that the sending protocol entity is reinitializing itself such that 
	the configuration of the agent or the protocol entity implementation might be altered. */
    COLD_START			=0x0,
	
	/*Controls the sending of SNMP warmstart notifications. A warmStart(1) trap signifies 
	that the sending protocol entity is reinitializing itself so that neither the agent
	configuration nor the protocol entity implementation can be altered.*/
    WARM_START			=0x1,
	
	/*Controls the how SNMP linkdown notifications are sent. A linkDown(2) trap signifies
	that the sending protocol entity recognizes a failure in one of the communication links 
	represented in the configuration of the agent. */
    LINK_DOWN			=0x2,
	
	/*Controls the sending of SNMP linkup notifications. A linkUp(3) trap signifies
	that the sending protocol entity recognizes that one of the communication links 
	represented in the configuration of the agent has come up. */
    LINK_UP			=0x3,
	
	/*Controls the distribution of SNMP authentication failure notifications. An authenticationFailure(4)
	trap signifies that the sending protocol entity is the addressee of a protocol message that is not 
	properly authenticated. Like Community Name authentication failure*/
    AUTH_FAILURE		=0x4,
	
	/*Controls the distribution of SNMP egpNeighborLoss notifications.An egpNeighborLoss(5) trap signifies 
	that an EGP neighbor for whom the sending protocol entity was an EGP peer has been marked down and 
	the peer relationship no longer exists. */
    EGP_NEIGHBOR_LOSS		=0x5,
	
	/*Controls the distribution of SNMP enterprise/-specific notifications.An enterpriseSpecific(6) trap 
	signifies that the sending protocol entity recognizes that some enterprise/-specific event 
	has occurred. The specific-trap field identifies the particular trap that occurred. */
    ENTERPRISE_SPECIFIC         =0x6
} SNMP_GENERIC_TRAP_NOTIFICATION_TYPE; 

// *****************************************************************************
/*
  Enumeration:
    SNMP_VENDOR_SPECIFIC_TRAP_NOTIFICATION_TYPE

  Summary:
    Definition to represent different SNMP vendor trap types.

  Description:
    List of different SNMP Vendor Notification types.
	
  Remarks:
    None.
*/
typedef enum
{
    VENDOR_TRAP_DEFAULT 	=0x0, // Default trap . Agent send  use this trap when  
	                              // there is authentication failure.
    BUTTON_PUSH_EVENT		=0x1, // PUSH button event trap notification
    POT_READING_MORE_512	=0x2  // Analog POT meter event trap notification
} SNMP_VENDOR_SPECIFIC_TRAP_NOTIFICATION_TYPE;

// *****************************************************************************
/*
  Structure:
    SNMP_NON_MIB_RECD_INFO

  Summary:
    Restrict access for specific OIDs.

  Description:
    This structure is used to restrict access to static variables of SNMPv3 OIDs 
	from SNMPv2c and SNMPv1 version.OID string length is restricted to 16. 
	
  Remarks:
    None.
*/
typedef struct
{
    uint8_t oidstr[16];
    uint8_t version;
}SNMP_NON_MIB_RECD_INFO;

// *****************************************************************************
// *****************************************************************************
// Section: SNMP GET and SET Request Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    TCPIP_NET_HANDLE TCPIP_SNMP_ClientGetNet(int *netIx,TCPIP_NET_HANDLE hNet);

  Summary:
    Get a network interface for SNMP TRAP.

  Description:
    This  function is used to get a network interface to transmit SNMP trap.

  Precondition:
    The SNMP module should be initialized.

  Parameters:
    netIx - Network index
	hNet - Network interface .If hNet is NULL, then a valid interface will returned.

  Returns:
    TCPIP_NET_HANDLE
	- Success - returns a valid interface
	- Failure - no interface

  Remarks:
    None.
*/
TCPIP_NET_HANDLE TCPIP_SNMP_ClientGetNet(int *netIx,TCPIP_NET_HANDLE hNet);


//*********************************************************************
/*
  Function:
    bool TCPIP_SNMP_VarbindSet(SNMP_ID var, SNMP_INDEX index,
                                   uint8_t ref, SNMP_VAL val)

  Summary:
    Sets the MIB variable with the requested value.

  Description:
    This is a callback function called by module for SNMP SET request.
	This function content is modified by the application developer.
	This function contains the implementation of WRITE enable MIB OID macros.  
	
	mib2bib.jar Microchip MIB compiler utility is used to generate mib.h, which 
	lists all the MIB OID ID value. 	
	
  Precondition:
    TCPIP_SNMP_ProcessVariables() is called.

  Parameters:
    var -   Write enable Variable id whose value is to be set
	index - For a scalar variable , Index value is zero. For sequence variable 
			index value specifies which index value need to be set. 
    ref -   Variable reference used to transfer multi-byte data
            0 if first byte is set otherwise non zero value to indicate
            corresponding byte being set. SNMP set will performed until ref is not 
			equal to SNMP_END_OF_VAR and SNMP set starts with ref = SNMP_START_OF_VAR.
    val -   Up to 4 byte data value:
            - If var data type is uint8_t, variable value is in val->byte
            - If var data type is uint16_t, variable value is in val->word
            - If var data type is uint32_t, variable value is in val->dword.
            - If var data type is IP_ADDRESS, COUNTER32, or GAUGE32, value is in val->dword
            - If var data type is OCTET_STRING, ASCII_STRING value is in val->byte; 
			  multi-byte transfer will be performed to transfer remaining
               bytes of data.

  Returns:
    - true  -   if it is okay to set more byte(s)
    - false -   if it is not okay to set more byte(s)

  Example:
	<code>
	switch(var)
    {
		// LED_D5 - generated from the Microchip style MIB script using mib2bib.jar is a scalar variable
        case LED_D5:
            LED2_IO = val.byte;
            return true;
			
		// TRAP_COMMUNITY - generated from the Microchip style MIB script using mib2bib.jar,
		// is a Sequence variable.	
		
		case TRAP_COMMUNITY:
			// Since this is a ASCII_STRING data type, SNMP will call with
			// SNMP_END_OF_VAR to indicate no more bytes or end of SNMP SET process
			// Use this information to determine if we just added new row
			// or updated an existing one.
			if ( ref ==  SNMP_END_OF_VAR )
			{
				// Index equal to table size means that we have new row.
				if ( index == trapInfo.Size )
					trapInfo.Size++;

				// Length of string is one more than index.
				trapInfo.table[index].communityLen++;

				return true;
			}

			// Make sure that index is within our range.
			if ( index < trapInfo.Size )
			{
				// Copy given value into local buffer.
				trapInfo.table[index].community[ref] = val.byte;
				// Keep track of length too.
				// This may not be NULL terminate string.
				trapInfo.table[index].communityLen = (uint8_t)ref;
				return true;
			}
			break;
	}
	</code>
  Remarks:
    This function may get called more than once depending on number
    of bytes in a specific set request for given variable.
    only dynamic read/-write variables needs to be handled.
 */
bool TCPIP_SNMP_VarbindSet(SNMP_ID var, SNMP_INDEX index,uint8_t ref, SNMP_VAL val);

//******************************************************************************
/*
  Function:
    bool TCPIP_SNMP_VarbindGet(SNMP_ID var, SNMP_INDEX index,uint8_t* ref, SNMP_VAL* val)

  Summary:
    Used to get/collect OID variable information.

  Description:
    This is a callback function called by SNMP module. 
	This function is called only when SNMP GET, GETNEXT and GETBULK request is made.
	This function content is modified by the application developer.
	This function contains the implementation of READ enable MIB OID macros.

	mib2bib.jar Microchip MIB compiler utility is used to generate mib.h, which 
	lists all the MIB OID ID value.
	
  PreCondition:
    TCPIP_SNMP_ProcessVariables() is called.

  parameters:
    var     -   Variable id whose value is to be returned
    index   -   For a scalar variable , Index value is zero. For sequence variable 
				index value specifies which index value need to be set. 
    ref     -   Variable reference used to transfer multi-byte data.
                It is always SNMP_START_OF_VAR when very first byte is requested.
                Otherwise, use this as a reference to keep track of multi-byte transfers.
    val     -   Pointer to up to 4 byte buffer:
                - If var data type is uint8_t, transfer data in val->byte
                - If var data type is uint16_t, transfer data in val->word
                - If var data type is uint32_t, transfer data in val->dword
                - If var data type is IP_ADDRESS, transfer data in val->v[] or val->dword
                - If var data type is COUNTER32, TIME_TICKS or GAUGE32, transfer data in val->dword
                - If var data type is ASCII_STRING or OCTET_STRING
                  transfer data in val->byte using multi-byte
                  transfer mechanism.

  Returns:
    - true  - If a value exists for given variable 
    - false - If a value does not exist for a given variable

  Example:
	Usage of MIB OID within this function for a ASCII string ( multi byte variable)
  <code>
	
	// In the beginning *ref is equal to SNMP_START_OF_VAR
	myRef = *ref;
	switch(var)
	{
	// TRAP_COMMUNITY - generated from the Microchip style MIB script using mib2bib.jar,
	// is a Sequence variable.
		case TRAP_COMMUNITY:   
			if ( index < trapInfo.Size )
			{
			// check if the myRef should not cross the maximum length 
				 if ( myRef == trapInfo.table[index].communityLen )
				 {
				 // End of SNMP GET process
					 *ref = SNMP_END_OF_VAR;
					 return true;
				 }
				if ( trapInfo.table[index].communityLen == 0u )
					*ref = SNMP_END_OF_VAR; // End of SNMP GET process
				else
				{
					// Start of SNMP GET process byte by byte
					val->byte = trapInfo.table[index].community[myRef];
					myRef++;
					*ref = myRef;
				}
				return true;
			}
			break;
	// LED_D5 - generated from the Microchip style MIB script using mib2bib.jar is a scalar variable
		case LED_D5:
            val->byte = LED2_IO;
            return true;
	}
	</code>
	
  Remarks:
    This function may get called more than once depending on number
    of bytes in a specific get request for given variable.
    only dynamic read variables needs to be handled.
 */
bool TCPIP_SNMP_VarbindGet(SNMP_ID var, SNMP_INDEX index,uint8_t* ref, SNMP_VAL* val);

//*********************************************************************
/*
  Function:
    bool TCPIP_SNMP_NextIndexGet(SNMP_ID var,SNMP_INDEX* index)

  Summary:
    To search for next index node in case of a Sequence variable.

  Description:
    This is a callback function called by SNMP module.
	This function contents are modified by the application developer
	with the new MIB Record ID.
	This function can be called for individual MIB ID with index or for a complete 
	MIB table without any instance. 
	- If the SNMP Sequence MIB variable processing is performed with index or instance value,
	it is the responsibility of the agent to send the next available index for that 
	requested variable. This is a only one iteration process. The Manager will not send any further 
	requests for the received index value. 
	- If SNMP request for a sequence variable starts with only OID without any instance, that is 
	a complete table is requested without any instance, it is the responsibility of the 
	agent to send the first available index of the table. The Manager will continue the 
	request with the transferred instance until it receives the reply from agent.
	
	This function will only be called for OID variable of type sequence.
	if the index value starts with SNMP_INDEX_INVALID , then user need to send the 
	response with first available index value.
	
  PreCondition:
    TCPIP_SNMP_ProcessVariables is called.

  Parameters:
    var     -   Variable id whose value is to be returned
    index   -   Next Index of variable that should be transferred

  Returns:
    - true  - If a next index value exists for given variable at given
              index and index parameter contains next valid index.
    - false - If a next index value does not exist for a given variable

  Remarks:
      Only sequence index needs to be handled in this function and
	  this function is called after TCPIP_SNMP_RecordIDValidation. 
 */
bool TCPIP_SNMP_NextIndexGet(SNMP_ID var, SNMP_INDEX* index);

//*********************************************************************
/*
  Function:
    bool TCPIP_SNMP_ExactIndexGet(SNMP_ID var,SNMP_INDEX *index)

  Summary:
    To search for exact index node in case of a Sequence variable.

  Description:
    This is a callback function called by SNMP module.
    This function contents are modified by the application developer
	with the new MIB Record ID.
	This function can be called for individual MIB ID with index. 
	This function is used to get the first available index from global 
	or dynamic allocated sequence variable table.

  PreCondition:
    TCPIP_SNMP_ProcessVariables() is called.

  Parameters:
    var     -   Variable id as per mib.h (input)
    index      -     Index of variable (input)

  Returns:
    -true  - If the exact index value exists for a given variable at a given
             index.
    -false - If the exact index value does not exist for the given variable

  Remarks:
      Only sequence index needs to be handled in this function and
	  this function is called after TCPIP_SNMP_RecordIDValidation. 
 */
bool TCPIP_SNMP_ExactIndexGet(SNMP_ID var, SNMP_INDEX *index);


//*********************************************************************
/*
  Function:
	bool TCPIP_SNMP_IsValidLength(SNMP_ID var, uint8_t len,uint8_t index)

  Summary:
    Validates the set variable data length to data type.

  Description:
    This function is used to validate the dynamic variable data length
    to the variable data type. It is called before the SET request is processed.
    This is a callback function called by module. User application
    must implement this function.

  PreCondition:
    TCPIP_SNMP_ProcessSetVar is called.

  Parameters:
    var -   Variable id whose value is to be set
    len -   Length value that is to be validated.

  Return:
    - true  - if given var can be set to given len
    - false - if the given var cannot be set to the given len

  Remarks:
    This function will be called for only dynamic variables that are
    defined as ASCII_STRING and OCTET_STRING.
 */
bool TCPIP_SNMP_IsValidLength(SNMP_ID var, uint8_t len,uint8_t index);


//*********************************************************************
/*
  Function:
     uint8_t TCPIP_SNMP_IsValidCommunity(uint8_t* community)

  Summary:
     Validates community name for access control.

  Description:
     This function validates the community name for the MIB access to SNMP manager.
     The SNMP community name received in the request PDU is validated for
     read and write community names. The agent gives an access to the mib
     variables only if the community matches with the predefined values.
     This routine also sets a global flag to send trap if authentication
     failure occurs.
	 
  PreCondition:
     TCPIP_SNMP_Initialize is already called.

  parameters:
     community - Pointer to community string as sent by SNMP manager

  Returns:
     This function returns the community validation result as
     READ_COMMUNITY or WRITE_COMMUNITY or INVALID_COMMUNITY.

  Remarks:
     This is a callback function called by module. User application must
     implement this function and verify that community matches with
     predefined value. This validation occurs for each SNMP manager request.
 */
uint8_t TCPIP_SNMP_IsValidCommunity(uint8_t* community);

//****************************************************************************
/*
  Function:
    bool  TCPIP_SNMP_WriteCommunityGet(int index,int len, uint8_t * dest)

  Summary:
    Gets the writeCommunity String with SNMP index.

  Description:
    This function is used to collect WRITE community string from the global 
	community table with respect to the index value.

  Precondition  :
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    index  -  One of the index of community table and it should be less than TCPIP_SNMP_MAX_COMMUNITY_SUPPORT.
	len - Length of the community string expected. It should not be more than TCPIP_SNMP_COMMUNITY_MAX_LEN.
	dest - Copy the community string to this address and it should have a valid address.

  Returns:
    - true  - if the community string is collected
	- false - if the community string is not collected

  Remarks:
    None.
 */
bool  TCPIP_SNMP_WriteCommunityGet(int index,int len, uint8_t * dest);


//****************************************************************************
/*
  Function:
    bool  TCPIP_SNMP_ReadCommunityGet(int index,int len, uint8_t * dest)

  Summary:
    Gets the readCommunity String with SNMP index.

  Description:
    This function is used to collect READ community string from the global 
	community table with respect to the index value.

  Precondition:
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    index  -  One of the index of community table and it should be less than TCPIP_SNMP_MAX_COMMUNITY_SUPPORT.
    len - Length of the community string expected. It should not be more than TCPIP_SNMP_COMMUNITY_MAX_LEN.
    dest - Copy the community string to this address and it should have a valid address.

  Returns:
    - true  - if the community string is collected
	- false - if the community string is not collected

  Remarks:
    None.
 */
bool  TCPIP_SNMP_ReadCommunityGet(int index,int len, uint8_t * dest);

//****************************************************************************
/*
  Function:
    bool  TCPIP_SNMP_WriteCommunitySet(int index,int len, uint8_t * src)

  Summary:
    Sets the writeCommunity String with SNMP index.

  Description:
    This function is used to collect WRITE community string from user and set the
    community table with respect to the index value.

  Precondition  :
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    index  -  One of the index of community table and it should be less than TCPIP_SNMP_MAX_COMMUNITY_SUPPORT.
    len - Length of the community string expected. It should not be more than TCPIP_SNMP_COMMUNITY_MAX_LEN.
    src - Copy this community string to SNMP community table.

  Returns:
    - true  - if the community string is collected
	- false - if the community string is not collected

  Remarks:
    None.
 */
bool  TCPIP_SNMP_WriteCommunitySet(int index,int len, uint8_t * src);

//****************************************************************************
/*
  Function:
    bool  TCPIP_SNMP_ReadCommunitySet(int index,int len, uint8_t * src)

  Summary:
    Sets the readCommunity String with SNMP index.

  Description:
    This function is used to configure READ community string from the user and
    configure the SNMP community table.

  Precondition:
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    index  -  One of the index of community table and it should be less than TCPIP_SNMP_MAX_COMMUNITY_SUPPORT.
    len - Length of the community string expected. It should not be more than TCPIP_SNMP_COMMUNITY_MAX_LEN.
    src - Copy this community string to snmp community table.

  Returns:
    - true  - if the community string is collected
	- false - if the community string is not collected

  Remarks:
    None.
 */
bool  TCPIP_SNMP_ReadCommunitySet(int index,int len, uint8_t * src);

//*******************************************************************************
/*
  Function:
    bool TCPIP_SNMP_RecordIDValidation(uint8_t snmpVersion,bool idPresent,
					uint16_t varId,uint8_t * oidValuePtr,uint8_t oidLen)

  Summary:
    Used to restrict the access dynamic and non dynamic OID string 
	for a particular SNMP Version.

  Description:
    This is a callback function called by SNMP module. SNMP user must
    implement this function as per SNMP version. One need to add the new SNMP
    MIB IDs here as per SNMP version (e.g., SYS_UP_TIME (250) is common for V1/V2/V3).
    ENGINE_ID - is the part of V3; therefore, place all of the SNMPv3 var IDs within
    the macro TCPIP_STACK_USE_SNMPV3_SERVER.

  PreCondition:
    TCPIP_SNMP_Initialize is already called.

  parameters:
    snmpVersion - different SNMP version
    idPresent - true if SNMp record id is present else false
    varId     -   dynamic record ID value as per mib.h 
    oidValuePtr - OID Value pointer
    oidLen - number of OIDs present to be processed  

  Returns:
    - true  - A record ID exists
    - false - A record ID does not exist

  Remarks:
    This function is specific for record ID validation and this can also be used  
	to restrict OID string.
	
 */
bool TCPIP_SNMP_RecordIDValidation(uint8_t snmpVersion,bool idPresent,
                        uint16_t varId,uint8_t * oidValuePtr,uint8_t oidLen);

//*****************************************************************************
//*****************************************************************************
// Section:  SNMP TRAP Related Functions
//*****************************************************************************
//*****************************************************************************

//****************************************************************************
/*
  Function:
    void TCPIP_SNMP_TrapSpecificNotificationGet(uint8_t *specTrap)

  Summary:
    Gets the specific trap.

  Description:
    This function is used to get specific trap value. Specific trap values are listed 
	in SNMP_VENDOR_SPECIFIC_TRAP_NOTIFICATION_TYPE.

  Precondition:
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    specTrap - Vendor specific trap value

  Returns:
    None.

  Remarks:
    None.
 */
void TCPIP_SNMP_TrapSpecificNotificationGet(uint8_t *specTrap);

//****************************************************************************
/*
  Function:
    void TCPIP_SNMP_TrapSpecificNotificationSet(uint8_t specTrap,uint8_t genTrap, 
	                                            SNMP_ID trapID)

  Summary:
    Sets the specific trap, generic trap, and trap ID.

  Description:
    The SNMP user needs to call this function before transmitting any traps. This 
	function will set the vendor specific trap, generic trap, and default trap ID value. 
	The SNMPv2 trap will use this trap ID while sending a specific trap.  

  Precondition:
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    specTrap - Vendor specific trap value 
	           (enumeration value of SNMP_VENDOR_SPECIFIC_TRAP_NOTIFICATION_TYPE)
	genTrap  - Generic trap 
	           (enumeration value of SNMP_GENERIC_TRAP_NOTIFICATION_TYPE)
	trapID - Trap ID 
	
  Returns:
    None.

  Remarks:
    The Trap ID is the NOTIFICATION-TYPE of the ASN.1 MIB format. From the Trap ID, 
	the SNMP agent will be able to obtain the OID string, which will be used while 
	preparing TRAPv2 second varbind.
 */
void TCPIP_SNMP_TrapSpecificNotificationSet(uint8_t specTrap,uint8_t genTrap, 
                                            SNMP_ID trapID);

//****************************************************************************
/*
  Function:
    void TCPIP_SNMP_TrapSendFlagGet(bool *trapNotify)

  Summary:
    Gets the status of trap send flag.

  Description:
    This function is used to get the trap send flag details and this is used only when 
	user is trying to send more than one varbind in a single PDU. That is more than one 
	notification details are required to be part of a single PDU.	

  Precondition:
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    trapNotify
	- true  - indicates when more than one varbind are waiting to be part of PDU
    - false - indicates the end of varbind list to be part of trap PDU
			
  Returns:
    None.

  Remarks:
   None.
 */
void TCPIP_SNMP_TrapSendFlagGet(bool *trapNotify);

//****************************************************************************
/*
  Function:
    void TCPIP_SNMP_SocketIDGet(UDP_SOCKET *socket)

  Summary:
    Gets the Socket ID for SNMP Server socket.

  Description:
    This function is used to get trap client socket ID for both IPv4 and IPv6 
	receiver Address.

  Precondition:
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    socket - Trap client socket ID	
			
  Returns:
    None.

  Remarks:
   None.
 */
void TCPIP_SNMP_SocketIDGet(UDP_SOCKET *socket);

//****************************************************************************
/*
  Function:
    void TCPIP_SNMP_SocketIDSet(UDP_SOCKET socket)

  Summary:
    Sets the Socket ID for SNMP Server socket.

  Description:
    This function is used to update socket value of SNMP trap global structure.
	
  Precondition:
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    socket - Trap client socket ID	
			
  Returns:
    None.

  Remarks:
   None.
 */
void TCPIP_SNMP_SocketIDSet(UDP_SOCKET socket);

//****************************************************************************
/*
  Function:
    void TCPIP_SNMP_TrapSendFlagSet(bool trapNotify)

  Summary:
    Sets the status of trap send flag.

  Description:
    This function is used to set the trap send flag details and this is used only when 
	user is trying to send more than one varbind in a single PDU. That is more than one 
	notification details are required to be part of a single PDU.
	By default TRAP send flag is set to false.
	If there is only varbind need to be part of Notification PDU, then this function should
	be called with boolean false. 
	Please find the usage of this flag in this following example.

  Precondition:
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    trapNotify
	- true  - indicates when more than one varbind are waiting to be part of PDU
	- false - indicates the end of varbind list to be part of trap PDU
			
  Returns:
    None.
	
  Example:
	<code>
	void SNMPv2TrapDemo(void)
	{
		//set TRAP send flag to true , it signifies that there are more than one
        // variable need to be the part of SNMP v2 TRAP.
		TCPIP_SNMP_TrapSendFlagSet(true);
		
		// PUSH button varbind 
		TCPIP_SNMP_Notify(PUSH_BUTTON,analogPotVal,0);
		
		// Before adding the last varbind to the TRAP PDU, TRAP send flag should 
		// be set to  false. That it indicates it is the last varbind to the 
		// Notification PDU.
		TCPIP_SNMP_TrapSendFlagSet(false);
		
		// Last varbind LED0_IO 
		TCPIP_SNMP_Notify(LED0_IO,analogPotVal,0);		
		
	}
	</code>

  Remarks:
   None.
 */
void TCPIP_SNMP_TrapSendFlagSet(bool trapNotify);

//****************************************************************************
/*
  Function:
    void TCPIP_SNMP_AuthTrapFlagSet(bool sendTrap)

  Summary:
    Sets the status of authentication trap flag.

  Description:
    This function is used to set the authentication trap send flag and this is used only when
    user is trying to send authentication failure trap. Ex- sending a trap if community do not
    match to the global community table.

  Precondition  :
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    sendTrap
	- true  - if an auth trap needs to be sent to the manager
    - false - if an auth trap does not need to be sent to the manager

  Returns:
    None.
	
  Remarks:
    None.
 */
void TCPIP_SNMP_AuthTrapFlagSet(bool sendTrap);

//****************************************************************************
/*
  Function:
    void TCPIP_SNMP_AuthTrapFlagGet(bool *sendTrap)

  Summary:
    Gets the status of authentication trap flag.

  Description:
    This function is used to Get the authentication trap send flag and this is 
	used only when the user is trying to send authentication failure trap. For example, 
	sending a trap if community does not match the global community table.

  Precondition:
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    sendTrap - Trap flag value.

  Returns:
    None.
	
  Remarks:
    None.
 */
void TCPIP_SNMP_AuthTrapFlagGet(bool *authTrap);

//****************************************************************************
/*
  Function:
    void TCPIP_SNMP_TRAPMibIDGet(uint32_t *mibID)

  Summary:
    Gets the agent MIB ID for SNP notification.

  Description:
    This function is used to get the MIB ID which is require while transmitting 
	SNMP notification.This MIB ID is a Agent ID value of the Microchip style MIB script.
	MIB ID macro value is present in mib.h which is generated by mib2bib.jar. 
	
  Precondition:
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    mibID - Trap client MIB ID	
			
  Returns:
    None.

  Remarks:
    None.
 */
void TCPIP_SNMP_TRAPMibIDGet(uint32_t *mibID);

//****************************************************************************
/*
  Function:
    void TCPIP_SNMP_MibIDSet(uint32_t *mibID)

  Summary:
    Sets the agent MIB ID for SNP notification.

  Description:
    This function is used to Set the MIB ID which is require while transmitting 
	SNMP notification.SNMP user can this function without adding this to the Microchip
	MIB script.
	
  Precondition:
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    mibID - Trap client MIB ID	
			
  Returns:
    None.

  Remarks:
    None.
 */
void TCPIP_SNMP_MibIDSet(uint32_t mibID);

//****************************************************************************
/*
  Function:
    void TCPIP_SNMP_TrapInterFaceSet(TCPIP_NET_HANDLE netIntf)

  Summary:
    Sets the TRAP interface for SNMP notification.

  Description:
    This function is used to Set the network interface to which the TRAP socket is 
	ready to transmit Notifications to the TRAP receiver address.
	
  Precondition:
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    netIntf - Network interface Trap is connected to receiver
			
  Returns:
    None.

  Remarks:
    None.
 */
void TCPIP_SNMP_TrapInterFaceSet(TCPIP_NET_HANDLE netIntf);

//**************************************************************************
/*
  Function:
    void TCPIP_SNMP_SendFailureTrap(void)

  Summary:
    Prepares and validates the remote node that will receive a trap and send 
	the trap PDU.

  Description:
    This function is used to send trap notification to previously
    configured IP address if trap notification is enabled. There are
    different trap notification code. The current implementation
    sends trap for authentication failure (4).

  PreCondition:
    If the defined application event occurs to send the trap.

  parameters:
    None.

  Returns:
    None.

  Remarks:
    This is a callback function called by the application on certain
    predefined events. This routine only implemented to send a
    authentication failure Notification-type macro with PUSH_BUTTON
    OID stored in MPFS. If the ARP is not resolved (i.e., if
    TCPIP_SNMP_NotifyIsReady returns false, this routine times
    out in 5 seconds). This function should be modified according to
    event occurred and should update the corresponding OID and notification
    type to the TRAP PDU.
*/
void TCPIP_SNMP_SendFailureTrap(void);

//****************************************************************************
/*
  Function:
    bool TCPIP_SNMP_TRAPv2Notify(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index,
                                SNMP_TRAP_IP_ADDRESS_TYPE eTrapMultiAddressType)

  Summary:
    Creates and sends TRAP PDU.

  Description:
    This function creates SNMP V2 Trap PDU and sends it to previously specified
    remoteHost. 

    SNMP V1 trap PDU:
       | PDU type | enterprise | agent addr | generic trap | specific trap |
       | time stamp | varbind list |

       The v1 enterprise is mapped directly to SNMPv2TrapOID.0

    SNMP V2 trap PDU:
       version (0 or 1) | community | SNMP PDU |PDU type | request id | error status
       |err index |varbinds

       The first two variables (in varbind list) of snmpv2 are: sysUpTime.0 and
        SNMPv2TrapOID.0

       Generic Trap OID is used as the varbind for authentication failure.

  Precondition:
    TCPIP_SNMP_NotifyIsReady is already called and returned true.

  Parameters:
    var     - SNMP var ID that is to be used in notification
    val     - Value of var. Only value of uint8_t, uint16_t or uint32_t can be sent.
    index   - Index of var. If this var is a single,index would be 0, or else
              if this var Is a sequence, index could be any value
              from 0 to 127
   eTrapMultiAddressType -  Trap address type

  Return Values:
    - true    -	If SNMP notification was successfully sent. However, this does 
	            not guarantee that the remoteHost received it.
    - false   - Notification sent failed. This would fail under the following conditions:
                - The specified SNMP_BIB_FILE does not exist in file system
                - The specified var does not exist
                - The previously specified agentID does not exist
                - Data type of a given var is unknown. This is possible if the file 
				  system itself was corrupted.

  Remarks:
    This would fail if there were not UDP socket to open.
 */
bool TCPIP_SNMP_TRAPv2Notify(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index,SNMP_TRAP_IP_ADDRESS_TYPE eTrapMultiAddressType);

//****************************************************************************
/*
  Function:
    bool TCPIP_SNMP_TRAPv1Notify(SNMP_ID var, SNMP_VAL val,
            SNMP_INDEX index,SNMP_TRAP_IP_ADDRESS_TYPE eTrapMultiAddressType)

  Summary:
    Creates and Sends TRAPv1 pdu.

  Description:
    This function creates SNMP trap PDU and sends it to previously specified
    remoteHost.
    snmpv1 trap pdu:
   | PDU-type | enterprise | agent-addr | generic-trap | specific-trap |
   | time-stamp | varbind-list |

   The v1 enterprise is mapped directly to SNMPv2TrapOID.0

  Precondition:
    TCPIP_SNMP_NotifyIsReady() is already called and returned true.

  Parameters:
    var     - SNMP var ID that is to be used in notification
    val     - Value of var. Only value of uint8_t, uint16_t or uint32_t can be sent.
    index   - Index of var. If this var is a single,index would be 0, or else
                      if this var Is a sequence, index could be any value
                      from 0 to 127
    eTrapMultiAddressType -  Trap address type

  Return Values:
    - true    -	If SNMP notification was successfully sent. However, this does 
	            not guarantee that the remoteHost received it.
    - false   - Notification sent failed. This would fail under the following conditions:
                - The specified SNMP_BIB_FILE does not exist in file system
                - The specified var does not exist
                - The previously specified agentID does not exist
                - Data type of a given var is unknown. This is possible if the file 
				  system itself was corrupted.

Remarks:
    This would fail if there were not UDP socket to open.
***************************************************************************/
bool TCPIP_SNMP_TRAPv1Notify(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index,SNMP_TRAP_IP_ADDRESS_TYPE eTrapMultiAddressType);

//****************************************************************************
/*
  Function:
    bool TCPIP_SNMP_NotifyIsReady(IP_MULTI_ADDRESS* remoteHost,
							SNMP_TRAP_IP_ADDRESS_TYPE eTrapMultiAddressType)

  Summary:
    Resolves given remoteHost IP address into MAC address.

  Description:
    This function resolves given remoteHost IP address into MAC address using
    ARP module. If remoteHost is not available, this function would never
    return true. Application must implement time out logic to handle
    "remoteHost not available" situation.

  Precondition:
    TCPIP_SNMP_NotifyPrepare is already called.

  Parameters:
    remoteHost  - Pointer to remote Host IP address
	eTrapMultiAddressType - IPv4 and IPv6 address type
	
  Return Values:
    - true    -	If remoteHost IP address is resolved and
				TCPIP_SNMP_Notify may be called.
    - false   - If remoteHost IP address is not resolved.

  Remarks:
    This would fail if there were not UDP socket to open.
 */
bool TCPIP_SNMP_NotifyIsReady(IP_MULTI_ADDRESS* remoteHost,
							SNMP_TRAP_IP_ADDRESS_TYPE eTrapMultiAddressType);


//****************************************************************************
/*
  Function:
    void TCPIP_SNMP_NotifyPrepare(IP_MULTI_ADDRESS* remoteHost,
                                  char* community,
                                  uint8_t communityLen,
                                  SNMP_ID agentIDVar,
                                  uint8_t notificationCode,
                                  uint32_t timestamp)

  Summary:
    Collects trap notification info and send ARP to remote host.

  Description:
    This function prepares SNMP module to send SNMP trap notification
    to remote host. It sends ARP request to remote host to learn remote
    host MAC address.

  Precondition:
    TCPIP_SNMP_Initialize() is already called.

  Parameters:
    remoteHost  - pointer to remote Host IP address
    community   - Community string to use to notify
    communityLen- Community string length
    agentIDVar  - System ID to use identify this agent
    notificaitonCode - Notification Code to use
    timestamp   - Notification timestamp in 100th of second.

  Returns:
    None.

  Remarks:
    This is first of series of functions to complete SNMP notification.
 */
void TCPIP_SNMP_NotifyPrepare(IP_MULTI_ADDRESS* remoteHost, char* community, 
						uint8_t communityLen, SNMP_ID agentIDVar, 
						uint8_t notificationCode, uint32_t timestamp);

//****************************************************************************
/*
  Function:
    uint32_t TCPIP_SNMP_TrapTimeGet(void)

  Summary:
    Gets SNMP Trap UDP client open socket time-out.

  Description:
    This function returns a uint32_t time(snmpTrapTimer) which is used to 
	time out a SNMP TRAP notification for a HOST. snmpTrapTimer is initialized 
	when there is UDP client socket open either for a HOST IPv4 or IPv6 address.

  Precondition:
    TCPIP_SNMP_Initialize() is already called.

  Parameters:
    None.

  Returns:
    uint32_t time
	
  Remarks:
    None.
*/
uint32_t TCPIP_SNMP_TrapTimeGet(void);

//****************************************************************************
/*
  Function:
    bool TCPIP_SNMP_TRAPTypeGet(void)

  Summary:
    Get SNMP Trap type for version v1 and v2c.

  Description:
    This function returns true if the trap tye is v2 and the TRAP pdu
    packet will be a TRAP v2 format.The return value is also used
    is validated when SNMP module is trying to send a trap for SNMP version v3.

  Precondition:
    TCPIP_SNMP_Initialize() is already called.

  Parameters:
    None.

  Returns:
    - true - trap version is v2
    - false -  trap version is v1
	
  Remarks:
    This function is used by the customer function.
*/
bool TCPIP_SNMP_TRAPTypeGet(void);

//****************************************************************************
/*
  Function:
    bool TCPIP_SNMPV3_TrapTypeGet(void)

  Summary:
    Gets SNMP Trap type for version v3.

  Description:
    This function returns true if SNMP module is trying to send trap version v2
    with SNMP version v3.

  Precondition:
    TCPIP_SNMP_Initialize() is already called.

  Parameters:
    None.

  Returns:
    - true - SNMP3 will use trap version v2
    - false - SNMP3 will use trap version v1
	
  Remarks:
    This function is used by the customer function.
*/
bool TCPIP_SNMPV3_TrapTypeGet(void);

//****************************************************************************
/*
  Function:
    bool TCPIP_SNMP_IsTrapEnabled(void)

  Summary:
    Gets the SNMP Trap status.

  Description:
    This function returns true if SNMP module is enabled to send the trap to
    to the SNMP manager device.

  Precondition:
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    None.

  Returns:
    - true - SNMP module has trap enabled
    - false - SNMP module has trap disabled
	
  Remarks:
    This function is used by the customer function.
*/
bool TCPIP_SNMP_IsTrapEnabled(void);

//****************************************************************************
/*
  Function:
    bool TCPIP_SNMP_ValidateTrapIntf(TCPIP_NET_HANDLE pIf)

  Summary:
    Gets the status of trap interface.

  Description:
    This function returns true if SNMP module trap interface is a valid interface.

  Precondition:
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    pif - network interface

  Returns:
    - true - SNMP trap interface is valid
    - false - SNMP trap interface is invalid
	
  Remarks:
    This function is used by the customer function.
*/
bool TCPIP_SNMP_ValidateTrapIntf(TCPIP_NET_HANDLE pIf);

// *****************************************************************************
/*
  Function:
    void  TCPIP_SNMP_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs SNMP module tasks in the TCP/IP stack.

  Precondition:
    The SNMP module should have been initialized

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_SNMP_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __SNMP_H