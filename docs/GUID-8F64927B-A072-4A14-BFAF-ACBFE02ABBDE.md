# TCPIP\_SNTP\_TIME\_STAMP Union

**Parent topic:**[SNTP Module](GUID-832A1C71-21E8-4386-BFCE-18B19538AC01.md)

## C

```
typedef union {
struct {
    uint32_t tStampFraction;
    uint32_t tStampSeconds;
}
uint64_t llStamp;
} TCPIP_SNTP_TIME_STAMP;
```

## Description

Describes the structure of the timestamp used by the SNTP module.

## Members

|Members|Description|
|-------|-----------|
|tStampFraction|2^32 fractionary part.|
|tStampSeconds|Seconds.|

## Remarks

Timestamp formats are based on the prime epoch i.e. the base date is the 0 h 1 January 1900 UTC for which the timestamp is 0.

The tStampFraction is the least significant 32-bit number and represent 2^32 fractions of a second.

The tStampSeconds is the most significant 32-bit number and represents seconds.

The value of the timestamp is equal to: tStampSeconds + tStampFraction / 2^32 seconds.

The timestamp format is converted to host machine order.

