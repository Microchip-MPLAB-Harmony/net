# TCPIP\_DHCPS\_MODULE\_CONFIG Structure

**Parent topic:**[DHCP Server Module \(V2\)](GUID-EE292D18-2DE6-478E-AAE1-74C69A31459E.md)

## C

```
typedef struct
{
    const TCPIP_DHCPS_INTERFACE_CONFIG* pIfConfig;
    uint16_t    nConfigs;
    uint8_t     nProbes; 
    uint8_t     conflictAttempts;
} TCPIP_DHCPS_MODULE_CONFIG;
```

## Description

```
DHCP Server module runtime and initialization configuration data.

DHCP server configuration and initialization data.
Configuration is part of tcpip_stack_init.c.
```

**Remarks**:

```
The number of interfaces that DHCPs supports cannot exceed the number of interfaces in the stack!
```

## Members

|Members|Description|
|-------|-----------|
|pIfConfig|array of DHCP server configurations per interface|
|nConfigs|number of DHCP server configurations in the pIfConfig array, one per interface|
||Cannot exceed the TCPIP\_DHCPS\_INTERFACE\_COUNT, which is the number of interfaces the server can support|
|nProbes|number of ICMP echo probes to send when doing address conflict detection - default 1|
|conflictAttempts|number of attempts to find a new IP address when ICMP detected an address conflict - default 1|

