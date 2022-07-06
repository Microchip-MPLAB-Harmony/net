# TCPIP\_STACK\_HEAP\_USAGE Enumeration

**Parent topic:**[Manager Module](GUID-B37C4F4C-DC2D-48D9-9909-AACBA987B57A.md)

## C

```
typedef enum {
TCPIP_STACK_HEAP_USE_DEFAULT = 0x00,
TCPIP_STACK_HEAP_USE_GENERIC = 0x01,
TCPIP_STACK_HEAP_USE_PACKET = 0x02,
TCPIP_STACK_HEAP_USE_SOCKET_TCP = 0x04,
TCPIP_STACK_HEAP_USE_SOCKET_UDP = 0x08,
TCPIP_STACK_HEAP_USE_POOL_UDP = 0x10
} TCPIP_STACK_HEAP_USAGE;
```

## Description

Enumeration: TCPIP\_STACK\_HEAP\_USAGE.

Definition of the heap usage/purpose. It defines what modules in the TCP/IP stack will use this heap.

## Preconditions

None.

## Members

|Members|Description|
|-------|-----------|
|TCPIP\_STACK\_HEAP\_USE\_DEFAULT = 0x00|Default heap usage.|
|TCPIP\_STACK\_HEAP\_USE\_GENERIC = 0x01|Generic heap usage.|
|TCPIP\_STACK\_HEAP\_USE\_PACKET = 0x02|Heap for packet allocations.|
|TCPIP\_STACK\_HEAP\_USE\_SOCKET\_TCP = 0x04|Heap for TCP sockets.|
|TCPIP\_STACK\_HEAP\_USE\_SOCKET\_UDP = 0x08|Heap for UDP sockets.|
|TCPIP\_STACK\_HEAP\_USE\_POOL\_UDP = 0x10|Heap for UDP TX pool buffers.|

## Remarks

This is an advanced definition that is currently not used. Multiple heaps usage will be added in the future. For now, only one heap type is supported and it is used for all allocation operations. New usages could be eventually added. Multiple usage flags can be simultaneously set.

