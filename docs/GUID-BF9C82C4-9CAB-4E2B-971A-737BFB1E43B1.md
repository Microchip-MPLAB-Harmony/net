# TCPIP\_ARP\_Resolve Function

**Parent topic:**[ARP Module](GUID-3DCBF1EB-5623-4419-B244-DDD9B01704EE.md)

## C

```
TCPIP_ARP_RESULT TCPIP_ARP_Resolve(
    TCPIP_NET_HANDLE hNet, 
    IPV4_ADDR* IPAddr
);
```

## Description

This function transmits and ARP request to determine the hardware address of a given IP address. Upon the address resolution it calls the registered handler \(if available\) with the supplied notification parameter \(if != 0\).

## Preconditions

The ARP module should have been initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|hNet|Interface to use.|
|IPAddr|The IP address to be resolved. The address must be specified in network byte order \(big endian\).|

## Returns

An element from the TCPIP\_ARP\_RESULT enumeration.

-   ARP\_RES\_ENTRY\_SOLVED - if the required entry is already solved.

-   ARP\_RES\_ENTRY\_QUEUED - if the required entry was already queued.

-   ARP\_RES\_ENTRY\_NEW - if the operation succeeded and a new entry was added \(and queued for resolving\).

-   ARP\_RES\_CACHE\_FULL - if new entry could not be inserted, the cache was full.

-   ARP\_RES\_BAD\_ADDRESS - bad address specified.

-   ARP\_RES\_NO\_INTERFACE - no such interface.


## Remarks

To retrieve the ARP query result, call the TCPIP\_ARP\_IsResolved function.

