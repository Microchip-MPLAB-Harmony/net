# TCPIP\_IGMP\_ExcludeSource Function

**Parent topic:**[IGMP Module](GUID-DCB13BC6-B7A2-45CA-89E7-9474EAF05EFB.md)

## C

```
TCPIP_IGMP_RESULT TCPIP_IGMP_ExcludeSource(
    UDP_SOCKET socket, 
    TCPIP_NET_HANDLE hNet, 
    IPV4_ADDR mcastAddress, 
    IPV4_ADDR sourceAddress
);
```

## Description

This function performs the unsubscription of a UDP socket to multicast traffic from one source.

## Preconditions

The IGMP module must be initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|socket|the UDP multicast socket.|
|hNet|Interface handle. if *hNet == 0*, then the default interface will be used.|
|mcastAddress|the multicast address to unsubscribe from. It has to be within the IANA specified multicast address range.|
|sourceAddress|destination source for which the socket wants to unsubscribe from.|

## Returns

-   TCPIP\_IGMP\_OK if - if operation succeeded, otherwise an error code appears.


## Remarks

The function supports ASM functionality. It is a shortcut to TCPIP\_IGMP\_Subscribe\(EXCLUDE, 1 source\).

