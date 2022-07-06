# TCPIP\_DNSS\_RESULT\_OBSOLETE Enumeration

**Parent topic:**[Dynamic DNS Module](GUID-2F39A35D-A741-49F8-BAA9-3B3C9B665486.md)

## C

```
typedef enum {
DNSS_RES_OK = 0,
DNSS_RES_NO_SERVICE = -1,
DNSS_RES_CACHE_FULL = -2,
DNSS_RES_NO_ENTRY = -3,
DNSS_RES_NO_IPADDRESS = -4,
DNSS_RES_MEMORY_FAIL = -5,
DNSS_RES_DUPLICATE_ENTRY = -6
} TCPIP_DNSS_RESULT_OBSOLETE;
```

## Description

Enumeration: TCPIP\_DNSS\_RESULT\_OBSOLETE.

DNS Server operations results.

## Members

|Members|Description|
|-------|-----------|
|DNSS\_RES\_OK = 0|Operation succeeded.|
|DNSS\_RES\_NO\_SERVICE = -1|DNS service not implemented or uninitialized.|
|DNSS\_RES\_CACHE\_FULL = -2|The cache is full and no entry could be added.|
|DNSS\_RES\_NO\_ENTRY = -3|DNSS no such name.|
|DNSS\_RES\_NO\_IPADDRESS = -4|No such IP address.|
|DNSS\_RES\_MEMORY\_FAIL = -5|Out of memory failure.|
|DNSS\_RES\_DUPLICATE\_ENTRY = -6|Duplicate entry was found.|

## Remarks

This type is obsolete and it will be removed. Please use the TCPIP\_DNSS\_RESULT\(\).

