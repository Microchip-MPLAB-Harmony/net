# NET\_PRES\_SocketWasReset Function

**Parent topic:**[Networking Presentation Layer](GUID-75470E5B-2289-4F94-AE85-2BB7DF4C4F07.md)

## C

```
bool NET_PRES_SocketWasReset(NET_PRES_SKT_HANDLE_T handle); 
```

## Description

This function is a self-clearing semaphore indicating whether or not a socket has been reset since the previous call. This function calls directly to the transport layer's WasReset function, if it exists.

## Preconditions

A socket needs to have been opened by NET\_PRES\_SocketOpen.

## Parameters

|Parameters|Description|
|----------|-----------|
|handle|The presentation layer socket handle.|

