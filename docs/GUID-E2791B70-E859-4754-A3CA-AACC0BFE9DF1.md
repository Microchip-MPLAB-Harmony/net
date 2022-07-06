# TCPIP\_HTTP\_PostValueRead Function

**Parent topic:**[HTTP Module](GUID-25A4CF50-2F8F-47E7-A90C-ABFA52814459.md)

## C

```
HTTP_READ_STATUS TCPIP_HTTP_PostValueRead(
    HTTP_CONN_HANDLE connHandle, 
    uint8_t* cData, 
    uint16_t wLen
);
```

## Description

This function reads a value from a URL encoded string in the TCP buffer. This function is meant to be called from a TCPIP\_HTTP\_PostExecute callback to facilitate easier parsing of incoming data. This function also prevents buffer overflows by forcing the programmer to indicate how many bytes are expected. At least 2 extra bytes are need in cData above the maximum length of data expected to be read.

This function will read until the next '&' character, which indicates the end of a value parameter. It assumes that the front of the buffer is the beginning of the value parameter to be read. If *pHttpCon-\>byteCount* indicates that all expected bytes are in the buffer, it assumes that all remaining data is the value and acts accordingly.

This function properly updates *pHttpCon-\>byteCount* by decrementing it by the number of bytes read. The terminating '&' character is also removed from the buffer.

## Preconditions

The front of the TCP buffer is the beginning of a name parameter, and the rest of the TCP buffer contains a URL-encoded string with a name parameter terminated by a '=' character.

## Parameters

|Parameters|Description|
|----------|-----------|
|connHandle|HTTP connection handle.|
|cData|Where to store the value once it is read.|
|wLen|How many bytes can be written to cData.|

## Returns

-   HTTP\_READ-OK - Value was successfully read.

-   HTTP\_READ\_TRUNCATED - Entire value could not fin in the buffer, so the value was truncated and data has been lost.

-   HTTP\_READ\_INCOMPLETE - Entire value was not yet in the buffer, so call this function again later to retrieve.


## Remarks

None.

