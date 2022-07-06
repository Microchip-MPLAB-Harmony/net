# TCPIP\_HTTP\_NET\_DynamicWriteString Function

**Parent topic:**[HTTP Net Module](GUID-4EFEB885-ECF8-44B5-8F23-1D05952E1845.md)

## C

```
bool TCPIP_HTTP_NET_DynamicWriteString(
    const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, 
    const char* str, 
    bool needAck
);
```

## Description

This function takes a 0 terminated ASCII string and sends it to the HTTP connection as part of the HTTP dynamic variable processing.

## Preconditions

varDcpt - a valid dynamic variable descriptor.

## Parameters

|Parameters|Description|
|----------|-----------|
|varDcpt|Dynamic variable descriptor as passed in the template\_DynPrint\(\) function.|
|str|The string to be written.|
|needAck|If true, once the buffer is processed internally, template\_DynAcknowledge\(\) will be called.|

## Returns

-   True - Data buffer has been queued for output.

-   False - An invalid buffer was provided or the buffer could not be queued because of lack of resources \(descriptors\).


## Remarks

This is just a helper. The actual function called is still TCPIP\_HTTP\_NET\_DynamicWrite\(\). That means that the supplied string has to be persistent. See the remarks for TCPIP\_HTTP\_NET\_DynamicWrite\(\).

