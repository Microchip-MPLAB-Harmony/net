# TCPIP\_IPV6\_DestAddressSet Function

**Parent topic:**[IPv6 Module](GUID-F2484EF9-7914-43EE-A5B7-4FFDC27C8135.md)

## C

```
void TCPIP_IPV6_DestAddressSet(
    IPV6_PACKET * p, 
    const IPV6_ADDR * addr
);
```

## Description

This API is used to configure the destination address for an IPv6 packet.

## Preconditions

TCPIP\_IPV6\_Initialize\(\) should be called. TCPIP\_IPV6\_InterfaceIsReady should be true.

## Parameters

|Parameters|Description|
|----------|-----------|
|p|Pointer to IPv6 packet.|
|addr|Destination address.|

## Returns

None.

## Remarks

None.

