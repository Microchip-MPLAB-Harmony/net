# TCPIP\_ANNOUNCE\_CallbackRegister Function

**Parent topic:**[Announce Module](GUID-2561DB16-7947-4C02-97DC-A0BFC17070C9.md)

## C

```
bool TCPIP_ANNOUNCE_CallbackRegister(
    TCPIP_ANNOUNCE_MESSAGE_CALLBACK callback
);
```

## Description

This function registers an application callback. The callback will be called when the Announce module needs to send a message. The user can insert its own message as part of a standard announce message.

## Preconditions

The Announce module should have been initialized.

## Parameters

|Parameters|Description|
|----------|-----------|
|callback|user function to becalled by the announce module. If callback == 0 then any previous callback will be erased.|

## Returns

-   true - if the call succeeded.

-   false - if failed; module is not up, for example.


## Remarks

Only one callback is supported currently. To delete the callback, simply call the register function with callback = 0. the callback function could use regular UDP calls to add its own data to the message. If the UDP socket TX buffer was not sufficient for storing the internal announce message, then the user part will be skipped.

