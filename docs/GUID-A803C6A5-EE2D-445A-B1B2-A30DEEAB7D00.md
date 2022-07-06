# TCPIP\_SMTP\_Flush Function

**Parent topic:**[SMTP Module](GUID-2B9B587D-5018-4CA9-AA8D-2395A2D004A5.md)

## C

```
void TCPIP_SMTP_Flush();
```

## Description

This function flushes the SMTP socket and forces all data to be sent.

## Preconditions

TCPIP\_SMTP\_UsageBegin\(\) returned true on a previous call.

## Returns

None.

## Remarks

This function should only be called externally when the SMTP client is generating an on-the-fly message \(i.e., TCPIP\_SMTP\_MailSend\(\) was called with SMTPClient.Body set to NULL\).

