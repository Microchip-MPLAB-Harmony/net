# DRV\_ENCX24J600\_Reinitialize Function

**Parent topic:**[ENCx24J600 Driver Library](GUID-F35BADF5-5469-4970-B3C5-52AB1E2287AB.md)

## C

```
void DRV_ENCX24J600_Reinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init); 
```

## Returns

None

## Description

ENCX24J600 Reinitialization This function will deinitialize and initialize the driver instance. As with DRV\_ENCX24J600\_Initialize DRV\_ENCX24J600\_SetMacCtrlInfo must be called for the driver to be useful.

## Remarks

This function is not planned to be implemented for the first release.

## Preconditions

The driver had to be successfully initialized with DRV\_ENCX24J600\_Initialize.

