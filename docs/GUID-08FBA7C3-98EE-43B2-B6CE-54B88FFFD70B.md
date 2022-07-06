# NET\_PRES\_TransRead Type

**Parent topic:**[Networking Presentation Layer](GUID-75470E5B-2289-4F94-AE85-2BB7DF4C4F07.md)

## C

```
typedef uint16_t (* NET_PRES_TransRead)(uintptr_t transHandle, uint8_t* buffer, uint16_t count); 
```

## Returns

The number of data bytes copied by the transport channel into the buffer.

## Description

Presentation Layer Transport Layer Read Function Pointer Prototype This function prototype is used to define the function that the Networking Presentation Layer will pass to the provider when it is initialized. The provider will use this function when it needs to read from the transport layer.

## Preconditions

None.

## Parameters

|Parameters|Description|
|----------|-----------|
|transHandle|This is the transport layer handle provided by the transport layer when a communications channel is open.|
|buffer|This is a pointer to the buffer that the transport layer will copy data to.|
|count|This is the size of the buffer.|

