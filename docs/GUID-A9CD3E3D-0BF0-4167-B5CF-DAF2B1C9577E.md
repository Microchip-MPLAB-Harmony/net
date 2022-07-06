# TCPIP\_ARP\_RESULT Enumeration

**Parent topic:**[ARP Module](GUID-3DCBF1EB-5623-4419-B244-DDD9B01704EE.md)

## C

```
typedef enum {
ARP_RES_OK = 0,
ARP_RES_ENTRY_NEW,
ARP_RES_ENTRY_SOLVED,
ARP_RES_ENTRY_QUEUED,
ARP_RES_ENTRY_EXIST,
ARP_RES_PERM_QUOTA_EXCEED,
ARP_RES_PROBE_OK,
ARP_RES_NO_ENTRY = -1,
ARP_RES_CACHE_FULL = -2,
ARP_RES_TX_FAILED = -3,
ARP_RES_BAD_INDEX = -4,
ARP_RES_BAD_ADDRESS = -5,
ARP_RES_NO_INTERFACE = -6,
ARP_RES_BAD_TYPE = -7,
ARP_RES_CONFIGURE_ERR = -8,
ARP_RES_PROBE_FAILED = -9
} TCPIP_ARP_RESULT;
```

## Description

Enumeration: TCPIP\_ARP\_RESULT.

Various definitions for success and failure codes.

## Members

|Members|Description|
|-------|-----------|
|ARP\_RES\_OK = 0|Operation succeeded.|
|ARP\_RES\_ENTRY\_NEW|Operation succeeded and a new entry was added.|
|ARP\_RES\_ENTRY\_SOLVED|The required entry is already solved.|
|ARP\_RES\_ENTRY\_QUEUED|The required entry was already queued.|
|ARP\_RES\_ENTRY\_EXIST|The required entry was already cached.|
|ARP\_RES\_PERM\_QUOTA\_EXCEED|Info: the quota of permanent entries was exceeded.|
|ARP\_RES\_PROBE\_OK|Requested probe sent.|
|ARP\_RES\_NO\_ENTRY = -1|No such entry exists.|
|ARP\_RES\_CACHE\_FULL = -2|The cache is full and no entry could be removed to make room.|
|ARP\_RES\_TX\_FAILED = -3|Failed to transmit an ARP message.|
|ARP\_RES\_BAD\_INDEX = -4|Bad query index.|
|ARP\_RES\_BAD\_ADDRESS = -5|Bad IP address specified.|
|ARP\_RES\_NO\_INTERFACE = -6|No such interface exists.|
|ARP\_RES\_BAD\_TYPE = -7|No such type is valid/exists.|
|ARP\_RES\_CONFIGURE\_ERR = -8|Interface is configuring now, no ARP probes.|
|ARP\_RES\_PROBE\_FAILED = -9|Requested probed failed.|

## Remarks

None.

