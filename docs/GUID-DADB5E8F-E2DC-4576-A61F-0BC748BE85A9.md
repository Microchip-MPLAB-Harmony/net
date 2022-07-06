# TCPIP\_SMTP\_PutIsDone Function

**Parent topic:**[SMTP Module](GUID-2B9B587D-5018-4CA9-AA8D-2395A2D004A5.md)

## C

```
void TCPIP_SMTP_PutIsDone();
```

## Description

This function indicates that the on-the-fly message is complete.

## Preconditions

TCPIP\_SMTP\_UsageBegin\(\) returned true on a previous call, and the SMTP client is generated an on-the-fly message \(i.e., TCPIP\_SMTP\_MailSend\(\) was called with SMTPClient.Body set to NULL\).

## Returns

None.

## Remarks

None.

