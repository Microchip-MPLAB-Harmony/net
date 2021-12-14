# DHCP Server V2 Configurations

**DHCP Server V2 Specific User Configurations**



- **Maximum Number Leases per Interface:**
    - Specifies the maximum number of leases that the DHCP server can support for an interface
    - The actual number of leases per interface could be smaller than this
    - Multiple of 32
    - Default value 32 
    - Symbol TCPIP_DHCPS_MAX_LEASES 


- **Number of ICMP Echo Probes:**
    - Number of ICMP echo probes to send when doing address conflict detection
    - Default value 1 


- **Number of Conflict Attempts for new IP Address:**
    - Number of attempts to find a new IP address when ICMP detected an address conflict
    - Default value 1

- **Number of Leases:**
    - Maximum number of leases for this interface
    - Must be <= TCPIP_DHCPS_MAX_LEASES 

- **Default Lease Duration(in Sec):**
    - Default lease duration to be granted to clients, seconds
    - Default value 28800 seconds (8 hours)    

- **DHCPS Server IP Address:**
    - Server IP address on this interface

- **Lease IP Address Starting from:**
    - Start lease IP address
    - The pool of addresses will be [startIPAddress, endIPAddress]
    - where endIPAddress is given by the prefixLen 
    
- **Prefix Length (CIDR notation):**
    - The number of the leading 1 bits in the network mask
    - number <= 32
    - the address is specified in the CIDR format: add/prefix_len
    - the maximum size of the pool is dictated by the prefixLen
    - For example: 192.168.0.0/24 will have 254 leases available 
    - (192.168.0.0 and 192.168.0.255 are excluded)
    - and endIPAddress == 192.168.0.254
    - Note: the number of possible leases == min(leaseEntries, endIPAddress - startIPAddress).
    - The number of leases derived from startIPAddress/prefixLen should be > leaseEntries.  

- **Client Options:**
    - DHCP server configuration of provided client options
    - Lists the options to be presented to clients requiring an address lease

    - **Router IP Address:**
        - Provide router/gateway option

    - **DNS IP Address:**
        -  Provide DNS option

    - **Time Server IP Address:**
        - Provide time server option

    - **Name Server IP Address:**
        - Provide name server option

    - **NTP Server IP Address:**
        - Provide NTP server option


- **Configuration Flags:**
    - **Disabled at Start of Interface:**
        - DHCP server should start disabled on this interface
        - Note: this setting is currently not used and overridden by the
        - TCPIP_NETWORK_CONFIG:: startFlags.TCPIP_NETWORK_CONFIG_DHCP_SERVER_ON

    - **Delete Lease Info on Restart:**
        - DHCP server should delete the existent info for leases
        - when the interface is restarted 

    - **Disable Conflict Detection:**
        - Disable DHCP server conflict detection of newly allocated addresses
        - By default the server should send an ICMP echo request when using a new IP address 

    - **Disable Lease Extension Request:**
        - Disable DHCP clients request for extending the lease
        - By default the server grants lease extension to the clients

    - **Keep Info of Not Requested Offers:**
        - Keep DB info for offers that have not been requested by the client
        - i.e. when client selected another server (multi-server environment)
        - By default the info for these clients is deleted to free some space in the DB 

    - **ICMP Probe Failure Abort:**
        - When DHCP server conflict detection is enabled, if sending echo requests failed
        - (ICMP module rejected the calls, TCPIP_DHCPS_EVENT_ECHO_FAIL event)
        - abort offering the lease to the client.
        - By default the server will send the offer to the client 

    - **Keep Silent if No Record:**
        - Keep silent when receiving a client request for which there is no previous record
        - (i.e. do not verify the requested IP address and send NAK, etc.)
        - By default the server will reply with NAK if the requested lease is invalid 

- **Minimum Lease Duration(in Sec):**
    -  Minimum lease duration that can be requested by a client, seconds 
    - If 0, then a default of 60 seconds will be used
    - Should be <= leaseDuration

- **Maximum Lease Duration(in Sec):**
    - Maximum lease duration that can be requested by a client, seconds 
    - If 0, then the leaseDuration will be used
    - Should be >= leaseDuration 

