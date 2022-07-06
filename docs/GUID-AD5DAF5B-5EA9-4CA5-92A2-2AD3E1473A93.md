# TCPIP\_DNS\_GetIPAddressesNumber Function

**Parent topic:**[DNS Module](GUID-D15C8F84-C30C-451F-8AB7-F8E62AD494C2.md)

## C

```
int TCPIP_DNS_GetIPAddressesNumber(
    const char* hostName, 
    IP_ADDRESS_TYPE type
);
```

## Description

This function returns the total count of IPv4 and/or IPv6 addresses that exist for a resolved host name.

## Preconditions

TCPIP\_DNS\_Resolve\(\) has been called.

## Parameters

|Parameters|Description|
|----------|-----------|
|hostName|A pointer to the null terminated string specifying the host name.|
|type|IP\_ADDRESS\_TYPE\_IPV4/IP\_ADDRESS\_TYPE\_IPV6/IP\_ADDRESS\_TYPE\_ANY.|

## Returns

None.

## Remarks

None.

