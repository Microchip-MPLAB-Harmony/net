# TCP\_OPTION\_THRES\_FLUSH\_TYPE Enumeration

**Parent topic:**[TCP Module](GUID-9461917B-27CE-44ED-80DB-67D963896E8F.md)

## C

```
typedef enum {
TCP_OPTION_THRES_FLUSH_AUTO,
TCP_OPTION_THRES_FLUSH_OFF,
TCP_OPTION_THRES_FLUSH_ON
} TCP_OPTION_THRES_FLUSH_TYPE;
```

## Description

Enumeration: TCP\_OPTION\_THRES\_FLUSH\_TYPE.

Describes the possible types of the socket half threshold TX flush.

## Parameters

|Parameters|Description|
|----------|-----------|
|TCP\_OPTION\_THRES\_FLUSH\_AUTO|The socket will set the half buffer flush based on the TX buffer size. This is the default setting. For TX buffers \>= 1.5 MSS the half buffer flush will be disabled. This results in higher performance/throughput for the socket.|
|TCP\_OPTION\_THRES\_FLUSH\_OFF|Always disable the socket flush at half buffer threshold. No socket flush is performed when the half TX buffer threshold is passed.|
|TCP\_OPTION\_THRES\_FLUSH\_ON|Always enable the socket flush at half buffer threshold. Socket flush is always performed when the half TX buffer threshold is passed. This is useful for small TX buffers when the remote party implements the delayed ACK algorithm.|

