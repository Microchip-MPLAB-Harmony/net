# UDP\_OPTION\_MULTICAST\_DATA Structure

**Parent topic:**[UDP Module](GUID-D2D8E9C8-0778-41E2-8F0B-194954B92250.md)

## C

```
typedef struct { 
    UDP_MULTICAST_FLAGS flagsMask; 
    UDP_MULTICAST_FLAGS flagsValue; 
} UDP_OPTION_MULTICAST_DATA; 
```

## Members

|Members|Description|
|-------|-----------|
|UDP\_MULTICAST\_FLAGS flagsMask;|mask of flags to be touched \(changed\) by the operation|
|UDP\_MULTICAST\_FLAGS flagsValue;|new value of flags to be changed to, for the flags that are touched, according to the mask|

## Description

Structure: UDP\_OPTION\_MULTICAST\_DATA<br />Allows the multicast options configuration of a socket.

## Remarks

Using the mask and value members, multiple flags can be set and cleared in one single operations. The TCPIP\_UDP\_OptionsSet\(\) operation for UDP\_OPTION\_MULTICAST will apply the folowing to the socket flags: flags &= ~flagsMask; // clear all flags touched by mask flags \|= \(flagsValue & flagsMask\); // set the corresponding flags that are set in flagsValue. All others will remain cleared.

## Example

```
// set the flag UDP_MCAST_FLAG_LOOP: 
UDP_OPTION_MULTICAST_DATA mcastData; 
mcastData.flagsMask = UDP_MCAST_FLAG_LOOP; // specify the flag to touch
mcastData.flagsValue = UDP_MCAST_FLAG_LOOP; // new value for the flag: 
                                  

TCPIP_UDP_OptionsSet(hUdp, UDP_OPTION_MULTICAST, &mcastData); 

// clear the flag UDP_MCAST_FLAG_LOOP: 
mcastData.flagsMask = UDP_MCAST_FLAG_LOOP; // specify the flag to touch 
mcastData.flagsValue = 0; // new value for the flag: 

TCPIP_UDP_OptionsSet(hUdp, UDP_OPTION_MULTICAST, &mcastData); 

// set UDP_MCAST_FLAG_LOOP and UDP_MCAST_FLAG_DISABLE_SOURCE_CHECK
// clear UDP_MCAST_FLAG_IGNORE_SOURCE_ADD and UDP_MCAST_FLAG_IGNORE_SOURCE_PORT 
mcastData.flagsMask = UDP_MCAST_FLAG_LOOP | UDP_MCAST_FLAG_DISABLE_SOURCE_CHECK | UDP_MCAST_FLAG_IGNORE_SOURCE_ADD | UDP_MCAST_FLAG_IGNORE_SOURCE_PORT; // flags to touch mcastData.flagsValue = UDP_MCAST_FLAG_LOOP | UDP_MCAST_FLAG_DISABLE_SOURCE_CHECK ; // value with bits set for the first 2 flags, cleared for the others 

TCPIP_UDP_OptionsSet(hUdp, UDP_OPTION_MULTICAST, &mcastData); 
```

