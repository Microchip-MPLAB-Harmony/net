# TCPIP\_SMTPC\_MAIL\_FLAGS Enumeration

**Parent topic:**[SMTPC Module](GUID-1477C704-4A26-476C-8E70-7514FAF123F4.md)

## C

```
typedef enum {
TCPIP_SMTPC_MAIL_FLAG_USE_IPV6 = 0x0001,
TCPIP_SMTPC_MAIL_FLAG_BODY_TYPE_BINARY = 0x0002,
TCPIP_SMTPC_MAIL_FLAG_BODY_ENCODE_ASCII = 0x0004,
TCPIP_SMTPC_MAIL_FLAG_CONNECT_TLS = 0x0010,
TCPIP_SMTPC_MAIL_FLAG_SKIP_TLS = 0x0020,
TCPIP_SMTPC_MAIL_FLAG_FORCE_TLS = 0x0040,
TCPIP_SMTPC_MAIL_FLAG_AUTH_PLAIN = 0x0100,
TCPIP_SMTPC_MAIL_FLAG_FORCE_AUTH = 0x0200,
TCPIP_SMTPC_MAIL_FLAG_GREET_HELO = 0x0400
} TCPIP_SMTPC_MAIL_FLAGS;
```

## Description

Enumeration: TCPIP\_SMTPC\_MAIL\_FLAGS.

Enumeration describing the possible mail message flags supported by the SMTPC client.

## Members

|Members|Description|
|-------|-----------|
|TCPIP\_SMTPC\_MAIL\_FLAG\_USE\_IPV6 = 0x0001|Use IPv6 connection to the server, rather than default IPv4.|
|TCPIP\_SMTPC\_MAIL\_FLAG\_BODY\_TYPE\_BINARY = 0x0002|Mail body is binary rather then standard plain text.|
|TCPIP\_SMTPC\_MAIL\_FLAG\_BODY\_ENCODE\_ASCII = 0x0004|Send mail body as plain ASCII encoding rather than default BASE64.|
|TCPIP\_SMTPC\_MAIL\_FLAG\_CONNECT\_TLS = 0x0010|Connect with TLS from the very beginning This flag is useful if the server accepts encrypted connections on port 465, for example Note that port 465 is not really a SMTP port, although supported by many servers.|
|TCPIP\_SMTPC\_MAIL\_FLAG\_SKIP\_TLS = 0x0020|Skip TLS even if the server supports it most servers won't accept mail if TLS is skipped though.|
|TCPIP\_SMTPC\_MAIL\_FLAG\_FORCE\_TLS = 0x0040|Start TLS even if the server does not indicate support for it.|
|TCPIP\_SMTPC\_MAIL\_FLAG\_AUTH\_PLAIN = 0x0100|Favor PLAIN authentication, if supported by default the LOGIN authentication is selected.|
|TCPIP\_SMTPC\_MAIL\_FLAG\_FORCE\_AUTH = 0x0200|Force authentication even if server doesn't specifically request one by default the authentication is skipped.|
|TCPIP\_SMTPC\_MAIL\_FLAG\_GREET\_HELO = 0x0400|Use old 'HELO' greeting insted of extended 'EHLO' which is default.|

## Remarks

Multiple flags can be set.

Only 16 bit flags supported.

If no flags are set, then the default settings are used for the mail message.

Normally the mail body is plain text. Using these flags, a binary mail body can be sent.

Normally the mail body is plain text so a standard ASCII encoding is used. Using these flags, a BASE64 encoding for the mail body can be selected. Note that if the mail body is selected to be binary then the BASE64 encoding is automatically selected.

BASE64 encoding has the advantage that the mail body lines are not limited in length and any sequence of characters can be present in the email body without the need of the SMTPC to search and replace the SMTP forbidden characters.

Normally the connection to the server occurs on a standard SMTP port: 25 or \(preferred\) 587 as a plain/non-encrypted connection. Once the server replies with TLS support, the encryption is started. Using the flags you can force the connection to be TLS from the very beginning. Or you can avoid engaging in TLS even if the server supports it. Or try to start TLS even if the server didn't advertise support for it.

