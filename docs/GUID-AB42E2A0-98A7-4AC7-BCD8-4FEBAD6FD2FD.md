# TCPIP\_DHCPS\_LEASE\_ENTRY Structure

**Parent topic:**[DHCP Server Module](GUID-27C514CD-DE28-4215-BB75-6C8EA971E12E.md)

## C

```
typedef struct {
TCPIP_MAC_ADDR hwAdd;
IPV4_ADDR ipAddress;
uint32_t leaseTime;
} TCPIP_DHCPS_LEASE_ENTRY;
```

## Description

Structure: TCPIP\_DHCPS\_LEASE\_ENTRY.

DHCP server lease data.

## Members

|Members|Description|
|-------|-----------|
|hwAdd|Client MAC address.|
|ipAddress|Leased IP address.|
|leaseTime|Lease period.|

