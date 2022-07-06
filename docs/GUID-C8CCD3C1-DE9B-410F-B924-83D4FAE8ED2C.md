# TCPIP\_SNMP\_TrapSpecificNotificationSet Function

**Parent topic:**[SNMP Module](GUID-7764E81C-8FC9-4B3E-8830-255BDE678AA0.md)

## C

```
void TCPIP_SNMP_TrapSpecificNotificationSet(
    uint8_t specTrap, 
    uint8_t genTrap, 
    SNMP_ID trapID
);
```

## Description

The SNMP user needs to call this function before transmitting any traps. This function will set the vendor specific trap, generic trap, and default trap ID value. The SNMPv2 trap will use this trap ID while sending a specific trap.

## Preconditions

TCPIP\_SNMP\_Initialize\(\) is already called.

## Parameters

|Parameters|Description|
|----------|-----------|
|specTrap|Vendor specific trap value \(enumeration value of SNMP\_VENDOR\_SPECIFIC\_TRAP\_NOTIFICATION\_TYPE\).|
|genTrap|Generic trap \(enumeration value of SNMP\_GENERIC\_TRAP\_NOTIFICATION\_TYPE\).|
|trapID|Trap ID.|

## Returns

None.

## Remarks

The Trap ID is the NOTIFICATION-TYPE of the ASN.1 MIB format. From the Trap ID, the SNMP agent will be able to obtain the OID string, which will be used while preparing TRAPv2 second varbind.

