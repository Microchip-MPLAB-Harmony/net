# TCPIP\_DHCPS\_INTERFACE\_CONFIG Structure

**Parent topic:**[DHCP Server Module \(V2\)](GUID-EE292D18-2DE6-478E-AAE1-74C69A31459E.md)

## C

```
typedef struct
{
uint8_t     ifIndex;
uint8_t     configFlags;
uint16_t    leaseEntries;
uint32_t    leaseDuration;
uint32_t    minLeaseDuration;
uint32_t    maxLeaseDuration;
uint32_t    unreqOfferTmo; 
const char* serverIPAddress;
const char* startIPAddress; 
size_t      prefixLen;
const TCPIP_DHCPS_CLIENT_OPTION_CONFIG* pOptConfig;
uint16_t    nOptConfigs; 
}TCPIP_DHCPS_INTERFACE_CONFIG;
```

## Description

Structure: TCPIP\_DHCPS\_INTERFACE\_CONFIG

```
DHCP server configuration for a specific interface.

DHCP server configuration and network initialization data.
Configuration is part of tcpip_stack_init.c.
```

## Members

|Members|Description|
|-------|-----------|
|ifIndex|interface number to which this configuration refers to|
|configFlags|a TCPIP\_DHCPS\_CONFIG\_FLAGS value|
|leaseEntries|max number of leases for this interface|
||Must be <= TCPIP\_DHCPS\_MAX\_LEASES|
|leaseDuration|default lease duration to be granted to clients, seconds|
|minLeaseDuration|minimum lease duration that can be requested by a client, seconds|
||if 0, then a default of 60 seconds will be used|
||should be <= leaseDuration|
|maxLeaseDuration|maximum lease duration that can be requested by a client, seconds|
||if 0, then the leaseDuration will be used|
||should be \>= leaseDuration|
|unreqOfferTmo|timeout for unrequested offer sent to the client, seconds|
||Note: used if TCPIP\_DHCPS\_CONFIG\_FLAG\_KEEP\_UNREQ\_OFFERS is not set|
||if 0, a default of 5 seconds will be used|
|serverIPAddress|server IP address on this interface|
|startIPAddress|start lease IP address|
||the pool of addresses will be |
||where endIPAddress is given by the prefixLen|
|prefixLen|the number of the leading 1 bits in the network mask|
||number <= 32|
||the address is specified in the CIDR format: add/prefix\_len|
||the maximum size of the pool is dictated by the prefixLen|
||For example: 192.168.0.0/24 will have 254 leases available|
||\(192.168.0.0 and 192.168.0.255 are excluded\)|
||and endIPAddress == 192.168.0.254|
||Note: the number of possible leases == min\(leaseEntries, endIPAddress - startIPAddress\).|
||The number of leases derived from startIPAddress/prefixLen should be \> leaseEntries.|
|pOptConfig|pointer to array of client options for the DHCP server|
|nOptConfigs|number of option configurations in the pOptConfig array|

