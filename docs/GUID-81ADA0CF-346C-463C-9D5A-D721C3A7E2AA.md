# NET\_PRES\_TransOpen Type

**Parent topic:**[Networking Presentation Layer](GUID-75470E5B-2289-4F94-AE85-2BB7DF4C4F07.md)

## C

```
typedef NET_PRES_SKT_HANDLE_T (* NET_PRES_TransOpen)(NET_PRES_TRANS_ADDRESS_TYPE addType, NET_PRES_SKT_PORT_T port, NET_PRES_ADDRESS * address); 
```

## Returns

-   NET\_PRES\_INVALID\_SOCKET - No sockets of the specified type were available to be opened.

-   NET\_PRES\_SKT\_HANDLE\_T handle - Returned when NET\_PRES\_INVALID\_SOCKET is returned. Save this handle and use it when calling all other presentation socket APIs.


## Description

Transport Layer Open Function Pointer Prototype This function is called by the presentation layer when an application wants to open a socket.

## Preconditions

Transport layer must be initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|addType|The type of address being used. This is passed unaltered to the transport layer.|
|port|The port to listen or to send to. This is passed unaltered to the transport layer.|
|address|The address to use. This is passed unaltered to the transport layer.|

