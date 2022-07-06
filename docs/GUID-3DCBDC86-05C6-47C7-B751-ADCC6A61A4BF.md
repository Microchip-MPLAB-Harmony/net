# DRV\_ENC28J60\_RegisterStatisticsGet Function

**Parent topic:**[ENC28J60 Driver Library](GUID-58EA08F2-E38D-48FD-BD75-C2972C0EE761.md)

## C

```
TCPIP_MAC_RES DRV_ENC28J60_RegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries); 
```

## Returns

-   TCPIP\_MAC\_RES\_TYPE\_ERR - if the hMac is invalid

-   TCPIP\_MAC\_RES\_OP\_ERR - if the hMac is valid


## Description

ENC28J60 Get Register Statistics Get the device specific statistics.

## Remarks

Statistics are not planned for the first release

## Preconditions

The client had to be successfully opened with DRV\_ENC28J60\_Open.

## Parameters

|Parameters|Description|
|----------|-----------|
|hMac|the successfully opened handle|
|pRegEntries||
|nEntries||
|pHwEntries||

