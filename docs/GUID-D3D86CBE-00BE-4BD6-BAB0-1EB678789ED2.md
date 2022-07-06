# DRV\_ETHPHY\_ClientStatus Function

**Parent topic:**[Ethernet PHY Driver Library](GUID-F4DF749A-0F8C-4482-8661-C005A0BE0CF4.md)

## C

```
DRV_ETHPHY_CLIENT_STATUS DRV_ETHPHY_ClientStatus(DRV_HANDLE handle); 
```

## Returns

-   DRV\_ETHPHY\_CLIENT\_STATUS value describing the current status of the driver.


## Description

This function gets the client-specific status of the Ethernet PHY driver associated with the given handle.

## Remarks

This function will not block for hardware access and will immediately return the current status. This function has to be used to check that a driver operation has completed. It will return DRV\_ETHPHY\_CLIENT\_STATUS\_BUSY when an operation is in progress. It will return DRV\_ETHPHY\_CLIENT\_STATUS\_READY when the operation has completed.

## Preconditions

-   The DRV\_ETHPHY\_Initialize routine must have been called.

-   DRV\_ETHPHY\_Open must have been called to obtain a valid opened device handle.


\}

