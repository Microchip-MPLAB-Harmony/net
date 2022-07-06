# TCPIP\_DNSS\_IsEnabled Function

**Parent topic:**[DNS Server Module](GUID-987D1913-E20A-467D-9E57-DEC60B2EBE5D.md)

## C

```
bool TCPIP_DNSS_IsEnabled(
    TCPIP_NET_HANDLE hNet
);
```

## Description

This function returns the current state of the DNS Server on the specified interface.

## Preconditions

The DNS server must be initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|hNet|Interface to query.|

## Returns

-   True - If the DNS Server is enabled on the specified interface.

-   False - If the DNS Server is not enabled on the specified interface.


## Remarks

None.

