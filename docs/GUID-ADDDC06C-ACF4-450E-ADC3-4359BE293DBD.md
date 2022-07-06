# TCPIP\_HTTP\_CurrentConnectionByteCountSet Function

**Parent topic:**[HTTP Module](GUID-25A4CF50-2F8F-47E7-A90C-ABFA52814459.md)

## C

```
void TCPIP_HTTP_CurrentConnectionByteCountSet(
    HTTP_CONN_HANDLE connHandle, 
    uint32_t byteCount
);
```

## Description

This function sets the current value of the counter showing the number of bytes available to be read from the connection.

## Preconditions

None.

## Parameters

|Parameters|Description|
|----------|-----------|
|connHandle|HTTP connection handle.|
|byteCount|Byte count to be set.|

## Returns

None.

## Remarks

None.

