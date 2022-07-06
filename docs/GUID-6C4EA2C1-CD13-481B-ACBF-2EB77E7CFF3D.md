# TCPIP\_STACK\_NetConfigGet Function

**Parent topic:**[Manager Module](GUID-B37C4F4C-DC2D-48D9-9909-AACBA987B57A.md)

## C

```
size_t TCPIP_STACK_NetConfigGet(
    TCPIP_NET_HANDLE netH, 
    void* configStoreBuff, 
    size_t configStoreSize, 
    size_t* pNeededSize
);
```

## Description

This function dumps the current configuration data of the network interface specified by the corresponding network handle into the supplied buffer.

## Preconditions

The TCP/IP stack should have been initialized by TCPIP\_STACK\_Initialize\(\) and the TCPIP\_STACK\_Status\(\) returned SYS\_STATUS\_READY.

## Parameters

|Parameters|Description|
|----------|-----------|
|netH|The handle that identifies the requested interface.|
|configStoreBuff|Pointer to a buffer that will receive the current configuration data. All the data that's needed to restore a TCPIP\_NETWORK\_CONFIG structure is stored in this buffer. Can be NULL if only the storage size is needed.|
|configStoreSize|Size of the supplied buffer.|
|pNeededSize|Pointer to store the size needed for storage. Can be NULL if not needed.|

## Returns

-   -1 - If the interface is invalid or the stack is not initialized.

-   0 - If no data is copied \(no supplied buffer or buffer too small\).

-   \> 0 - For success, indicating the amount of data copied.


## Remarks

The function is a helper for retrieving the network configuration data. Its companion function, TCPIP\_STACK\_NetConfigSet\(\), restores the TCPIP\_NETWORK\_CONFIG from the dump buffer.

Currently, the data is saved in plain binary format into the supplied buffer. However, the application must not make use of this assumption as it may change in a future release \(some compression scheme may be implemented\).

## Example

```
uint8_t currConfig[200];
size_t neededSize, result;
TCPIP_NET_HANDLE hNet = TCPIP_STACK_NetHandleGet("PIC32INT");
result = TCPIP_STACK_NetConfigGet(hNet, currConfig, sizeof(currConfig), &neededSize);
if(result > 0)
{   
    // store the currConfig to some external storage
}
```

