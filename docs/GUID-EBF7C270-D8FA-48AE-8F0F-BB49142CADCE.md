# TCPIP\_DHCPS\_CLIENT\_OPTION\_TYPE Enumeration

**Parent topic:**[DHCP Server Module \(V2\)](GUID-EE292D18-2DE6-478E-AAE1-74C69A31459E.md)

## C

```
typedef enum
{
    TCPIP_DHCPS_CLIENT_OPTION_ROUTER           = 0,
    TCPIP_DHCPS_CLIENT_OPTION_DNS,
    TCPIP_DHCPS_CLIENT_OPTION_TIME_SERVER,
    TCPIP_DHCPS_CLIENT_OPTION_NAME_SERVER,
    TCPIP_DHCPS_CLIENT_OPTION_T1_RENEWAL,
    TCPIP_DHCPS_CLIENT_OPTION_T2_REBINDING,
    TCPIP_DHCPS_CLIENT_OPTION_NTP_SERVER,
}TCPIP_DHCPS_CLIENT_OPTION_TYPE;
```

## Description

Enumeration: TCPIP\_DHCPS\_CLIENT\_OPTION\_TYPE

```
DHCP server options to be provided to the clients for a specific interface.

Describes the list of options to be provided to the clients by the DHCP server running an interface.
```

**Remarks**:

```
More options will be eventually added as needed

Maximum 16 options are supported for now.

The TCPIP_DHCPS_CLIENT_OPTION_T1_RENEWAL and TCPIP_DHCPS_CLIENT_OPTION_T2_REBINDING options
are controlled by the global build symbol TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS (configuration.h).
They are enabled by default and have the default values of 1/2 and 7/8 respectively.
When they are not suppressed, different values than default are configurable using this option.
```

## Members

|Members|Description|
|-------|-----------|
|TCPIP\_DHCPS\_CLIENT\_OPTION\_ROUTER|Provide router/gateway option|
|TCPIP\_DHCPS\_CLIENT\_OPTION\_DNS|Provide DNS option|
|TCPIP\_DHCPS\_CLIENT\_OPTION\_TIME\_SERVER|Provide time server option|
|TCPIP\_DHCPS\_CLIENT\_OPTION\_NAME\_SERVER|Provide name server option|
|TCPIP\_DHCPS\_CLIENT\_OPTION\_T1\_RENEWAL|Provide T1 renewal option|
|TCPIP\_DHCPS\_CLIENT\_OPTION\_T2\_REBINDING|Provide T2 rebinding option|
|TCPIP\_DHCPS\_CLIENT\_OPTION\_NTP\_SERVER|Provide NTP server option|

