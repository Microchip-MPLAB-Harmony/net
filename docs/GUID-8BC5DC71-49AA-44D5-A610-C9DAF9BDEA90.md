# DRV\_ETHPHY\_ClientOperationResult Function

**Parent topic:**[Ethernet PHY Driver Library](GUID-F4DF749A-0F8C-4482-8661-C005A0BE0CF4.md)

## C

```
DRV_ETHPHY_RESULT DRV_ETHPHY_ClientOperationResult(DRV_HANDLE handle); 
```

## Returns

DRV\_ETHPHY\_RESULT value describing the current operation result: DRV\_ETHPHY\_RES\_OK for success; operation has been completed successfully DRV\_ETHPHY\_RES\_PENDING operation is in progress an DRV\_ETHPHY\_RESULT error code if the operation failed.

## Description

Returns the result of a client operation initiated by the Ethernet PHY driver.

## Remarks

This function will not block for hardware access and will immediately return the current status. This function returns the result of the last driver operation. It will return DRV\_ETHPHY\_RES\_PENDING if an operation is still in progress. Otherwise a DRV\_ETHPHY\_RESULT describing the operation outcome.

## Preconditions

-   The DRV\_ETHPHY\_Initialize routine must have been called.

-   DRV\_ETHPHY\_Open must have been called to obtain a valid opened device handle.

-   A driver operation was started and completed


