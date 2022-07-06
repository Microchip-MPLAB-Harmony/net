# TCPIP\_DHCPS\_LEASE\_INFO Structure

**Parent topic:**[DHCP Server Module \(V2\)](GUID-EE292D18-2DE6-478E-AAE1-74C69A31459E.md)

## C

```
typedef struct
{
IPV4_ADDR ipAddress; 
uint32_t leaseTime; 
uint8_t leaseState; 
uint8_t clientIdLen; 
uint8_t clientId[]; 
}TCPIP_DHCPS_LEASE_INFO;
```

## Description

Structure: TCPIP\_DHCPS\_LEASE\_INFO

```
DHCP Server module lease data.  

DHCP Server lease info data  
```

**Remarks**:

```
The clientId field should hold at least TCPIP_DHCPS_CLIENT_ID_MAX_SIZE characters otherwise truncation will occur! 
```

## Members

|Members|Description|
|-------|-----------|
|ipAddress|leased IP address|
|leaseTime|remaining lease time|
|leaseState|A TCPIP\_DHCPS\_LEASE\_STATE value, the current state|
|clientIdLen|on input this stores the size of the clientId|
||on return, this field stores the actual client ID size|
|clientId|the client ID used for this lease|
||should hold \>= than TCPIP\_DHCPS\_CLIENT\_ID\_MAX\_SIZE|
||otherwise it will be truncated|
||normally a hardware MAC address|
||but clients can use other IDs|

