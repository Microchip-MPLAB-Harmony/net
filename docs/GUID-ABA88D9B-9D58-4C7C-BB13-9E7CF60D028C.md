# TCPIP\_DHCPS\_LeaseGetInfoById Function

**Parent topic:**[DHCP Server Module \(V2\)](GUID-EE292D18-2DE6-478E-AAE1-74C69A31459E.md)

## C

TCPIP\_DHCPS\_RES TCPIP\_DHCPS\_LeaseGetInfoById\(TCPIP\_NET\_HANDLE netH, const uint8\_t\* clientId, size\_t idLen, TCPIP\_DHCPS\_LEASE\_INFO\* pLeaseInfo\);

## Description

**Summary**

```
Get the lease entry details for the requested client ID  
```

**Description**

```
This function returns a DB lease entry info for the selected ID on the selected interface  
```

**Precondition**

```
The DHCP Server module should have been initialized.  
```

## Parameters

|Parameters|Description|
|----------|-----------|
|hNet|selected interface handle|
|clientId|the client ID, usually a MAC address|
|idLen|length of the ID|
|pLeaseInfo|address to store the lease entry details Could be NULL if details not needed|

**Returns**

*- TCPIP\_DHCPS\_RES\_OK* - the call is successful and the pLeaseInfo has been updated, if not NULL

*- < 0* - if wrong interface or DHCP server is not running on that interface, or wrong/unused index

*- TCPIP\_DHCPS\_RES\_ACCESS\_LOCKED* - if access to the DHCP server DB is locked by another thread and the the call can be retried.<br />Multi-threaded access is enabled \(build symbol TCPIP\_DHCPS\_MULTI\_THREADED\_ACCESS != 0\)

**Remarks**

```
The maximum size of the client ID should be TCPIP_DHCPS_CLIENT_ID_MAX_SIZE.
Extra characters will be truncated.
Usually the client ID is a MAC address. However some clients can use different, longer IDs.  
```

