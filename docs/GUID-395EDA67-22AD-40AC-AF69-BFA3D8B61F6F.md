# TCPIP\_MAC\_TX\_STATISTICS Structure

**Parent topic:**[MAC Driver Module](GUID-0C1AF471-66D4-472F-84AF-212E9E18B21D.md)

## C

```
typedef struct {
int nTxOkPackets;
int nTxPendBuffers;
int nTxErrorPackets;
int nTxQueueFull;
} TCPIP_MAC_TX_STATISTICS;
```

## Description

MAC TX Statistics Data.

This structure defines the TX statistics data for the MAC driver.

## Parameters

|Parameters|Description|
|----------|-----------|
|nTxOkPackets|Number of OK transmitted packets.|
|nTxPendBuffers|number of unacknowledged pending TX buffers in the driver queues. This is equal with pending TX packets when each packet is contained within a TX buffer.|
|nTxErrorPackets|Number of packets that could not be transmitted.|
|nTxQueueFull|Number of times the TX queue was full this may signal that the number of TX descriptors is too small.|

## Remarks

This statistics are recorded by the driver, not by the hardware.

