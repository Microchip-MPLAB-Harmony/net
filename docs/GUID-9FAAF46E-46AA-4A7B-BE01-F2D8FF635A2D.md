# TCPIP\_HTTP\_NET\_ConnectionHasArgsGet Function

**Parent topic:**[HTTP Net Module](GUID-4EFEB885-ECF8-44B5-8F23-1D05952E1845.md)

## C

```
uint8_t TCPIP_HTTP_NET_ConnectionHasArgsGet(
    TCPIP_HTTP_NET_CONN_HANDLE connHandle
);
```

## Description

The function will get the value of the "cookies or get arguments" that are present.

## Preconditions

None.

## Parameters

|Parameters|Description|
|----------|-----------|
|connHandle|HTTP connection handle.|

## Returns

-   The current value of the connection *hasArgs*.


## Remarks

None.

## Example

```
uint8_t hasArgs = TCPIP_HTTP_NET_ConnectionHasArgsGet(connHandle);
```

