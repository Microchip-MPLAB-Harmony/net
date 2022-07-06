# TCPIP\_IPV4\_PacketFilterClear Function

**Parent topic:**[IPv4 Module](GUID-EA29E72F-4194-41F9-9F19-D8BBA00D62F2.md)

## C

```
TCPIP_IPV4_FILTER_TYPE TCPIP_IPV4_PacketFilterClear(
    TCPIP_IPV4_FILTER_TYPE filtType
);
```

## Description

The function will clear the IPv4 packet filters. The filters that are present in the mask will be cleared. Other filters won't be touched.

## Preconditions

filtType - valid IPv4 filter IPv4 properly initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|filtType|Packet filter mask to clear.|

## Returns

-   The current value of the IPV4 packet filters after this mask was applied.


## Remarks

None.

