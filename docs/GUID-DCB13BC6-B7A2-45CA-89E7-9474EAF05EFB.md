# IGMP Module

The IGMP module implements the IGMPv3 protocol. This protocol is used by hosts and routers to establish multicast group memberships.

**Abstraction Model**

IGMP Software Abstraction Block Diagram

![TCPIP IGMP Abstraction Model](GUID-D20219EB-3633-4BCB-80FB-4DBC166D34C6-low.png)

**Library Interface**

|Name|Description|
|----|-----------|
|**Macros**||
|*IGMP\_CONFIG\_H*|This is macro *IGMP\_CONFIG\_H*.|
|TCPIP\_IGMP\_INTERFACES|the maximum number of interfaces that support IGMP Should be <= interfaces that are present at the stack initialization|
|TCPIP\_IGMP\_MCAST\_GROUPS|maximum number of multicast groups \(addresses\) for which a IGMP application can subscribe Each group can listen to multiple sources Should be prime number for best performance|
|TCPIP\_IGMP\_ROBUSTNESS\_VARIABLE|The default Robustness Variable value The Robustness Variable allows tuning for the expected packet loss on a network. If a network is expected to be lossy, the Robustness Variable may be increased. IGMP is robust to \(Robustness Variable - 1\) packet losses. RFC 3376 IGMPv3: The Robustness Variable MUST NOT be zero, and SHOULD NOT be one. Default: 2 This is just a default value. The actual value is taken from a router query.|
|TCPIP\_IGMP\_SOCKET\_RECORDS\_PER\_SOURCE|maximum number of socket records that are part of a IGMP source unicast address for a specific group G Note that same socket may need multiple records if it listens on multiple interfaces! When operating in IGMPv2 style, it represents the total number of sockets that can join/leave a specific group.|
|TCPIP\_IGMP\_SOURCES\_PER\_GROUP|maximum number of unicast sources per group a IGMP application can listen to Each unicast source can be used by multiple sockets Should be prime number for best performance|
|TCPIP\_IGMP\_TASK\_TICK\_RATE|The IGMP task processing rate: number of milliseconds to generate an IGMP tick. This is the tick that advances the IGMP state machine. The default value is 33 milliseconds. The lower the rate \(higher the frequency\) the higher the module priority and higher module performance can be obtained The value cannot be lower than the TCPIP\_STACK\_TICK\_RATE.|
|TCPIP\_IGMP\_UNSOLICITED\_REPORT\_INTERVAL|The default Unsolicited Report Interval, milliseconds The Unsolicited Report Interval is the time between repetitions of a host’s initial report of membership in a group. RFC 3376 IGMPv3: Default: 1 second.|
|TCPIP\_IGMP\_USER\_NOTIFICATION|enable/disable user notification functions|
|TCPIP\_IGMPV2\_SUPPORT\_ONLY|If this symbol is !0, then only Join and Leave operations are supported, without support for source addresses required by IGMPv3 This results in smaller code and less RAM needed by the IGMP module.|
|TCPIP\_STACK\_USE\_IGMP|IGMPv3 functionality|
|**Functions**||
|TCPIP\_IGMP\_ExcludeSource|Helper to unsubscribe from one source.|
|TCPIP\_IGMP\_GroupInfoGet|Gets current info about a multicast group.|
|TCPIP\_IGMP\_GroupsGet|Gets current info about multicast groups.|
|TCPIP\_IGMP\_HandlerDeRegister|Deregisters a previously registered IGMP client handler.|
|TCPIP\_IGMP\_HandlerRegister|Registers a IGMP client Handler.|
|TCPIP\_IGMP\_IncludeSource|Helper to subscribe to one source.|
|TCPIP\_IGMP\_Join|IGMPv2 style "join" function.|
|TCPIP\_IGMP\_Leave|IGMPv2 style "leave" function.|
|TCPIP\_IGMP\_Subscribe|Subscribes to an IGMP multicast group.|
|TCPIP\_IGMP\_SubscribeGet|Returns the subscription to sources in an IGMP multicast group.|
|TCPIP\_IGMP\_Task|Standard TCP/IP stack module task function.|
|TCPIP\_IGMP\_Unsubscribe|Removes the subscription to sources in an IGMP multicast group.|
|**Data Types and Constants**||
|TCPIP\_IGMP\_EVENT\_HANDLER|Notification handler that can be called when an IGMP event ocurrs and the user needs to be notified|
|TCPIP\_IGMP\_EVENT\_TYPE|This enumeration lists the IGMP events used to notify IGMP client applications.|
|TCPIP\_IGMP\_FILTER\_TYPE|This enumeration lists the supported IGMP filters.|
|TCPIP\_IGMP\_GROUP\_INFO|IGMP group information|
|TCPIP\_IGMP\_HANDLE|IGMP client handle.|
|TCPIP\_IGMP\_MODULE\_CONFIG|IGMP module run-time configuration/initialization data.|
|TCPIP\_IGMP\_RESULT|This enumeration is used to report the result of an IGMP API call.|

