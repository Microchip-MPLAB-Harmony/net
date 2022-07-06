# DRV\_ETHPHY\_NegotiationResultGet Function

**Parent topic:**[Ethernet PHY Driver Library](GUID-F4DF749A-0F8C-4482-8661-C005A0BE0CF4.md)

## C

```
C 
DRV_ETHPHY_RESULT DRV_ETHPHY_NegotiationResultGet(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult); 
```

## Returns

-   DRV\_ETHPHY\_RES\_PENDING operation is ongoing

-   an DRV\_ETHPHY\_RESULT error code if the procedure failed.


## Description

This function returns the PHY negotiation data gathered after a completed negotiation.

## Remarks

Use DRV\_ETHPHY\_ClientStatus\(\) and DRV\_ETHPHY\_ClientOperationResult\(\) to check when the operation was completed and its outcome. When operation is completed but negotiation has failed, DRV\_ETHPHY\_ClientOperationResult will return:

-   DRV\_ETHPHY\_RES\_NEGOTIATION\_INACTIVE if no negotiation in progress

-   DRV\_ETHPHY\_RES\_NEGOTIATION\_NOT\_STARTED if negotiation not yet started yet \(means time out if waitComplete was requested\)

-   DRV\_ETHPHY\_RES\_NEGOTIATION\_ACTIVE if negotiation ongoing The returned value for the negotiation flags is valid only if the negotiation was completed successfully.


## Preconditions

-   The DRV\_ETHPHY\_Initialize routine must have been called.

-   DRV\_ETHPHY\_Open must have been called to obtain a valid device handle.

-   DR\_ETHPHY\_Setup must have been called to properly configure the PHY

-   DRV\_ETHPHY\_RestartNegotiation, and DRV\_ETHPHY\_NegotiationIsComplete should have been called.


