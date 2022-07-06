# TCPIP\_DHCPS\_LeaseEntriesGet Function

**Parent topic:**[DHCP Server Module \(V2\)](GUID-EE292D18-2DE6-478E-AAE1-74C69A31459E.md)

## C

TCPIP\_DHCPS\_RES TCPIP\_DHCPS\_LeaseEntriesGet\(TCPIP\_NET\_HANDLE netH, uint16\_t\* pLeases, uint16\_t\* pInUse\);

## Description

**Summary**

```
Get the total and in use number of lease entries for the selected interface.  
```

**Description**

```
This function returns the total and in use number of entries in the DB for the selected interface.  
```

**Precondition**

```
The DHCP Server module must be initialized.  
```

## Parameters

|Parameters|Description|
|----------|-----------|
|hNet|Interface to query|
|pLeases|address to store the total number of leases|
||Can be NULL if not needed|
|pInUse|address to store the number of in use lease entries|
||Can be NULL if not needed|

## Returns

*- TCPIP\_DHCPS\_RES\_OK* - the call is successful and the number of entries in the DB for the selected interface have been updated

*- < 0* - An error code if wrong interface, or DHCP server is not running on that interface

*- TCPIP\_DHCPS\_RES\_ACCESS\_LOCKED* - if access to the DHCP server DB is locked by another thread and the the call can be retried.<br />Multi-threaded access is enabled \(build symbol TCPIP\_DHCPS\_MULTI\_THREADED\_ACCESS != 0\)

**Remarks**

```
The returned value in pLeases should match the corresponding TCPIP_DHCPS_INTERFACE_CONFIG::leaseEntries;  
```

