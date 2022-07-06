# DRV\_ENC28J60\_Status Function

**Parent topic:**[ENC28J60 Driver Library](GUID-58EA08F2-E38D-48FD-BD75-C2972C0EE761.md)

## C

```
SYS_STATUS DRV_ENC28J60_Status(SYS_MODULE_OBJ obect); 
```

## Returns

-   SYS\_STATUS\_ERROR - if an invalid handle has been passed in

-   SYS\_STATUS\_UNINITIALIZED - if the driver has not completed initialization

-   SYS\_STATUS\_BUSY - if the driver is closing and moving to the closed state

-   SYS\_STATUS\_READY - if the driver is ready for client commands


## Description

ENC28J60 Status This function will get the status of the driver instance.

## Preconditions

The driver had to be successfully initialized with DRV\_ENC28J60\_Initialize\(\).

## Parameters

|Parameters|Description|
|----------|-----------|
|object|The object valid passed back to DRV\_ENC28J60\_Initialize\(\)|

