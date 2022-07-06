# NET\_PRES\_EncSessionStatus Enumeration

**Parent topic:**[Networking Presentation Layer](GUID-75470E5B-2289-4F94-AE85-2BB7DF4C4F07.md)

## C

```
typedef enum { 
    NET_PRES_ENC_SS_UNKNOWN, 
    NET_PRES_ENC_SS_WAITING_TO_START_NEGOTIATION, 
    NET_PRES_ENC_SS_CLIENT_NEGOTIATING, 
    NET_PRES_ENC_SS_SERVER_NEGOTIATING, 
    NET_PRES_ENC_SS_OPEN, 
    NET_PRES_ENC_SS_FAILED, 
    NET_PRES_ENC_SS_CLOSING, 
    NET_PRES_ENC_SS_CLOSED 
} NET_PRES_EncSessionStatus; 
```

## Members

|Members|Description|
|-------|-----------|
|NET\_PRES\_ENC\_SS\_UNKNOWN|Presentation encryption is in an unknown/default state|
|NET\_PRES\_ENC\_SS\_WAITING\_TO\_START\_NEGOTIATION|Presentation encryption has not started negotiation|
|NET\_PRES\_ENC\_SS\_CLIENT\_NEGOTIATING|Presentation encryption client negotiation is in progress|
|NET\_PRES\_ENC\_SS\_SERVER\_NEGOTIATING|Presentation encryption server negotiation is in progress|
|NET\_PRES\_ENC\_SS\_OPEN|Presentation encryption negotiation is complete and data can be sent/received|
|NET\_PRES\_ENC\_SS\_FAILED|Presentation encryption negotiation failed or some other failure|
|NET\_PRES\_ENC\_SS\_CLOSING|Presentation encryption is closing, but connection needs to be pumped for final packets|
|NET\_PRES\_ENC\_SS\_CLOSED|Presentation encryption is closed, provider data has been freed|

## Description

MPLAB Harmony Networking Presentation Layer Encryption status type This enumeration defines the enumeration for the state and status of the encrypted portion of a connection.

## Remarks

None.

