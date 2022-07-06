# TCPIP\_SNMP\_ReadCommunityGet Function

**Parent topic:**[SNMP Module](GUID-7764E81C-8FC9-4B3E-8830-255BDE678AA0.md)

## C

```
bool TCPIP_SNMP_ReadCommunityGet(
    int index, 
    int len, 
    uint8_t * dest
);
```

## Description

This function is used to collect READ community string from the global community table with respect to the index value.

## Preconditions

TCPIP\_SNMP\_Initialize\(\) is already called.

## Parameters

|Parameters|Description|
|----------|-----------|
|index|One of the index of community table and it should be less than TCPIP\_SNMP\_MAX\_COMMUNITY\_SUPPORT.|
|len|Length of the community string expected. It should not be more than TCPIP\_SNMP\_COMMUNITY\_MAX\_LEN.|
|dest|Copy the community string to this address and it should have a valid address.|

## Returns

-   True - If the community string is collected.

-   False - If the community string is not collected.


## Remarks

None.

