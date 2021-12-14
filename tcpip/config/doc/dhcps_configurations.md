# Dynamic Host Configuration Protocol (DHCPS)  Configurations

**DHCPS Specific User Configurations**

- **Maximum Number of Entries in the Lease Table:**
    - The Maximum Number of entries in the lease table
    - Default total number of entries for all the the interface
    - Default value: 15
    - Symbol TCPIP_DHCPS_LEASE_ENTRIES_DEFAULT


- **Number of DHCP Server Instances:**
    - Number of interfaces the DHCP server to run on
    - Default value: 1
    - Symbol TCPIP_DHCP_SERVER_INSTANCES_NUMBER

- **DHCPS Address Range Start:**
    - Start of IP address Range , network_config.h ipaddress and this start of IP address should be in same SUBNET
    - RECOMENDED - network_config.h ipaddress should be 192.168.1.1 if DHCP server ip address range starts from 192.168.1.100.
    - Default value: "192.168.1.100"
    - Symbol TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START

- **DHCPS Server IP Address:**
    - DHCP server Address per interface. DHCP server Address selection should  be in the same subnet.
    - Default value: "192.168.1.1"
    - Symbol TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS

- **DHCPS Netmask:**
    - The DHCP server network mask 
    - Default value: "254.255.255.0"
    - Symbol TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS

- **Default Gateway:**
    - DHCP server gateway sddress per interface.
    - Default value: "192.168.1.1"
    - Symbol TCPIP_DHCPS_DEFAULT_SERVER_GATEWAY_ADDRESS

- **Primary DNS Server Address:**
    - DHCP server  DNS primary Address
    - Default value: "192.168.1.1"
    - Symbol TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS

- **Secondary DNS Server Address:**
    - DHCP server  DNS secondary Address
    - Default value: "192.168.1.1"
    - Symbol TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS

- **DHCP Server Task Rate (in msec):**
    - DHCPS task processing rate, in milliseconds.
    - The DHCPS module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - Choose it so that the other TMO are multiple of this
    -  The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 200
    - Symbol TCPIP_DHCPS_TASK_PROCESS_RATE

- **Timeout for a Solved Entry in the Cache (in sec):**
    - Timeout for a solved entry in the cache
    - The entry will be removed if the TMO lapsed and the entry has not been referenced again
    - Value is in seconds
    - Default value: (20 * 60)
    - Symbol TCPIP_DHCPS_LEASE_SOLVED_ENTRY_TMO

- **Timeout for an Unsolved Entry (in sec):**
    - Timeout for a unsolved entry
    - An entry should be removed from the cache if there is no REQUEST after OFFER 
    - Value is in seconds
    - Default value: 5
    - Symbol TCPIP_DHCPS_LEASE_REMOVED_BEFORE_ACK

- **Delete Old Entries:**
    - Delete the existing cached entries at stack/interface restart
    - Default value: true
    - Symbol TCPIP_DHCP_SERVER_DELETE_OLD_ENTRIES



