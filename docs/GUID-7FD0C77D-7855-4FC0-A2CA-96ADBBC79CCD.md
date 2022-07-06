# TCPIP\_STACK\_HEAP\_INTERNAL\_CONFIG Structure

**Parent topic:**[Manager Module](GUID-B37C4F4C-DC2D-48D9-9909-AACBA987B57A.md)

## C

```
typedef struct {
TCPIP_STACK_HEAP_TYPE heapType;
TCPIP_STACK_HEAP_FLAGS heapFlags;
TCPIP_STACK_HEAP_USAGE heapUsage;
void* (* malloc_fnc)(size_t bytes);
void* (* calloc_fnc)(size_t nElems, size_t elemSize);
void (* free_fnc)(void* ptr);
size_t heapSize;
} TCPIP_STACK_HEAP_INTERNAL_CONFIG;
```

## Description

Internal heap configuration data.

This data type defines the data required to initialize the TCP/IP stack internal heap.

## Preconditions

None.

## Members

|Members|Description|
|-------|-----------|
|heapType|Type of this heap: TCPIP\_STACK\_HEAP\_TYPE\_INTERNAL\_HEAP.|
|heapFlags|Heap creation flags TCPIP\_STACK\_HEAP\_FLAG\_ALLOC\_UNCACHED will be always internally set.|
|heapUsage|Currently not used.|
|\(\* malloc\_fnc\)\(size\_t bytes\)|Malloc style function for allocating the internal heap itself.|
|\(\* calloc\_fnc\)\(size\_t nElems, size\_t elemSize\)|Calloc style function for allocating the internal heap itself.|
|\(\* free\_fnc\)\(void\* ptr\)|Free style function for releasing the allocated internal heap.|
|heapSize|Size of the internal heap to be created and maintained.|

## Remarks

None.

