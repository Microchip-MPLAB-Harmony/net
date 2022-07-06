# TCPIP\_TCP\_SignalHandlerRegister Function

**Parent topic:**[TCP Module](GUID-9461917B-27CE-44ED-80DB-67D963896E8F.md)

## C

```
TCPIP_TCP_SIGNAL_HANDLE TCPIP_TCP_SignalHandlerRegister(
    TCP_SOCKET s, 
    TCPIP_TCP_SIGNAL_TYPE sigMask, 
    TCPIP_TCP_SIGNAL_FUNCTION handler, 
    const void* hParam
);
```

## Description

This function registers a TCP socket signal handler. The TCP module will call the registered handler when a TCP signal \(TCPIP\_TCP\_SIGNAL\_TYPE\) occurs.

## Preconditions

TCP valid socket.

## Parameters

|Parameters|Description|
|----------|-----------|
|s|The TCP socket.|
|sigMask|Mask of signals to be reported.|
|handler|SIgnal handler to be called when a TCP event occurs.|
|hParam|Parameter to be used in the handler call. This is user supplied and is not used by the TCP module.|

## Returns

-   Returns a valid handle if the call succeeds

-   Returns a null handle if the call failed \(null handler, no such socket, existent handler\).


## Remarks

Only one signal handler per socket is supported. A new handler does not override the existent one. Instead TCPIP\_TCP\_SignalHandlerDeregister has to be explicitly called.

The handler has to be short and fast. It is meant for setting an event flag, not for lengthy processing.

The hParam is passed by the client.

For multi-threaded systems the TCP/IP packet dispatch does not occur on the user thread. The signal handler will be called on a different thread context. It is essential that this handler is non blocking and really fast.

For multi-threaded systems, once set, it is not recommended to change the signal handler at run time. Synchronization between user threads and packet dispatch threads could be difficult. If really need to be changed, make sure that the old handler could still be called and it should be valid until the new one is taken into account. TCPIP\_TCP\_SignalHandlerDeregister needs to be called before registering another handler.

