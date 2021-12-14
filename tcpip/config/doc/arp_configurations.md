# Address Resolution Protocol (ARP) Configurations 

**ARP Specific User Configurations**

- **Number of Cache Entries:**
    - Default number of entries per interface.
    - Default value 5
    - Symbol TCPIP_ARP_CACHE_ENTRIES


- **Enable ARP Commands:**
    - Enable the ARP TCP/IP Console Commands
    - Default value: false
    - Symbol: TCPIP_ARP_COMMANDS_ENABLE


- **Delete Old Cache:**
    - On initialization, delete the old cache if still in place
    - Else do not re-initialize
    - Default value: 1
    - Symbol TCPIP_ARP_CACHE_DELETE_OLD

- **Time-Out for removal of Solved Entry (in sec):**
    - Timeout for a solved entry in the cache.
    - The entry will be removed if the tmo elapsed and the entry has not been referenced again
    - Value is in seconds
    - Default value: (20 * 60)
    - Symbol TCPIP_ARP_CACHE_SOLVED_ENTRY_TMO


- **Time-Out to solve Pending Cache Entry (in sec):**
    - Timeout for a cache entry pending to be solved
    - The entry will be removed if the tmo elapsed and the entry has not been solved.
    - A solved entry moves to the solved entries timeout.
    - Value is in seconds
    - Default value: (1 * 60)
    - Symbol TCPIP_ARP_CACHE_PENDING_ENTRY_TMO


- **Time-Out for Resending an ARP Request for Pending Entry (in sec):**
    - Timeout for resending an ARP request for a pending entry
    - In order to prevent the ARP flooding the standard recommends it to be greater than 1 sec.
    - It should be less than TCPIP_ARP_CACHE_PENDING_ENTRY_TMO  
    - Value is in seconds
    - Default value: 2
    - Symbol TCPIP_ARP_CACHE_PENDING_RETRY_TMO

    
- **Number of Retries for Resolving an Entry:**
    - Number of ARP requests generated for resolving an entry.
    - Default value: 3
    - Symbol TCPIP_ARP_CACHE_ENTRY_RETRIES


- **Number of Retries for Gratuitous ARP Request:**
    - Number of ARP requests generated when sending a gratuitous ARP probe.
    - Default value: 1
    - Symbol TCPIP_ARP_GRATUITOUS_PROBE_COUNT


- **Purge Threshold Percentage:**
    - Default purge threshold, percentage
    - Once the number of resolved entries in the cache gets beyond the threshold some resolved entries will be purged.
    - Default value: 75
    - Symbol TCPIP_ARP_CACHE_PURGE_THRESHOLD

- **Number of Entries to Delete Once the Threshold is Reached:**
    - The number of entries to delete, once the threshold is reached.
    - Default value: 3
    - Symbol TCPIP_ARP_CACHE_PURGE_QUANTA


- **Maximum Percentage of Permanent Entries Allowed in Cache:**
    - Maximum percentage of permanent entries in the cache.
    - Note that since permanent entries cannot be removed they tend to degrade the efficiency of the cache look up.  
    - Default value: 50
    - Symbol TCPIP_ARP_CACHE_PERMANENT_QUOTA


- **Alias Interfaces Share One Cache:**
    - ARP caches configuration for aliases.
    - If this symbol is true, then the alias interfaces share the cache with the primary interface.
    - Otherwise each interface uses its own cache.
    - Default value should be 1 if the alias interfaces are in the same network with the primary interface
    - Symbol TCPIP_ARP_PRIMARY_CACHE_ONLY


- **ARP Task Rate (in msec):**
    - ARP task processing rate, in milliseconds.
    - The ARP module will process a timer event with this rate  for maintaining its own queues, processing timeouts, etc.
    - Choose it so that the other ARP_CACHE_xxx_TMO are multiple of this
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 2000
    - Symbol TCPIP_ARP_TASK_PROCESS_RATE





















