# TCPIP\_UDP\_IsConnected Function

**Parent topic:**[UDP Module](GUID-D2D8E9C8-0778-41E2-8F0B-194954B92250.md)

## C

```
bool TCPIP_UDP_IsConnected(UDP_SOCKET hUDP); 
```

## Description

This function determines if a socket has an established connection to a remote node. Call this function after calling TCPIP\_UDP\_ServerOpen/TCPIP\_UDP\_ClientOpen to determine when the connection is set up and ready for use.

## Remarks

An UDP server or client socket can always send data, as long as it has a valid destination address, even if there's no remote socket connected to it. This function will return true if the socket is opened and currently has a valid destination. Note that this is true if a client socket was opened with a remote host address, a socket received data from a remote host, or a explicit remote bind was done.

## Preconditions

None.

## Parameters

|Parameters|Description|
|----------|-----------|
|hUDP|The socket to check.|

