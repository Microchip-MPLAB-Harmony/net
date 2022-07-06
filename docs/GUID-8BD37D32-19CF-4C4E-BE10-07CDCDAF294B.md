# hostent Structure

**Parent topic:**[Berkeley Module](GUID-5F35C98C-EC8E-40FF-9B62-3B31D508F820.md)

## C

```
struct hostent {
    char * h_name;
    char ** h_alias;
    int h_addrtype;
    int h_length;
    char ** h_addr_list;
};
```

## Description

This is record hostenet

## Members

|Members|Description|
|-------|-----------|
|h\_name|Points to a string containing the name of the host.|
|h\_alias|Points to a null terminated list of pointers that point to the aliases of the host.|
|h\_addrtype|Contains the address type for the host. Currently only AF\_INET is supported.|
|h\_length|Contains the length of the *h\_addr\_list*.|
|h\_addr\_list|Points to a NULL terminated list of pointers that point to the address of the host.|

