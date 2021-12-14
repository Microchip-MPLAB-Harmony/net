# Simple Network Management Protocol (SNMP)  Configurations

**SNMP Specific User Configurations**

- **SNMP Server Root Directory Path:**
    - The mount point for the files used by SNMP
    - Default value: "/mnt/mchpSite1/"
    - Symbol TCPIP_SNMP_MOUNT_POINT

- **BIB file name:**
    - The Microchip mib2bib.jar compiler is used to convert the Microchip MIB script to binary format and it is compatible with the Microchip SNMP agent. which is written in ASCII format.
    - Name of the bib file for SNMP is snmp.bib.
    - Default value:"snmp.bib" 
    - Symbol TCPIP_SNMP_BIB_FILE_NAME

- **Maximum SNMP message size:**
    - The maximum length in octets of an SNMP message which this SNMP agent able to process.
    - As per RFC 3411 snmpEngineMaxMessageSize and RFC 1157 ( section 4- protocol specification ) and implementation  supports more than 480 whenever feasible.
    -It should be divisible by 16
    - Default value: 480
    - Symbol TCPIP_SNMP_MAX_MSG_SIZE

- **Maximum Community Length:**
    - This is the maximum length for community string.
    - Application must ensure that this length is observed.
    - SNMP module adds one byte extra after TCPIP_SNMP_COMMUNITY_MAX_LEN for adding '\0' NULL character.
    - Default value: 16
    - Symbol TCPIP_SNMP_COMMUNITY_MAX_LEN

- **Maximum Community Entries:**
    - The maximum number of community entries
    - Specifying more strings than TCPIP_SNMP_MAX_COMMUNITY_SUPPORT will result in the later strings being ignored (but still wasting program memory).
   - Specifying fewer strings is legal, as long as at least one is present.
    - Default value: 3
    - Symbol TCPIP_SNMP_MAX_COMMUNITY_SUPPORT


- **Read Community Name:**
    - The name of the read community
    - Default value: "harmony"/"read"
    - Symbol TCPIP_SNMP_STACK_READCOMMUNITY_NAME_IDXn

- **Write Community Name:**
    - The name of the write community
    - Default value: "unicorn"/"write"/"harmony"
    - Symbol TCPIP_SNMP_STACK_WRITECOMMUNITY_NAME_IDXn

- **Trap Support:**
    - Enable trap support
    - Default value: true
    - Symbol TCPIP_SNMP_USE_TRAP_SUPPORT

- **Trap table size:**
    -This table maintains list of interested receivers that should receive notifications when some interesting event occurs.
    - Default value: 2
    - Symbol TCPIP_SNMP_TRAP_TABLE_SIZE

- **V2 Trap Support:**
    - Enable V2 trap support
    - Default value: true
    - Symbol TCPIP_SNMP_STACK_USE_V2_TRAP

- **Include SNMP Custom Template:**
    - Include the supplied template/model into the project
    - Default value: true
    - Symbol TCPIP_SNMP_CUSTOM_TEMPLATE

- **SNMP Task Rate (in msec):**
    - SNMP task processing rate, in milliseconds.
    - The SNMP module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Value is in milliseconds
    - Default value: 200
    - Symbol TCPIP_SNMP_TASK_PROCESS_RATE

- **Maximum OID Length:**
    - Maximum length for the OID String.
    - Change this to match your OID string length.
    - Default value: 18
    - Symbol TCPIP_SNMP_OID_MAX_LEN

- **Maximum non record OID:**
    - Update the Non record id OID value which is part of CustomSnmpDemoApp.c file.
    - This is the maximum size for gSnmpNonMibRecInfo[] which is the list of static variable Parent 
    - OIDs which are not part of mib.h file. This structure is used to restrict access to static variables of SNMPv3 OIDs from SNMPv2c and SNMPv1 version.
    - With SNMPv3 all the OIDs accessible but when we are using SNMPv2c version , static variables of the SNMPv3 cannot be accessible with SNMP version v2c.
    - SNMP agent supports both SMIv1 and SMIv2 standard and snmp.mib has been updated with respect to SMIV2 standard and it also includes MODULE-IDENTITY ( number 1) after ENTERPRISE-ID.
    - Default value: 3
    - Symbol TCPIP_SNMP_MAX_NON_REC_ID_OID

- **Notify Community Length:**
    - Maximum length for SNMP Trap community name 
    - Default value: TCPIP_SNMP_COMMUNITY_MAX_LEN
    - Symbol TCPIP_SNMP_NOTIFY_COMMUNITY_LEN

- **Trap community maximum length:**
    - The trap comunity maximum length
    - Default value: 16
    - Symbol TCPIP_SNMP_TRAP_COMMUNITY_MAX_LEN_MEM_USE

