# TCP\_ADJUST\_FLAGS Enumeration

**Parent topic:**[TCP Module](GUID-9461917B-27CE-44ED-80DB-67D963896E8F.md)

## C

```
typedef enum {
TCP_ADJUST_GIVE_REST_TO_RX = 0x01,
TCP_ADJUST_GIVE_REST_TO_TX = 0x02,
TCP_ADJUST_PRESERVE_RX = 0x04,
TCP_ADJUST_PRESERVE_TX = 0x08,
TCP_ADJUST_TX_ONLY = 0x10,
TCP_ADJUST_RX_ONLY = 0x20
} TCP_ADJUST_FLAGS;
```

## Description

Enumeration: TCP\_ADJUST\_FLAGS.

Adjusts socket RX and TX buffer sizes.

## Members

|Members|Description|
|-------|-----------|
|TCP\_ADJUST\_GIVE\_REST\_TO\_RX = 0x01|Resize flag: extra bytes go to RX.|
|TCP\_ADJUST\_GIVE\_REST\_TO\_TX = 0x02|Resize flag: extra bytes go to TX.|
|TCP\_ADJUST\_PRESERVE\_RX = 0x04|Resize flag: attempt to preserve RX buffer.|
|TCP\_ADJUST\_PRESERVE\_TX = 0x08|Resize flag: attempt to preserve TX buffer.|
|TCP\_ADJUST\_TX\_ONLY = 0x10|Resize flag: adjust the TX buffer only.|
|TCP\_ADJUST\_RX\_ONLY = 0x20|Resize flag: adjust the RX buffer only.|

