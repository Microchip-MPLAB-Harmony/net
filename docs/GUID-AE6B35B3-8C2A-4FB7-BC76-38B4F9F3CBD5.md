# TCPIP\_ANNOUNCE\_MESSAGE\_CALLBACK Type

**Parent topic:**[Announce Module](GUID-2561DB16-7947-4C02-97DC-A0BFC17070C9.md)

## C

```
typedef void (* TCPIP_ANNOUNCE_MESSAGE_CALLBACK)(TCPIP_NET_HANDLE hNet, UDP_SOCKET s);
```

## Description

Type: TCPIP\_ANNOUNCE\_MESSAGE\_CALLBACK - Prototype of an announce callback function that can be registered by the user. This callback will be called by the announce module when sending a broadcast message.

## Remarks

The callback function could use regular UDP calls to add its own data to the message. The UDP socket TX buffer has limited space and the standard announce message is in the buffer. The ammount of data that can be pushed is limited by the TCPIP\_ANNOUNCE\_MAX\_PAYLOAD symbol. If the UDP socket TX buffer was not sufficient for storing the internal announce message, then the user part will be skipped.

