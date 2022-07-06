# TCPIP\_TFTPC\_OPERATION\_RESULT Enumeration

**Parent topic:**[TFTP Client Module](GUID-5AE30EFB-7B7C-4B4E-8BCF-FAB8D8FB9A89.md)

## C

```
typedef enum {
    TFTPC_ERROR_NONE = 0,
    TFTPC_ERROR_FILE_NOT_FOUND = -1,
    TFTPC_ERROR_BUSY = -2,
    TFTPC_ERROR_DISK_FULL = -3,
    TFTPC_ERROR_INVALID_OPERATION = -4,
    TFTPC_ERROR_UNKNOWN_TID = -5,
    TFTPC_ERROR_FILE_EXISTS = -6,
    TFTPC_ERROR_NO_SUCH_USE = -7,
    TFTPC_ERROR_DNS_RESOLVE_ERR = -8,
    TFTPC_ERROR_INVALID_INTERFACE = -9,
    TFTPC_ERROR_INVALID_FILE_LENGTH = -10,
    TFTPC_ERROR_INVALID_SERVER_ADDR = -11
} TCPIP_TFTPC_OPERATION_RESULT;
```

## Description

Enumeration: TCPIP\_TFTPC\_OPERATION\_RESULT.

This enumeration defines the standard error codes for TFTP PUT and GET command operation.

## Members

|Members|Description|
|-------|-----------|
|TFTPC\_ERROR\_FILE\_NOT\_FOUND = -1|TFTP client file not found.|
|TFTPC\_ERROR\_BUSY = -2|TFTP client is busy when one file PUT or GET transfer is going on.|
|TFTPC\_ERROR\_DISK\_FULL = -3|TFTP client buffer full.|
|TFTPC\_ERROR\_INVALID\_OPERATION = -4|TFTP client invalid command operation.|
|TFTPC\_ERROR\_UNKNOWN\_TID = -5|TfTP ID error.|
|TFTPC\_ERROR\_FILE\_EXISTS = -6|TFTP client file already exists.|
|TFTPC\_ERROR\_NO\_SUCH\_USE = -7|TFTP client not in use.|
|TFTPC\_ERROR\_DNS\_RESOLVE\_ERR = -8|TFTP client DNS resolve error.|
|TFTPC\_ERROR\_INVALID\_INTERFACE = -9|TFTP client interface error.|
|TFTPC\_ERROR\_INVALID\_FILE\_LENGTH = -10|TFTP client file length is more than the expected size, which should be the size of SYS\_FS\_FILE\_NAME\_LEN.|
|TFTPC\_ERROR\_INVALID\_SERVER\_ADDR = -11|Invali Server Address.|

