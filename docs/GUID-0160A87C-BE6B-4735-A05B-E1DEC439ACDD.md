# TCPIP\_UDP\_SocketsNumberGet Function

**Parent topic:**[UDP Module](GUID-D2D8E9C8-0778-41E2-8F0B-194954B92250.md)

## C

```
int TCPIP_UDP_SocketsNumberGet(); 
```

## Returns

The number of UDP sockets

## Description

This function returns the number of created UDP sockets. This is the maximum number of sockets that can be opened at any moment as it's been passed as parameter when UDP module was created.

## Preconditions

UDP module properly initialized

