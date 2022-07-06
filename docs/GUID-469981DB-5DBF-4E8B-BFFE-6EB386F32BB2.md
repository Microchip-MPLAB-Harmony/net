# TCPIP\_STACK\_NetMACStatisticsGet Function

**Parent topic:**[Manager Module](GUID-B37C4F4C-DC2D-48D9-9909-AACBA987B57A.md)

## C

```
bool TCPIP_STACK_NetMACStatisticsGet(
    TCPIP_NET_HANDLE netH, 
    TCPIP_MAC_RX_STATISTICS* pRxStatistics, 
    TCPIP_MAC_TX_STATISTICS* pTxStatistics
);
```

## Description

This function returns the statistics data of the MAC that's attached to the specified network interface.

## Preconditions

The TCP/IP stack should have been initialized by TCPIP\_STACK\_Initialize\(\) and the TCPIP\_STACK\_Status\(\) returned SYS\_STATUS\_READY. The network interface should be up and running.

## Parameters

|Parameters|Description|
|----------|-----------|
|netH|Handle of the interface to use.|
|pRxStatistics|Pointer to a TCPIP\_MAC\_RX\_STATISTICS that will receive the current RX statistics counters. Can be NULL if not needed.|
|pTxStatistics|Pointer to a TCPIP\_MAC\_TX\_STATISTICS that will receive the current TX statistics counters. Can be NULL if not needed.|

## Returns

-   True - If the call succeeded.

-   False - If the call failed \(the corresponding MAC does not implement statistics counters\).


## Remarks

If the *netH* refers to an alias interface, the MAC statistics of the primary interface are returned.

## Example

```
TCPIP_MAC_RX_STATISTICS rxStatistics;
TCPIP_MAC_TX_STATISTICS txStatistics;
TCPIP_NET_HANDLE netH = TCPIP_STACK_NetHandleGet("PIC32INT");
if(TCPIP_STACK_NetMACStatisticsGet(netH, &rxStatistics, &txStatistics))
{
    // display the statistics for the internal PIC32 MAC attached to this interface
}
```

