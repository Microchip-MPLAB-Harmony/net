# Microchip MPLAB Harmony 3 Release Notes
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

* SDMMC driver may block inside an internal routine while using the HTTP server and locks up th ewhole application. No workaround exists.
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


