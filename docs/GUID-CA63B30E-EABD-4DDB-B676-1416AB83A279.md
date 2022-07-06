# TCPIP\_ZCLL\_IsEnabled Function

**Parent topic:**[Zeroconf Module](GUID-1076C2EA-7DC5-419E-A3E7-BBA92CFF5CFB.md)

## C

```
bool TCPIP_ZCLL_IsEnabled(
    TCPIP_NET_HANDLE hNet
);
```

## Description

This API is used by end-user application check whether or not an interface is enabled for zero configuration.

## Preconditions

The TCP/IP stack must be initialized before calling this function.

## Parameters

|Parameters|Description|
|----------|-----------|
|hNet|Handle of the network to be examined.|

## Returns

-   True - Interface is enable for zero configuration.

-   False - Interface is not enabled for zero configuration.


## Remarks

None.

