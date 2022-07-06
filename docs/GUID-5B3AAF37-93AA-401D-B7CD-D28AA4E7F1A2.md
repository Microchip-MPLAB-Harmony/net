# TCPIP\_SNMP\_TrapSendFlagSet Function

**Parent topic:**[SNMP Module](GUID-7764E81C-8FC9-4B3E-8830-255BDE678AA0.md)

## C

```
void TCPIP_SNMP_TrapSendFlagSet(
    bool trapNotify
);
```

## Description

This function is used to set the trap send flag details and this is used only when user is trying to send more than one varbind in a single PDU. That is more than one notification details are required to be part of a single PDU. By default TRAP send flag is set to false. If there is only varbind need to be part of Notification PDU, then this function should be called with boolean false. Please find the usage of this flag in this following example.

## Preconditions

TCPIP\_SNMP\_Initialize\(\) is already called.

## Returns

None.

## Remarks

None.

## Example

```
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
```

