# TCPIP\_SMTPC\_ATTACH\_BUFFER Structure

**Parent topic:**[SMTPC Module](GUID-1477C704-4A26-476C-8E70-7514FAF123F4.md)

## C

typedef struct \{<br />TCPIP\_SMTPC\_ATTACH\_TYPE attachType;<br />TCPIP\_SMTPC\_ENCODE\_TYPE attachEncode;<br />const char\* attachName;<br />const uint8\_t\* attachBuffer;<br />size\_t attachSize;<br />\} TCPIP\_SMTPC\_ATTACH\_BUFFER;

## Description

Structure: TCPIP\_SMTPC\_ATTACH\_BUFFER

This data structure describes a mail buffer attachment.

## Members

|Members|Description|
|-------|-----------|
|attachType|Type of the contained data.|
|attachEncode|Preferred encoding.|
|attachName|File name to be used for the attachment.|
|attachBuffer|Buffer containing the data.|
|attachSize|Buffer size.|

## Remarks

A buffer attachment is attached to the email just like a file. But data is taken from the persistent user buffer rather than from a file.

If the attach type is application octet stream then the suggested encoding will be overwritten and BASE64 encoding will be used.

The user supplied buffer has to be persistent until the mail completion callback will be called.

