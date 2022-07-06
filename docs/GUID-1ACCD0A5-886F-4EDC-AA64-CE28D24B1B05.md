# TCPIP\_IPV4\_MacPacketTransmit Function

**Parent topic:**[IPv4 Module](GUID-EA29E72F-4194-41F9-9F19-D8BBA00D62F2.md)

## C

```
bool TCPIP_IPV4_MacPacketTransmit(
    TCPIP_MAC_PACKET* pPkt, 
    TCPIP_NET_HANDLE hNet, 
    IPV4_ADDR* pDestAddress
);
```

## Description

The function is a helper to transmit a fully constructed MAC packet over an interface. The MAC packet is supposed to have all the fields properly constructed \(including the TCPIP\_MAC\_ETHERNET\_HEADER fields\). The MAC packet does not need ARP resolution since the *DestMACAddr* is already known. The packet acknowledge function should be properly set \(it will be called by the MAC driver once the packet is transmitted\).

## Preconditions

*pPkt* should have been properly formatted and constructed into a valid MAC packet that can go out over an interface *pDestAddress* - valid pointer

## Parameters

|Parameters|Description|
|----------|-----------|
|pPkt|The packet to be transmitted.|
|hNet|Interface over which the packet is to be transmitted.|
|pDestAddress|The IPv4 packet destination address. It is used only for routing purposes, to decide if the packet is routed internally or sent to the network interface|

## Returns

-   True - Iif the packet was handed to the MAC or queued internally.

-   False - The packet cannot be transmitted \(wrong interface, etc.\).


## Remarks

Only single packets can be transmitted. Chained packets are not supported for now.

The packet could be composed of multiple fragments. If the transmission fails, some fragments could have been transmitted while others failed. The flag TCPIP\_MAC\_PKT\_FLAG\_QUEUED could be used to determine which fragments didn't make it through.