- **Unrequested Offer Timeout(in Sec):**
    - Timeout for unrequested offer sent to the client, seconds
    - Note: used if TCPIP_DHCPS_CONFIG_FLAG_KEEP_UNREQ_OFFERS is not set 
    - If 0, a default of 5 seconds will be used 

- **T1 Renewal Time Calculation:**
    - Timeout used for TCPIP_DHCPS_CLIENT_OPTION_T1_RENEWAL 

    - **Multiplication/Division Factors:**
        - The multiplication/division factors of T1 
        - The T1 is obtained as percentages of lease time:
        - T1 = (leaseTime * mult) / div;   default values are mult = 1, div = 2;
        - Default values are used for mult or div if any of them are 0 
        - T2 needs to be greater than T1! 


- **T2 Rebind Time Calculation:**
    - Timeout used for TCPIP_DHCPS_CLIENT_OPTION_T2_REBINDING 

    - **Multiplication/Division Factors:**
        - The multiplication/division factors of T2 
        - The T2 is obtained as percentages of lease time:
        - T2 = (leaseTime * mult) / div;   default values are mult = 7, div = 8; 
        - Default values are used for mult or div if any of them are 0 
        - T2 needs to be greater than T1! 

- **DHCP Server Interface Count:**
    - Maximum number of interfaces on which DHCPs could run
    - Note that this could be less than the maximum number of interfaces in the stack for this build (TCPIP_STACK_NETWORK_INTERAFCE_COUNT)!
    - Symbol: TCPIP_DHCPS_INTERFACE_COUNT


- **DHCP Server Task Rate (in msec):**
    - DHCPS task processing rate, in milliseconds.
    - The DHCPS module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - Choose it so that the other TMO are multiple of this
    - The default value is 200 milliseconds.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.

- **Client ID Maximum Size:**
    - Maximum size of the client identifier field
    - Needs to be >= size of a MAC Address
    - Note: when a client is requesting a lease using a longer client ID the ID will be truncated! 

- **ICMP Echo Request Buffer Size:**
    - Size of the buffer when sending a ICMP echo request for address conflict detection
    - Should be between 8 and 32 bytes 
    - Default value is 16

- **Number of ICMP Request Retries:**
    - Number of retries if ICMP is not able to send a ping
    - This should only happen if there are too many ICMP echo requests ongoing 
    - Default value is 2

- **Maximum Number of Router Value Options:**
    - Maximum number of values for the options to be sent to clients
    - A value defined as 0 excludes that option 

- **Maximum Number of DNS Value Options:**
    - Maximum number of values for the options to be sent to clients
    - A value defined as 0 excludes that option 

- **Maximum Number of Time Server Value Options:**
    - Maximum number of values for the options to be sent to clients
    - A value defined as 0 excludes that option 

- **Maximum Number of Name Server Value Options:**
    - Maximum number of values for the options to be sent to clients
    - A value defined as 0 excludes that option 

- **Maximum Number of NTP Server Value Options:**
    - Maximum number of values for the options to be sent to clients
    - A value defined as 0 excludes that option 

- **Suppress T1 Renewal and T2 Rebind:**
    - Suppress the 'renewal T1' and 'rebinding T2' options when replying to a client request
    - By default these options are enabled 

- **Maximum Number of Event Registrations:**
    - Maximum number of event registrations
    - If 0 or not defined, there is no event registration


- **Enable Error Events Report:**
    - Enable the report of error events using the event notification mechanism

- **Enable Client Events Report:**
    - Enable the report of client events using the event notification mechanism 

- **Enable Statistics:**
    - Maintain DHCPs statistics counts
    - Symbol TCPIP_DHCPS_ENABLE_STATISTICS

- **Enable Dynamic DB Manipulation:**
    - Allow dynamic manipulation of the DHCPs DB 
    - Symbol TCPIP_DHCPS_DYNAMIC_DB_ACCESS   

- **Enable Multi-Threaded Access:**
    - Enable multi-threaded access
    - Symbol TCPIP_DHCPS_MULTI_THREADED_ACCESS   

- **DHCP Server Heap Size (bytes):**
    - An estimation of the heap required by the DHCP server

