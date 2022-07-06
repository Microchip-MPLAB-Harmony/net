# TCPIP\_HTTP\_NET\_ConnectionIndexGet Function

**Parent topic:**[HTTP Net Module](GUID-4EFEB885-ECF8-44B5-8F23-1D05952E1845.md)

## C

```
int TCPIP_HTTP_NET_ConnectionIndexGet(
    TCPIP_HTTP_NET_CONN_HANDLE connHandle
);
```

## Description

This function will return the index of the requested HTTP connection.

## Preconditions

None.

## Parameters

|Parameters|Description|
|----------|-----------|
|connHandle|The HTTP connection handle.|

## Returns

-   The connection index.


## Remarks

None.

## Example

```
int connIx;

connIx = TCPIP_HTTP_NET_ConnectionIndexGet(connHandle);
```

