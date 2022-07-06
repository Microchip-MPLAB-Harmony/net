# NET\_PRES\_EncProviderRead Type

**Parent topic:**[Networking Presentation Layer](GUID-75470E5B-2289-4F94-AE85-2BB7DF4C4F07.md)

## C

```
typedef int32_t (* NET_PRES_EncProviderRead)(void * providerData, uint8_t * buffer, uint16_t size); 
```

## Returns

The number of bytes transferred.

## Description

Presentation Encryption Provider Read Function Pointer Prototype This function pointer defines the read function. It is called by the presentation layer when the presentation client wants to read from a secured connection.

## Remarks

If the supplied buffer is NULL the operation is ignored.

## Preconditions

A connection must have already been created, and be in the open state.

## Parameters

|Parameters|Description|
|----------|-----------|
|providerData|A pointer to the buffer for the provider to keep connection specific data.|
|buffer|A pointer to the buffer that will be read from the provider.|
|count|Size of the buffer.|

