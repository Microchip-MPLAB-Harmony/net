# DRV\_GMAC\_Open Function

**Parent topic:**[Ethernet GMAC Driver Library](GUID-A24BDAD2-C63E-40B1-894D-1DC3CC6CB66A.md)

## C

```
DRV_HANDLE DRV_GMAC_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent); 
```

## Returns

-   DRV\_HANDLE - handle \(pointer\) to MAC client

-   0 if call failed


## Description

This function opens a client instance of the PIC32 MAC Driver. Used by tcpip\_module\_manager.

## Remarks

The intent parameter is not used in the current implementation and is maintained only for compatibility with the generic driver Open function signature.

## Preconditions

DRV\_GMAC\_Initialize\(\) should have been called.

