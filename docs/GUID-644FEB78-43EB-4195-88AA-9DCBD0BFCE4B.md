# DRV\_ETHPHY\_CLIENT\_STATUS Enumeration

**Parent topic:**[Ethernet PHY Driver Library](GUID-F4DF749A-0F8C-4482-8661-C005A0BE0CF4.md)

## C

```
C 
typedef enum { 
    DRV_ETHPHY_CLIENT_STATUS_ERROR, 
    DRV_ETHPHY_CLIENT_STATUS_CLOSED, 
    DRV_ETHPHY_CLIENT_STATUS_BUSY, 
    DRV_ETHPHY_CLIENT_STATUS_READY 
} DRV_ETHPHY_CLIENT_STATUS; 
```

## Description

Ethernet PHY Driver Client Status<br />This enumeration identifies the client-specific status of the Ethernet PHY driver.

-   DRV\_ETHPHY\_CLIENT\_STATUS\_ERROR - Unspecified error condition

-   DRV\_ETHPHY\_CLIENT\_STATUS\_CLOSED - Client is not open

-   DRV\_ETHPHY\_CLIENT\_STATUS\_BUSY - An operation is currently in progress

-   DRV\_ETHPHY\_CLIENT\_STATUS\_READY - Up and running, no operations running


## Remarks

None.

