# TFTP Client Module

The TCP/IP Stack Library Trivial File Transfer Protocol \(TFTP\) module is a simple protocol used to transfer files.

The TFTP client module supports reading and writing of files \(or mail\) from/to a remote server.

> Note: The TFTP client module cannot list directories, and currently has no provisions for user authentication.

**Abstraction Model**

![tftp_diagram](GUID-2B0BBC96-8D5B-469C-8E93-95FAA63B4FBA-low.png)

**TFTP Client Server Communication**

*TFTP Write Request \(WRQ\)*

The following diagram illustrates a write request.

![TCPIP TFTP WRQ](GUID-F055E860-D348-4339-94DE-31AD5B539075-low.png)

*TFTP Read Request \(RRQ\)*

The following diagram illustrates a read request.

![TCPIP TFTP RRQ](GUID-994291E0-070A-4D50-80D0-6A6FDFB8162E-low.png)

**Library Interface**

|Name|Description|
|----|-----------|
|**Macros**||
|TCPIP\_TFTPC\_ARP\_TIMEOUT|The number of seconds to wait before declaring a TIMEOUT error on PUT.|
|TCPIP\_TFTPC\_CMD\_PROCESS\_TIMEOUT|The number of seconds to wait before declaring a TIMEOUT in seconds error on GET or PUT command processing.|
|TCPIP\_TFTPC\_DEFAULT\_IF|The default TFTP interface for multi-homed hosts.|
|TCPIP\_TFTPC\_FILENAME\_LEN|The maximum value for the file name size.|
|TCPIP\_TFTPC\_HOSTNAME\_LEN|The maximum TFTP host name length size.|
|TCPIP\_TFTPC\_MAX\_RETRIES|The number of attempts before declaring a TIMEOUT error.|
|TCPIP\_TFTPC\_TASK\_TICK\_RATE|The TFTP client task rate in milliseconds. The default value is 100 milliseconds. The lower the rate \(higher the frequency\) the higher the module priority and higher module performance can be obtained The value cannot be lower than the TCPIP\_STACK\_TICK\_RATE.|
|TCPIP\_TFTPC\_USER\_NOTIFICATION|allow TFTP client user notification if enabled, the TCPIP\_TFTPC\_HandlerRegister/TCPIP\_TFTPC\_HandlerDeRegister functions exist and can be used|
|*TFTP\_CONFIG\_H*|This is macro *TFTP\_CONFIG\_H*.|
|**Functions**||
|TCPIP\_TFTPC\_GetEventNotification|Get the details of the TFTP client file mode communication event details.|
|TCPIP\_TFTPC\_HandlerDeRegister|Deregisters a previously registered TFTP Client handler.|
|TCPIP\_TFTPC\_HandlerRegister|Registers a TFTPC Handler.|
|TCPIP\_TFTPC\_SetCommand|TFTP client command operation configuration.|
|TCPIP\_TFTPC\_SetServerAddress|Set the Server IP address for TFTP Client .|
|TCPIP\_TFTPC\_Task|Standard TCP/IP stack module task function.|
|**Data Types and Constants**||
|\_TFTP\_CMD\_TYPE|File command mode used for TFTP PUT and GET commands.|
|TCPIP\_TFTP\_CMD\_TYPE|File command mode used for TFTP PUT and GET commands.|
|TCPIP\_TFTPC\_MODULE\_CONFIG|Placeholder for TFTP Client module configuration.|
|\_\_TFTPC\_H|This is macro \_\_TFTPC\_H.|
|TCPIP\_TFTPC\_EVENT\_HANDLER|TFTPC event handler prototype.|
|TCPIP\_TFTPC\_EVENT\_TYPE|TFTP client Event Type|
|TCPIP\_TFTPC\_HANDLE|TFTPC handle.|
|TCPIP\_TFTPC\_OPERATION\_RESULT|Standard error codes for TFTP PUT and GET command operation.|

-   **[\_\_TFTPC\_H Macro](GUID-1884C1BD-2859-4F9B-8930-2563FC2D5806.md)**  

-   **[TCPIP\_TFTP\_CMD\_TYPE Enumeration](GUID-ECDC8391-8950-4819-A1A9-C9961FAA4A28.md)**  

-   **[TCPIP\_TFTPC\_EVENT\_HANDLER Type](GUID-09EF2F19-F47A-479E-9BDC-245F47156017.md)**  

-   **[TCPIP\_TFTPC\_EVENT\_TYPE Enumeration](GUID-0FA47FC0-56FB-4142-8D30-1DCF4630822C.md)**  

-   **[TCPIP\_TFTPC\_GetEventNotification Function](GUID-69B2EBFA-D8E2-49BF-8191-62EA2F423BF0.md)**  

-   **[TCPIP\_TFTPC\_HANDLE Type](GUID-3B616A02-D750-4527-8DEE-F2813CFF7E6C.md)**  

-   **[TCPIP\_TFTPC\_HandlerDeRegister Function](GUID-4625117E-A939-44EE-BBF7-DBA7AD015F21.md)**  

-   **[TCPIP\_TFTPC\_HandlerRegister Function](GUID-3AC113E5-F369-44CF-B126-C38669674C51.md)**  

-   **[TCPIP\_TFTPC\_MODULE\_CONFIG Structure](GUID-B2C4C893-00FD-4B14-90F7-6AF0BDB868E7.md)**  

-   **[TCPIP\_TFTPC\_OPERATION\_RESULT Enumeration](GUID-8AACCF8D-A5AF-4355-AB80-0A5347C1F0D6.md)**  

-   **[TCPIP\_TFTPC\_SetCommand Function](GUID-F8D3500E-80D6-419C-958A-CCB9C85B6CAF.md)**  

-   **[TCPIP\_TFTPC\_SetServerAddress Function](GUID-D2535796-AE16-4FA5-AE47-F3B133B92327.md)**  

-   **[TCPIP\_TFTPC\_Task Function](GUID-DDF2F979-BF4C-44FD-BB0E-DEFE99C8E1DA.md)**  


**Parent topic:**[MPLAB® Harmony TCP/IP Library](GUID-01A0A1D8-EC9B-4EFF-B8E4-D154B555FEF2.md)

