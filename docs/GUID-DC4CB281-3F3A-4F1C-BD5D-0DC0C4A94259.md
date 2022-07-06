# TCPIP\_DHCPS\_Enable Function

**Parent topic:**[DHCP Server Module \(V2\)](GUID-EE292D18-2DE6-478E-AAE1-74C69A31459E.md)

## C

TCPIP\_DHCPS\_RES TCPIP\_DHCPS\_Enable\(TCPIP\_NET\_HANDLE hNet\)

## Description

**Summary**

```
Enables the DHCP Server for the specified interface.  
```

**Description**

```
This function enables the DHCP Server for the specified interface, if it is disabled.
If it is already enabled, no action is taken.  
```

**Precondition**

```
The DHCP Server module must be initialized.  
```

## Parameters

|Parameters|Description|
|----------|-----------|
|hNet|Interface on which to enable the DHCP Server|

## Returns

*- TCPIP\_DHCPS\_RES\_OK* - the call is successful and the DHCP server has been enabled on the selected interface

*- < 0* - An error code if DHCP server could not be started, wrong interface, etc.

*- TCPIP\_DHCPS\_RES\_ACCESS\_LOCKED* - if access to the DHCP server is locked by another thread and the the call can be retried.<br />Multi-threaded access is enabled \(build symbol TCPIP\_DHCPS\_MULTI\_THREADED\_ACCESS != 0\)

