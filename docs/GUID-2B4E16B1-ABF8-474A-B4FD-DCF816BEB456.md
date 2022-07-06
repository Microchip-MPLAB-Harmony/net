# TCPIP\_TCP\_SignalHandlerDeregister Function

**Parent topic:**[TCP Module](GUID-9461917B-27CE-44ED-80DB-67D963896E8F.md)

## C

```
bool TCPIP_TCP_SignalHandlerDeregister(
    TCP_SOCKET s, 
    TCPIP_TCP_SIGNAL_HANDLE hSig
);
```

## Description

This function deregisters the TCP socket signal handler.

## Preconditions

hSig valid TCP handle.

## Parameters

|Parameters|Description|
|----------|-----------|
|s|The TCP socket.|
|hSig|A handle returned by a previous call to TCPIP\_TCP\_SignalHandlerRegister.|

## Returns

-   True - If the call succeeds.

-   False - If no such handler is registered.


## Remarks

None.

