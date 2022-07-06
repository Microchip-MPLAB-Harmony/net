# STD\_BASED\_SNMP\_MESSAGE\_PROCESSING\_MODEL Enumeration

**Parent topic:**[SNMP Module](GUID-7764E81C-8FC9-4B3E-8830-255BDE678AA0.md)

## C

```
typedef enum {
SNMPV1_MSG_PROCESSING_MODEL = 0X00,
SNMPV2C_MSG_PROCESSING_MODEL = 0X01,
SNMPV2U_V2_MSG_PROCESSING_MODEL = 0X02,
SNMPV3_MSG_PROCESSING_MODEL = 0X03
} STD_BASED_SNMP_MESSAGE_PROCESSING_MODEL;
```

## Description

Enumeration: STD\_BASED\_SNMP\_MESSAGE\_PROCESSING\_MODEL.

SNMP Message processing model is responsible for processing an SNMP version specific message and for coordinating the interaction with security sub system. SNMP message processing subsystem is part of an SNMP engine which interacts with the Dispatcher to handle the version specific SNMP messages.

A Message Processing Model describes the version-specific procedures for extracting data from messages, generating messages, calling upon a securityModel to apply its security services to messages.

## Members

|Members|Description|
|-------|-----------|
|SNMPV1\_MSG\_PROCESSING\_MODEL = 0X00|SNMP version 1 Message processing model.|
|SNMPV2C\_MSG\_PROCESSING\_MODEL = 0X01|SNMP version 2 Message processing model with community as security feature.|
|SNMPV2U\_V2\_MSG\_PROCESSING\_MODEL = 0X02|SNMP version 2 Message processing model.|
|SNMPV3\_MSG\_PROCESSING\_MODEL = 0X03|SNMP version 3 Message processing model with authentication and encryption and decryption.|

## Remarks

None.

