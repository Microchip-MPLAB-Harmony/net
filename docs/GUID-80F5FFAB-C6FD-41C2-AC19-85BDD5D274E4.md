# TCPIP\_MAC\_PACKET\_FLAGS Enumeration

**Parent topic:**[MAC Driver Module](GUID-0C1AF471-66D4-472F-84AF-212E9E18B21D.md)

## C

```
typedef enum {
TCPIP_MAC_PKT_FLAG_STATIC = 0x0001,
TCPIP_MAC_PKT_FLAG_TX = 0x0002,
TCPIP_MAC_PKT_FLAG_SPLIT = 0x0004,
TCPIP_MAC_PKT_FLAG_QUEUED = 0x0008,
TCPIP_MAC_PKT_FLAG_UNICAST = 0x0010,
TCPIP_MAC_PKT_FLAG_BCAST = 0x0020,
TCPIP_MAC_PKT_FLAG_MCAST = 0x0040,
TCPIP_MAC_PKT_FLAG_CAST_MASK = 0x0070,
TCPIP_MAC_PKT_FLAG_CAST_DISABLED = 0x0000,
TCPIP_MAC_PKT_FLAG_USER = 0x0100
} TCPIP_MAC_PACKET_FLAGS;
```

## Description

MAC Packet Flags.

This enumeration contains the definitions of MAC packet flags: packet allocation flags and general purpose flags.

## Members

|Members|Description|
|-------|-----------|
|TCPIP\_MAC\_PKT\_FLAG\_STATIC = 0x0001|Packet can not be dynamically deallocated. Set when the packet is allocated.|
|TCPIP\_MAC\_PKT\_FLAG\_TX = 0x0002|If set, it is a TX packet/segment. Otherwise, it is a RX packet.|
|TCPIP\_MAC\_PKT\_FLAG\_SPLIT = 0x0004|Packet data spans multiple segments - ZC functionality. If not set then the packet has only one data segment. It is set by the MAC driver when a RX packet spans multiple data segments.|
|TCPIP\_MAC\_PKT\_FLAG\_QUEUED = 0x0008|Packet data is queued somewhere, cannot be freed. The flag is set by a module processing the packet to show that the packet is in use and queued for further processing \(normally the MAC driver does that\). Cleared by the packet destination when the packet processing was completed.|
|TCPIP\_MAC\_PKT\_FLAG\_UNICAST = 0x0010|RX flag, MAC updated. Specifies an unicast packet.|
|TCPIP\_MAC\_PKT\_FLAG\_BCAST = 0x0020|RX flag, MAC updated. Specifies a broadcast packet.|
|TCPIP\_MAC\_PKT\_FLAG\_MCAST = 0x0040|RX flag, MAC updated. Specifies an multicast packet.|
|TCPIP\_MAC\_PKT\_FLAG\_CAST\_MASK = 0x0070|Packet cast mask bits.|
|TCPIP\_MAC\_PKT\_FLAG\_CAST\_DISABLED = 0x0000|Packet cast mask. Specifies a packet where the MCAST/BCAST fields are not updated by the MAC RX process.|
|TCPIP\_MAC\_PKT\_FLAG\_USER = 0x0100|Available user flags.|

## Remarks

16 bits only packet flags are supported.

