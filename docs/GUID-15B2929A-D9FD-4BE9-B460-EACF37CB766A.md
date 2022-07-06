# TCPIP\_TELNET\_STATE Enumeration

**Parent topic:**[Telnet Module](GUID-B0FCC6E9-74D8-443B-8F61-7317500EEFF3.md)

## C

```
typedef enum {
TCPIP_TELNET_IDLE = 0,
TCPIP_TELNET_GET_LOGIN,
TCPIP_TELNET_GET_PASSWORD,
TCPIP_TELNET_AUTHENTICATED,
TCPIP_TELNET_CONNECTED
} TCPIP_TELNET_STATE;
```

## Description

Enumeration: TCPIP\_TELNET\_STATE.

Enumeration describing the possible states of a telnet connection.

