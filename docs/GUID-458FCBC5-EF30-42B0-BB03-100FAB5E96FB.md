# DRV\_ETHMAC\_PIC32MACPacketTx Function

**Parent topic:**[Ethernet MAC Driver Library](GUID-A4DC3D07-DDAD-4748-A855-304CA3439336.md)

## C

```
TCPIP_MAC_RES DRV_ETHMAC_PIC32MACPacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket); 
```

## Returns

-   TCPIP\_MAC\_RES\_OK if success

-   a TCPIP\_MAC\_RES error value if failed


## Description

This is the MAC transmit function. Using this function a packet is submitted to the MAC driver for transmission.

## Remarks

-   The MAC driver supports internal queuing. A packet is rejected only if it's not properly formatted. Otherwise it will be scheduled for transmission and queued internally if needed.

-   Once the packet is scheduled for transmission the MAC driver will set the TCPIP\_MAC\_PKT\_FLAG\_QUEUED flag so that the stack is aware that this packet is under processing and cannot be modified.

-   Once the packet is transmitted, the TCPIP\_MAC\_PKT\_FLAG\_QUEUED will be cleared, the proper packet acknowledgment result \(ackRes\) will be set and the packet acknowledgment function \(ackFunc\) will be called.


## Preconditions

DRV\_ETHMAC\_PIC32MACInitialize\(\) should have been called. DRV\_ETHMAC\_PIC32MACOpen\(\) should have been called to obtain a valid handle.

