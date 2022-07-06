# TCPIP\_HTTP\_NET\_DynamicFileInclude Function

**Parent topic:**[HTTP Net Module](GUID-4EFEB885-ECF8-44B5-8F23-1D05952E1845.md)

## C

```
bool TCPIP_HTTP_NET_DynamicFileInclude(
    TCPIP_HTTP_NET_CONN_HANDLE connHandle, 
    const TCPIP_HTTP_DYN_VAR_DCPT* varDcpt, 
    const char* fileName
);
```

## Description

This function allows an entire file to be included as part of a dynamic variable processing. This reduces unneeded duplication of visual elements such as headers, menus, etc.

This function is NOT meant for the processing of the "inc" keyword in the web pages \(see the Remarks section below\).

## Preconditions

None.

## Parameters

|Parameters|Description|
|----------|-----------|
|connHandle|HTTP connection handle.|
|fileName|The name of the file to be included.|

## Returns

-   True - if the call succeeded.

-   False - if the file could not be opened.


## Remarks

The function allows to insert a file, as part of the dynamic variable processing. Should be called ONLY from within a dynamic variable processing function context \(i.e. from within a dynamicPrint\(\) function\). The *connHandle* and *varDcpt* need to be valid handles.

The included file can contain dynamic variables. However there's a limit for recursive calls \(see TCPIP\_HTTP\_NET\_MAX\_RECURSE\_LEVEL\(\)\).

The file is just added to the processing queue. Returning true does not mean that the whole file has been already sent to the connection.

If the function returns true but an error occurs during the file processing an event will be reported using the template\_EventReport\(\) callback.

If the function returns false an event will be reported using the template\_EventReport\(\) callback with additional info.

Please note that the processing of HTTP dynamic keywords in the HTML code such as ~inc:filename.ext~ is processed internally by the HTTP module. For such a dynamic variable, control is not passed to the user.

