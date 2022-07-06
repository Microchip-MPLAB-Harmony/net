# TCPIP\_SNTP\_ConnectionInitiate Function

**Parent topic:**[SNTP Module](GUID-832A1C71-21E8-4386-BFCE-18B19538AC01.md)

## C

```
TCPIP_SNTP_RESULT TCPIP_SNTP_ConnectionInitiate();
```

## Description

This function will start a connection to the NTP server.

## Preconditions

The TCP/IP Stack should have been inititialized.

## Returns

-   SNTP\_RES\_OK - If the call succeeded.

-   SNTP\_RES\_PROGRESS - If a connection already in progress then another one could not be started.


## Remarks

None.

