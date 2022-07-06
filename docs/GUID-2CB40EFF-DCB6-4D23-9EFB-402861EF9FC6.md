# TCPIP\_SNMP\_MODULE\_CONFIG Structure

**Parent topic:**[SNMP Module](GUID-7764E81C-8FC9-4B3E-8830-255BDE678AA0.md)

## C

```
typedef struct {
bool trapEnable;
bool snmp_trapv2_use;
bool snmpv3_trapv1v2_use;
const char* snmp_bib_file;
TCPIP_SNMP_COMMUNITY_CONFIG * read_community_config;
TCPIP_SNMP_COMMUNITY_CONFIG * write_community_config;
TCPIP_SNMPV3_USM_USER_CONFIG * usm_config;
TCPIP_SNMPV3_TARGET_ENTRY_CONFIG * trap_target_config;
const char* mountPath;
} TCPIP_SNMP_MODULE_CONFIG;
```

## Description

Structure: TCPIP\_SNMP\_MODULE\_CONFIG.

This structure is used to configure SNMP details for runtime configuration.

## Members

|Members|Description|
|-------|-----------|
|trapEnable|True = agent can send the trap, false = agent shouldn't send the trap.|
|snmp\_trapv2\_use|True = agent uses Trap version v2 and false = uses Tarp version 1.|
|snmpv3\_trapv1v2\_use|SNMPv3 trap should be true , only if SNMP version is 3.|
|read\_community\_config|Read-only Community configuration.|
|write\_community\_config|Write-only Community configuration.|
|usm\_config|SNMPv3 USM configuration.|
|trap\_target\_config|SNMPv3 trap configuration The SNMP server will access snmp.bib file or other MIB files located under this top directory. No access is possible above this directory. As a good practice to follow, use something like: "/mnt\_point/snmp/" to limit an external user access to this only directory in the file system when accessing files.|

## Remarks

None.

