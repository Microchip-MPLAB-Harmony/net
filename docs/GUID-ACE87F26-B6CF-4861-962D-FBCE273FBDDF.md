# DRV\_MIIM\_Reinitialize Function

**Parent topic:**[MIIM Driver Library](GUID-A8906C8D-A608-4572-AE74-1E517DD2B0BE.md)

## C

```
void DRV_MIIM_Reinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init); 
```

## Returns

None.

## Description

This function reinitializes the driver and refreshes any associated hardware settings using the initialization data given, but it will not interrupt any ongoing operations.

## Remarks

-   This function can be called multiple times to reinitialize the module.

-   This operation can be used to refresh any supported hardware registers as specified by the initialization data or to change the power state of the module.

-   This function is currently NOT IMPLEMENTED.


## Preconditions

The DRV\_MIIM\_Initialize function must have been called before calling this routine and a valid SYS\_MODULE\_OBJ must have been returned.

