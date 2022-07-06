# TCPIP\_HTTP\_CurrentConnectionDataBufferSizeGet Function

**Parent topic:**[HTTP Module](GUID-25A4CF50-2F8F-47E7-A90C-ABFA52814459.md)

## C

```
uint16_t TCPIP_HTTP_CurrentConnectionDataBufferSizeGet(
    HTTP_CONN_HANDLE connHandle
);
```

## Description

This function returns the size of the HTTP connection internal data buffer. This buffer is created at the HTTP initialization using the size specified in TCPIP\_HTTP\_MODULE\_CONFIG.dataLen .

## Preconditions

None.

## Parameters

|Parameters|Description|
|----------|-----------|
|connHandle|HTTP connection handle.|

## Returns

Size of the connection's general purpose data buffer.

## Remarks

Currently the size of the buffer is the same for all connections. The connHandle parameter is not used.

