# Microchip MPLAB Harmony 3 Release Notes
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
















