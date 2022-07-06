# TCPIP\_Helper\_FormatNetBIOSName Function

**Parent topic:**[Manager Module](GUID-B37C4F4C-DC2D-48D9-9909-AACBA987B57A.md)

## C

```
void TCPIP_Helper_FormatNetBIOSName(
    uint8_t Name[]
);
```

## Description

This function formats a string to a valid NetBIOS name. Names will be exactly 16 characters, as defined by the NetBIOS spec. The 16th character will be a 0x00 byte, while the other 15 will be the provided string, padded with spaces as necessary.

## Preconditions

Name - valid pointer to a 16 byte area to be populated.

## Parameters

|Parameters|Description|
|----------|-----------|
|Name|The string to format as a NetBIOS name. This parameters must have at least 16 bytes allocated.|

## Returns

None.

