# TCPIP\_TCP\_WasReset Function

**Parent topic:**[TCP Module](GUID-9461917B-27CE-44ED-80DB-67D963896E8F.md)

## C

```
bool TCPIP_TCP_WasReset(
    TCP_SOCKET hTCP
);
```

## Description

This function is a self-clearing semaphore indicating whether or not a socket has been reset since the previous call. This function indicates that a RST was received from the remote node, a TCPIP\_TCP\_Disconnect was called or the socket was somehow re-initalized.

It also returns true just after the socket was opened.

## Preconditions

TCP is initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|hTCP|The socket to check.|

## Returns

None.

## Remarks

A server socket upon receiving a RST signal will go to listen state.

A client socket will be closed when RST is received.

The info that the FIN was received could be also obtained by registering a signal with the socket \(the preferred way\).

