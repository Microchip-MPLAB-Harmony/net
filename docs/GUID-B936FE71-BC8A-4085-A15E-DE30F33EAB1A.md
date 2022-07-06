# SNMP\_GENERIC\_TRAP\_NOTIFICATION\_TYPE Enumeration

**Parent topic:**[SNMP Module](GUID-7764E81C-8FC9-4B3E-8830-255BDE678AA0.md)

## C

```
typedef enum {
COLD_START = 0x0,
WARM_START = 0x1,
LINK_DOWN = 0x2,
LINK_UP = 0x3,
AUTH_FAILURE = 0x4,
EGP_NEIGHBOR_LOSS = 0x5,
ENTERPRISE_SPECIFIC = 0x6
} SNMP_GENERIC_TRAP_NOTIFICATION_TYPE;
```

## Description

Enumeration: SNMP\_GENERIC\_TRAP\_NOTIFICATION\_TYPE.

List of different SNMP specific Notification types.

## Members

|Members|Description|
|-------|-----------|
|COLD\_START = 0x0|Controls the sending of SNMP coldstart notifications. A coldStart\(0\) trap signifies that the sending protocol entity is reinitializing itself such that the configuration of the agent or the protocol entity implementation might be altered.|
|WARM\_START = 0x1|Controls the sending of SNMP warmstart notifications. A warmStart\(1\) trap signifies that the sending protocol entity is reinitializing itself so that neither the agent configuration nor the protocol entity implementation can be altered.|
|LINK\_DOWN = 0x2|Controls the how SNMP linkdown notifications are sent. A linkDown\(2\) trap signifies that the sending protocol entity recognizes a failure in one of the communication links represented in the configuration of the agent.|
|LINK\_UP = 0x3|Controls the sending of SNMP linkup notifications. A linkUp\(3\) trap signifies that the sending protocol entity recognizes that one of the communication links represented in the configuration of the agent has come up.|
|AUTH\_FAILURE = 0x4|Controls the distribution of SNMP authentication failure notifications. An authenticationFailure\(4\) trap signifies that the sending protocol entity is the addressee of a protocol message that is not properly authenticated. Like Community Name authentication failure.|
|EGP\_NEIGHBOR\_LOSS = 0x5|Controls the distribution of SNMP egpNeighborLoss notifications. An egpNeighborLoss\(5\) trap signifies that an EGP neighbor for whom the sending protocol entity was an EGP peer has been marked down and the peer relationship no longer exists.|
|ENTERPRISE\_SPECIFIC = 0x6|Controls the distribution of SNMP enterprise/-specific notifications. An enterpriseSpecific\(6\) trap signifies that the sending protocol entity recognizes that some enterprise/-specific event has occurred. The specific-trap field identifies the particular trap that occurred.|

## Remarks

ENTERPRISE\_SPECIFIC and AUTH\_FAILURE are used while sending specific trap.

