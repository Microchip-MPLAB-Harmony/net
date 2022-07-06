# TCPIP\_TELNET\_AuthenticationRegister Function

**Parent topic:**[Telnet Module](GUID-B0FCC6E9-74D8-443B-8F61-7317500EEFF3.md)

## C

```
TCPIP_TELNET_HANDLE TCPIP_TELNET_AuthenticationRegister(
    TCPIP_TELNET_AUTH_HANDLER authHandler, 
    const void* handlerParam
);
```

## Description

This function registers a new authentication handler. telnet module will call the handler when an user tries to login and needs to be authenticated.

## Preconditions

Telnet properly initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|authHandler|The handler which will be called for authentication.|
|handlerParam|Handler parameter that will be called with the handler unused by telnet.|

## Returns

-   A valid TCPIP\_TELNET\_HANDLE - If the operation succeeded.

-   NULL - If the operation failed.


## Remarks

Currently only one authentication handler is supported by the telnet module. The call will fail if a handler is already registered. Use TCPIP\_TELNET\_AuthenticationDeregister\(\) first.

This is the PREFERRED authentication method! If there's no registrated handler, any login attempt will fail.

The build should have TCPIP\_TELNET\_OBSOLETE\_AUTHENTICATION == false.

## Example

```
TCPIP_TELNET_HANDLE authHandle = TCPIP_TELNET_AuthenticationRegister( myAuthHandler, myParam );
```

