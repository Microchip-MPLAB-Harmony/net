# TCPIP\_DNSS\_CacheEntryRemove Function

**Parent topic:**[DNS Server Module](GUID-987D1913-E20A-467D-9E57-DEC60B2EBE5D.md)

## C

```
TCPIP_DNSS_RESULT TCPIP_DNSS_CacheEntryRemove(
    const char* name, 
    IP_ADDRESS_TYPE type, 
    IP_MULTI_ADDRESS* pAdd
);
```

## Description

This function is used for command prompt dnsDelSrv and to delete a entry IPv4 or IPv6 from hostName. Every time this is used to delete either one IPv4 or IPv6 address with respect to the DNS hostname, there will be an expire time for a Host entry.

## Preconditions

The DNS server must be initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|name|Hostname.|
|type|IP\_ADDRESS\_TYPE\_IPV4 or IP\_ADDRESS\_TYPE\_IPV6.|
|pAdd|V4 or V6 address.|

## Returns

-   TCPIP\_DNSS\_RES\_OK - If entry remove is successful.

-   TCPIP\_DNSS\_RES\_NO\_ENTRY - No such entry is present.

-   TCPIP\_DNSS\_RES\_MEMORY\_FAIL - No Memory is present for IPv4 or IPv6 address type.


## Remarks

None.