-   **[TCPIP\_IGMP\_EVENT\_HANDLER Type](GUID-9F3B95DE-F127-481D-8456-3405E093B3B5.md)**  

-   **[TCPIP\_IGMP\_EVENT\_TYPE Enumeration](GUID-FF2D219A-C393-4997-BB07-0E154846FA02.md)**  

-   **[TCPIP\_IGMP\_ExcludeSource Function](GUID-077C63BB-3BEF-4AD0-912B-54F67EFCD6F3.md)**  

-   **[TCPIP\_IGMP\_FILTER\_TYPE Enumeration](GUID-3FB194B0-D981-437C-A414-589E2F6AA6B1.md)**  

-   **[TCPIP\_IGMP\_GROUP\_INFO Structure](GUID-3677FC08-B717-4AF7-A6F7-0255D4A37ECB.md)**  

-   **[TCPIP\_IGMP\_GroupInfoGet Function](GUID-31B86115-03F3-41B6-B2A0-77E01FE35D84.md)**  

-   **[TCPIP\_IGMP\_GroupSet Function](GUID-613CC0F0-8DDF-4B50-ABF5-12633AE21D65.md)**  

-   **[TCPIP\_IGMP\_HANDLE Type](GUID-EB4FEDE2-B865-4A2B-A345-CE10E6447167.md)**  

-   **[TCPIP\_IGMP\_HandlerDeRegister Function](GUID-717CE47B-9DA1-4C30-AFBE-1807B04FCFC4.md)**  

-   **[TCPIP\_IGMP\_HandlerRegister Function](GUID-CD5B40E8-2DB7-4F6A-BC85-4178EC2A05E9.md)**  

-   **[TCPIP\_IGMP\_IncludeSource Function](GUID-5096062A-E0FC-47ED-84D7-2B89E4D066A5.md)**  

-   **[TCPIP\_IGMP\_Join Function](GUID-87849350-1F04-4175-887F-0EB875774E92.md)**  

-   **[TCPIP\_IGMP\_Leave Function](GUID-6144796B-8069-4B41-BF89-981E67656516.md)**  

-   **[TCPIP\_IGMP\_MODULE\_CONFIG Structure](GUID-77B72E29-93E6-4604-ADED-4A3D10585673.md)**  

-   **[TCPIP\_IGMP\_RESULT Enumeration](GUID-949459F3-589F-4E96-A337-0A34DD9A8F1F.md)**  

-   **[TCPIP\_IGMP\_Subscribe Function](GUID-A97488A2-3063-4B2C-98A3-085563D4CA3C.md)**  

-   **[TCPIP\_IGMP\_SubscribeGet Function](GUID-587A9A50-323B-4310-A7D0-CDA58E9123CB.md)**  

-   **[TCPIP\_IGMP\_Task Function](GUID-2C68C719-82B4-46CC-A8D8-5B9205D81E6F.md)**  

-   **[TCPIP\_IGMP\_Unsubscribe Function](GUID-C2B672F3-CF8F-44EB-9942-706F420D5CC2.md)**  


**Parent topic:**[MPLAB® Harmony TCP/IP Library](GUID-01A0A1D8-EC9B-4EFF-B8E4-D154B555FEF2.md)

