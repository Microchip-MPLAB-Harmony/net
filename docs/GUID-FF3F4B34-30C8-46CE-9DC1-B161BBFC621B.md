# TCPIP\_TCP\_PutIsReady Function

**Parent topic:**[TCP Module](GUID-9461917B-27CE-44ED-80DB-67D963896E8F.md)

## C

```
uint16_t TCPIP_TCP_PutIsReady(
    TCP_SOCKET hTCP
);
```

## Description

Call this function to determine how many bytes can be written to the TCP TX buffer. If this function returns zero, the application must return to the main stack loop before continuing in order to transmit more data.

## Preconditions

TCP is initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|hTCP|The socket to check.|

## Returns

The number of bytes available to be written in the TCP TX buffer.

## Remarks

None.

