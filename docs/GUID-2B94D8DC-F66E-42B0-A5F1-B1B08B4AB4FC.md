# TCPIP\_DHCPV6\_CONFIG\_FLAGS Enumeration

**Parent topic:**[DHCPv6 Module](GUID-0B7ADACD-E078-4FE5-BC6A-B7CABFE390D3.md)

## C

```
typedef enum {
TCPIP_DHCPV6_FLAG_NONE = 0,
TCPIP_DHCPV6_FLAG_START_ENABLE = 0x01,
TCPIP_DHCPV6_FLAG_DAD_DISABLE = 0x02,
TCPIP_DHCPV6_FLAG_IA_IGNORE_RENEW_LTIME = 0x04,
TCPIP_DHCPV6_FLAG_IA_IGNORE_REBIND_LTIME = 0x08,
TCPIP_DHCPV6_FLAG_IA_NOTIFY_SUB_STATE = 0x80
} TCPIP_DHCPV6_CONFIG_FLAGS;
```

## Description

DHCPV6 start up flags.

## Members

|Members|Description|
|-------|-----------|
|TCPIP\_DHCPV6\_FLAG\_START\_ENABLE = 0x01|Enable the DHCPv6 at stack start up.|
|TCPIP\_DHCPV6\_FLAG\_DAD\_DISABLE = 0x02|Disable the DAD processing for DHCP generated addresses Use only for testing or in special cases Default should be disabled.|
|TCPIP\_DHCPV6\_FLAG\_IA\_IGNORE\_RENEW\_LTIME = 0x04|If enabled, the IA \(and its associated address\) renew process will be valid as dictated by t1/defaultIataT1 and its address preferred lifetime will be ignored If disabled, the IA and its address will attempt renew when the minimum of address preferred lifetime and t1/defaultIataT1 expired Default should be disabled.|
|TCPIP\_DHCPV6\_FLAG\_IA\_IGNORE\_REBIND\_LTIME = 0x08|If enabled, the IA \(and its associated address\) rebind process will be valid as dictated by t2/defaultIataT2 and its address valid lifetime will be ignored If disabled, the IA and its address will attempt rebind when the minimum of address valid lifetime and t2/defaultIataT2 expired Default should be disabled.|
|TCPIP\_DHCPV6\_FLAG\_IA\_NOTIFY\_SUB\_STATE = 0x80|If enabled, the IA notifications will be generated for IA substate changes too \(finer grain\) if disabled, notifications will be generated for IA state changes only Default should be disabled.|

