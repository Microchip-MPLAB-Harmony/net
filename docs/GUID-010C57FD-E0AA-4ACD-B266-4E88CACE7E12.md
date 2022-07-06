# IPV4\_OPTION\_FIELD Structure

**Parent topic:**[IPv4 Module](GUID-EA29E72F-4194-41F9-9F19-D8BBA00D62F2.md)

## C

```
typedef struct {
struct {
    uint8_t optNumber : 5;
    uint8_t optClass : 2;
    uint8_t optCopied : 1;
}
uint8_t optLength;
uint16_t optData[];
} IPV4_OPTION_FIELD;
```

## Description

IPv4 packet option definition.

This is the structure of an IPv4 option field in the IPv4 header.

## Parameters

|Parameters|Description|
|----------|-----------|
|uint8\_t optNumber : 5;|Specifies an option.|
|uint8\_t optClass : 2;|Option class: 0 - control; 2 - debugging and measurement; 1, 3 - reserved.|
|uint8\_t optCopied : 1;|Option copied to all fragments.|
|uint8\_t optLength;|Size of the entire option.|
|uint16\_t optData;|Option specific variable data.|

## Returns

None.

## Remarks

None.

