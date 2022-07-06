# DRV\_ENC28J60\_ConfigGet Function

**Parent topic:**[ENC28J60 Driver Library](GUID-58EA08F2-E38D-48FD-BD75-C2972C0EE761.md)

## C

```
size_t DRV_ENC28J60_ConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize); 
```

## Returns

Number of bytes copied to the buffer

## Description

ENC28J60 Get Configuration Gets the current configuration.

## Remarks

This function does nothing in the first release.

## Preconditions

The client had to be successfully opened with DRV\_ENC28J60\_Open.

## Parameters

|Parameters|Description|
|----------|-----------|
|hMac|the successfully opened handle|
|configBuff|location to copy the configuration too|
|buffSize|buffer size|
|pConfigSize|configuration size needed|
|||

