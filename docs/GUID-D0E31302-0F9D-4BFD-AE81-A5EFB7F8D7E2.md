# TCPIP\_IPV4\_PacketFormatTx Function

**Parent topic:**[IPv4 Module](GUID-EA29E72F-4194-41F9-9F19-D8BBA00D62F2.md)

## C

```
void TCPIP_IPV4_PacketFormatTx(
    IPV4_PACKET* pPkt, 
    uint8_t protocol, 
    uint16_t ipLoadLen, 
    TCPIP_IPV4_PACKET_PARAMS* pParams
);
```

## Description

The necessary fields are set into the IPv4 packet.

## Preconditions

Properly allocated *pPkt*. The source and destination addresses should be updated in the packet. The network interface should be updated in the IPV4\_PACKET.

## Parameters

|Parameters|Description|
|----------|-----------|
|pPkt|The packet to be formatted.|
|protocol|The protocol associated with the packet.|
|ipLoadLen|The IPv4 packet payload length.|
|pParams|Pointer to a structure containing parameters for this packet. If NULL, then default values are used.|

## Returns

None.

## Remarks

The segments should be properly updated with the right number of bytes \( *segLen*\). The IP payload length \(*ipLoadLen*\) is added only to the 1st segment of the packet. Other segments \(for packets having multiple packets\) are not touched.

