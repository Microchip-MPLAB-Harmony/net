# TCPIP\_SNMP\_TRAPTypeGet Function

**Parent topic:**[SNMP Module](GUID-7764E81C-8FC9-4B3E-8830-255BDE678AA0.md)

## C

```
bool TCPIP_SNMP_TRAPTypeGet();
```

## Description

This function returns true if the trap tye is v2 and the TRAP pdu packet will be a TRAP v2 format.The return value is also used is validated when SNMP module is trying to send a trap for SNMP version v3.

## Preconditions

TCPIP\_SNMP\_Initialize\(\) is already called.

## Returns

-   True - Trap version is v2.

-   False - Trap version is v1.


## Remarks

This function is used by the customer function.

