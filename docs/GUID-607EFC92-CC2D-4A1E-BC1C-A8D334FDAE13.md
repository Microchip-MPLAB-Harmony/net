# TCPIP\_UDP\_Disconnect Function

**Parent topic:**[UDP Module](GUID-D2D8E9C8-0778-41E2-8F0B-194954B92250.md)

## C

```
bool TCPIP_UDP_Disconnect(UDP_SOCKET hUDP, bool flushRxQueue); 
```

## Returns

-   true - Indicates success

-   false - Indicates failure


## Description

Disconnects a UDP socket and re-initializes it. Call this function to return the socket to its initial open state and to use it for future communication.<br />This function is meant especially for server sockets that could listen on multiple interfaces and on both IPv4 and IPv6 networks. When a server socket received an inbound IPv4 connection it will be bound to IPv4 connections until it's closed or disconnected. Same is true for IPv6 connections.

## Remarks

The call will try to maintain as much as possible from the socket state. For a server socket the remote port will be cleared unless a TCPIP\_UDP\_RemoteBind\(\) was performed. If the remote port is<br />cleared, a new TCPIP\_UDP\_RemoteBind or TCPIP\_UDP\_DestinationPortSet operation will be needed if the remote port needs to be maintained. For a client socket the remote port is maintained. This will free the allocated TX buffers if the socket was opened with IP\_ADDRESS\_TYPE\_ANY. All the pending RX packets will be cleared when flushRxQueue is set. Otherwise the packets will be kept and will be available for<br />next read operations. Note that if the RX pending packets exist and are not flushed, the socket will be bound immediately to the connection corresponding to the pending packet in the queue.

## Preconditions

UDP socket should have been opened with TCPIP\_UDP\_ServerOpen\(\)/TCPIP\_UDP\_ClientOpen\(\)\(\). hUDP - valid socket

## Parameters

|Parameters|Description|
|----------|-----------|
|hUDP|The socket handle to be disconnected.|
|flushRxQueue|boolean to flush the pending RX queue|
|||

