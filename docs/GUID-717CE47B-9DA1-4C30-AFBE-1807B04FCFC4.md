# TCPIP\_IGMP\_HandlerDeRegister Function

**Parent topic:**[IGMP Module](GUID-DCB13BC6-B7A2-45CA-89E7-9474EAF05EFB.md)

## C

```
bool TCPIP_IGMP_HandlerDeRegister(
    TCPIP_IGMP_HANDLE hIgmp
);
```

## Description

This function deregisters the IGMP client event handler.

## Preconditions

The IGMP module must be initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|hIgmp|A handle returned by a previous call to TCPIP\_IGMP\_HandlerRegister.|

## Returns

-   true - If the call succeeds.

-   false - If no such handler is registered.


## Remarks

None.

