# TCPIP\_SMTPC\_ATTACH\_FILE Structure

**Parent topic:**[SMTPC Module](GUID-1477C704-4A26-476C-8E70-7514FAF123F4.md)

## C

typedef struct \{<br />TCPIP\_SMTPC\_ATTACH\_TYPE attachType;<br />TCPIP\_SMTPC\_ENCODE\_TYPE attachEncode;<br />const char\* fileName;<br />\} TCPIP\_SMTPC\_ATTACH\_FILE;

## Description

Structure: TCPIP\_SMTPC\_ATTACH\_FILE.

This data structure describes a mail file attachment.

## Members

|Members|Description|
|-------|-----------|
|attachType|Type of the contained data.|
|attachEncode|Preferred encoding.|
|fileName|Name of the file to be opened.|

## Remarks

The specified file will be opened for reading and attached to the email.

If the attach type is application octet stream then the suggested encoding will be overwritten and BASE64 encoding will be used.

The file has to be persistent until the mail completion callback will be called.

If file could not be opened when the mail is sent a warning will be issued and the file will be ignored.

