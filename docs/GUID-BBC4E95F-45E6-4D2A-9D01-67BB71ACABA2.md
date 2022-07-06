# TCPIP\_SMTP\_IsBusy Function

**Parent topic:**[SMTP Module](GUID-2B9B587D-5018-4CA9-AA8D-2395A2D004A5.md)

## C

```
bool TCPIP_SMTP_IsBusy();
```

## Description

Call this function to determine if the SMTP client is busy performing background tasks. This function should be called after any call to TCPIP\_SMTP\_MailSend\(\), TCPIP\_SMTP\_PutIsDone\(\) to determine if the stack has finished performing its internal tasks. It should also be called prior to any call to TCPIP\_SMTP\_IsPutReady\(\) to verify that the SMTP client has not prematurely disconnected. When this function returns false, the next call should be to TCPIP\_SMTP\_UsageEnd\(\) to release the module and obtain the status code for the operation.

