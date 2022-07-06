# TCPIP\_DNSS\_EntryGet Function

**Parent topic:**[DNS Server Module](GUID-987D1913-E20A-467D-9E57-DEC60B2EBE5D.md)

## C

```
TCPIP_DNSS_RESULT TCPIP_DNSS_EntryGet(
    uint8_t * hostName, 
    IP_ADDRESS_TYPE type, 
    int index, 
    IP_MULTI_ADDRESS* pGetAdd, 
    uint32_t * ttltime
);
```

## Description

This function is used to get the DNS server IPv4 or IPv6 address from resolver pool entry as per hostname and IP type and from the index value. This IP type can be a IPv4 and IPv6 type. This is used for DNS record type.

## Preconditions

DNSServerInit\(\) should be called.

## Parameters

|Parameters|Description|
|----------|-----------|
|hostName|DNS host name.|
|type|DNS IP type \( it will be ipv4 or ipv6 type and it is mapped to DNS record type\) this can be used for DNS record type.|
|index|Get the next entry after this index value.|
|pGetAdd|Get the zero th IPv4 or IPv6 address.|
|ttltime|Timeout value.|

## Returns

TCPIP\_DNSS\_RESULT

## Remarks

None.

