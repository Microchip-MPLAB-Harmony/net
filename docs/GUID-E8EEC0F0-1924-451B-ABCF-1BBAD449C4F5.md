# TCPIP\_ARP\_EVENT\_HANDLER Type

**Parent topic:**[ARP Module](GUID-3DCBF1EB-5623-4419-B244-DDD9B01704EE.md)

## C

```
typedef void (* TCPIP_ARP_EVENT_HANDLER)(TCPIP_NET_HANDLE hNet, const IPV4_ADDR* ipAdd, const TCPIP_MAC_ADDR* MACAddr, TCPIP_ARP_EVENT_TYPE evType, const void* param);
```

## Description

Type: TCPIP\_ARP\_EVENT\_HANDLER.

The format of a notification handler registered with the ARP module.

## Remarks

The parameter member significance is module dependent. It can be an IP address, pointer to some other structure, etc. The handler is called when an event of some sort occurs for a particular IP address entry. If pNetIf == 0, the notification is called for events on any interface.

