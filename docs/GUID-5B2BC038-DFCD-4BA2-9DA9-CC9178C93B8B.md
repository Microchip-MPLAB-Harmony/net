# NET\_PRES\_SocketOptionsGet Function

**Parent topic:**[Networking Presentation Layer](GUID-75470E5B-2289-4F94-AE85-2BB7DF4C4F07.md)

## C

```
bool NET_PRES_SocketOptionsGet(NET_PRES_SKT_HANDLE_T handle, NET_PRES_SKT_OPTION_TYPE option, void* optParam); 
```

## Returns

-   true - Indicates success

-   false - Indicates failure


## Description

Various options can be obtained at the socket level. This function calls directly to the transport layer's OptionGet function, if it exists.

## Preconditions

A socket needs to have been opened by NET\_PRES\_SocketOpen.

## Parameters

|Parameters|Description|
|----------|-----------|
|handle|The socket to set options for.|
|option|The specific option to set, this is passed unaltered to the transport layer.|
|optParam|The option value, which is passed unaltered to the transport layer.|

