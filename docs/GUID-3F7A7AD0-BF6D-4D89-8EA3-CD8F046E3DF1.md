# DRV\_ENC28J60\_StackInitialize Function

**Parent topic:**[ENC28J60 Driver Library](GUID-58EA08F2-E38D-48FD-BD75-C2972C0EE761.md)

## C

```
SYS_MODULE_OBJ DRV_ENC28J60_StackInitialize(SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init); 
```

## Returns

Returns a valid handle to the driver instance - If successful SYS\_MODULE\_OBJ\_INVALID - If unsuccessful

## Description

ENC28J60 Stack Initialization This function is used by the TCP/IP stack to fully initialize the driver with both the ENC28J60 specific configuration and the MAC control information. With this function there is no need to call DRV\_ENC28J60\_SetMacCtrlInfo.

## Preconditions

None.

## Parameters

|Parameters|Description|
|----------|-----------|
|index|This is the index of the driver instance to be initialized. The definition DRV\_ENC28J60\_NUM\_DRV\_INSTANCES controls how many instances are available.|
|init|This is a pointer to a TCPIP\_MAC\_INIT structure.|

