# TCPIP\_DNS\_CLIENT\_CACHE\_UNSOLVED\_ENTRY\_TMO Macro

**Parent topic:**[DNS Module](GUID-D15C8F84-C30C-451F-8AB7-F8E62AD494C2.md)

## C

```
#define TCPIP_DNS_CLIENT_CACHE_UNSOLVED_ENTRY_TMO (10)
```

## Description

Time-out for the a unsolved name, in seconds. The name resolution will be aborted if the TMO elapsed and the name could not be solved Should be greater than TCPIP\_DNS\_CLIENT\_LOOKUP\_RETRY\_TMO.

