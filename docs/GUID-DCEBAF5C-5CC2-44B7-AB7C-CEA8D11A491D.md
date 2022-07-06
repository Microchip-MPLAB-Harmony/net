# TCPIP\_STACK\_HandlerRegister Function

**Parent topic:**[Manager Module](GUID-B37C4F4C-DC2D-48D9-9909-AACBA987B57A.md)

## C

```
TCPIP_EVENT_HANDLE TCPIP_STACK_HandlerRegister(
    TCPIP_NET_HANDLE hNet, 
    TCPIP_EVENT evMask, 
    TCPIP_STACK_EVENT_HANDLER notifyHandler, 
    const void* notifyfParam
);
```

## Description

This function sets a new event notification handler. The caller can use the handler to be notified of stack events.

## Preconditions

The TCP/IP stack should have been initialized by TCPIP\_STACK\_Initialize\(\) and the TCPIP\_STACK\_Status\(\) returned SYS\_STATUS\_Ready. The network interface should be up and running.

## Parameters

|Parameters|Description|
|----------|-----------|
|hNet|Network handle.|
|evMask|Mask of events to be notified of.|
|notifyHandler|The event notification handler.|
|notifyfParam|Notification handler parameter.|

## Returns

-   A valid TCPIP\_EVENT\_HANDLE - If the operation succeeded.

-   NULL - If the operation failed.


## Remarks

The notification handler may be called from the ISR which detects the corresponding event. The event notification handler has to be kept as short as possible and non-blocking.

Without a notification handler the stack user can still call TCPIP\_STACK\_GetPending\(\) to see if processing by the stack needed. Currently only a primary interface can report events. The call will fail if the specified network is an alias.

## Example

```
TCPIP_EVENT_HANDLE myHandle = TCPIP_STACK_HandlerRegister( hNet, TCPIP_EV_CONN_ALL, myEventHandler, myParam );
```

