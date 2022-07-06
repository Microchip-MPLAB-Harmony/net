# TCPIP\_SNMP\_SendFailureTrap Function

**Parent topic:**[SNMP Module](GUID-7764E81C-8FC9-4B3E-8830-255BDE678AA0.md)

## C

```
void TCPIP_SNMP_SendFailureTrap();
```

## Description

This function is used to send trap notification to previously configured IP address if trap notification is enabled. There are different trap notification code. The current implementation sends trap for authentication failure \(4\).

## Preconditions

If the defined application event occurs to send the trap.

## Returns

None.

## Remarks

This is a callback function called by the application on certain predefined events. This routine only implemented to send a authentication failure Notification-type macro with PUSH\_BUTTON OID stored in MPFS. If the ARP is not resolved \(i.e., if TCPIP\_SNMP\_NotifyIsReady returns false, this routine times out in 5 seconds\). This function should be modified according to event occurred and should update the corresponding OID and notification type to the TRAP PDU.

