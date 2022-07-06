# NET\_PRES\_SocketOpen Function

**Parent topic:**[Networking Presentation Layer](GUID-75470E5B-2289-4F94-AE85-2BB7DF4C4F07.md)

## C

```
NET_PRES_SKT_HANDLE_T NET_PRES_SocketOpen(NET_PRES_INDEX index, NET_PRES_SKT_T socketType, NET_PRES_SKT_ADDR_T addrType, NET_PRES_SKT_PORT_T port, NET_PRES_ADDRESS * addr, NET_PRES_SKT_ERROR_T* error); 
```

## Returns

-   NET\_PRES\_INVALID\_SOCKET - No sockets of the specified type were available to be opened

-   NET\_PRES\_SKT\_HANDLE\_T handle - Returned when NET\_PRES\_INVALID\_SOCKET is returned. Save this handle and use it when calling all other presentation socket APIs.


## Description

Provides a unified method for opening all presentation sockets types. Sockets are created at the presentation layer module initialization, and can be claimed with this function and freed using NET\_PRES\_SocketClose. The presentation layer will call the corresponding open function in the transport layer, and if encryption is specified the presentation layer will also handle encryption negotiation.

## Preconditions

The MPLAB Harmony Networking Presentation Layer is initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|index|Index of the presentation layer.|
|socketType|The type of socket to open.|
|addType|The type of address being used. This is passed unaltered to the transport layer.|
|port|The port to listen or to send to. This is passed unaltered to the transport layer.|
|addr|Address to use. This is passed unaltered to the transport layer.|
|error|The extended error code of the function.|

