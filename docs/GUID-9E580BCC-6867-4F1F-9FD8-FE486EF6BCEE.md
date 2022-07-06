# TCPIP\_DNSS\_EntryAdd Function

**Parent topic:**[DNS Server Module](GUID-987D1913-E20A-467D-9E57-DEC60B2EBE5D.md)

## C

```
TCPIP_DNSS_RESULT TCPIP_DNSS_EntryAdd(
    const char* name, 
    IP_ADDRESS_TYPE type, 
    IP_MULTI_ADDRESS* pAdd, 
    uint32_t entryTimeout
);
```

## Description

This function is used to add a IPv4 or IPv6 entry. Every time this is used to add either one IPv4 or IPv6 address with respect to the DNS hostname, there will be an expire time for a Host entry. "entryTimeout" parameter is used as the timeout value for this specific entry.

## Preconditions

The DNS server must be initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|name|Hostname.|
|type|IP\_ADDRESS\_TYPE\_IPV4 or IP\_ADDRESS\_TYPE\_IPV6.|
|pAdd|IPv4 or IPv6 address.|
|entryTimeout|Time-out value for the entry.|

## Returns

-   TCPIP\_DNSS\_RES\_OK - If entry remove is successful.

-   TCPIP\_DNSS\_RES\_NO\_ENTRY - If Hostname is NULL value and Invalid IP type.

-   TCPIP\_DNSS\_RES\_MEMORY\_FAIL - No Memory is present for IPv4 or IPv6 address type.

-   TCPIP\_DNSS\_RES\_CACHE\_FULL - If there is no space for the new entry.


## Remarks

When the DNS server entryTimeout value is 0,it specifies that the entry is a permanent entry. user can remove this entry by using TCPIP\_DNSS\_CacheEntryRemove\(\). TTL time for this entry will be the value of TCPIP\_DNSS\_PERMANENT\_ENTRY\_TTL\_TIME.

