# NET\_PRES\_EncProviderIsInitialized Type

**Parent topic:**[Networking Presentation Layer](GUID-75470E5B-2289-4F94-AE85-2BB7DF4C4F07.md)

## C

```
typedef bool (* NET_PRES_EncProviderIsInitialized)(void); 
```

## Returns

-   true - The provider has been initialized

-   false - The provider has not been initialized


## Description

Presentation Encryption Provider Is Initialized Pointer Prototype<br />This function pointer determines whether the encryption provider has been initialized and informs the presentation layer.

## Preconditions

A connection must have already been created, and be in the open state.

