# IPV6\_DATA\_SEGMENT\_HEADER Structure

**Parent topic:**[IPv6 Module](GUID-F2484EF9-7914-43EE-A5B7-4FFDC27C8135.md)

## C

```
typedef struct _IPV6_DATA_SEGMENT_HEADER {
uint8_t* dataLocation;
unsigned short segmentSize;
unsigned short segmentLen;
unsigned char memory;
unsigned char segmentType;
struct _IPV6_DATA_SEGMENT_HEADER * nextSegment;
void * data[];
} IPV6_DATA_SEGMENT_HEADER;
```

## Description

Structure : IPV6\_DATA\_SEGMENT\_HEADER.

Data structure is used to allocate a data segment header and an optional payload.

## Members

|Members|Description|
|-------|-----------|
|dataLocation|Location of the data to transmit.|
|segmentSize|Size of this data segment.|
|segmentLen|Number of bytes of data in this segment.|
|memory|Type: IPV6\_DATA\_NONE, IPV6\_DATA\_DYNAMIC\_BUFFER, IPV6\_DATA\_NETWORK\_FIFO, IPV6\_DATA\_PIC\_RAM.|
|segmentType|Type of segment contents.|
|nextSegment|Pointer to the next data segment.|
|data|Optional buffer space.|

## Remarks

None.

