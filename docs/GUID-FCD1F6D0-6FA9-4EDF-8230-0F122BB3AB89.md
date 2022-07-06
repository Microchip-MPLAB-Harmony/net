# TCPIP\_SNTP\_RESULT Enumeration

**Parent topic:**[SNTP Module](GUID-832A1C71-21E8-4386-BFCE-18B19538AC01.md)

## C

```
typedef enum {
    SNTP_RES_OK,
    SNTP_RES_PROGRESS,
    SNTP_RES_BUSY = -1,
    SNTP_RES_TSTAMP_ERROR = -2,
    SNTP_RES_TSTAMP_STALE = -3,
    SNTP_RES_SKT_ERR = -4,
    SNTP_RES_SKT_BIND_ERR = -5,
    SNTP_RES_NTP_SERVER_TMO = -6,
    SNTP_RES_NTP_VERSION_ERR = -7,
    SNTP_RES_NTP_TSTAMP_ERR = -8,
    SNTP_RES_NTP_SYNC_ERR = -9,
    SNTP_RES_NTP_KOD_ERR = -10,
    SNTP_RES_NTP_DNS_ERR = -11,
    SNTP_RES_NTP_IF_ERR = -12,
    SNTP_RES_NTP_CONN_ERR = -13
} TCPIP_SNTP_RESULT;
```

## Description

Provides a list of possible SNTP results.

## Members

|Members|Description|
|-------|-----------|
|SNTP\_RES\_OK|The operation was successful.|
|SNTP\_RES\_PROGRESS|An NTP operation is in progress.|
|SNTP\_RES\_BUSY = -1|Module is busy.|
|SNTP\_RES\_TSTAMP\_ERROR = -2|No timestamp is available, server not contacted.|
|SNTP\_RES\_TSTAMP\_STALE = -3|Timestamp is stale, there's no recent timestamp.|
|SNTP\_RES\_SKT\_ERR = -4|NTP socket operation error.|
|SNTP\_RES\_SKT\_BIND\_ERR = -5|NTP socket bind failed.|
|SNTP\_RES\_NTP\_SERVER\_TMO = -6|NTP server could not be accessed.|
|SNTP\_RES\_NTP\_VERSION\_ERR = -7|Wrong NTP cersion received.|
|SNTP\_RES\_NTP\_TSTAMP\_ERR = -8|Wrong NTP timestamp received.|
|SNTP\_RES\_NTP\_SYNC\_ERR = -9|NTP time synchronization error.|
|SNTP\_RES\_NTP\_KOD\_ERR = -10|An NTP KissOfDeath code has been received.|
|SNTP\_RES\_NTP\_DNS\_ERR = -11|An NTP DNS error.|
|SNTP\_RES\_NTP\_IF\_ERR = -12|An NTP interface error.|
|SNTP\_RES\_NTP\_CONN\_ERR = -13|An NTP connection type error.|

