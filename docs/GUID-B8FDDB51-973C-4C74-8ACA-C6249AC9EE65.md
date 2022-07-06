# TCPIP\_IPV6\_PacketHandlerDeregister Function

**Parent topic:**[IPv6 Module](GUID-F2484EF9-7914-43EE-A5B7-4FFDC27C8135.md)

## C

```
bool TCPIP_IPV6_PacketHandlerDeregister(
    TCPIP_IPV6_PROCESS_HANDLE pktHandle
);
```

## Description

This function removes a packet processing handler.

## Preconditions

IPv6 properly initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|pktHandle|TCPIP packet handle obtained by a call to TCPIP\_IPV6\_PacketHandlerRegister\(\).|

## Returns

-   True - If the operation succeeded.

-   False - If the operation failed.


## Remarks

Exists only if TCPIP\_IPV6\_EXTERN\_PACKET\_PROCESS is true.

## Example

```
TCPIP_IPV6_PROCESS_HANDLE myHandle = TCPIP_IPV6_PacketHandlerRegister(myPacketHandler, myParam );
// process incoming packets
// now we're done with it
TCPIP_IPV6_PacketHandlerDeregister(myHandle);
```

