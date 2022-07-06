# TCPIP\_SNMP\_NextIndexGet Function

**Parent topic:**[SNMP Module](GUID-7764E81C-8FC9-4B3E-8830-255BDE678AA0.md)

## C

```
bool TCPIP_SNMP_NextIndexGet(
    SNMP_ID var, 
    SNMP_INDEX* index
);
```

## Description

This is a callback function called by SNMP module. This function contents are modified by the application developer with the new MIB Record ID. This function can be called for individual MIB ID with index or for a complete MIB table without any instance.

-   If the SNMP Sequence MIB variable processing is performed with index or instance value, it is the responsibility of the agent to send the next available index for that requested variable. This is a only one iteration process. The Manager will not send any further requests for the received index value.

-   If SNMP request for a sequence variable starts with only OID without any instance, that is a complete table is requested without any instance, it is the responsibility of the agent to send the first available index of the table. The Manager will continue the request with the transferred instance until it receives the reply from agent.


This function will only be called for OID variable of type sequence. if the index value starts with SNMP\_INDEX\_INVALID , then user need to send the response with first available index value.

## Preconditions

TCPIP\_SNMP\_ProcessVariables\(\) is called.

## Parameters

|Parameters|Description|
|----------|-----------|
|var|Variable id whose value is to be returned.|
|index|Next Index of variable that should be transferred.|

## Returns

-   True - If a next index value exists for given variable at given index and index parameter contains next valid index.

-   False - If a next index value does not exist for a given variable.


## Remarks

Only sequence index needs to be handled in this function and this function is called after TCPIP\_SNMP\_RecordIDValidation\(\).

