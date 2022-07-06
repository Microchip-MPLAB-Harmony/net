# SNMP\_INDEX Type

**Parent topic:**[SNMP Module](GUID-7764E81C-8FC9-4B3E-8830-255BDE678AA0.md)

## C

```
typedef uint32_t SNMP_ID;
```

## Description

Type: SNMP\_ID.

Only dynamic and AgentID variables can contain a dyanmic ID. MIB2BIB utility enforces this rule when BIB was generated. All the dynamic IDs are are listed in mib.h. The maximum value of a dynamic ID is 1023.

## Remarks

mib2bib.jar utility generates mib.h and snmp.bib from Microchip MIB script. DynamicVar - This command declares defined OID variable as dynamic. Syntax - $DynamicVar\(oidName, id\).

