# TCPIP\_DHCPS\_Enable Function

**Parent topic:**[DHCP Server Module](GUID-27C514CD-DE28-4215-BB75-6C8EA971E12E.md)

## C

```
bool TCPIP_DHCPS_Enable(
    TCPIP_NET_HANDLE hNet
);
```

## Description

This function enables the DHCP Server for the specified interface, if it is disabled. If it is already enabled, nothing is done.

## Preconditions

The DHCP server module must be initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|hNet|Interface on which to enable the DHCP Server.|

## Returns

-   True - If successful.

-   False - If unsuccessful.


## Remarks

None.

