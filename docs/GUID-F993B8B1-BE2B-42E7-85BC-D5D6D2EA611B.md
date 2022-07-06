# TCPIP\_MAC\_INIT Structure

**Parent topic:**[MAC Driver Module](GUID-0C1AF471-66D4-472F-84AF-212E9E18B21D.md)

## C

```
typedef struct {
SYS_MODULE_INIT moduleInit;
const TCPIP_MAC_MODULE_CTRL* const macControl;
const void* const moduleData;
} TCPIP_MAC_INIT;
```

## Description

MAC Device Driver Initialization Data.

This data structure contains all the data necessary to initialize the MAC device.

## Members

|Members|Description|
|-------|-----------|
|moduleInit|System module initialization.|
|macControl|Stack prepared data.|
|moduleData|Driver specific initialization data.|

## Remarks

A pointer to a structure of this format containing the desired initialization data must be passed into the TCPIP\_MAC\_Initialize\(\) routine.

