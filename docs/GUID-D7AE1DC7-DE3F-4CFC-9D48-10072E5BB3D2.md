# TCPIP\_UDP\_SignalHandlerRegister Function

**Parent topic:**[UDP Module](GUID-D2D8E9C8-0778-41E2-8F0B-194954B92250.md)

## C

```
TCPIP_UDP_SIGNAL_HANDLE TCPIP_UDP_SignalHandlerRegister(UDP_SOCKET s, TCPIP_UDP_SIGNAL_TYPE sigMask, TCPIP_UDP_SIGNAL_FUNCTION handler, const void* hParam); 
```

## Returns

## Returns a valid handle if the call succeeds, or a null handle if the call failed \(null handler, no such socket, existent handler\).

## Description

This function registers a UDP socket signal handler. The UDP module will call the registered handler when a UDP signal \(TCPIP\_UDP\_SIGNAL\_TYPE\) occurs.

## Remarks

Only one signal handler per socket is supported. A new handler does not override the existent one. Instead TCPIP\_UDP\_SignalHandlerDeregister has to be explicitly called. The handler has to be short and fast. It is meant for setting an event flag, not for lengthy processing!<br />The hParam is passed by the client but is currently not used and should be 0. For multi-threaded systems the TCP/IP packet dispatch does not occur on the user thread. The signal handler will be called on a different thread context. It is essential that this handler is non blocking and really fast.<br />For multi-threaded systems, once set, it is not recommended to change the signal handler at run time. Synchronization between user threads and packet dispatch threads could be difficult. If really need to be changed, make sure that the old handler could still be called and it should be valid until the new one is taken into account. TCPIP\_UDP\_SignalHandlerDeregister needs to be called before registering another handler.

## Preconditions

UDP valid socket.

## Parameters

|Parameters|Description|
|----------|-----------|
|s|The UDP socket|
|sigMask|mask of signals to be reported|
|handler|signal handler to be called when a UDP signal occurs.|
|hParam|Parameter to be used in the handler call. This is user supplied and is not used by the UDP module. Currently not used and it should be null.|
|||

