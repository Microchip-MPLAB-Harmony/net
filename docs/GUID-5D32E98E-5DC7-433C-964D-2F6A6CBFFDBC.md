# gethostname Function

**Parent topic:**[Berkeley Module](GUID-5F35C98C-EC8E-40FF-9B62-3B31D508F820.md)

## C

```
int gethostname(
    char* name, 
    int namelen
);
```

## Description

This function returns the standard host name of the system which is calling this function. The returned name is null-terminated.

## Preconditions

None.

## Parameters

\|Parameters \|Description \|<br />\|name \|Pointer to a buffer that receives the local host name. \|<br />\|namelen \|Size of the name array. \|

## Returns

-   0 - The function was successful.

-   SOCKET\_ERROR \(-1\) - Name is too short to hold the host name or any other error occurs. *errno* is set accordingly and \*name will be unmodified and no null terminator will be generated.


## Remarks

The function returns the host name as set on the default network interface.

