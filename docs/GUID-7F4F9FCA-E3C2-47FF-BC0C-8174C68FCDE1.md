# TCPIP\_DHCPS\_Configure Function

**Parent topic:**[DHCP Server Module \(V2\)](GUID-EE292D18-2DE6-478E-AAE1-74C69A31459E.md)

## C

TCPIP\_DHCPS\_RES TCPIP\_DHCPS\_Configure\(const TCPIP\_DHCPS\_INTERFACE\_CONFIG\* pIfConfig, uint16\_t nConfigs\);

## Description

**Summary**

```
Configure the DHCP server on multiple interfaces  
```

**Description**

```
This function is used to configure the DHCP server at run time for the selected interfaces  
```

**Precondition**

```
The DHCP Server module should have been initialized.  
```

## Parameters

|Parameters|Description|
|----------|-----------|
|pIfConfig|pointer to an array of configuration parameters|
|nConfigs|number of configurations in the pIfConfig array|

## Returns

*- TCPIP\_DHCPS\_RES\_OK* - if the interfaces were properly configured

*- < 0* - if wrong interface or allocation failed, etc.

*- TCPIP\_DHCPS\_RES\_ACCESS\_LOCKED* - if access to the DHCP server is locked by another thread and the the call should be retried.<br />Multi-threaded access is enabled \(build symbol TCPIP\_DHCPS\_MULTI\_THREADED\_ACCESS != 0\)

**Remarks**

```
The DHCP server for the selected interfaces will be completely reconfigured by this call.
All the information about the existing leases will be lost.

Preferrably the call should be done with the DHCP server disabled.

The call will attempt to lock access to the DHCP server DB.
During this time, the server will not reply to clients and any received packets are ignored.  
```

