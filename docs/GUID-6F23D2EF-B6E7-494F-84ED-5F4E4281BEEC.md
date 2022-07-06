# TCPIP\_TCP\_Flush Function

**Parent topic:**[TCP Module](GUID-9461917B-27CE-44ED-80DB-67D963896E8F.md)

## C

```
bool TCPIP_TCP_Flush(
    TCP_SOCKET hTCP
);
```

## Description

This function immediately transmits all pending TX data with a PSH flag. If this function is not called, data will automatically be sent by the TCP state machine \(see the TCPIP\_TCP\_ArrayPut\(\) description/notes\).

## Preconditions

TCP is initialized and the socket is connected.

## Parameters

|Parameters|Description|
|----------|-----------|
|hTCP|The socket whose data is to be transmitted.|

## Returns

-   True - If data is flushed from the socket.

-   False - Otherwise.


## Remarks

The application should not call this function explicitly because doing this will interfere with the TCP algorithm and degrade performance of the socket data transfer. One exception is when the application knows that it put all the data it needed into the TCP buffer and it makes sense to flush the socket instead of waiting TCP\_AUTO\_TRANSMIT\_TIMEOUT\_VAL timeout to elapse.

