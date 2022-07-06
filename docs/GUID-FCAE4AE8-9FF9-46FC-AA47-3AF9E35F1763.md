# TCPIP\_HTTP\_NET\_ActiveConnectionCountGet Function

**Parent topic:**[HTTP Net Module](GUID-4EFEB885-ECF8-44B5-8F23-1D05952E1845.md)

## C

```
int TCPIP_HTTP_NET_ActiveConnectionCountGet(
    int* pOpenCount
);
```

## Description

This function will return the number of active and total opened HTTP connections at the current time.

## Preconditions

None.

## Parameters

|Parameters|Description|
|----------|-----------|
|pOpenCount|Address to store the number of total HTTP opened connections. Could be NULL if not needed.|

## Returns

-   The number of active/total connections.


## Remarks

The value returned by this function is informational only. The number of active connections changes dynamically.

## Example

```
int nConns;

nConns = TCPIP_HTTP_NET_ActiveConnectionCountGet(0);
```

