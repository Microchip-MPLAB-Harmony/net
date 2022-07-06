# TCPIP\_UDP\_ArrayGet Function

**Parent topic:**[UDP Module](GUID-D2D8E9C8-0778-41E2-8F0B-194954B92250.md)

## C

```
uint16_t TCPIP_UDP_ArrayGet(UDP_SOCKET hUDP, uint8_t * cData, uint16_t wDataLen); 
```

## Returns

The number of bytes successfully read from the UDP buffer. If this value is less than wDataLen, then the buffer was emptied and no more data is available.

## Description

This function reads an array of bytes from the UDP socket, while adjusting the current read pointer and decrementing the remaining bytes available. TCPIP\_UDP\_GetIsReady should be used before calling this function to get the number of the available bytes in the socket.

## Remarks

For discarding a number of bytes in the RX buffer the TCPIP\_UDP\_RxOffsetSet\(\) can also be used. The UDP socket queues incoming RX packets in an internal queue. This call will try to retrieve the bytes from the current<br />processing packet but it won't advance the processed packet. TCPIP\_UDP\_GetIsReady should be called to advance the processed RX packet. TCPIP\_UDP\_Discard should be called when done processing the current RX packet.

## Preconditions

UDP socket should have been opened with TCPIP\_UDP\_ServerOpen/TCPIP\_UDP\_ClientOpen. hUDP - valid socket

## Parameters

|Parameters|Description|
|----------|-----------|
|hUDP|UDP socket handle|
|cData|The buffer to receive the bytes being read. If NULL, the bytes are simply discarded|
|wDataLen|Number of bytes to be read from the socket.|
|||

