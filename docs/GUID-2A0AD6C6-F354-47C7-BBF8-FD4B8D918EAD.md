# DRV\_GMAC\_EventPendingGet Function

**Parent topic:**[Ethernet GMAC Driver Library](GUID-A24BDAD2-C63E-40B1-894D-1DC3CC6CB66A.md)

## C

```
TCPIP_MAC_EVENT DRV_GMAC_EventPendingGet(DRV_HANDLE hMac); 
```

## Returns

The currently stack pending events.

## Description

This function returns the currently pending Ethernet MAC events. Multiple events will be ORed together as they accumulate. The stack should perform processing whenever a transmission related event \(TCPIP\_MAC\_EV\_RX\_PKTPEND, TCPIP\_MAC\_EV\_TX\_DONE\) is present. The other, non critical events, may not be managed by the stack and passed to an user. They will have to be eventually acknowledged if re-enabling is needed.

## Remarks

-   This is the preferred method to get the current pending MAC events. The stack maintains a proper image of the events from their occurrence to their acknowledgment.

-   Even with a notification handler in place it's better to use this function to get the current pending events rather than using the events passed by the notification handler which could be stale.

-   The events are persistent. They shouldn't be re-enabled unless they have been processed and the condition that generated them was removed. Re-enabling them immediately without proper processing will have dramatic effects on system performance.

-   The returned value is just a momentary value. The pending events can change any time.


## Preconditions

DRV\_GMAC\_Initialize\(\) should have been called. DRV\_GMAC\_Open\(\) should have been called to obtain a valid handle.

## Example

TCPIP\_MAC\_EVENT currEvents = DRV\_GMAC\_EventPendingGet\( hMac\);

