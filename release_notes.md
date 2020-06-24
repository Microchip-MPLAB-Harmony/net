# Microchip MPLAB Harmony 3 Release Notes
## Net Release v3.6.1 (June, 2020)
### ADDITIONS AND UPDATES FOR  3.6.1:

- **New features**

The following table provides the list of the new features included in this dot release:

| Module                | Description                          |
| ------ | ------ |
| MAC driver            | Added support for PIC32MZW1 controller|
| NET_PRES              | Added certification validation option |

- **Release notes for v3.6.0 apply**


## Net Release v3.6.0 (June, 2020)
### ADDITIONS AND UPDATES FOR  3.6.0:

- **New Configurations**

The following table provides the list of the new configurations included in this release:

| Application                 | Platform          | Description                                             |
| ------------ | ------------ |  ------------     |
| web_ftp_server_usb_fatfs    | SAME54                         | HTTP + FTP and USB combined demo for SAME54 device for both baremetal and FreeRTOS|
| web_net_server_qspi_mpfs    | SAM9X60                        | Web server with MPFS file system on the onboard QSPI Flash Memory|
| tcpip_tcp_server            | PIC32MZEF ESK with ENC24xJ600  | TCP Server demo using Harmony native API   |
| tcpip_tcp_server            | PIC32MX ESK II with ENC28J60   | TCP Server demo using Harmony native API   |
| tcpip_tcp_client            | PIC32MZEF ESK with ENC24xJ600  | TCP Client demo using Harmony native API   |
| tcpip_tcp_client            | PIC32MX ESK II with ENC28J60   | TCP Client demo using Harmony native API   |


- **New features**

The following table provides the list of the new features included in this release:

| Module                | Description                          |
| ------ | ------ |
| Ethernet PHY driver   | Added support for KSZ8041, KSZ8863, LAN8700, LAN8720, LAN9303, DP83640, DP83848, IP101GR |
| Ethernet MAC driver   | Added support for  ENC24xJ600 and ENC28J60 controllers|
| TCP/IP MHC            | Added TCP/IP Stack heap estimation in MHC configuration|
| TCP/IP MHC            | MQTT_NET Glue code is now optional|
| TCP/IP MHC            | MHC Network configuration flags updated to allow all the possible combinations|
| TCP                   | Changed the TCPIP_TCP_CLOSE_WAIT_TIMEOUT default value to 0|
| TCP, NET_PRES         | Added API to indicate remote party disconnect (FIN)|
| FTP                   | Added FTP server run time authentication|
| TCP Demos             | Updated all the TCP demos to specifically check for the remote party sending FIN |
| MQTT Demo             | Added MQTT ping command |
| MPFS Utility          | mpfs.jar renamed to mpfs_net.jar| 
| MPFS Utility          | mpfs_net.jar utility will generate a MPFS_Net_SettingDetails.xml file for the configuration backup | 

- **Updated Applications**

    - All applications have been regenerated and tested to work with the latest repositories the TCP/IP stack depends on: core, csp, crypto, etc.

    - The TCP TCPIP_TCP_CLOSE_WAIT_TIMEOUT default value is now 0.
        - This means that the sockets do not close themselves when receiving a FIN from the remote node.
        - The socket owner needs to actively close the socket.
        - The TCPIP_TCP_WasDisconnected() and NET_PRES_SocketWasDisconnected() APIs were added for support.
        - The TCPIP_TCP_CLOSE_WAIT_TIMEOUT is marked as obsolete and will be eventually removed. However the application can still use it for now with a value != 0 if needed. 


- **Bug Fixes**

The following table provides the list of bug fixes in this release:

| Module                | Description                                             |
| ------ | ------ |
| drv_gmac          | GMAC driver fix to de-allocate all memory when the driver is closed|
| drv_gmac          | Fix for SAMA5D2 GMAC showing incorrect queue numbers|
| FTP server        | Added a new creation flag for the SYS_FS shell to allow proper relative root access|
| FTP server        | FTP directory search uses now SYS_FS_ATTR_FILE attribute to find all the files and directories within a directory|
| NET_PRES          | Fix for NET_PRES_SignalHandler callback function pointer|
| HTTP_NET, HTTP    | Fix for MPFS upload functionality |
| TCP/IP Stack      | Updated the use of the flexible arrays to conform to the language C standard |
| TCP/IP MHC        | Fixed MHC error occurring while modifying the NET_PRES component in MHC |
| TCP/IP MHC        | Fixed MHC error failing to activate IPv6 component in MHC |
| MPFS Utility      | Fixed MPFS "pre-build" configuration was not able to validate the input MPFS image path |
| SNMP              | Dependency on legacy aes_pic32mx.a library has been removed. WolfSSL AES-CFB 128 bit is used now for SNMP encryption and decryption|
| HTTP_NET          | HTTP EventReport converts all event infos to strings |

### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v5.40](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v2.41](https://www.microchip.com/mplab/compilers) or later
- [IAR Embedded Workbench for ARM - version 8.50.1](https://www.iar.com/iar-embedded-workbench/) or later for SAMA5D2, SAM9X60 projects
- Harmony net repository, 3.6.0

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, 3.7.2
- Harmony csp repository, 3.7.1
- Harmony bsp repository, 3.7.0
- Harmony dev_packs repository, 3.7.0
- Harmony crypto repository, 3.5.1
- Harmony usb repository, 3.5.0 for demos requiring USB
- wolfSSL  v4.3.0-stable (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v4.3.0-stable) for demos requiring wolfSSL encryption
- wolfMQTT v1.2 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.2) for demos requiring wolfMQTT support
- CMSIS-FreeRTOS 10.3.0 (https://github.com/Microchip-MPLAB-Harmony/CMSIS-FreeRTOS/tree/v10.3.0) for demos requiring FreeRTOS support
- MPLAB Harmony Configurator (MHC) v.3.4.0
- MPLAB® Harmony Configurator (MHC) plug-in, v3.5.0


#### Development Kit Support

This release supports applications for the following development kits

| Development Kits |
| --- |
| [PIC32MZEF Embedded Connectivity with FPU Starter Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/DM320007-C) |
| [PIC32MZ Embedded Graphics with Stacked DRAM (DA) Starter Kit](https://www.microchip.com/developmenttools/ProductDetails/DM320010-C) |
| [SAM A5D2 Xplained Ultra Evaluation Kit](https://www.microchip.com/Developmenttools/ProductDetails/ATSAMA5D2C-XULT) |
| [SAM E54 Xplained Pro Evaluation Kit](https://www.microchip.com/developmenttools/ProductDetails/ATSAME54-XPRO) |
| [SAM E70 Xplained Ultra Evaluation Kit](https://www.microchip.com/DevelopmentTools/ProductDetails.aspx?PartNO=ATSAME70-XULT) |
| [SAM V71 Xplained Ultra Evaluation Kit](https://www.microchip.com/DevelopmentTools/ProductDetails.aspx?PartNO=ATSAMV71-XULT) |
| [PIC32MX Ethernet Starter Kit II](http://www.microchip.com/DevelopmentTools/ProductDetails.aspx?PartNO=DM320004-2) |
| [SAM9X60-EK Evaluation Kit](https://www.microchip.com/developmenttools/ProductDetails/DT100126) |

### KNOWN ISSUES

The current known issues are as follows:

* SYS_DEBUG module needs to be removed in demos with external Ethernet controllers ENCx24J600 and ENC28J60:
    - This is due to DRV_SPI_TransferStatusGet() function returning "Transfer Handle Expired"

* When migrating a project from Net v3.5.1 to v3.6.0, the user may need to remove the "FILE SYSTEM" component from the project graph and then add it back to solve build issues

* wolfMQTT MHC configuration:
    - In 'wolfMQTT Library Configuration' when checking the 'Enable MQTT TLS' the generated code will fail build because missing of 'wolfSSL/options.h' file.
    - This file is not currently generated for wolfSSL and needs to be added manually

* web_net_server_qspi_mpfs on sam_a5d2_xult:
    - Run time exceptions occur after running the application for a while
    - This issue is currently under investigation

* web_net_server_sdcard_fatfs on sam_9x60_ek:
    - The HTTP access to the files and the HTTP connection is very slow
    - This issue is currently under investigation

* web_ftp_server_usb_fatfs on sam_e54_sk and sam_e54_sk_freertos:
    - DFP pack needs to be updated for these demos and SAME54_DFP v3.3.64 should be used

* For the TCP/IP demos using dual file system with SDMMC and NVM (web_ftp_server_mpfs_fatfs project for example):
    - A run time issue can occur when HTTP asks for a file status: the SYS_FS call can fail
    - The workaround is to have DRV_MEMORY_Initialize() (used by MPFS) called before the the call to DRV_SDSPI_Initialize() call (used by the FAT FS)
    - The mount of MPFS should be done before the FAT FS mount.
    - This is a known SYS_FS issue which will be solved in a future release.

* The DHCP server module does not handle well the initialization data on multiple interfaces
    - A run time exception can occur when using the DHCP server on multiple interfaces.

* wolfSSL code using the PIC32MZ crypto engine does not work correctly
    - PIC32MZ hardware crypto should not be selected in MHC
    - This is a known issue that is taken care of in a newer wolfSSL release.

* SAM9X60 driver corrupted RX packets may occur:
    - For RX packets that span more than one descriptor (i.e. packet size > 128 bytes) data corruption may occur.
    - Some packets may be reported as having wrong checksum and discarded.
    - This issue is under investigation and will be solved in a future release.

Older known issues:

* When creating an IAR project with MHC for the SAME54 Xplained Pro platform the following extra step is needed:
    - Add '\_\_PIC32C\_\_' in  project options-> c/c++ compiler -> preprocessor-> Defined symbols

* For PIC32MX projects on the Ethernet Stater Kit II:
    - PIC32MX device supports only the core timer. If other timer is selected there will be MHC errors during the demo generation.
    - Ethernet Starter Kit II doesn’t have on board UART2 connector
        - If a serial console is needed, a quick workaround may be attaching the Starter Kit to an Explorer 16 Development Board by using a Starter Kit I/O Expansion Board.
        - Another option is to use a telnet connection
        - MHC will throw an error if the UART component is not selected
    - It is preferred that the demos add both telnet and UART2 support. 

* SAM9X60 projects:
    - SAM9x60 projects with FAT FS work with optimization 0 only
        - Reading of the status.xml from the SD card could experience temporary lock ups.
    - The application code uses a Phy reset function
        - This is caused by a bug on the SAM9X60-EK board (KSZ8081 too sensitive to a jittery clock source)
    - The processor selection for debugging should be ARM926EJ-S

* For all the IAR projects:
    - The 'VLA -- variable length arrays' must be allowed in the compiler configuration.
    - Suppress the diagnostics Pe186, Pe188, Pe1029

* SAMA5D2, SAM9X60 IAR projects known issues:
    - The SNMP protocol and corresponding demo applications are not available.
    - IPv6 ULA generation is not available
    - ZCLL module is not supported
    - Crypto library is now supported on the SAMA5D2 processor. However the net_pres layer with wolfSSL encryption has not yet been fully tested for these projects. This will be done in a future release.
    - A workaround for the QSPI plib is in place for this release. When regenerating the code, the workaround must NOT be overwritten.
    - The SAMA5D2 projects need the symbols defined: \_\_SAMA5D2\_\_, \_\_PIC32C\_\_

* The code has been updated to allow the integrating of the WINC driver to the H3 TCP/IP Stack.
    - However the WINC support is not yet available. It will be added in a future release.

* SAM9X60 documentation issues:
    - The documentation for the SAM9X60 EMAC driver is currently missing.
    - The code/RAM resource tables do not contain the SAM9X60 platform.
    - They will be added in a future release.

* When regenerating a project that includes wolfSSL and crypto, MHC will try to include the HAVE_AES_ECB symbol in the configuration.h.
    -  Do not include this symbol as the build will fail. Simply ignore the change.

* The SAME70 XULT board has some issues and detection of the PHY board may fail. This issue is under investigation.
    - For now the SAME70 projects are present in the repo just for reference and not for actual running demos.
    - For now the SAME70 projects are generated using the LAN8740 PHY instead of KSZ8061 which is the default PHY for this board.

* The ICD4 loads the reset line of the SAM V71 Xplained Ultra board. When running demo projects using the V71, the ICD4 flex cable should be removed after programming to run the application.

* Interactive help using the Show User Manual Entry in the Right-click menu for configuration options provided by this module is not yet available from within the MPLAB Harmony Configurator (MHC).  Please see the *Configuring the Library* section in the help documentation in the doc folder for this Harmony 3 module instead.  Help is available in both CHM and PDF formats.

* SDMMC driver may block inside an internal routine while using the HTTP server and locks up the whole application. No workaround exists.
* EDBG programming of a hex file on the SAME54 XPRO board may fail.
* There are some mandatory MISRA violations in some of the repo files. They will be fixed in some next release:
    - System: sys_debug.c, sys_command.c
    - TCP/IP: smtpc.c, ndp.c, custom_http_net_app.c, icmpv6.c,
* Adding TCP/IP components to the project must be done using the TCP/IP Configurators in order to 
  load the dependencies properly. Adding the TCP/IP components from the "Available Components",
  without using configurator, might cause build issues.
* Similarly, remove the TCP/IP components by using the TCP/IP Configurator for the respective layer
* Do not add the TCP/IP components to the project by using both the TCP/IP Configurator and manually adding from "Available Components".
* Do not delete the TCP/IP Configurators from project graph.
* Interactive help using the "Show User Manual Entry" in the Right-click menu for configuration options provided
  by this module is not yet available from within the MPLAB Harmony Configurator (MHC).
  Please see the "Configuring the Library" section in the help documentation in the doc folder for this module instead.
  Help is available in both CHM and PDF formats.

* When the File System service is used in a demonstration application, make sure to set 
  "Maximum Simultaneous File Access" to a number above 10.
  Especially the HTTP server needs to open multiple files simultaneously.
* When the RTOS is enabled, make sure to include RTOS system API’s (like Time Delay functions)
  in application task to give other tasks the chance to execute.
  This can be achieved by enabling the "Use Task Delay" from Application Configuration -> RTOS Configuration.
  


### RELEASE CONTENTS

This topic lists the contents of this release and identifies each module.

#### Description

This table lists the contents of this release, including a brief description.


| Folder                                | Description                                                          | 
| --- | --- | 
| net/apps/berkeley_tcp_client          | TCP Client demo using BSD API                                        |
| net/apps/berkeley_tcp_server          | TCP Server demo using BSD API                                        |
| net/apps/berkeley_udp_client          | UDP Client demo using BSD API                                        |
| net/apps/berkeley_udp_relay           | UDP Relay demo using BSD API                                         |
| net/apps/berkeley_udp_server          | UDP Server demo using BSD API                                        |
| net/apps/iperf_demo                   | Demo for running iperf and measuring the network performance         |
| net/apps/snmpv3_nvm_mpfs              | SNMPv3 demo with MPFS file system using NVM storage                  |
| net/apps/snmpv3_sdcard_fatfs          | SNMPv3 demo with  FAT FS file system using external SD card          |
| net/apps/tcpip_client_server          | Multi-threaded example with TCP and UDP server and client threads    |
| net/apps/tcpip_tcp_client             | TCP Client demo using Harmony native API                             |
| net/apps/tcpip_tcp_client_server      | TCP Client and Server demo using Harmony native API                  |
| net/apps/tcpip_tcp_demo_at24mac       | Demo using the TCP/IP Stack initialization callback feature to use the board MAC address stored on the AT24MAC402 EEPROM storage|
| net/apps/tcpip_tcp_server             | TCP Server demo using Harmony native API                             |
| net/apps/tcpip_udp_client             | UDP Client demo using Harmony native API                             |
| net/apps/tcpip_udp_client_server      | UDP Client and Server demo using Harmony native API                  |
| net/apps/tcpip_udp_server             | UDP Server demo using Harmony native API                             |
| net/apps/web_ftp_server_mpfs_fatfs    | Dual SYS_FS demo, with HTTP web server using MPFS/NVM and FTP server using FAT FS/SDCARD|
| net/apps/web_ftp_server_usb_fatfs     | HTTP server demo with MSD Host Class Driver to read/write files from USB Flash Drive |
| net/apps/web_net_server_nvm_mpfs      | Web server with MPFS file system using the NVM storage               |
| net/apps/web_net_server_qspi_mpfs     | Web server with MPFS file system on the onboard QSPI Flash Memory    |
| net/apps/web_net_server_sdcard_fatfs  | Web server with FAT FS file system on the external SD card           |
| net/apps/web_server_nvm_mpfs          | Old style web server with MPFS file system using the NVM storage     |
| net/apps/web_server_sdcard_fatfs      | Old style web server with FAT FS file system on the external SD card |
| net/apps/wolfmqtt_demo                | MQTT demo using the 3rd party wolfMQTT library |
| net/apps/wolfssl_tcp_client           | Demo connecting to a Web server using TLS to encrypt the connection with wolfSSL|
| net/apps/wolfssl_tcp_server           | Web server demo accepting encrypted TLS connections using wolfSSL |


## Net Release v3.5.0 (November, 2019)
### ADDITIONS AND UPDATES FOR  3.5.0:

- **New part support** -This release introduces initial support for [PIC32MX](https://www.microchip.com/design-centers/32-bit/pic-32-bit-mcus/pic32mx-family) and [SAM 9X60](https://www.microchip.com/design-centers/32-bit-mpus) families of 32-bit MPUs.


- **New Applications**

The following table provides the list of the new applications including bare metal and FreeRTOS configurations:

| Application                 | Platform          | Description                                                                                      |
| ------------ | ------------ |  ------------     |
| iperf_demo                    | PIC32MZDA, PIC32MZEF, SAME54, SAME70, SAMV71  | Standard network benchmark demo |
| tcpip_tcp_demo_at24mac        | SAME54, SAME70, SAMV71                        | Demo using the board MAC address from the AT24MAC402 EEPROM storage |
| web_ftp_server_mpfs_fatfs     | PIC32MZDA, PIC32MZEF, SAME54, SAME70          | Demo with HTTP web server using MPFS/NVM and FTP server using FATFS/SDCARD |
| wolfssl_tcp_client            | PIC32MZEF, SAME54, SAME70, SAMA5D2            | wolfSSL TCP client demo |
| wolfssl_tcp_server            | PIC32MZEF, SAME54, SAME70, SAMA5D2            | wolfSSL TCP server demo |
| web_net_server_nvm_mpfs       | PIC32MZDA, PIC32MZEF, SAME54, SAME70, SAMV71  | FreeRTOS Web server with MPFS file system using the NVM storage |
| web_net_server_sdcard_fatfs   | PIC32MZDA, PIC32MZEF, SAME54, SAME70, SAMV71  | FreeRTOS Web server with FAT FS file system on the external SD Card |

- **New features**

The following table provides the list of the new features included in this release:

| Module                | Description                          |
| ------ | ------ |
|  TCP/IP Manager   | Added initialization callback to delay the initialization until the moment the configuration data is available|
|  TCP/IP Manager   | Added registration for external TCP/IP packet processing|
|  TCP              | Added registration for external TCP packet processing|
|  UDP              | Added registration for external UDP packet processing|
|  IPv4             | Added registration for external IPv4 packet processing|
|  IPv6             | Added registration for external IPv6 packet processing|
|  telnet           | Added run time authentication registration|
|  SNTP             | SNTP detects an empty/null server name as a way to stop its operation and stay idle|
|  BSD              | Added TCPIP_BSD_PresSocket() API to return the NET_PRES socket|
|  TCP/IP Packet logger | Added packet logger reset operation|
|  HTTP_NET, HTTP, FTPs, TFTPs, SNMP | Removed Sys_FS_Wrapper. Added a SYS_FS_Shell object. Server modules have a initialization parameter to control the serving directory |
|  FTP Client       | Added FTP Client implementation, Beta support|
|  MAC driver       | Applied changes for allowing the integrating WINC driver to the H3 TCP/IP Stack|
|  Demos            | The SYS_FS mount points are now controlled by the applications. The application has to configure the TCP/IP servers with the mount points |


- **Updated Applications**

Support for applications using the IAR IDE for SAME70, SAME54, SAMV71 and SAM9X60 has been added.
The following table provides the list of the updated IAR applications including bare metal and FreeRTOS configurations:

| IAR Application             | Platform                                              | Description         |
| ------------ | ------------ | ------------ |
| tcpip_tcp_client            | SAM9X60, SAMA5D2, SAME54, SAME70, SAMV71 | TCP Client demo using Harmony native API                         |
| tcpip_tcp_server            | SAM9X60, SAMA5D2, SAME54, SAME70, SAMV71 | TCP Server demo using Harmony native API                         |
| tcpip_tcp_client_server     | SAM9X60, SAMA5D2                         | TCP Client and Server demo using Harmony native API              |
| tcpip_udp_client            | SAM9X60, SAMA5D2                         | UDP Client demo using Harmony native API                         |
| tcpip_udp_client_server     | SAM9X60, SAMA5D2                         | UDP Client and Server demo using Harmony native API              |
| tcpip_udp_server            | SAM9X60, SAMA5D2                         | UDP Server demo using Harmony native API                         |
| web_net_server_sdcard_fatfs | SAM9X60, SAMA5D2                         | Web server with FAT FS file system on the external SD Card       |
| tcpip_client_server         | SAMA5D2                                  | Multi-threaded example with TCP and UDP server and client threads|


The following table provides the list of the updated MPLABX applications including bare metal and FreeRTOS configurations:

| MPLABX Application          | Platform                                                | Description                                                          |
| ------------ | ------------ | ------------ |
| berkeley_tcp_client         | PIC32MZEF, PIC32MZDA, SAME54, SAME70, SAMV71            | TCP Client demo using BSD API                                        |
| berkeley_tcp_server         | PIC32MZEF, PIC32MZDA, SAME54, SAME70, SAMV71            | TCP Server demo using BSD API                                        |
| berkeley_udp_client         | PIC32MZEF, PIC32MZDA , SAME54, SAME70, SAMV71           | UDP Client demo using BSD API                                        |
| berkeley_udp_relay          | PIC32MZEF, PIC32MZDA , SAME54, SAME70, SAMV71           | UDP Relay demo using BSD API                                         |
| berkeley_udp_server         | PIC32MZEF, PIC32MZDA , SAME54, SAME70, SAMV71           | UDP Server demo using BSD API                                        |
| snmpv3_nvm_mpfs             | PIC32MZEF, SAME54, SAME70, SAMV71                       | SNMPv3 demo with MPFS file system using NVM storage                  |
| snmpv3_sdcard_fatfs         | PIC32MZEF, PIC32MZDA, SAME54, SAME70, SAMV71            | SNMPv3 demo with  FAT FS file system using external SD card          |
| tcpip_client_server         | PIC32MZDA, PIC32MZEF, SAME54, SAME70                    | Multi-threaded example with TCP and UDP server and client threads    |
| tcpip_tcp_client            | PIC32MX, PIC32MZDA, PIC32MZEF, SAME54, SAME70, SAMV71   | TCP Client demo using Harmony native API                             |
| tcpip_tcp_client_server     | PIC32MZDA, PIC32MZEF, SAME54, SAME70, SAMV71            | TCP Client and Server demo using Harmony native API                  |
| tcpip_tcp_server            | PIC32MZDA, PIC32MZEF, SAME54, SAME70, SAMV71            | TCP Server demo using Harmony native API                             |
| tcpip_udp_client            | PIC32MZDA, PIC32MZEF, SAME54, SAME70, SAMV71            | UDP Client demo using Harmony native API                             |
| tcpip_udp_client_server     | PIC32MZDA, PIC32MZEF, SAME54, SAME70, SAMV71            | UDP Client and Server demo using Harmony native API                  |
| tcpip_udp_server            | PIC32MZDA, PIC32MZEF, SAME54, SAME70, SAMV71            | UDP Server demo using Harmony native API                             |
| web_net_server_nvm_mpfs     | PIC32MZDA, PIC32MZEF, SAME54, SAME70, SAMV71            | Web server with MPFS file system using the NVM storage               |
| web_net_server_sdcard_fatfs | PIC32MZDA, PIC32MZEF, SAME54, SAME70, SAMV71            | Web server with FAT FS file system on the external SD Card           |
| web_server_nvm_mpfs         | PIC32MZEF, SAME54, SAME70, SAMV71                       | Old style web server with MPFS file system using the NVM storage     |
| web_server_sdcard_fatfs     | SAME54, SAME70, SAMV71                                  | Old style web server with FAT FS file system on the external SD card |
| wolfmqtt_demo               | PIC32MZEF, SAME70                                       | wolfMQTT application demonstrates a simple MQTT application using the 3rd party wolfMQTT library |

- **Updated Drivers**

The following table provides the list of new support for MAC drivers:

| Driver                | Platform |
| --------- | --------- |
| drv_emac.c, drv_emac_lib.c   | SAM9X60 |


- **Bug Fixes**

The following table provides the list of bug fixes in this release:

| Module                | Description                                             |
| ------ | ------ |
| drv_gmac          | Fixed GMAC driver deadlock during BNA|
| NET_PRES          | Fixed the provider free context only when the context is open.|
| FTP server        | The 'rmdir' command has been fixed to send an error message to the client if the directory is not empty |
| HTTP_NET, HTTP    | Removed SYS_FS dependencies from the web pages and custom_http|
| MAC driver        | Moved the part specific structures from the tcpip_mac.h to the corresponding MAC driver header file|
| SNTP              | Detected loss of link while waiting for DNS and switched to an wait state|
| IGMP              | Updated the query processing for IGMP to avoid discarding query nodes|
| TCP/IP Commands   | Enabled the inclusion of drv_miim.h only when MIIM commands are selected|
| telnet            | updated the _Telnet_GETC() to properly discard control characters in the buffer|
| DHCPv6            | Replaced SYS_CONSOLE_MESSAGE with SYS_CONSOLE_PRINT for the DHCPv6 debug messages|
| TCP/IP Manager    | Removed TCP/IP packet redundant allocation map to non cached memory|
| drv_ethphy        | Remove obsolete DRV_ETHPHY_OBJECT_BASE MIIM access functions. Removed unsupported WOL|
| drv_gmac          | Removed PIC32C_GMAC_ISR_TX option |
| drv_ethmac        | Removed obsolete ETH_PIC32_INT_MAC_ISR_TX symbol|


### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v5.25](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v2.30](https://www.microchip.com/mplab/compilers) or later
- [IAR Embedded Workbench for ARM - version 8.32.1](https://www.iar.com/iar-embedded-workbench/) or later for SAMA5D2, SAM9X60 projects
- Harmony net repository, 3.5.0

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules:

- MPLAB Harmony Configurator (MHC) v.3.3.3
- MPLAB® Harmony Configurator (MHC) plug-in, v3.3.0.1
- Harmony bsp repository, 3.5.0
- Harmony csp repository, 3.5.2
- Harmony core repository, 3.5.2
- Harmony dev_packs repository, 3.5.0
- Harmony crypto repository, 3.4.1
- wolfSSL  v4.1.0-stable (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v4.1.0-stable) for demos requiring wolfSSL encryption
- wolfMQTT v1.2 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.2) for demos requiring wolfMQTT support
- CMSIS-FreeRTOS 10.2.0 (https://github.com/Microchip-MPLAB-Harmony/CMSIS-FreeRTOS/tree/10.2.0) for demos requiring FreeRTOS support


#### Development Kit Support

This release supports applications for the following development kits

| Development Kits |
| --- |
| [PIC32MZEF Embedded Connectivity with FPU Starter Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/DM320007-C) |
| [PIC32MZ Embedded Graphics with Stacked DRAM (DA) Starter Kit](https://www.microchip.com/developmenttools/ProductDetails/DM320010-C) |
| [SAM A5D2 Xplained Ultra Evaluation Kit](https://www.microchip.com/Developmenttools/ProductDetails/ATSAMA5D2C-XULT) |
| [SAM E54 Xplained Pro Evaluation Kit](https://www.microchip.com/developmenttools/ProductDetails/ATSAME54-XPRO) |
| [SAM E70 Xplained Ultra Evaluation Kit](https://www.microchip.com/DevelopmentTools/ProductDetails.aspx?PartNO=ATSAME70-XULT) |
| [SAM V71 Xplained Ultra Evaluation Kit](https://www.microchip.com/DevelopmentTools/ProductDetails.aspx?PartNO=ATSAMV71-XULT) |
| [PIC32MX Ethernet Starter Kit II](http://www.microchip.com/DevelopmentTools/ProductDetails.aspx?PartNO=DM320004-2) |
| [SAM9X60-EK Evaluation Kit](https://www.microchip.com/developmenttools/ProductDetails/DT100126) |

### KNOWN ISSUES

The current known issues are as follows:

* For creating an IAR project with MHC for the SAME54 Xplained Pro platform the following steps are needed:
    - Run MHC in standalone mode
    - Create new configuration
    - Uncheck checkbox 'convert to relative path for configuration' in MPLAB Harmony Framework Management Tool 
    - Add configuration name as 'sam_54_xpro' 
    - Select device as SAME54P20A
    - Add components in MHC
    - Configure TCP/IP Heap size, configure clock, configure Pins
    - Select the toolchain as IAR, under System module
    - Configure System Heap size.
    - Generate Code
    - Copy flash.icf file from IAR toolchain folder to directory config\<config_name>\
    - Open IAR IDE
    - Create new project
    - Add project connection with .ipcf file in directory config\<config_name>\
    - Override default linker script with the flash.icf in config\<config_name>\ folder by Project->Options->Linker->Config->Override Default
    - Update heap and stack size in linker option at Project->Options->Linker->Config
    - To suppress warnings, add 'Pe186,Pe188' in project options-> c/c++ compiler -> diagnostics -> suppress these diagnostics
    - Add '\_\_PIC32C\_\_' in  project options-> c/c++ compiler -> preprocessor-> Defined symbols
    - Add any application source files (not generated by MHC) to project by Project->Add Files
    - Select the debugger as CMSIS-DAP
    - Build and debug the project

* For creating an IAR project with MHC for the SAME70/SAMV71 Xplained Ultra platforms the following steps are needed:
    - Run MHC in standalone mode
    - Create new configuration
    - Uncheck checkbox 'convert to relative path for configuration' in MPLAB Harmony Framework Management Tool 
    - Add configuration name as 'sam_e70_xult_iar'
    - Select device as SAME70Q21B
    - Add components in MHC
    - Configure TCP/IP Heap size, configure clock, configure Pins
    - Select the toolchain as IAR, under System module
    - Configure System Heap size.
    - Generate Code
    - Copy flash.icf file from IAR toolchain folder to directory config\<config_name>\
    - Open IAR IDE
    - Create new project
    - Add project connection with .ipcf file in directory config\<config_name>\
    - For SAME70 project, select the device name at Project->Options->General Options as 'Microchip ATSAME70Q21xB'
    - Override default linker script with the flash.icf in config\<config_name>\ folder by Project->Options->Linker->Config->Override Default
    - Update heap and stack size in linker option at Project->Options->Linker->Config
    - To suppress warnings, add 'Pe186,Pe188' in project options-> c/c++ compiler -> diagnostics -> suppress these diagnostics
    - Add '\_\_PIC32C\_\_' in  project options-> c/c++ compiler -> preprocessor-> Defined symbols
    - Add any application source files (not generated by MHC) to project by Project->Add Files
    - Select the debugger as CMSIS-DAP
    - Build and debug the project.

* For PIC32MX projects on the Ethernet Stater Kit II:
    - PIC32MX device supports only the core timer. If other timer is selected there will be MHC errors during the demo generation.
    - Ethernet Starter Kit II doesn’t have on board UART2 connector
        - If a serial console is needed, a quick workaround may be attaching the Starter Kit to an Explorer 16 Development Board by using a Starter Kit I/O Expansion Board.
        - Another option is to use a telnet connection
        - MHC will throw an error if the UART component is not selected
    - It is preferred that the demos add both telnet and UART2 support. 

* For the web_ftp_server_mpfs_fatfs project configurations:
    - A run time exception occurs with the initialization code generated by MHC
    - The workaround in the SYS_Initialize() is to have DRV_MEMORY_Initialize() call (which uses the MPFS file system) earlier than the to DRV_SDSPI_Initialize() call (which uses the FAT FS)
    - Do NOT overwrite the workaround when regenerating the project.
    - This is a known SYS_FS issue which will be solved in a future release.


* SAM9X60 projects:
    - SAM9x60 projects with FAT FS work with optimization 0 only
        - Reading of the status.xml from the SD card could experience temporary lock ups.
    - The application code uses a Phy reset function
        - This is caused by a bug on the SAM9X60-EK board (KSZ8081 too sensitive to a jittery clock source)
    - The processor selection for debugging should be ARM926EJ-S
    - The SST26 component is currently disabled and can not be currently added for SAM9X60 projects
        - The project web_net_server_qspi_mpfs does not support the SAM9X60 configuration

* For all the IAR projects:
    - The 'VLA -- variable length arrays' must be allowed in the compiler configuration.
    - Suppress the diagnostics Pe186, Pe188, Pe1029

* SAMA5D2, SAM9X60 IAR projects known issues:
    - The SNMP protocol and corresponding demo applications are not available.
    - IPv6 ULA generation is not available
    - ZCLL module is not supported
    - Crypto library is now supported on the SAMA5D2 processor. However the net_pres layer with wolfSSL encryption has not yet been fully tested for these projects. This will be done in a future release.
    - A workaround for the QSPI plib is in place for this release. When regenerating the code, the workaround must NOT be overwritten.
    - The SAMA5D2 projects need the symbols defined: \_\_SAMA5D2\_\_, \_\_PIC32C\_\_

* The code has been updated to allow the integrating of the WINC driver to the H3 TCP/IP Stack.
    - However the WINC support is not yet available. It will be added in a future release.

* SAM9X60 documentation issues:
    - The documentation for the SAM9X60 EMAC driver is currently missing.
    - The code/RAM resource tables do not contain the SAM9X60 platform.
    - They will be added in a future release.

Older known issues:

* MHC may hang when configuring NET_PRES Presentation Layer component.
    - Short-term work around is to save MHC state, close and reopen MHC.
* When regenerating a project that includes wolfSSL and crypto, MHC will try to include the HAVE_AES_ECB symbol in the configuration.h.
    -  Do not include this symbol as the build will fail. Simply ignore the change.
* The SAME70 XULT board has some issues and detection of the PHY board may fail. This issue is under investigation.
    - For now the SAME70 projects are present in the repo just for reference and not for actual running demos.
    - For now the SAME70 projects are generated using the LAN8740 PHY instead of KSZ8061 which is the default PHY for this board.
* The ICD4 loads the reset line of the SAM V71 Xplained Ultra board. When running demo projects using the V71, the ICD4 flex cable should be removed after programming to run the application.
* Interactive help using the Show User Manual Entry in the Right-click menu for configuration options provided by this module is not yet available from within the MPLAB Harmony Configurator (MHC).  Please see the *Configuring the Library* section in the help documentation in the doc folder for this Harmony 3 module instead.  Help is available in both CHM and PDF formats.

* The SAME70 and SAMV71 TCPIP demos with SDCARD, won’t work with optimisation 0. Use optimization level 1.

* SDMMC driver may block inside an internal routine while using the HTTP server and locks up the whole application. No workaround exists.
* EDBG programming of a hex file on the SAME54 XPRO board may fail.
* There are some mandatory MISRA violations in some of the repo files. They will be fixed in some next release:
    - System: sys_debug.c, sys_command.c
    - TCP/IP: smtpc.c, ndp.c, custom_http_net_app.c, icmpv6.c,
* Adding TCP/IP components to the project must be done using the TCP/IP Configurators in order to 
  load the dependencies properly. Adding the TCP/IP components from the "Available Components",
  without using configurator, might cause build issues.
* Similarly, remove the TCP/IP components by using the TCP/IP Configurator for the respective layer
* Do not add the TCP/IP components to the project by using both the TCP/IP Configurator and manually adding from "Available Components".
* Do not delete the TCP/IP Configurators from project graph.
* Interactive help using the "Show User Manual Entry" in the Right-click menu for configuration options provided
  by this module is not yet available from within the MPLAB Harmony Configurator (MHC).
  Please see the "Configuring the Library" section in the help documentation in the doc folder for this module instead.
  Help is available in both CHM and PDF formats.

* The tcpip_client_server application does not currently have complete documentation. The commands to 
  use this demonstration are provided in the run time help available as console commands.
  This will be added in a future release.
* When the File System service is used in a demonstration application, make sure to set 
  "Maximum Simultaneous File Access" to a number above 10.
  Especially the HTTP server needs to open multiple files simultaneously.
* When the RTOS is enabled, make sure to include RTOS system API’s (like Time Delay functions)
  in application task to give other tasks the chance to execute.
  This can be achieved by enabling the "Use Task Delay" from Application Configuration -> RTOS Configuration.
  


### RELEASE CONTENTS

This topic lists the contents of this release and identifies each module.

#### Description

This table lists the contents of this release, including a brief description.


| Folder                                | Description                                                          | 
| --- | --- | 
| net/apps/berkeley_tcp_client          | TCP Client demo using BSD API                                        |
| net/apps/berkeley_tcp_server          | TCP Server demo using BSD API                                        |
| net/apps/berkeley_udp_client          | UDP Client demo using BSD API                                        |
| net/apps/berkeley_udp_relay           | UDP Relay demo using BSD API                                         |
| net/apps/berkeley_udp_server          | UDP Server demo using BSD API                                        |
| net/apps/iperf_demo                   | Demo for running iperf and measuring the network performance         |
| net/apps/snmpv3_nvm_mpfs              | SNMPv3 demo with MPFS file system using NVM storage                  |
| net/apps/snmpv3_sdcard_fatfs          | SNMPv3 demo with  FAT FS file system using external SD card          |
| net/apps/tcpip_client_server          | Multi-threaded example with TCP and UDP server and client threads    |
| net/apps/tcpip_tcp_client             | TCP Client demo using Harmony native API                             |
| net/apps/tcpip_tcp_client_server      | TCP Client and Server demo using Harmony native API                  |
| net/apps/tcpip_tcp_demo_at24mac       | Demo using the TCP/IP Stack initialization callback feature to use the board MAC address stored on the AT24MAC402 EEPROM storage|
| net/apps/tcpip_tcp_server             | TCP Server demo using Harmony native API                             |
| net/apps/tcpip_udp_client             | UDP Client demo using Harmony native API                             |
| net/apps/tcpip_udp_client_server      | UDP Client and Server demo using Harmony native API                  |
| net/apps/tcpip_udp_server             | UDP Server demo using Harmony native API                             |
| net/apps/web_ftp_server_mpfs_fatfs    | Dual SYS_FS demo, with HTTP web server using MPFS/NVM and FTP server using FAT FS/SDCARD|
| net/apps/web_net_server_nvm_mpfs      | Web server with MPFS file system using the NVM storage               |
| net/apps/web_net_server_qspi_mpfs     | Web server with MPFS file system on the onboard QSPI Flash Memory    |
| net/apps/web_net_server_sdcard_fatfs  | Web server with FAT FS file system on the external SD card           |
| net/apps/web_server_nvm_mpfs          | Old style web server with MPFS file system using the NVM storage     |
| net/apps/web_server_sdcard_fatfs      | Old style web server with FAT FS file system on the external SD card |
| net/apps/wolfmqtt_demo                | MQTT demo using the 3rd party wolfMQTT library |
| net/apps/wolfssl_tcp_client           | Demo connecting to a Web server using TLS to encrypt the connection with wolfSSL|
| net/apps/wolfssl_tcp_server           | Web server demo accepting encrypted TLS connections using wolfSSL |


## Net Release v3.4.0 (July, 2019)
### ADDITIONS AND UPDATES FOR  3.4.0:

- **New features**
    - This release introduces initial support for wolfMQTT in the Harmony framework, including encryption support with wolfSSL.
    - The HTTP_NET demos have been updated to support encryption.

- **New Applications**

The following table provides the list of the new applications including bare metal and FreeRTOS configurations:

| Application                 | Platform          | Description                                                                                      |
| ------------ | ------------ |  ------------     |
| wolfmqtt_demo               | PIC32MZEF, SAME70 | wolfMQTT application demonstrates a simple MQTT application using the 3rd party wolfMQTT library |


- **Updated Applications**

The following table provides the list of the updated applications including bare metal and FreeRTOS configurations:

| Application                 | Platform                                              | Description                                                          |
| ------------ | ------------ | ------------ |
| berkeley_tcp_client         | PIC32MZEF, PIC32MZDA, SAME54, SAME70, SAMV71          | TCP Client demo using BSD API                                        |
| berkeley_tcp_server         | PIC32MZEF, PIC32MZDA, SAME54, SAME70, SAMV71          | TCP Server demo using BSD API                                        |
| berkeley_udp_client         | PIC32MZEF, PIC32MZDA , SAME54, SAME70, SAMV71         | UDP Client demo using BSD API                                        |
| berkeley_udp_relay          | PIC32MZEF, PIC32MZDA , SAME54, SAME70, SAMV71         | UDP Relay demo using BSD API                                         |
| berkeley_udp_server         | PIC32MZEF, PIC32MZDA , SAME54, SAME70, SAMV71         | UDP Server demo using BSD API                                        |
| snmpv3_nvm_mpfs             | PIC32MZEF , SAME54, SAME70, SAMV71                    | SNMPv3 demo with MPFS file system using NVM storage                  |
| snmpv3_sdcard_fatfs         | PIC32MZEF , SAME54, SAME70, SAMV71                    | SNMPv3 demo with  FAT FS file system using external SD card          |
| tcpip_client_server         | PIC32MZDA, PIC32MZEF, SAMA5D2, SAME54, SAME70         | Multi-threaded example with TCP and UDP server and client threads    |
| tcpip_tcp_client            | PIC32MZDA, PIC32MZEF, SAMA5D2, SAME54, SAME70, SAMV71 | TCP Client demo using Harmony native API                             |
| tcpip_tcp_client_server     | PIC32MZDA, PIC32MZEF, SAMA5D2, SAME54, SAME70, SAMV71 | TCP Client and Server demo using Harmony native API                  |
| tcpip_tcp_server            | PIC32MZDA, PIC32MZEF, SAMA5D2, SAME54, SAME70, SAMV71 | TCP Server demo using Harmony native API                             |
| tcpip_udp_client            | PIC32MZDA, PIC32MZEF, SAMA5D2, SAME54, SAME70, SAMV71 | UDP Client demo using Harmony native API                             |
| tcpip_udp_client_server     | PIC32MZDA, PIC32MZEF, SAMA5D2, SAME54, SAME70, SAMV71 | UDP Client and Server demo using Harmony native API                  |
| tcpip_udp_server            | PIC32MZDA, PIC32MZEF, SAMA5D2, SAME54, SAME70, SAMV71 | UDP Server demo using Harmony native API                             |
| web_net_server_nvm_mpfs     | PIC32MZDA, PIC32MZEF, SAMA5D2, SAME54, SAME70, SAMV71 | Web server with MPFS file system using the NVM storage               |
| web_net_server_qspi_mpfs    | SAMA5D2                                               | Web server with MPFS file system on the onboard QSPI Flash Memory    |
| web_net_server_sdcard_fatfs | PIC32MZDA, PIC32MZEF, SAMA5D2, SAME54, SAME70, SAMV71 | Web server with FAT FS file system on the external SD Card           |
| web_server_nvm_mpfs         | PIC32MZEF, SAME54, SAME70, SAMV71                     | Old style web server with MPFS file system using the NVM storage     |
| web_server_sdcard_fatfs     | SAME54, SAME70, SAMV71                                | Old style web server with FAT FS file system on the external SD card |

- **New Third party adapters**

The following table provides the list of new support for third part adapters:

| Module                | Third Party |
| --------- | --------- |
| wolfMQTT NET glue | Glue code for wolfMQTT support |




- **Bug Fixes**

The following table provides the list of bug fixes in this release:

| Module                | Description                                             |
| ------ | ------ |
| telnet, iperf, tcpip_commands | Updated the modules to work with the new version of the core command processor        |
| SNTP                          | Fixed the missing entry for the time stamp in the _SNTP_DbgResError_Tbl debug table   |
| NET_PRES                      | Updates for integration of wolfSSL                                                    |
| TCP/IP Helpers                | Added MQTT ports to the TCP/IP stack tables                                           |
| drv_gmac                      | Update for GMAC RX buffer management                                                  |
| drv_gmac                      | Updated return value of DRV_GMAC_PacketTx                                             |
| drv_gmac                      | Updated return value of DRV_GMAC_PacketTx                                             |
| drv_gmac                      | Enabled the GMAC TX interrupt                                                         |



### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v5.20](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v2.20](https://www.microchip.com/mplab/compilers) or later
- [IAR Embedded Workbench for ARM - version 8.32.1] (https://www.iar.com/iar-embedded-workbench/) or later for SAMA5D2 projects
- Harmony net repository, 3.4.0

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules:

- MPLAB Harmony Configurator (MHC) v.3.3.1
- Harmony bsp repository, 3.4.0
- Harmony csp repository, 3.4.0
- Harmony core repository, 3.4.0
- Harmony dev_packs repository, 3.4.0
- Harmony crypto repository, 3.3.0
- wolfSSL v3.15.7 (https://github.com/wolfSSL/wolfssl/commits/v3.15.7-stable) for demos requiring wolfSSL encryption
- wolfMQTT (https://github.com/wolfSSL/wolfMQTT) for demos requiring wolfMQTT support

#### Development Kit Support

This release supports applications for the following development kits

| Development Kits |
| --- |
| [PIC32MZEF Embedded Connectivity with FPU Starter Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/DM320007-C) |
| [PIC32MZ Embedded Graphics with Stacked DRAM (DA) Starter Kit](https://www.microchip.com/developmenttools/ProductDetails/DM320010-C) |
| [SAM A5D2 Xplained Ultra Evaluation Kit](https://www.microchip.com/Developmenttools/ProductDetails/ATSAMA5D2C-XULT) |
| [SAM E54 Xplained Pro Evaluation Kit](https://www.microchip.com/developmenttools/ProductDetails/ATSAME54-XPRO) |
| [SAM E70 Xplained Ultra Evaluation Kit](https://www.microchip.com/DevelopmentTools/ProductDetails.aspx?PartNO=ATSAME70-XULT) |
| [SAM V71 Xplained Ultra Evaluation Kit](https://www.microchip.com/DevelopmentTools/ProductDetails.aspx?PartNO=ATSAMV71-XULT) |

### KNOWN ISSUES

The current known issues are as follows:

* MHC may hang when configuring NET_PRES Presentation Layer component.
    - Short-term work around is to save MHC state, close and reopen MHC.
* Encryption support has been tested using wolfSSL v3.15.7-stable
    -  Support for newer wolfSSL versions will be added in a future release
* When regenerating a project that includes wolfSSL and crypto, MHC will try to include the HAVE_AES_ECB symbol in the configuration.h.
    -  Do not include this symbol as the build will fail. Simply ignore the change.
* HTTP_NET demos using encryption (wolfSSL) with FreeRTOS are not part of this release 
    - There are some memory overwrite issues when the HTTP_NET + wolfSSL run in multi-threaded mode
    - This is currently under investigation
* The SAME70 XULT board has some issues and detection of the PHY board may fail. This issue is under investigation.
    - For now the SAME70 projects are present in the repo just for reference and not for actual running demos.
    - For now the SAME70 projects are generated using the LAN8740 PHY instead of KSZ8061 which is the default PHY for this board.
* The ICD4 loads the reset line of the SAM V71 Xplained Ultra board. When running demo projects using the V71, the ICD4 flex cable should be removed after programming to run the application.
* Interactive help using the Show User Manual Entry in the Right-click menu for configuration options provided by this module is not yet available from within the MPLAB Harmony Configurator (MHC).  Please see the *Configuring the Library* section in the help documentation in the doc folder for this Harmony 3 module instead.  Help is available in both CHM and PDF formats.
* The SAM E70/V71/E54/A5D2 processors do not have a built in factory pre-programmed MAC address.
    - The XULT boards use the AT24MAC402 serial EEPROM for having an unique MAC address per board.
    - Currently the demo applications do not use the AT24MAC402 EEPROM for reading the MAC address. This will be added in a future release.

* The SAME70 and SAMV71 TCPIP demos with SDCARD, won’t work with optimisation 0. Use optimization level 1.

* SAMA5D2 projects known issues:
    - The SNMP protocol and corresponding demo applications is not be available.
    - IPv6 ULA generation is not available
    - ZCLL module is not supported
    - Crypto library is now supported on the SAMA5D2 processor. However the net_pres layer with wolfSSL encryption has not yet been fully tested for these projects. This will be done in a future release.
    - A workaround for the QSPI plib is in place for this release. When regenerating the code, the workaround must NOT be overwritten.

* SDMMC driver may block inside an internal routine while using the HTTP server and locks up the whole application. No workaround exists.
* EDBG programming of a hex file on the SAME54 XPRO board may fail.
* There are some mandatory MISRA violations in some of the repo files. They will be fixed in some next release:
    - System: sys_debug.c, sys_command.c
    - TCP/IP: smtpc.c, ndp.c, custom_http_net_app.c, icmpv6.c,
* Adding TCP/IP components to the project must be done using the TCP/IP Configurators in order to 
  load the dependencies properly. Adding the TCP/IP components from the "Available Components",
  without using configurator, might cause build issues.
* Similarly, remove the TCP/IP components by using the TCP/IP Configurator for the respective layer
* Do not add the TCP/IP components to the project by using both the TCP/IP Configurator and manually adding from "Available Components".
* Do not delete the TCP/IP Configurators from project graph.
* Interactive help using the "Show User Manual Entry" in the Right-click menu for configuration options provided
  by this module is not yet available from within the MPLAB Harmony Configurator (MHC).
  Please see the "Configuring the Library" section in the help documentation in the doc folder for this module instead.
  Help is available in both CHM and PDF formats.

* The tcpip_client_server application does not currently have complete documentation. The commands to 
  use this demonstration are provided in the run time help available as console commands.
  This will be added in a future release.
* When the File System service is used in a demonstration application, make sure to set 
  "Maximum Simultaneous File Access" to a number above 10.
  Especially the HTTP server needs to open multiple files simultaneously.
* When the RTOS is enabled, make sure to include RTOS system API’s (like Time Delay functions)
  in application task to give other tasks the chance to execute.
  This can be achieved by enabling the "Use Task Delay" from Application Configuration -> RTOS Configuration.
  


### RELEASE CONTENTS

This topic lists the contents of this release and identifies each module.

#### Description

This table lists the contents of this release, including a brief description.


| Folder                                | Description                                                          | 
| --- | --- | 
| net/apps/berkeley_tcp_client          | TCP Client demo using BSD API                                        |
| net/apps/berkeley_tcp_server          | TCP Server demo using BSD API                                        |
| net/apps/berkeley_udp_client          | UDP Client demo using BSD API                                        |
| net/apps/berkeley_udp_relay           | UDP Relay demo using BSD API                                         |
| net/apps/berkeley_udp_server          | UDP Server demo using BSD API                                        |
| net/apps/snmpv3_nvm_mpfs              | SNMPv3 demo with MPFS file system using NVM storage                  |
| net/apps/snmpv3_sdcard_fatfs          | SNMPv3 demo with  FAT FS file system using external SD card          |
| net/apps/tcpip_client_server          | Multi-threaded example with TCP and UDP server and client threads    |
| net/apps/tcpip_tcp_client             | TCP Client demo using Harmony native API                             |
| net/apps/tcpip_tcp_client_server      | TCP Client and Server demo using Harmony native API                  |
| net/apps/tcpip_tcp_server             | TCP Server demo using Harmony native API                             |
| net/apps/tcpip_udp_client             | UDP Client demo using Harmony native API                             |
| net/apps/tcpip_udp_client_server      | UDP Client and Server demo using Harmony native API                  |
| net/apps/tcpip_udp_server             | UDP Server demo using Harmony native API                             |
| net/apps/web_net_server_nvm_mpfs      | Web server with MPFS file system using the NVM storage               |
| net/apps/web_net_server_qspi_mpfs     | Web server with MPFS file system on the onboard QSPI Flash Memory    |
| net/apps/web_net_server_sdcard_fatfs  | Web server with FAT FS file system on the external SD card           |
| net/apps/web_server_nvm_mpfs          | Old style web server with MPFS file system using the NVM storage     |
| net/apps/web_server_sdcard_fatfs      | Old style web server with FAT FS file system on the external SD card |


## Net Release v3.3.0 (May, 2019)
### ADDITIONS AND UPDATES FOR  3.3.0:

- **New part support** -This release introduces initial support for [PIC32MZEF](https://www.microchip.com/design-centers/32-bit/pic-32-bit-mcus/pic32mz-ef-family) and [PIC32MZDA](https://www.microchip.com/design-centers/32-bit/pic-32-bit-mcus/pic32mz-da-family) families of 32-bit microcontrollers.

- **Updated Applications**

The following table provides the list of the updated applications including bare metal and FreeRTOS configurations:

| Application                 | Platform                        | Description                                                          |
| ------------ | ------------ | ------------ |
| berkeley_tcp_client         | PIC32MZEF, PIC32MZDA            | TCP Client demo using BSD API                                        |
| berkeley_tcp_server         | PIC32MZEF, PIC32MZDA            | TCP Server demo using BSD API                                        |
| berkeley_udp_client         | PIC32MZEF, PIC32MZDA            | UDP Client demo using BSD API                                        |
| berkeley_udp_relay          | PIC32MZEF, PIC32MZDA            | UDP Relay demo using BSD API                                         |
| berkeley_udp_server         | PIC32MZEF, PIC32MZDA            | UDP Server demo using BSD API                                        |
| snmpv3_nvm_mpfs             | PIC32MZEF                       | SNMPv3 demo with MPFS file system using NVM storage                  |
| snmpv3_sdcard_fatfs         | PIC32MZEF                       | SNMPv3 demo with  FAT FS file system using external SD card          |
| tcpip_client_server         | PIC32MZEF, PIC32MZDA            | Multi-threaded example with TCP and UDP server and client threads    |
| tcpip_tcp_client            | PIC32MZEF, PIC32MZDA            | TCP Client demo using Harmony native API                             |
| tcpip_tcp_client_server     | PIC32MZEF, PIC32MZDA            | TCP Client and Server demo using Harmony native API                  |
| tcpip_tcp_server            | PIC32MZEF, PIC32MZDA            | TCP Server demo using Harmony native API                             |
| tcpip_udp_client            | PIC32MZEF, PIC32MZDA            | UDP Client demo using Harmony native API                             |
| tcpip_udp_client_server     | PIC32MZEF, PIC32MZDA            | UDP Client and Server demo using Harmony native API                  |
| tcpip_udp_server            | PIC32MZEF, PIC32MZDA            | UDP Server demo using Harmony native API                             |
| web_net_server_nvm_mpfs     | PIC32MZEF, PIC32MZDA            | Web server with MPFS file system using the NVM storage               |
| web_server_nvm_mpfs         | PIC32MZEF                       | Old style web server with MPFS file system using the NVM storage     |
| web_net_server_sdcard_fatfs | PIC32MZEF, PIC32MZDA            | Web server with FAT FS file system on the external SD Card           |


- **Updated Drivers**

The following table provides the list of new support for MAC drivers:

| Driver                | Platform |
| --------- | --------- |
| drv_ethmac.c, drv_ethmac_lib.c   | PIC32MZEF, PIC32MZDA |


- **Updated Utililties**

The following table provides the list of updated utilities for the TCP/IP stack :

| Module                | Description                                             |
| ------ | ------ |
| mpfs.jar              | - This tool updates and replaces the existent mpfs2.jar.<br> - The new MPFS utility version 3.3.3 is capable of parsing SSI directives. <br> - The utility is intended to be used with the HTTP_NET server. <br> - It generates the http_net_print.h and http_net_print.c files. <br> - HTTP_Net Application initialization and HTTP registration are now part of http_net_print.c file. <br> - The tool has been updated to disable the default compression of html files. <br> - When the application Custom Template is enabled, the Web pages are added to the project. <br> - The Source directory path is the source of the web page files. <br> - The Destination directory path is the location where the web page files will be copied. <br> - The Destination location is not currently configurable. <br> - The maximum number of supported web page files is currently 100. <br> - The NVM MPFS demos use a customized pre-build step to generate the mpfs_net_img.c file: <br> java -jar ../../../../utilities/mpfs_generator/mpfs.jar /c /z "snmp.bib" "../src/web_pages" "../src" "mpfs_net_img.c" <br> - If there is no JRE installed on the building machine, the project will not build. The pre-build step needs to be disabled manually|

- **Bug Fixes**

The following table provides the list of bug fixes in this release:

| Module                | Description                                             |
| ------ | ------ |
| TCP/IP Manager        | Created function to update the default interface at run time after an interface is changed or brought down |
| TCP/IP Packet logger  | Improvements to the packet logger. Now is part of the main source code.                                           |
| HTTP_NET              | Fixed an error in the upload state machine that was discarding the MPFS signature from the incoming stream |
| drv_extphy_smsc9303   | Added patch to eliminate leaks based on the customer suggestion.                                           |



### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB X IDE v5.15](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB XC32 C/C++ Compiler v2.15](https://www.microchip.com/mplab/compilers) or later
- [IAR Embedded Workbench for ARM - version 8.32.1] (https://www.iar.com/iar-embedded-workbench/) or later for SAMA5D2 projects
- Harmony net repository, 3.3.0

In order to regenerate source code for any of the applications, you will also need the following to be installed:

- MPLAB Harmony Configurator (MHC) v.3.2.0
- Harmony mplabx_plugin repository, 3.2.0
- Harmony bsp repository, 3.2.1
- Harmony csp repository, 3.2.1
- Harmony core repository, 3.2.1
- Harmony dev_packs repository, 3.2.1
- Harmony crypto repository, 3.2.1
- CMSIS-FreeRTOS repository, 10.0.1 if building a FreeRTOS project (from www.github.com/arm-software/cmsis-freertos)

#### Development Kit Support

This release supports applications for the following development kits

| Development Kits |
| --- |
| [PIC32MZEF Embedded Connectivity with FPU Starter Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/DM320007-C) |
| [PIC32MZ Embedded Graphics with Stacked DRAM (DA) Starter Kit](https://www.microchip.com/developmenttools/ProductDetails/DM320010-C) |
| [SAM A5D2 Xplained Ultra Evaluation Kit](https://www.microchip.com/Developmenttools/ProductDetails/ATSAMA5D2C-XULT) |
| [SAM E54 Xplained Pro Evaluation Kit](https://www.microchip.com/developmenttools/ProductDetails/ATSAME54-XPRO) |
| [SAM E70 Xplained Ultra Evaluation Kit](https://www.microchip.com/DevelopmentTools/ProductDetails.aspx?PartNO=ATSAME70-XULT) |
| [SAM V71 Xplained Ultra Evaluation Kit](https://www.microchip.com/DevelopmentTools/ProductDetails.aspx?PartNO=ATSAMV71-XULT) |

### KNOWN ISSUES

The current known issues are as follows:

* For projects using the mpfs_img2.c and mpfs_img2_net.c files, these may be absent from the project after regeneration. The MPFS module needs to be removed and re-added to the project.
* The SAME70 XULT board has some issues and detection of the PHY board may fail. This issue is under investigation.
    - For now the SAME70 projects are present in the repo just for reference and not for actual running demos.
    - For now the SAME70 projects are generated using the LAN8740 PHY instead of KSZ8061 which is the default PHY for this board.
* The ICD4 loads the reset line of the SAM V71 Xplained Ultra board. When running demo projects using the V71, the ICD4 flex cable should be removed after programming to run the application.
* Interactive help using the Show User Manual Entry in the Right-click menu for configuration options provided by this module is not yet available from within the MPLAB Harmony Configurator (MHC).  Please see the *Configuring the Library* section in the help documentation in the doc folder for this Harmony 3 module instead.  Help is available in both CHM and PDF formats.
* The SAM E70/V71/E54/A5D2 processors do not have a built in factory pre-programmed MAC address.
    - The XULT boards use the AT24MAC402 serial EEPROM for having an unique MAC address per board.
    - Currently the demo applications do not use the AT24MAC402 EEPROM for reading the MAC address. This will be added in a future release.

* The SAME70 and SAMV71 TCPIP demos with SDCARD, won’t work with optimisation 0. Use optimization level 1.

* SAMA5D2 projects known issues:
    - The SNMP protocol and corresponding demo applications is not be available.
    - IPv6 ULA generation is not available
    - ZCLL module is not supported
    - Crypto library is now supported on the SAMA5D2 processor. However the net_pres layer has not yet been fully tested for these projects. This will be done in a next release when wolfSSL support is added to the TCP/IP stack.  
    - A workaround for the QSPI plib is in place for this release. When regenerating the code, the workaround must NOT be overwritten.

* SDMMC driver may block inside an internal routine while using the HTTP server and locks up the whole application. No workaround exists.
* The operation over the secure connections using wolfSSL has not been added to the net repo for this release.
    - There are 2 wolfSSL TCP demos in the crypto repo.
    - The web_net_server_nvm_mpfs demo does not currently support encrypted connections.
    - The wolfSSL third party package cannot be added to the application as an encryption provider.
* EDBG programming of a hex file on the SAME54 XPRO board may fail.
* There are some mandatory MISRA violations in some of the repo files. They will be fixed in the next release:
    - System: sys_debug.c, sys_command.c
    - TCP/IP: smtpc.c, ndp.c, custom_http_net_app.c, icmpv6.c,
* Adding TCP/IP components to the project must be done using the TCP/IP Configurators in order to 
  load the dependencies properly. Adding the TCP/IP components from the "Available Components",
  without using configurator, might cause build issues.
* Similarly, remove the TCP/IP components by using the TCP/IP Configurator for the respective layer
* Do not add the TCP/IP components to the project by using both the TCP/IP Configurator and manually adding from "Available Components".
* Do not delete the TCP/IP Configurators from project graph.
* When multiple modules depending on the "netPres" component are added to the project (like Berkeley, HTTP_NET, TELNET etc.),
  multiple 'netPres' nodes will be shown on TCP/IP STACK group box.
  But internally all these TCP/IP modules are using a single instance of the NET_PRES service.
  So only one instance of NET_PRES is needed.
* Interactive help using the "Show User Manual Entry" in the Right-click menu for configuration options provided
  by this module is not yet available from within the MPLAB Harmony Configurator (MHC).
  Please see the "Configuring the Library" section in the help documentation in the doc folder for this module instead.
  Help is available in both CHM and PDF formats.

* The tcpip_client_server application does not currently have complete documentation. The commands to 
  use this demonstration are provided in the run time help available as console commands.
  This will be added in a future release.
* When the File System service is used in a demonstration application, make sure to set 
  "Maximum Simultaneous File Access" to a number above 10.
  Especially the HTTP server needs to open multiple files simultaneously.
* When the RTOS is enabled, make sure to include RTOS system API’s (like Time Delay functions)
  in application task to give other tasks the chance to execute.
  This can be achieved by enabling the "Use Task Delay" from Application Configuration -> RTOS Configuration.
  


### RELEASE CONTENTS

This topic lists the contents of this release and identifies each module.

#### Description

This table lists the contents of this release, including a brief description, and the release type (Alpha, Beta, Production, or Vendor).


| Folder                                | Description                                                          | 
| --- | --- | 
| net/apps/berkeley_tcp_client          | TCP Client demo using BSD API                                        |
| net/apps/berkeley_tcp_server          | TCP Server demo using BSD API                                        |
| net/apps/berkeley_udp_client          | UDP Client demo using BSD API                                        |
| net/apps/berkeley_udp_relay           | UDP Relay demo using BSD API                                         |
| net/apps/berkeley_udp_server          | UDP Server demo using BSD API                                        |
| net/apps/snmpv3_nvm_mpfs              | SNMPv3 demo with MPFS file system using NVM storage                  |
| net/apps/snmpv3_sdcard_fatfs          | SNMPv3 demo with  FAT FS file system using external SD card          |
| net/apps/tcpip_client_server          | Multi-threaded example with TCP and UDP server and client threads    |
| net/apps/tcpip_tcp_client             | TCP Client demo using Harmony native API                             |
| net/apps/tcpip_tcp_client_server      | TCP Client and Server demo using Harmony native API                  |
| net/apps/tcpip_tcp_server             | TCP Server demo using Harmony native API                             |
| net/apps/tcpip_udp_client             | UDP Client demo using Harmony native API                             |
| net/apps/tcpip_udp_client_server      | UDP Client and Server demo using Harmony native API                  |
| net/apps/tcpip_udp_server             | UDP Server demo using Harmony native API                             |
| net/apps/web_net_server_nvm_mpfs      | Web server with MPFS file system using the NVM storage               |
| net/apps/web_net_server_qspi_mpfs     | Web server with MPFS file system on the onboard QSPI Flash Memory    |
| net/apps/web_net_server_sdcard_fatfs  | Web server with FAT FS file system on the external SD card           |
| net/apps/web_server_nvm_mpfs          | Old style web server with MPFS file system using the NVM storage     |
| net/apps/web_server_sdcard_fatfs      | Old style web server with FAT FS file system on the external SD card |


## Net Release v3.2.0 (March, 2019)
### ADDITIONS AND UPDATES FOR  3.2.0:

- **New part support** -This release introduces initial support for [SAM E5x](https://www.microchip.com/design-centers/32-bit/sam-32-bit-mcus/sam-e-mcus) families of 32-bit microcontrollers and [SAM A5D2](https://www.microchip.com/design-centers/32-bit-mpus) families of 32-bit MPUs.

- **New Applications**

The following table provides the list of the new applications including bare metal and FreeRTOS configurations:

| Application                 | Platform                        | Description                                                          |
| ------------ | ------------ |  ------------ |
| web_net_server_qspi_mpfs    | SAMA5D2                         | Web server with MPFS file system using the onboard QSPI Flash Memory |
| web_net_server_sdcard_fatfs | SAMA5D2, SAME54, SAMV71, SAME70 | Web server with FAT FS file system on the external SD Card           |


- **Updated Applications**

The following table provides the list of the updated applications including bare metal and FreeRTOS configurations:

| Application                 | Platform                        | Description                                                          |
| ------------ | ------------ | ------------ |
| berkeley_tcp_client         | SAME54, SAMV71, SAME70          | TCP Client demo using BSD API                                        |
| berkeley_tcp_server         | SAME54, SAMV71, SAME70          | TCP Server demo using BSD API                                        |
| berkeley_udp_client         | SAME54, SAMV71, SAME70          | UDP Client demo using BSD API                                        |
| berkeley_udp_relay          | SAME54, SAMV71, SAME70          | UDP Relay demo using BSD API                                         |
| berkeley_udp_server         | SAME54, SAMV71, SAME70          | UDP Server demo using BSD API                                        |
| snmpv3_nvm_mpfs             | SAME54, SAMV71, SAME70          | SNMPv3 demo with MPFS file system using NVM storage                  |
| snmpv3_sdcard_fatfs         | SAME54, SAMV71, SAME70          | SNMPv3 demo with  FAT FS file system using external SD card          |
| tcpip_client_server         | SAMA5D2, SAME54, SAME70         | Multi-threaded example with TCP and UDP server and client threads    |
| tcpip_tcp_client            | SAMA5D2, SAME54, SAMV71, SAME70 | TCP Client demo using Harmony native API                             |
| tcpip_tcp_client_server     | SAMA5D2, SAME54, SAMV71, SAME70 | TCP Client and Server demo using Harmony native API                  |
| tcpip_tcp_server            | SAMA5D2, SAME54, SAMV71, SAME70 | TCP Server demo using Harmony native API                             |
| tcpip_udp_client            | SAMA5D2, SAME54, SAMV71, SAME70 | UDP Client demo using Harmony native API                             |
| tcpip_udp_client_server     | SAMA5D2, SAME54, SAMV71, SAME70 | UDP Client and Server demo using Harmony native API                  |
| tcpip_udp_server            | SAMA5D2, SAME54, SAMV71, SAME70 | UDP Server demo using Harmony native API                             |
| web_net_server_nvm_mpfs     | SAME54, SAMV71, SAME70          | Web server with MPFS file system using the NVM storage               |
| web_server_nvm_mpfs         | SAME54, SAMV71, SAME70          | Old style web server with MPFS file system using the NVM storage     |
| web_server_sdcard_fatfs     | SAME54, SAMV71, SAME70          | Old style web server with FAT FS file system on the external SD card |



- **Updated Drivers**

The following table provides the list of new support for MAC drivers:

| Driver                | Platform |
| --------- | --------- |
| drv_gmac_lib_samA5D2  | SAMA5D2  |
| drv_gmac_lib_samE5x   | SAME54   |

- **Bug Fixes**

The following table provides the list of bug fixes in this release:

| Module                | Description                                             |
| ------ | ------ |
| UDP                   | Updated the Disconnect to keep the remote port binding  |
| TFTP                  | Fixed PUT command behavior during file upload           |
| HTTP_NET, Announce    | Removed the alloca usage                                |
| All                   | Clean up code for removing IAR build errors             |


### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB X IDE v5.15](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB XC32 C/C++ Compiler v2.15](https://www.microchip.com/mplab/compilers) or later
- [IAR Embedded Workbench for ARM - version 8.32.1] (https://www.iar.com/iar-embedded-workbench/) or later for SAMA5D2 projects
- Harmony net repository, 3.2.0

In order to regenerate source code for any of the applications, you will also need the following to be installed:

- MPLAB Harmony Configurator (MHC) v.3.2.0
- Harmony mplabx_plugin repository, 3.2.0
- Harmony bsp repository, 3.2.0
- Harmony csp repository, 3.2.0
- Harmony core repository, 3.2.0
- Harmony dev_packs repository, 3.2.0
- Harmony crypto repository, 3.2.0
- CMSIS-FreeRTOS repository, 10.0.1 if building a FreeRTOS project (from www.github.com/arm-software/cmsis-freertos)

#### Development Kit Support

This release supports applications for the following development kits

| Development Kits |
| --- |
| [SAM A5D2 Xplained Ultra Evaluation Kit](https://www.microchip.com/Developmenttools/ProductDetails/ATSAMA5D2C-XULT) |
| [SAM E54 Xplained Pro Evaluation Kit](https://www.microchip.com/developmenttools/ProductDetails/ATSAME54-XPRO) |
| [SAM E70 Xplained Ultra Evaluation Kit](https://www.microchip.com/DevelopmentTools/ProductDetails.aspx?PartNO=ATSAME70-XULT) |
| [SAM V71 Xplained Ultra Evaluation Kit](https://www.microchip.com/DevelopmentTools/ProductDetails.aspx?PartNO=ATSAMV71-XULT) |

### KNOWN ISSUES

The current known issues are as follows:

* The SAME70 XULT board has some issues and detection of the PHY board may fail. This issue is under investigation.
    - For now the SAME70 projects are present in the repo just for reference and not for actual running demos.
    - For now the SAME70 projects are generated using the LAN8740 PHY.
* The ICD4 loads the reset line of the SAM V71 Xplained Ultra board. When running demo projects using the V71, the ICD4 flex cable should be removed after programming to run the application.
* Interactive help using the Show User Manual Entry in the Right-click menu for configuration options provided by this module is not yet available from within the MPLAB Harmony Configurator (MHC).  Please see the *Configuring the Library* section in the help documentation in the doc folder for this Harmony 3 module instead.  Help is available in both CHM and PDF formats.

* The SAME70 and SAMV71 TCPIP demos with SDCARD, won’t work with optimisation 0. Use optimization level 1.

* SAMA5D2 projects known issues:
    - The SNMP protocol and corresponding demo applications is not be available.
    - IPv6 ULA generation is not available
    - ZCLL module is not supported
    - Crypto library is not supported
    - A workaround for the QSPI plib is in place for this release. When regenerating the code, the workaround must NOT be overwritten.

* SDMMC driver may block inside an internal routine while using the HTTP server and locks up the whole application. No workaround exists.
* The operation over the secure connections using wolfSSL has not been added to the net repo for this release.
    - There are 2 wolfSSL TCP demos in the crypto repo.
* For projects using the mpfs_img2.c and mpfs_img2_net.c files, these may be absent from the project after regeneration. The MPFS module needs to be removed and re-added to the project.
* EDBG programming of a hex file on the SAME54 XPRO board may fail.
* There are some mandatory MISRA violations in some of the repo files. They will be fixed in the next release:
    - System: sys_debug.c, sys_command.c
    - TCP/IP: smtpc.c, ndp.c, custom_http_net_app.c, icmpv6.c,



### RELEASE CONTENTS

This topic lists the contents of this release and identifies each module.

#### Description

This table lists the contents of this release, including a brief description, and the release type (Alpha, Beta, Production, or Vendor).


| Folder                                | Description                                                          | Release Type |
| --- | --- | --- |
| net/apps/berkeley_tcp_client          | TCP Client demo using BSD API                                        | Beta |
| net/apps/berkeley_tcp_server          | TCP Server demo using BSD API                                        | Beta |
| net/apps/berkeley_udp_client          | UDP Client demo using BSD API                                        | Beta |
| net/apps/berkeley_udp_relay           | UDP Relay demo using BSD API                                         | Beta |
| net/apps/berkeley_udp_server          | UDP Server demo using BSD API                                        | Beta |
| net/apps/snmpv3_nvm_mpfs              | SNMPv3 demo with MPFS file system using NVM storage                  | Beta |
| net/apps/snmpv3_sdcard_fatfs          | SNMPv3 demo with  FAT FS file system using external SD card          | Beta |
| net/apps/tcpip_client_server          | Multi-threaded example with TCP and UDP server and client threads    | Beta |
| net/apps/tcpip_tcp_client             | TCP Client demo using Harmony native API                             | Beta |
| net/apps/tcpip_tcp_client_server      | TCP Client and Server demo using Harmony native API                  | Beta |
| net/apps/tcpip_tcp_server             | TCP Server demo using Harmony native API                             | Beta |
| net/apps/tcpip_udp_client             | UDP Client demo using Harmony native API                             | Beta |
| net/apps/tcpip_udp_client_server      | UDP Client and Server demo using Harmony native API                  | Beta |
| net/apps/tcpip_udp_server             | UDP Server demo using Harmony native API                             | Beta |
| net/apps/web_net_server_nvm_mpfs      | Web server with MPFS file system using the NVM storage               | Beta |
| net/apps/web_net_server_qspi_mpfs     | Web server with MPFS file system on the onboard QSPI Flash Memory    | Beta |
| net/apps/web_net_server_sdcard_fatfs  | Web server with FAT FS file system on the external SD card           | Beta |
| net/apps/web_server_nvm_mpfs          | Old style web server with MPFS file system using the NVM storage     | Beta |
| net/apps/web_server_sdcard_fatfs      | Old style web server with FAT FS file system on the external SD card | Beta |


## Net Release v3.1.0 (December, 2018)
### ADDITIONS AND UPDATES FOR  3.1.0:


- **Updated Applications**

The following table provides the list of the updated applications:

| Application                 | Platform                        | Description                                                          |
| ------------ | ------------ | ------------ |
| berkeley_tcp_client         | SAMV71, SAME70                  | TCP Client demo using BSD API                                        |
| berkeley_tcp_server         | SAMV71, SAME70                  | TCP Server demo using BSD API                                        |
| berkeley_udp_client         | SAMV71, SAME70                  | UDP Client demo using BSD API                                        |
| berkeley_udp_relay          | SAMV71, SAME70                  | UDP Relay demo using BSD API                                         |
| berkeley_udp_server         | SAMV71, SAME70                  | UDP Server demo using BSD API                                        |
| snmpv3_nvm_mpfs             | SAMV71, SAME70                  | SNMPv3 demo with MPFS file system using NVM storage                  |
| snmpv3_sdcard_fatfs         | SAMV71, SAME70                  | SNMPv3 demo with  FAT FS file system using external SD card          |
| tcpip_client_server         | SAME70                          | Multi-threaded example with TCP and UDP server and client threads    |
| tcpip_tcp_client            | SAMV71, SAME70                  | TCP Client demo using Harmony native API                             |
| tcpip_tcp_client_server     | SAMV71, SAME70                  | TCP Client and Server demo using Harmony native API                  |
| tcpip_tcp_server            | SAMV71, SAME70                  | TCP Server demo using Harmony native API                             |
| tcpip_udp_client            | SAMV71, SAME70                  | UDP Client demo using Harmony native API                             |
| tcpip_udp_client_server     | SAMV71, SAME70                  | UDP Client and Server demo using Harmony native API                  |
| tcpip_udp_server            | SAMV71, SAME70                  | UDP Server demo using Harmony native API                             |
| web_net_server_nvm_mpfs     | SAME70                          | Web server with MPFS file system using the NVM storage               |
| web_server_nvm_mpfs         | SAMV71, SAME70                  | Old style web server with MPFS file system using the NVM storage     |
| web_server_sdcard_fatfs     | SAMV71, SAME70                  | Old style web server with FAT FS file system on the external SD card |


- **Updated Drivers**

The following table provides the list of new support for MAC drivers:

| Driver    | Platform      |
| --------- | ------------- |
| drv_gmac  | SAME70, SAMV71 |


- **Bug Fixes**

The following table provides the list of bug fixes in this release:

| Module                | Description                                             |
| ------ | ------ |
| UDP                   | Fixed comments in the UDP header for TCPIP_UDP_OptionSet() not providing 100% compatibility with the BSD setsockopt().    |
| UDP                   | Provided better explanation for some of the UDP_MULTICAST_FLAGS flags.                                                    |
| HTTP_NET              | Added HTTP_NET connection events: open, close, timeout                                                                    |
| HTTP_NET              | Updated the HTTP app with the new SMTPC module changes. Old SMTP module is obsolete now.                                  |
| HTTP_NET              | Added case-insensitive comparison for the HTTP headers. Added stricmp() helper.                                           |
| HTTP_NET              | Added file open/close events.                                                                                             |
| HTTP_NET              | Fixed the legacy mechanism of using callback position. Cleared the callbackPos counter only when the dynamic variable print is called first time. |
| Telnet                | Better processing of the telnet options.                                                                                  |
| Telnet                | Improved the telnet usage of the NET_PRES layer.                                                                          |
| TCPIP Manager         | Updated the processing of the network interface flags to avoid the MAC Initialize flag to remain set if the network interface is stopped before completing initialization.|
| TCPIP Manager         | Fixed the selection of the DNS client as default only if the module is enabled.                                           |
| TCPIP Manager         | Fixed the tcpip_heap_external semaphore delete not checking that it was successfully created.                             |
| mDNS                  | Fixed the use of the wrong field to detect an authoritative reply. The field "qr" was used instead of "aa" when deciding for an authoritative answer.|
| IPv6                  | Fixed the routine converting a string to an IPv6 address.                                                                 |
| TCPIP Commands        | Added missing Close() when MIIM operation failed.                                                                         |

### TESTED WITH:

#### Software Requirements

Before using MPLAB Harmony Net, ensure that the following are installed:
- MPLAB X IDE 5.10 or later
- MPLAB XC32 C/C++ Compiler V2.15 or later
- MPLAB Harmony Configurator 3.1.0
- Harmony net repository, 3.1.0
- Harmony bsp repository, 3.1.0
- Harmony csp repository, 3.10.0
- Harmony core repository, 3.1.0
- Harmony dev_packs repository, 3.1.0
- Harmony mhc repository, 3.1.0
- Harmony mplabx_plugin repository, 3.1.0
- CMSIS-FreeRTOS repository, 10.0.1 for building the FreeRTOS projects (from www.github.com/arm-software/cmsis-freertos)



### KNOWN ISSUES

The current known issues are as follows:

* Adding TCP/IP components to the project must be done using the TCP/IP Configurators in order to 
  load the dependencies properly. Adding the TCP/IP components from the "Available Components",
  without using configurator, might cause build issues.
* Similarly, remove the TCP/IP components by using the TCP/IP Configurator for the respective layer
* Do not add the TCP/IP components to the project by using both the TCP/IP Configurator and manually adding from "Available Components".
* Do not delete the TCP/IP Configurators from project graph.
* When multiple modules depending on the "netPres" component are added to the project (like Berkeley, HTTP_NET, TELNET etc.),
  multiple 'netPres' nodes will be shown on TCP/IP STACK group box.
  But internally all these TCP/IP modules are using a single instance of the NET_PRES service.
  So only one instance of NET_PRES is needed.
* Interactive help using the "Show User Manual Entry" in the Right-click menu for configuration options provided
  by this module is not yet available from within the MPLAB Harmony Configurator (MHC).
  Please see the "Configuring the Library" section in the help documentation in the doc folder for this module instead.
  Help is available in both CHM and PDF formats.

* The web_net_server_nvm_mpfs demo does not currently support encrypted connections.
  The wolfSSL third party package cannot be added to the application as an encryption provider.
  This will be added in a future release.
* The tcpip_client_server application does not currently have documentation. The commands to 
  use this demonstration are provided in the run time help available as console commands.
  This will be added in a future release.
* When the File System service is used in a demonstration application, make sure to set 
  "Maximum Simultaneous File Access" to a number above 10.
  Especially the HTTP server needs to open multiple files simultaneously.
* When the RTOS is enabled, make sure to include RTOS system API’s (like Time Delay functions)
  in application task to give other tasks the chance to execute.
  This can be achieved by enabling the "Use Task Delay" from Application Configuration -> RTOS Configuration.
  

### RELEASE CONTENTS

This topic lists the contents of this release and identifies each module.

#### Description

This table lists the contents of this release, including a brief description, and the release type (Alpha, Beta, Production, or Vendor).


| Folder                                | Description                                                          | Release Type |
| --- | --- | --- |
| net/apps/tcpip_udp_client             | UDP Client demo using Harmony native API                             | Beta |
| net/apps/tcpip_udp_client_server      | UDP Client and Server demo using Harmony native API                  | Beta |
| net/apps/tcpip_udp_server             | UDP Server demo using Harmony native API                             | Beta |
| net/apps/web_net_server_nvm_mpfs      | Web server with MPFS file system using the NVM storage               | Beta |
| net/apps/web_server_nvm_mpfs          | Old style web server with MPFS file system using the NVM storage     | Beta |
| net/apps/web_server_sdcard_fatfs      | Old style web server with FAT FS file system on the external SD card | Beta |
| net/apps/berkeley_tcp_client          | TCP Client demo using BSD API                                        | Beta |
| net/apps/berkeley_tcp_server          | TCP Server demo using BSD API                                        | Beta |
| net/apps/berkeley_udp_client          | UDP Client demo using BSD API                                        | Beta |
| net/apps/berkeley_udp_relay           | UDP Relay demo using BSD API                                         | Beta |
| net/apps/berkeley_udp_server          | UDP Server demo using BSD API                                        | Beta |
| net/apps/snmpv3_nvm_mpfs              | SNMPv3 demo with MPFS file system using NVM storage                  | Beta |
| net/apps/snmpv3_sdcard_fatfs          | SNMPv3 demo with  FAT FS file system using external SD card          | Beta |
| net/apps/tcpip_tcp_client             | TCP Client demo using Harmony native API                             | Beta |
| net/apps/tcpip_tcp_client_server      | TCP Client and Server demo using Harmony native API                  | Beta |
| net/apps/tcpip_tcp_server             | TCP Server demo using Harmony native API                             | Beta |
| net/apps/tcpip_client_server          | Multi-threaded example with TCP and UDP server and client threads    | Beta |
















