# TCPIP\_HTTP\_NET\_SSIVariableDelete Function

**Parent topic:**[HTTP Net Module](GUID-4EFEB885-ECF8-44B5-8F23-1D05952E1845.md)

## C

```
bool TCPIP_HTTP_NET_SSIVariableDelete(
    const char* varName
);
```

## Description

This function deletes an SSI variable if it exists.

## Preconditions

The HTTP module should have been initialized. SSI should be enabled.

## Parameters

|Parameters|Description|
|----------|-----------|
|varName|The SSI variable name.|

## Returns

-   True - If the variable existed and was deleted.

-   False - The variable didn't exist.


## Remarks

None.

