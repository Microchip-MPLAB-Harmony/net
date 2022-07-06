# TCPIP\_SNMP\_TrapSpecificNotificationGet Function

**Parent topic:**[SNMP Module](GUID-7764E81C-8FC9-4B3E-8830-255BDE678AA0.md)

## C

```
void TCPIP_SNMP_TrapSpecificNotificationGet(
    uint8_t * specTrap
);
```

## Description

This function is used to get specific trap value. Specific trap values are listed in SNMP\_VENDOR\_SPECIFIC\_TRAP\_NOTIFICATION\_TYPE.

## Preconditions

TCPIP\_SNMP\_Initialize\(\) is already called.

## Parameters

|Parameters|Description|
|----------|-----------|
|specTrap|Vendor specific trap value.|

## Returns

None.

## Remarks

None.

