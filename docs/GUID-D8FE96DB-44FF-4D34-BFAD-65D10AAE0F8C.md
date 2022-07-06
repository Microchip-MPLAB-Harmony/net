# TCPIP\_DHCP\_HandlerDeRegister Function

**Parent topic:**[DHCP Client Module](GUID-9356D53B-4F42-4E1E-B051-90F7C4D448E6.md)

## C

```
bool TCPIP_DHCP_HandlerDeRegister(
    TCPIP_DHCP_HANDLE hDhcp
);
```

## Description

This function deregisters the DHCP event handler.

## Preconditions

The DHCP module must be initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|hDhcp|A handle returned by a previous call to TCPIP\_DHCP\_HandlerRegister\(\).|

## Returns

-   True - If the call succeeds.

-   False - If no such handler is registered.


## Remarks

None.

