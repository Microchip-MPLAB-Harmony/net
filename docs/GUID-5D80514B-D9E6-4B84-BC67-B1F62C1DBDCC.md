# TCPIP\_TCP\_Discard Function

**Parent topic:**[TCP Module](GUID-9461917B-27CE-44ED-80DB-67D963896E8F.md)

## C

```
uint16_t TCPIP_TCP_Discard(
    TCP_SOCKET hTCP
);
```

## Description

This function discards any pending data in the TCP RX FIFO.

## Preconditions

TCP is initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|hTCP|The socket whose RX buffer/FIFO is to be cleared.|

## Returns

The number of bytes that have been discarded from the RX buffer.

