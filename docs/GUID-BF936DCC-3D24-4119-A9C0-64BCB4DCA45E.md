# TCPIP\_MAC\_PROCESS\_FLAGS Enumeration

**Parent topic:**[MAC Driver Module](GUID-0C1AF471-66D4-472F-84AF-212E9E18B21D.md)

## C

```
typedef enum {
TCPIP_MAC_PROCESS_FLAG_NONE = 0x0000,
TCPIP_MAC_PROCESS_FLAG_RX = 0x0001,
TCPIP_MAC_PROCESS_FLAG_TX = 0x0002,
TCPIP_MAC_PROCESS_FLAG_ANY = 0x0100
} TCPIP_MAC_PROCESS_FLAGS;
```

## Description

MAC Process Flags.

List of specific MAC processing flags that indicate to the user of the MAC \(TCP/IP stack\) the processing that is expected by the MAC driver.

## Parameters

|Parameters|Description|
|----------|-----------|
|TCPIP\_MAC\_PROCESS\_FLAG\_NONE = 0x0000|The stack never has to call the TCPIP\_MAC\_Process\(\) function.|
|TCPIP\_MAC\_PROCESS\_FLAG\_RX = 0x0001|The stack has to call the TCPIP\_MAC\_Process\(\) after an RX signal.|
|TCPIP\_MAC\_PROCESS\_FLAG\_TX = 0x0002|The stack has to call the TCPIP\_MAC\_Process\(\) after a TX signal.|
|TCPIP\_MAC\_PROCESS\_FLAG\_ANY = 0x0100|The stack has to call the TCPIP\_MAC\_Process\(\) after any type of signal.|

## Remarks

Multiple flags can be ORed.

