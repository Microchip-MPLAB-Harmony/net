# TCPIP\_DHCP\_Disable Function

**Parent topic:**[DHCP Client Module](GUID-9356D53B-4F42-4E1E-B051-90F7C4D448E6.md)

## C

```
bool TCPIP_DHCP_Disable(
    TCPIP_NET_HANDLE hNet
);
```

## Description

This function disables the DHCP client for the specified interface. If it is already disabled, no action is taken.

## Preconditions

The DHCP module must be initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|pNetIf|Interface to disable the DHCP client on.|

## Returns

-   True - If successful.

-   False - If unsuccessful.


## Remarks

When the DHCP client is disabled and the interface continues using its old configuration, it is possible that the lease may expire and the DHCP server provide the IP address to another client. The application should not keep the old lease unless it is sure that there is no danger of conflict.

