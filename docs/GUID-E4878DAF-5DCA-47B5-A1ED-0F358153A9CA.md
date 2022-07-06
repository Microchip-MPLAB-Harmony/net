# DRV\_ETHPHY\_Close Function

**Parent topic:**[Ethernet PHY Driver Library](GUID-F4DF749A-0F8C-4482-8661-C005A0BE0CF4.md)

## C

```
void DRV_ETHPHY_Close(DRV_HANDLE handle); 
```

## Returns

None

## Description

This function closes an opened instance of the Ethernet PHY driver, invalidating the handle.

## Remarks

-   After calling this routine, the handle passed in "handle" must not be used with any of the remaining driver routines. A new handle must be obtained by calling DRV\_ETHPHY\_Open before the caller may use the driver again.

-   Usually there is no need for the driver client to verify that the Close operation has completed.


## Preconditions

The DRV\_ETHPHY\_Initialize routine must have been called for the specified Ethernet PHY driver instance. DRV\_ETHPHY\_Open must have been called to obtain a valid opened device handle.

