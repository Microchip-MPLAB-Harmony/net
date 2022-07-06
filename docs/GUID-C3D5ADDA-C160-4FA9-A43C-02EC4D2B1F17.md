# TCPIP\_MAC\_PKT\_AckF Type

**Parent topic:**[MAC Driver Module](GUID-0C1AF471-66D4-472F-84AF-212E9E18B21D.md)

## C

```
typedef void (* TCPIP_MAC_PKT_AckF)(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, int moduleId);
```

## Description

Packet Acknowledgment Function: typedef void \(\*TCPIP\_MAC\_PKT\_AckF\)\(TCPIP\_MAC\_PACKET\* pPkt, TCPIP\_MAC\_PKT\_ACK\_RES ackRes, int moduleId\);

This function is used by the MAC to acknowledge a TCPIP\_MAC\_PACKET packet when the packet processing is completed.

## Parameters

|Parameters|Description|
|----------|-----------|
|pPkt|Pointer to a valid TCPIP\_MAC\_PACKET packet.|
|ackRes|The result of the packet processing.|
|moduleId|The ID of the module calling the acknowledge function.|

## Returns

None.

## Remarks

A TCPIP\_MAC\_PACKET packet always has an acknowledgment function. This function should clear the TCPIP\_MAC\_PKT\_FLAG\_QUEUED flag. The packet's ackRes is updated only if the parameter ackRes != TCPIP\_MAC\_PKT\_ACK\_NONE.

