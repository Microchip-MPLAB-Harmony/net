# TCPIP\_HTTP\_NET\_UserHandlerRegister Function

**Parent topic:**[HTTP Net Module](GUID-4EFEB885-ECF8-44B5-8F23-1D05952E1845.md)

## C

```
TCPIP_HTTP_NET_USER_HANDLE TCPIP_HTTP_NET_UserHandlerRegister(
    const TCPIP_HTTP_NET_USER_CALLBACK* userCallback
);
```

## Description

The function registers a user callback data structure with the HTTP server. The HTTP server will call the user supplied callbacks at run-time when processing the web pages.

## Preconditions

The HTTP server module properly initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|userCallback|User callback to be registered with the HTTP server.|

## Returns

-   A valid handle if the call succeeds.

-   A null handle if the call failed \(out of memory, for example\).


## Remarks

Currently only one user callback structure could be registered. The call will fail if a user callback structure is already registered.

