# TCPIP\_UDP\_SIGNAL\_TYPE Enumeration

**Parent topic:**[UDP Module](GUID-D2D8E9C8-0778-41E2-8F0B-194954B92250.md)

## C

```
typedef enum { 
    TCPIP_UDP_SIGNAL_TX_DONE = 0x0001, 
    TCPIP_UDP_SIGNAL_RX_DATA = 0x0100, 
    TCPIP_UDP_SIGNAL_IF_DOWN = 0x2000, 
    TCPIP_UDP_SIGNAL_IF_CHANGE = 0x4000 
} TCPIP_UDP_SIGNAL_TYPE; 
```

## Members

|Members|Description|
|-------|-----------|
|TCPIP\_UDP\_SIGNAL\_TX\_DONE = 0x0001|A data packet was successfully transmitted on the interface There may be available buffer space to send new data|
|TCPIP\_UDP\_SIGNAL\_RX\_DATA = 0x0100|A data packet was successfully received and there is data available for this socket|
|TCPIP\_UDP\_SIGNAL\_IF\_DOWN = 0x2000|associated interface is going down; sockets connected on this interface will be disconnected, but still alive|
|TCPIP\_UDP\_SIGNAL\_IF\_CHANGE = 0x4000|associated interface has changed address sockets connected on this interface will be disconnected, but still alive|

## Description

Enumeration: TCPIP\_UDP\_SIGNAL\_TYPE<br />Description of the signals/events that a UDP socket can generate.

## Remarks

These signals are used in the socket event handling notification functions. Currently a UDP notification doesn't set multiple flags as the TX and RX events are handled separately.<br />The signals are 16 bits wide.

