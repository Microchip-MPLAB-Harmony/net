# SNMPV3\_PRIV\_PROT\_TYPE Enumeration

**Parent topic:**[SNMP Module](GUID-7764E81C-8FC9-4B3E-8830-255BDE678AA0.md)

## C

```
typedef enum {
SNMPV3_DES_PRIV = 0x0,
SNMPV3_AES_PRIV,
SNMPV3_NO_PRIV
} SNMPV3_PRIV_PROT_TYPE;
```

## Description

Enumeration: SNMPV3\_PRIV\_PROT\_TYPE.

These below privacy types are supported by the SNMPv3 USM model for data confidentiality. SNMPv3 agent supports both AES-CFB and DES-CBC encryption and decryption algorithm. For DES-CBC privacy protocol, SNMPv3 agent will use Harmony Crypto Library. For AES-CFB privacy protocol, SNMPv3 agent will use Legacy TCP/IP Crypto Library \(For AES, include -aes\_pic32mx.a to the project.\).

## Members

|Members|Description|
|-------|-----------|
|SNMPV3\_DES\_PRIV = 0x0|Data Encryption Standard \(DES-CBC\) encryption and decryption protocol.|
|SNMPV3\_AES\_PRIV|Advanced Encryption Standard \(AES-CFB\) encryption and decryption protocol.|
|SNMPV3\_NO\_PRIV|No encryption or decryption protocol is supported.|

## Remarks

128-bit, 192-bit and 256-bit AES are supported.

