# TCPIP\_SNMP\_ReadCommunitySet Function

**Parent topic:**[SNMP Module](GUID-7764E81C-8FC9-4B3E-8830-255BDE678AA0.md)

## C

```
bool TCPIP_SNMP_ReadCommunitySet(
    int index, 
    int len, 
    uint8_t * src
);
```

## Description

This function is used to configure READ community string from the user and configure the SNMP community table.

## Preconditions

TCPIP\_SNMP\_Initialize\(\) is already called.

## Parameters

|Parameters|Description|
|----------|-----------|
|index|One of the index of community table and it should be less than TCPIP\_SNMP\_MAX\_COMMUNITY\_SUPPORT.|
|len|Length of the community string expected. It should not be more than TCPIP\_SNMP\_COMMUNITY\_MAX\_LEN.|
|src|Copy this community string to snmp community table.|

## Returns

-   True - If the community string is collected.

-   False - If the community string is not collected.


## Remarks

None.

