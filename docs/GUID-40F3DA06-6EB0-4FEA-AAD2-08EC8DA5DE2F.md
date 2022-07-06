# TCPIP\_UDP\_PacketHandlerRegister Function

**Parent topic:**[UDP Module](GUID-D2D8E9C8-0778-41E2-8F0B-194954B92250.md)

## C

```
TCPIP_UDP_PROCESS_HANDLE TCPIP_UDP_PacketHandlerRegister(TCPIP_UDP_PACKET_HANDLER pktHandler, const void* handlerParam); 
```

## Returns

-   a valid TCPIP\_UDP\_PROCESS\_HANDLE - if the operation succeeded

-   NULL - if the operation failed


## Description

This function registers a new packet processing handler. The caller can use the handler to be notified of incoming packets and given a chance to examine/process them.

## Remarks

Currently only one packet handler is supported for the UDP module. The call will fail if a handler is already registered. Use TCPIP\_UDP\_PacketHandlerDeregister first Exists only if TCPIP\_UDP\_EXTERN\_PACKET\_PROCESS is true

## Preconditions

UDP properly initialized

## Example

TCPIP\_UDP\_PROCESS\_HANDLE pktHandle = TCPIP\_UDP\_PacketHandlerRegister\( myPktHandler, myParam \);

## Parameters

|Parameters|Description|
|----------|-----------|
|pktHandler|the packet handler which will be called for an incoming packet|
|handlerParam|packet handler parameter|
|||

