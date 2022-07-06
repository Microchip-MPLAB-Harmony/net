# DRV\_ETHMAC\_PIC32MACPacketRx Function

**Parent topic:**[Ethernet MAC Driver Library](GUID-A4DC3D07-DDAD-4748-A855-304CA3439336.md)

## C

```
TCPIP_MAC_PACKET* DRV_ETHMAC_PIC32MACPacketRx(DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, const TCPIP_MAC_PACKET_RX_STAT** ppPktStat); 
```

## Returns

-   a valid pointer to an available RX packet

-   0 if no packet pending/available


## Description

This function will return a packet if such a pending packet exists.<br />Additional information about the packet is available by providing the pRes and ppPktStat fields.

## Remarks

-   Once a pending packet is available in the MAC driver internal RX queues this function will dequeue the packet and hand it over to the MAC driver's client - i.e., the stack - for further processing.

-   The flags for a RX packet are updated by the MAC driver:

-   TCPIP\_MAC\_PKT\_FLAG\_RX will be set

-   TCPIP\_MAC\_PKT\_FLAG\_UNICAST is set if that packet is a unicast packet

-   TCPIP\_MAC\_PKT\_FLAG\_BCAST is set if that packet is a broadcast packet

-   TCPIP\_MAC\_PKT\_FLAG\_MCAST is set if that packet is a multicast packet

-   TCPIP\_MAC\_PKT\_FLAG\_QUEUED is set

-   TCPIP\_MAC\_PKT\_FLAG\_SPLIT is set if the packet has multiple data segments

-   The MAC driver dequeues and return to the caller just one single packet. That is the packets are not chained.

-   The packet buffers are allocated by the Ethernet MAC driver itself, Once the higher level layers in the stack are done with processing the RX packet, they have to call the corresponding packet acknowledgment function that tells the MAC driver that it can resume control of that packet.

-   Once the stack modules are done processing the RX packets and the acknowledge function is called the MAC driver will reuse the RX packets.

-   The MAC driver may use the DRV\_ETHMAC\_PIC32MACProcess\(\) for obtaining new RX packets if needed.


## Preconditions

DRV\_ETHMAC\_PIC32MACInitialize\(\) should have been called. DRV\_ETHMAC\_PIC32MACOpen\(\) should have been called to obtain a valid handle.

