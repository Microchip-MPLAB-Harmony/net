# TCPIP\_STACK\_HEAP\_EXTERNAL\_CONFIG Structure

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
} TCPIP_STACK_HEAP_EXTERNAL_CONFIG;
```

## Description

External heap configuration data.

This data type defines the data required to intialize the TCP/IP stack external heap.

## Members

|Members|Description|
|-------|-----------|
|heapType|Type of this heap: TCPIP\_STACK\_HEAP\_TYPE\_EXTERNAL\_HEAP.|
|heapFlags|Heap creation flags. TCPIP\_STACK\_HEAP\_FLAG\_ALLOC\_UNCACHED will be always internally set. TCPIP\_STACK\_HEAP\_FLAG\_NO\_MTHREAD\_SYNC should be cleared if the external heap functions provide multi-threaded synchronization. Should be set if the synchronization needs to be provided by the TCP/IP heap implementation. TCPIP\_STACK\_HEAP\_FLAG\_ALLOC\_UNALIGN should be set if the external heap functions provide cache line aligned buffers.|
|heapUsage|Currently not used.|
|\(\* malloc\_fnc\)\(size\_t bytes\)|Malloc style function to be called when allocation is needed.|
|\(\* calloc\_fnc\)\(size\_t nElems, size\_t elemSize\)|Calloc style function to be called when allocation is needed.|
|\(\* free\_fnc\)\(void\* ptr\)|Free style function for releasing the allocated memory.|

## Remarks

Due to the internal alignment restrictions the externally supplied calloc function is not currently used. Instead, a less efficient approach is taken: the malloc function is used and the resulted buffer is zeroed out.

