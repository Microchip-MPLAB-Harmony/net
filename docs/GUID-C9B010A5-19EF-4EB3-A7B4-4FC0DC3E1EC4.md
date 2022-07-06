# DRV\_ENCX24J600\_StatisticsGet Function

**Parent topic:**[ENCx24J600 Driver Library](GUID-F35BADF5-5469-4970-B3C5-52AB1E2287AB.md)

## C

```
TCPIP_MAC_RES DRV_ENCX24J600_StatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics); 
```

## Returns

-   TCPIP\_MAC\_RES\_TYPE\_ERR - if the hMac is invalid

-   TCPIP\_MAC\_RES\_OP\_ERR - if the hMac is valid


## Description

ENCX24J600 Get Statistics Get the current statistics stored in the driver.

## Remarks

Statistics are not planned for the first release.

## Preconditions

The client had to be successfully opened with DRV\_ENCX24J600\_Open.

## Parameters

|Parameters|Description|
|----------|-----------|
|hMac|the successfully opened handle|

