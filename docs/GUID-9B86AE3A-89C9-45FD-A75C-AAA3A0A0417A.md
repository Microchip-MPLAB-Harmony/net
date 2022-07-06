# TCPIP\_TCP\_Abort Function

**Parent topic:**[TCP Module](GUID-9461917B-27CE-44ED-80DB-67D963896E8F.md)

## C

```
void TCPIP_TCP_Abort(
    TCP_SOCKET hTCP, 
    bool killSocket
);
```

## Description

This function aborts a connection to a remote node by sending a RST \(if currently connected\). Any pending TX/RX data is discarded.

A client socket will always be closed and the associated resources released. The socket cannot be used again after this call.

A server socket will abort the current connection:

-   if killSocket == false the socket will remain listening

-   if killSocket == true the socket will be closed and all associated resources released. The socket cannot be used again after this call.


## Preconditions

TCP socket should have been opened with TCPIP\_TCP\_ServerOpen\(\)/TCPIP\_TCP\_ClientOpen\(\). hTCP - valid socket.

## Parameters

|Parameters|Description|
|----------|-----------|
|hTCP|Handle to the socket to disconnect.|
|killSocket|If true, it kills a server socket.|

## Returns

None.

## Remarks

None.

