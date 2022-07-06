# TCPIP\_TCP\_STATE Enumeration

**Parent topic:**[TCP Module](GUID-9461917B-27CE-44ED-80DB-67D963896E8F.md)

## C

```
typedef enum {
TCPIP_TCP_STATE_LISTEN,
TCPIP_TCP_STATE_SYN_SENT,
TCPIP_TCP_STATE_SYN_RECEIVED,
TCPIP_TCP_STATE_ESTABLISHED,
TCPIP_TCP_STATE_FIN_WAIT_1,
TCPIP_TCP_STATE_FIN_WAIT_2,
TCPIP_TCP_STATE_CLOSING,
TCPIP_TCP_STATE_TIME_WAIT,
TCPIP_TCP_STATE_CLOSE_WAIT,
TCPIP_TCP_STATE_LAST_ACK,
TCPIP_TCP_STATE_CLIENT_WAIT_DISCONNECT,
TCPIP_TCP_STATE_CLIENT_WAIT_CONNECT,
TCPIP_TCP_STATE_KILLED
} TCPIP_TCP_STATE;
```

## Description

Enumeration: TCPIP\_TCP\_STATE.

Enumeration describing the possible states of a TCP socket.

## Members

|Members|Description|
|-------|-----------|
|TCPIP\_TCP\_STATE\_LISTEN|Socket is listening for connections.|
|TCPIP\_TCP\_STATE\_SYN\_SENT|A SYN has been sent, awaiting an SYN+ACK.|
|TCPIP\_TCP\_STATE\_SYN\_RECEIVED|A SYN has been received, awaiting an ACK.|
|TCPIP\_TCP\_STATE\_ESTABLISHED|Socket is connected and connection is established.|
|TCPIP\_TCP\_STATE\_FIN\_WAIT\_1|FIN WAIT state 1.|
|TCPIP\_TCP\_STATE\_FIN\_WAIT\_2|FIN WAIT state 2.|
|TCPIP\_TCP\_STATE\_CLOSING|Socket is closing.|
|TCPIP\_TCP\_STATE\_TIME\_WAIT|2MSL state.|
|TCPIP\_TCP\_STATE\_CLOSE\_WAIT|Waiting to close the socket.|
|TCPIP\_TCP\_STATE\_LAST\_ACK|The final ACK has been sent.|
|TCPIP\_TCP\_STATE\_CLIENT\_WAIT\_DISCONNECT|Client socket lost connection, waiting for app close/disconnect.|
|TCPIP\_TCP\_STATE\_CLIENT\_WAIT\_CONNECT|Client socket waiting for connection.|
|TCPIP\_TCP\_STATE\_KILLED|Socket is killed; debug reporting state.|

