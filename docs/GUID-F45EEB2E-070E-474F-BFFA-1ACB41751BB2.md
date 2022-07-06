# TCPIP\_HTTP\_NET\_ConnectionHasArgsSet Function

**Parent topic:**[HTTP Net Module](GUID-4EFEB885-ECF8-44B5-8F23-1D05952E1845.md)

## C

```
void TCPIP_HTTP_NET_ConnectionIsAuthorizedSet(
    TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
    uint8_t auth
);
```

## Description

This function sets the authorization status for the current HTTP connection. This has to be one of the values in the set returned by the template\_ConnectionFileAuthenticate\(\) function.

## Preconditions

None.

## Parameters

|Parameters|Description|
|----------|-----------|
|connHandle|HTTP connection handle.|
|auth|New authorization state.|

## Returns

None.

## Remarks

None.

## Example

```
uint8_t auth = 0x80;

TCPIP_HTTP_NET_ConnectionIsAuthorizedSet(connHandle, auth);
```

