# DRV\_ENCX24J600\_StackInitialize Function

**Parent topic:**[ENCx24J600 Driver Library](GUID-F35BADF5-5469-4970-B3C5-52AB1E2287AB.md)

## C

```
C 
SYS_MODULE_OBJ DRV_ENCX24J600_StackInitialize(SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init); 
```

## Returns

Returns a valid handle to the driver instance - If successful SYS\_MODULE\_OBJ\_INVALID - If unsuccessful

## Description

ENCX24J600 Stack Initialization This function is used by the TCP/IP stack to fully initialize the driver with both the ENCX24J600 specific configuration and the MAC control information. With this function there is no need to call DRV\_ENCX24J600\_SetMacCtrlInfo.

## Preconditions

None.

## Parameters

|Parameters|Description|
|----------|-----------|
|init|This is a pointer to a TCPIP\_MAC\_INIT structure.|

