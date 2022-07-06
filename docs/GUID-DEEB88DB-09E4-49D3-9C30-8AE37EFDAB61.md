# TCPIP\_DHCPV6\_CLIENT\_STATE Enumeration

**Parent topic:**[DHCPv6 Module](GUID-0B7ADACD-E078-4FE5-BC6A-B7CABFE390D3.md)

## C

```
typedef enum {
TCPIP_DHCPV6_CLIENT_STATE_INIT = 0,
TCPIP_DHCPV6_CLIENT_STATE_IDLE,
TCPIP_DHCPV6_CLIENT_STATE_RUN,
TCPIP_DHCPV6_CLIENT_STATE_WAIT_LINK,
TCPIP_DHCPV6_CLIENT_STATE_NUMBER
} TCPIP_DHCPV6_CLIENT_STATE;
```

## Description

Enumeration: TCPIP\_DHCPV6\_CLIENT\_STATE.

This enumeration lists the current status of the DHCPv6 module. Used in getting info about the DHCPv6 state machine.

## Members

|Members|Description|
|-------|-----------|
|TCPIP\_DHCPV6\_CLIENT\_STATE\_INIT = 0|Initialization state/unknown.|
|TCPIP\_DHCPV6\_CLIENT\_STATE\_IDLE|Idle/inactive state.|
|TCPIP\_DHCPV6\_CLIENT\_STATE\_RUN|Up and running in one of the run states.|
|TCPIP\_DHCPV6\_CLIENT\_STATE\_WAIT\_LINK|Up and running, waiting for a connection.|
|TCPIP\_DHCPV6\_CLIENT\_STATE\_NUMBER|Number of states.|

