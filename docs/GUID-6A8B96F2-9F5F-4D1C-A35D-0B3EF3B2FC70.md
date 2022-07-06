# TCPIP\_STACK\_NetAddressDnsPrimarySet Function

**Parent topic:**[Manager Module](GUID-B37C4F4C-DC2D-48D9-9909-AACBA987B57A.md)

## C

```
bool TCPIP_STACK_NetAddressDnsPrimarySet(
    TCPIP_NET_HANDLE netH, 
    IPV4_ADDR* ipAddress
);
```

## Description

This function sets the network interface primary IPv4 DNS address.

## Preconditions

The TCPIP\_STACK\_Initialize\(\) function must have been called before calling this function. The network interface should be up and running.

## Parameters

|Parameters|Description|
|----------|-----------|
|netH|Interface handle to set the DNS address of.|
|ipAddress|IP address to set.|

## Returns

-   True - If success.

-   False - If no such interface or interface is not enabled.


## Remarks

Exercise extreme caution when using these functions to change the settings of a running network interface. Chaning these parameters at runtime can lead to unexpected behavior or loss of network connectivity. The preferred way to change the parameters for a running interface is to do so as part of the network configuration passed at the stack initialization.

## Example

```
TCPIP_NET_HANDLE netH = TCPIP_STACK_NetHandleGet("PIC32INT");
TCPIP_STACK_NetAddressDnsPrimarySet(netH, &myIPAddress);
```

