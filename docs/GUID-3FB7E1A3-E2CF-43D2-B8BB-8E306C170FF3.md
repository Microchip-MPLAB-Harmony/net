# FTP Server Module

An embedded File Transfer Protocol \(FTP\) server is an excellent addition to any network-enabled device. FTP server capability facilitates the uploading of files to, and downloading of files from, an embedded device. Almost all computers have, at the very least, a command line FTP client that will allow a user to\_connect\_to an embedded FTP server.

**Library Interface**

|Name|Description|
|----|-----------|
|**Macros**||
|TCPIP\_FTP\_PASSWD\_LEN|Specifies the max length of FTP login password|
|TCPIP\_FTP\_USER\_NAME\_LEN|Specifies the max length for user name|
|TCPIP\_FTP\_DATA\_SKT\_RX\_BUFF\_SIZE|Define the size of the RX buffer for the FTP Data socket Use 0 for default TCP RX buffer size. The default recommended value for high throughput is \> 2MSS \(3 KB\). The performance of a socket is highly dependent on the size of its buffers so it's a good idea to use as large as possible buffers for the sockets that need high throughput.|
|TCPIP\_FTP\_DATA\_SKT\_TX\_BUFF\_SIZE|Define the size of the TX buffer for the FTP Data socket Use 0 for default TCP TX buffer size. The default recommended value for high throughput is \> 2MSS \(3 KB\). The performance of a socket is highly dependent on the size of its buffers so it's a good idea to use as large as possible buffers for the sockets that need high throughput.|
|TCPIP\_FTP\_MAX\_CONNECTIONS|Maximum number of FTP connections allowed|
|TCPIP\_FTP\_PUT\_ENABLED|Comment this line out to disable MPFS|
|TCPIP\_FTP\_TIMEOUT|FTP timeout, seconds|
|**General Functions**||
|TCPIP\_FTP\_ServerTask|Standard TCP/IP stack module task function.|
|**Data Types and Constants**||
|TCPIP\_FTP\_MODULE\_CONFIG|FTP Sever module runtime and initialization configuration data.|
|TCPIP\_FTPS\_TASK\_TICK\_RATE|The FTP server task rate, milliseconds The default value is 33 milliseconds. The lower the rate \(higher the frequency\) the higher the FTP server priority and higher transfer rates could be obtained The value cannot be lower than the TCPIP\_STACK\_TICK\_RATE.|

-   **[TCPIP\_FTP\_MODULE\_CONFIG Structure](GUID-16F5A03B-47F0-4952-A3F9-305ED76E2118.md)**  

-   **[TCPIP\_FTP\_ServerTask Function](GUID-F975E40B-3D00-4C7D-B0B3-8049012CF086.md)**  

-   **[TCPIP\_FTPS\_TASK\_TICK\_RATE Macro](GUID-1892D05F-4E46-4A63-8D3F-37FD6BFF051E.md)**  


**Parent topic:**[MPLAB® Harmony TCP/IP Library](GUID-01A0A1D8-EC9B-4EFF-B8E4-D154B555FEF2.md)

