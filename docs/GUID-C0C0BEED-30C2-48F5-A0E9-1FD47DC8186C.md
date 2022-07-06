# TCPIP\_DHCP\_INFO Structure

**Parent topic:**[DHCP Client Module](GUID-9356D53B-4F42-4E1E-B051-90F7C4D448E6.md)

## C

```
typedef struct {
TCPIP_DHCP_STATUS status;
uint32_t dhcpTime;
uint32_t leaseStartTime;
uint32_t leaseDuration;
uint32_t renewTime;
uint32_t rebindTime;
IPV4_ADDR dhcpAddress;
IPV4_ADDR subnetMask;
IPV4_ADDR serverAddress;
const char* bootFileName;
uint8_t timeServersNo;
uint8_t ntpServersNo;
const IPV4_ADDR* timeServers;
const IPV4_ADDR* ntpServers;
} TCPIP_DHCP_INFO;
```

## Description

Structure: TCPIP\_DHCP\_INFO

This data structure is used for reporting current info and status of the DHCP module. Used in getting info about the DHCP module.

## Members

|Members|Description|
|-------|-----------|
|status|Current status.|
|dhcpTime|current DHCP time, seconds the following fields are significant only if a lease has been obtained and is currently valid i.e. status \>= TCPIP\_DHCP\_BOUND.|
|leaseStartTime|Time when lease was requested.|
|leaseDuration|Lease duration as returned by the server, in seconds.|
|renewTime|The time for moving to renew state, in seconds.|
|rebindTime|The time for moving to rebind state, in seconds.|
|dhcpAddress|IPv4 address obtained by DHCP.|
|subnetMask|IPv4 sub net mask obtained by DHCP.|
|serverAddress|IPv4 address of the server that granted the lease.|
|bootFileName|Pointer to the bootfile name that was returned by the serve.r This will be 0 if TCPIP\_DHCP\_STORE\_BOOT\_FILE\_NAME option is not enabled.|
|timeServersNo|Number of time servers available. This will be 0 if TCPIP\_DHCP\_USE\_OPTION\_TIME\_SERVER option is not enabled.|
|ntpServersNo|Number of NTP servers available. This will be 0 if TCPIP\_DHCP\_USE\_OPTION\_NTP\_SERVER option is not enabled.|
|timeServers|Pointer to array of addresses for the time servers. Size is given by *timeServersNo*.|
|ntpServers|Pointer to array of addresses for the NTP servers. Size is given by *ntpServersNo*.|

