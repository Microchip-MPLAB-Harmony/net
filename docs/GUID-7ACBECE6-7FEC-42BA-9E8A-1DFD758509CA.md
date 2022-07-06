# TCPIP\_SNMPV3\_EngineUserDataBaseSet Function

**Parent topic:**[SNMP Module](GUID-7764E81C-8FC9-4B3E-8830-255BDE678AA0.md)

## C

```
bool TCPIP_SNMPV3_EngineUserDataBaseSet(
    TCPIP_SNMPV3_USERDATABASECONFIG_TYPE userDataBaseType, 
    uint8_t len, 
    uint8_t userIndex, 
    void * val
);
```

## Description

This function is used to set SNMPv3 Engine data base details using TCPIP\_SNMPV3\_USERDATABASECONFIG\_TYPE enumeration.

## Preconditions

TCPIP\_SNMP\_Initialize\(\) is already called.

## Parameters

|Parameters|Description|
|----------|-----------|
|userDataBaseType|SNMPv3 data base configuration type.|
|len|Number of bytes need to be read from data base.|
|userIndex|SNMPv3 user index.|
|val|Void pointer to a any data type. Length parameter value is changed according to data type.|

## Returns

None.

## Remarks

None.

## Example

```
bool TCPIP_SNMP_VarbindSet(SNMP_ID var, SNMP_INDEX index, uint8_t ref, SNMP_VAL val)
{
switch(var)
{
    case USER_SECURITY_NAME:
        if ( ref ==  SNMP_END_OF_VAR )
        {
            if(TCPIP_SNMPV3_EngineUserDataBaseSet(SNMPV3_USERNAME_CONFIG_TYPE,strlen((char*)gSnmpv3UserSecurityName),
                                            index,gSnmpv3UserSecurityName) != true)
                return false;
        }
        // Make sure that index is within our range.
        if ( index < TCPIP_SNMPV3_USM_MAX_USER )
        {
            // Copy given value into local buffer.
            gSnmpv3UserSecurityName[ref]=val.byte;
            return true;
        }
        break;
}
}
```

