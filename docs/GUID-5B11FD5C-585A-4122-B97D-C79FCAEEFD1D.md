# TCPIP\_MAC\_Initialize Function

**Parent topic:**[MAC Driver Module](GUID-0C1AF471-66D4-472F-84AF-212E9E18B21D.md)

## C

```
SYS_MODULE_OBJ TCPIP_MAC_Initialize(
    const SYS_MODULE_INDEX index, 
    const SYS_MODULE_INIT * const init
);
```

## Description

MAC Initialize function. SYS\_MODULE\_OBJ TCPIP\_MAC\_Initialize\(const SYS\_MODULE\_INDEX index, const SYS\_MODULE\_INIT \* const init\);

This is the function that initializes the MAC. It is called by the stack as a result of one interface going up.

## Precondtions

None.

## Parameters

|Parameters|Description|
|----------|-----------|
|index|Index for the MAC driver instance to be initialized.|
|init|Pointer to TCPIP\_MAC\_INIT initialization data containing: - macControl - Stack prepared data. - moduleData - Driver specific. Dependent on the MAC type. For PIC32 MAC driver, the TCPIP\_MODULE\_MAC\_PIC32INT\_CONFIG is used.|

## Returns

-   A valid handle to a driver object - If successful.

-   SYS\_MODULE\_OBJ\_INVALID - If initialization failed.


## Remarks

If this function fails, the stack won't turn up that interface. If the operation needs to wait for the hardware, the initialization function can return a pending code.

The returned object must be passed as argument to TCPIP\_MAC\_Reinitialize\(\), TCPIP\_MAC\_Deinitialize\(\), TCPIP\_MAC\_Tasks\(\) and TCPIP\_MAC\_Status routines\(\).

