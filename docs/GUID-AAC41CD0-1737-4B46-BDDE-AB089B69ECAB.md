# TCPIP\_DDNS\_UpdateForce Function

**Parent topic:**[Dynamic DNS Module](GUID-2F39A35D-A741-49F8-BAA9-3B3C9B665486.md)

## C

```
void TCPIP_DDNS_UpdateForce();
```

## Description

This function forces the DDNS Client to execute a full update immediately. Any error message is cleared, and the update will be executed whether the IP address has changed or not. Call this function every time the DDNSClient parameters have been modified.

## Preconditions

TCPIP\_DDNS\_Initialize\(\) must have been called.

## Returns

None.

