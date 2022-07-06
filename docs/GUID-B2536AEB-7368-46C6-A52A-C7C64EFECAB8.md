# DRV\_ETHPHY\_RestartNegotiation Function

**Parent topic:**[Ethernet PHY Driver Library](GUID-F4DF749A-0F8C-4482-8661-C005A0BE0CF4.md)

## C

```
DRV_ETHPHY_RESULT DRV_ETHPHY_RestartNegotiation(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex); 
```

## Returns

-   DRV\_ETHPHY\_RES\_PENDING operation has been scheduled successfully

-   an DRV\_ETHPHY\_RESULT error code if the procedure failed.


## Description

This function restarts auto-negotiation of the Ethernet PHY link.

## Remarks

Use DRV\_ETHPHY\_ClientStatus\(\) and DRV\_ETHPHY\_ClientOperationResult\(\) to check when the operation was completed and its outcome.

## Preconditions

-   The DRV\_ETHPHY\_Initialize routine must have been called.

-   DRV\_ETHPHY\_Open must have been called to obtain a valid device handle.

-   DRV\_ETHPHY\_Setup must have been called to properly configure the PHY


