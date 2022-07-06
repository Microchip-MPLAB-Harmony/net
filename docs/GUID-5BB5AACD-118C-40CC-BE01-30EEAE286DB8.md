# IPV6\_ADDRESS\_TYPE Union

**Parent topic:**[IPv6 Module](GUID-F2484EF9-7914-43EE-A5B7-4FFDC27C8135.md)

## C

```
typedef union {
unsigned char byte;
struct {
    unsigned scope : 4;
    unsigned type : 2;
} bits;
} IPV6_ADDRESS_TYPE;
```

## Desscription

Type: IPV6\_ADDRESS\_TYPE.

This type defines the data structure for IPv6 address types.

## Remarks

None.

