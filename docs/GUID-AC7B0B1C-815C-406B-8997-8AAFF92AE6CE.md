# TCPIP\_DHCP\_Request Function

**Parent topic:**[DHCP Client Module](GUID-9356D53B-4F42-4E1E-B051-90F7C4D448E6.md)

## C

```
bool TCPIP_DHCP_Request(
    TCPIP_NET_HANDLE hNet, 
    IPV4_ADDR reqAddress
);
```

## Description

If the DHCP client is not enabled on that interface, this call will first try to enable it. If this succeeds or the DHCP client was already enabled, the following steps are taken: The DHCP client probes the DHCP server and requests the supplied IPv4 address as a valid lease for the specified interface. If the server acknowledges the request, then this is the new IPv4 address of the interface. If the DHCP server rejects the request, then the whole DHCP process is resumed starting with the DHCP Discovery phase.

## Preconditions

The DHCP module must be initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|hNet|Interface to renew the DHCP lease on.|

## Returns

-   True - If successful.

-   False - If the supplied IP address is invalid or the DHCP client is in the middle of a transaction.


## Remarks

The requested IPv4 address should be a previous lease that was granted to the host. This call should be used when the host is restarting.

