# SNMP\_VAL Union

**Parent topic:**[SNMP Module](GUID-7764E81C-8FC9-4B3E-8830-255BDE678AA0.md)

## C

```
typedef union {
uint32_t dword;
uint16_t word;
uint8_t byte;
uint8_t v[sizeof(uint32_t)];
} SNMP_VAL;
```

## Description

Union: SNMP\_VAL.

SNMP agent processes different variable types.

## Remarks

None.

