# TCPIP\_HTTP\_NET\_ConnectionIsAuthorizedGet Function

**Parent topic:**[HTTP Net Module](GUID-4EFEB885-ECF8-44B5-8F23-1D05952E1845.md)

## C

```
uint8_t TCPIP_HTTP_NET_ConnectionIsAuthorizedGet(
    TCPIP_HTTP_NET_CONN_HANDLE connHandle
);
```

## Description

This function returns the authorization status for the current HTTP connection. This is one of the values returned by the template\_ConnectionFileAuthenticate\(\) function.

## Preconditions

None.

## Parameters

|Parameters|Description|
|----------|-----------|
|connHandle|HTTP connection handle.|

## Returns

-   A uint8\_t representing the authorization status.


## Remarks

None.

## Example

```
uint8_t isAuth;

isAuth = TCPIP_HTTP_NET_ConnectionIsAuthorizedGet(connHandle);
```

