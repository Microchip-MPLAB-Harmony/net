# TCPIP\_TCP\_Find Function

**Parent topic:**[TCP Module](GUID-9461917B-27CE-44ED-80DB-67D963896E8F.md)

## C

```
uint16_t TCPIP_TCP_Find(
    TCP_SOCKET hTCP, 
    uint8_t cFind, 
    uint16_t wStart, 
    uint16_t wSearchLen, 
    bool bTextCompare
);
```

## Description

This function finds the first occurrence of a byte in the TCP RX buffer. It can be used by an application to abstract searches out of their own application code. For increased efficiency, the function is capable of limiting the scope of search to a specific range of bytes. It can also perform a case-insensitive search if required.

For example, if the buffer contains "I love PIC MCUs!" and the cFind byte is ' ', a value of 1 will be returned.

## Preconditions

TCP is initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|hTCP|The socket to search within.|
|cFind|The byte to find in the buffer.|
|wStart|Zero-indexed starting position within the buffer.|
|wSearchLen|Length from wStart to search in the buffer.|
|bTextCompare|True for case-insensitive text search, false for binary search.|

## Remarks

For better performance of this function, try to search for characters that are expected to exist or limit the scope of the search as much as possible. The HTTP module, for example, uses this function to parse headers. However, it searches for newlines, then the separating colon, then reads the header name to an internal buffer for comparison. This has proven to be significantly faster than searching for full header name strings outright.

