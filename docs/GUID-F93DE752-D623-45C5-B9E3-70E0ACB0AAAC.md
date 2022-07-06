# TCPIP\_FTPC\_DATA\_EVENT\_CALLBACK\_TYPE Type

**Parent topic:**[FTP Client Module](GUID-CE11EBFA-49BD-4D91-86C5-FFD24810B03C.md)

## C

```
typedef bool (* TCPIP_FTPC_DATA_EVENT_CALLBACK_TYPE)(TCPIP_FTPC_CONN_HANDLE_TYPE ftpcHandle, TCPIP_FTPC_DATA_EVENT_TYPE ftpcEvent, TCPIP_FTPC_CMD cmd, char * databuff, uint16_t * datalen); 
```

## Returns

Return 'true' when callback function does the processing of Rx and Tx Data; no need to save data on file system. Return 'false' when callback function does not process Rx and Tx Data; should save data on file system.

## Description

Type: TCPIP\_FTPC\_DATA\_EVENT\_CALLBACK\_TYPE Data type describing a function that will be called by the FTPC module upon data socket events FTPC will parse and process the FTP client-server data messages anyway but it can optionally pass the information to the user.

## Remarks

When FTP Client receives data \(for GET, NLST, LIST commands\), "databuff" is the address of buffer where received data is<br />stored. The"datalen" is the number of bytes received. When FTP Client transmits data \(for PUT commands\), "databuff" is the address where data to be copied for transmission. The "datalen" is the number of bytes to be transmitted by FTP Client.

## Parameters

|Parameters|Description|
|----------|-----------|
|ftpcEvent|the data socket event|
|cmd|FTP Command|
|databuff|pointer to Data socket data buffer|
|datalen|pointer to receive data length|
|||

