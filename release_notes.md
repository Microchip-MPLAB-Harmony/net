# Microchip MPLAB® Harmony 3 Release Notes
## Net Release v3.14.3 (August, 2025)
### ADDITIONS AND UPDATES FOR v3.14.3:

- **New Features**
    - Addition of VLAN support - beta

- **Applications**
  - MPLAB® Harmony 3 provides Net library application examples to accelerate learning and reduce the development cycles for your embedded systems with reusable software components. The applications examples are available in the [product family specific repository](apps/readme.md).


- **Improvements and Bug Fixes**
	-	The following table provides the list of improvements and bug fixes in the v3.14.3 release:

		| Module        | Description |
		| ------------- | ----------- |
        | MAC drivers   | Provided configuration support for multi-client |
        | GMAC driver   | Fix for the PIC32CZ GMAC driver name |
        | GMAC driver   | PHY configuration fix for PIC32CK_SG/GC |
        | EMAC driver   | Fix for the EMAC driver descriptor pool clear |
        | WS Client     | Fix for minimum payload length for medium frames |
        | TCP/IP stack  | Provided initial/beta VLAN support |
        

### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.25](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.60](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, v3.14.3
- Harmony net demo apps repositories
- MPLAB Code Configurator (MCC), v5.5.2 or later

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, v3.15.2
- Harmony csp repository, v3.22.4
- Harmony bsp repository, v3.22.0
- Harmony crypto repository, v3.8.2
- harmony-services repository, v1.5.0
- wolfMQTT v1.19.2 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.19.2) for demos requiring wolfMQTT support
- wolfSSL v5.4.0 (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v5.4.0) for demos requiring encryption wolfSSL support


## Net Release v3.14.2 (July, 2025)
### ADDITIONS AND UPDATES FOR v3.14.2:

- **New Features**
	- None

- **Applications**
  - MPLAB® Harmony 3 provides Net library application examples to accelerate learning and reduce the development cycles for your embedded systems with reusable software components. The applications examples are available in the [product family specific repository](apps/readme.md).


- **Improvements and Bug Fixes**
	-	The following table provides the list of improvements and bug fixes in the v3.14.2 release:

		| Module        | Description |
		| ------------- | ----------- |
        | MCC scripts   | Updates for the LAN865x module |

### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.25](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.60](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, v3.14.2
- Harmony net demo apps repositories
- MPLAB Code Configurator (MCC), v5.5.2 or later

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, v3.15.2
- Harmony csp repository, v3.22.4
- Harmony bsp repository, v3.22.0
- Harmony crypto repository, v3.8.2
- harmony-services repository, v1.5.0
- wolfMQTT v1.19.2 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.19.2) for demos requiring wolfMQTT support
- wolfSSL v5.4.0 (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v5.4.0) for demos requiring encryption wolfSSL support


### KNOWN ISSUES
- Some MISRA C required violations are still reported by the Coverity analysis tool.
    - The issues are not in the stack modules.
    - They will be fixed in a future release.

## Net Release v3.14.1 (June, 2025)
### ADDITIONS AND UPDATES FOR v3.14.1:

- **New Features**
	- None

- **Applications**
  - MPLAB® Harmony 3 provides Net library application examples to accelerate learning and reduce the development cycles for your embedded systems with reusable software components. The applications examples are available in the [product family specific repository](apps/readme.md).


- **Improvements and Bug Fixes**
	-	The following table provides the list of improvements and bug fixes in the v3.14.1 release:

		| Module        | Description |
		| ------------- | ----------- |
        | GMAC          | Added missing include of tcpip_mac_object.h |
        | GMAC          | Fix for SingleListTailAdd wrong cast |
        | GMAC          | Fix GMAC lockup under heavy traffic |
        | TFTPc      	| Fix for the TFTPIsPutReady function |
        | ENCx24J600    | Fix for the DRV_SPI_TRANSFER_SETUP call |
        | FTPc          | Fix for usage of ctrlTxBuff size |
        | PHY drivers   | Updated RMII configuration for PIC32M platforms |
        | WSC           | Basic authentication support has been updated | 
        | All modules   | MISRA C violations fixes |
        | All modules   | Fixes for xc32 v5.0 reported issues |

### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.25](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.60](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, v3.14.1
- Harmony net demo apps repositories
- MPLAB Code Configurator (MCC), v5.5.2 or later

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, v3.15.2
- Harmony csp repository, v3.22.4
- Harmony bsp repository, v3.22.0
- Harmony crypto repository, v3.8.2
- harmony-services repository, v1.5.0
- wolfMQTT v1.19.2 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.19.2) for demos requiring wolfMQTT support
- wolfSSL v5.4.0 (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v5.4.0) for demos requiring encryption wolfSSL support


### KNOWN ISSUES
- Some MISRA C required violations are still reported by the Coverity analysis tool.
    - The issues are not in the stack modules.
    - They will be fixed in a future release.

## Net Release v3.14.0 (April, 2025)
### ADDITIONS AND UPDATES FOR v3.14.0:

- **New Features**
    - Addition of net support for PIC32CX-BZ6 and PIC32WM-BZ6 family of devices
    - Addition of WebSocket Client module - beta
    - TCP, UDP IPv4 and IPv6 added priority queue support

- **Applications**
  - MPLAB® Harmony 3 provides Net library application examples to accelerate learning and reduce the development cycles for your embedded systems with reusable software components. The applications examples are available in the [product family specific repository](apps/readme.md).

- **Improvements and Bug Fixes**
	-	The following table provides the list of improvements and bug fixes in the v3.14.0 release:

		| Module        | Description |
		| ------------- | ----------- |
        | MCC, drivers  | Support for PIC32CX-BZ6 and PIC32WM-BZ6 family of devices |
        | WebSocket Client | New TCP/IP module to support the WebSocket client communication - beta |
        | SNTP          | Bug fix for skipping the INIT state when restarting the network interface |
        | IPv6, UDP, TCP | Added DSCP option for MAC TX priority queues |
        | IPv4, UDP, TCP | Added TOS option for MAC TX priority queues |  
        | HTTP, SMTP, LLDP | These obsolete modules have been removed |
        | all       	| Major rework to fix MISRA C required violations |


### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.25](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.60](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, v3.14.0
- Harmony net demo apps repositories
- MPLAB Code Configurator (MCC), v5.5.2 or later

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, v3.15.1
- Harmony csp repository, v3.22.2
- Harmony bsp repository, v3.22.0
- Harmony crypto repository, v3.8.2
- harmony-services repository, v1.5.0
- wolfMQTT v1.19.2 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.19.2) for demos requiring wolfMQTT support
- wolfSSL v5.4.0 (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v5.4.0) for demos requiring encryption wolfSSL support

### KNOWN ISSUES
- Some MISRA C required violations are still reported by the Coverity analysis tool.
    - The issues are not in the stack modules.
    - They will be fixed in a future release.


## Net Release v3.13.0 (December, 2024)
### ADDITIONS AND UPDATES FOR v3.13.0:

- **New Features**
    - Addition of net support for PIC32CZ CA7 family of devices
    - PIC32CX-BZ6 and WBZ653 family of devices net support
    - Added Random Interface Identifier generation for IPv6

- **Applications**
  - MPLAB® Harmony 3 provides Net library application examples to accelerate learning and reduce the development cycles for your embedded systems with reusable software components. The applications examples are available in the [product family specific repository](apps/readme.md).

- **Improvements and Bug Fixes**
    - Updated net support for PIC32CZ CA8 & CA9 family of devices

	-	The following table provides the list of improvements and bug fixes in the v3.13.0 release:

		| Module        | Description |
		| ------------- | ----------- |
        | Ethernet PHY       	| Fixed the PHY interface mode selection on PIC32CZ CA8 & CA9 devices |
        | SMTP Client       	| Fixed the syntax error in the SMTP message sender address |

### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.20](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.45](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, v3.13.0
- Harmony net demo apps repositories
- MPLAB Code Configurator (MCC), v5.5.1 or later

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, v3.14.0
- Harmony csp repository, v3.20.0
- Harmony bsp repository, v3.21.1
- Harmony crypto repository, v3.8.2
- harmony-services repository, v1.5.0
- wolfMQTT v1.11.1 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.11.1) for demos requiring wolfMQTT support

### KNOWN ISSUES
- None


## Net Release v3.13.0-E1 (November, 2024)
### ADDITIONS AND UPDATES FOR v3.13.0-E1:

- **New Features**
    - This **engineering release** contains:
        - Added GMAC driver support PIC32CX-BZ6 and WBZ653 family of devices

- **Applications**
  - MPLAB® Harmony 3 provides Net library application examples to accelerate learning and reduce the development cycles for your embedded systems with reusable software components. The applications examples are available in the [product family specific repository](apps/readme.md).


- **Improvements and Bug Fixes**
	-	The following table provides the list of improvements and bug fixes in the v3.13.0-E1 release:

		| Module        | Description |
		| ------------- | ----------- |
        | TCP       	        | Fix for Restoring the fast transmission on duplicate acknowledgments |
        | TCPIP_Helpers         | Updates for generic list manipulation |
        |                       | Updated the IPv4 and IPv6 string conversion routines |         
        | IPv6 & NDP            | Fix for open socket to an invalid destination uses a discarded NDP entry |
        | LAN8720 & LAN8740 PHY | Updated 'RMII Data Interface' as default PHY connection flag for 'ETH' | 
        | DNS                   | Updates for DNS IPv6 support |                  
        | Plugin       	        | Improvements on drag and drop of component & loading of dependency from component |

### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.20](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.45](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, v3.13.0-E1
- Harmony net demo apps repositories
- MPLAB Code Configurator (MCC), v5.5.1 or later

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, v3.14.0-E1
- Harmony csp repository, v3.20.0-E1
- Harmony bsp repository, v3.21.0-E1
- Harmony crypto repository, v3.8.2
- harmony-services repository, v1.5.0
- wolfMQTT v1.11.1 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.11.1) for demos requiring wolfMQTT support

### KNOWN ISSUES
- None


## Net Release v3.12.3 (October, 2024)
### ADDITIONS AND UPDATES FOR v3.12.3:

- **New Features**
	- None

- **Applications**
  - MPLAB® Harmony 3 provides Net library application examples to accelerate learning and reduce the development cycles for your embedded systems with reusable software components. The applications examples are available in the [product family specific repository](apps/readme.md).


- **Improvements and Bug Fixes**
	-	The following table provides the list of improvements and bug fixes in the v3.12.3 release:

		| Module        | Description |
		| ------------- | ----------- |
        | LAN8840 PHY   | Improved the board-specific skew settings implementation for LAN8840 PHY driver | 
        | IPv6          | Fixed the IPv6 fragmentation assembly issue |
        | GMAC      	| Fixed the transmit failure issue when GMAC Priority Queue is enabled |
        |           	| Fixed the exception when Priority Queues are enabled |
        | UDP       	| Fix for socket remote bind last used address |
        | Plugin       	| Migrated to react version of Net Manager(TCP/IP Configurator) |

### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.20](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.45](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, v3.12.3
- Harmony net demo apps repositories
- MPLAB Code Configurator (MCC), v5.5.1 or later

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, v3.13.5
- Harmony csp repository, v3.19.6
- Harmony bsp repository, v3.20.1
- Harmony crypto repository, v3.8.2
- Harmony usb repository, v3.14.0 for demos requiring USB
- wolfSSL v5.4.0 (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v5.4.0) for demos requiring wolfSSL encryption
- wolfMQTT v1.11.1 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.11.1) for demos requiring wolfMQTT support
- CMSIS-FreeRTOS v10.5.1 (https://github.com/ARM-software/CMSIS-FreeRTOS/tree/v10.5.1) for demos requiring FreeRTOS support

### KNOWN ISSUES
- None


## Net Release v3.12.2 (August, 2024)
### ADDITIONS AND UPDATES FOR v3.12.2:

- **New Features**
	- None

- **Applications**
  - MPLAB® Harmony 3 provides Net library application examples to accelerate learning and reduce the development cycles for your embedded systems with reusable software components. The applications examples are available in the [product family specific repository](apps/readme.md).


- **Improvements and Bug Fixes**
	-	The following table provides the list of improvements and bug fixes in the v3.12.2 release:

		| Module        | Description |
		| ------------- | ----------- |
        | LAN8840 PHY   | Board-specific skew settings implementation for LAN8840 PHY driver |
        | UDP           | Updated the UDP socket Rx and Tx limits |
		| GMAC      	| Increased GMAC DMA burst size for SAM9X7 GMAC driver to Max |
		| Documentation | Updated the 'Network Metrics' page | 

### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.20](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.45](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, v3.12.2
- Harmony net demo apps repositories
- MPLAB Code Configurator (MCC), v5.5.1 or later

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, v3.13.5
- Harmony csp repository, v3.19.4
- Harmony bsp repository, v3.20.1
- Harmony crypto repository, v3.8.2
- Harmony usb repository, v3.14.0 for demos requiring USB
- wolfSSL v5.4.0 (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v5.4.0) for demos requiring wolfSSL encryption
- wolfMQTT v1.11.1 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.11.1) for demos requiring wolfMQTT support
- CMSIS-FreeRTOS v10.5.1 (https://github.com/ARM-software/CMSIS-FreeRTOS/tree/v10.5.1) for demos requiring FreeRTOS support

### KNOWN ISSUES
- IPv6 fragment assembly fails, issue is under investigation.


## Net Release v3.12.1 (July, 2024)
### ADDITIONS AND UPDATES FOR v3.12.1:

- **New Features**
	- None

- **Applications**
  - MPLAB® Harmony 3 provides Net library application examples to accelerate learning and reduce the development cycles for your embedded systems with reusable software components. The applications examples are available in the [product family specific repository](apps/readme.md).


- **Improvements and Bug Fixes**
	-	The following table provides the list of improvements and bug fixes in the v3.12.1 release:

		| Module        | Description |
		| ------------- | ----------- |
		| GMAC      	| Fixed the GMAC reference clock selection not working on SAM9X75 |
		|  	   	    	| Fixed the Missing GMAC interrupts in TCP/IP Configurator->Configuration Summary for SAMA7D6 | 
        |               | Fixed the SAM9X7 and SAMA7D6 MAC priority Queue support |
		| SNMPv3		| SNMPv3 USM command updated |
		| MCC			| Referencing to components' online help from MCC is implemented |
		| Documentation | Updated the documentation and added the missing sections | 


### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.20](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.45](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, v3.12.1
- Harmony net demo apps repositories
- MPLAB Code Configurator (MCC), v5.5.1 or later

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, v3.13.5
- Harmony csp repository, v3.19.3
- Harmony bsp repository, v3.20.1
- Harmony crypto repository, v3.8.1
- Harmony usb repository, v3.14.0 for demos requiring USB
- wolfSSL  v5.4.0 (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v5.4.0) for demos requiring wolfSSL encryption
- wolfMQTT v1.11.1 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.11.1) for demos requiring wolfMQTT support
- CMSIS-FreeRTOS v10.5.1 (https://github.com/ARM-software/CMSIS-FreeRTOS/tree/v10.5.1) for demos requiring FreeRTOS support


### KNOWN ISSUES
- None


## Net Release v3.12.0 (June, 2024)
### ADDITIONS AND UPDATES FOR  3.12.0:

- **New features**

    - The release provides the following new features:
		- GMAC driver support for SAMA7D6.
		- PHY driver support for LAN8840.

- **Applications**
  - MPLAB® Harmony 3 provides Net library application examples to accelerate learning and reduce the development cycles for your embedded systems with reusable software components. The applications examples are available in the [product family specific repository](apps/readme.md).


- **Bug Fixes**

	-	The following table provides the list of fixes in the 3.12.0 release:

		| Module                | Description     |
		| ------ | -------- |
		| DHCPS            | Lowered the Minimum DHCP Discovery valid packet size |
		| TCPIP Commands   | Fixed MISRA C violations | 


### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.20](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.40](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, v3.12.0
- Harmony net demo apps repositories
- MPLAB Code Configurator (MCC), v5.5.1 or later

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, v3.13.4
- Harmony csp repository, v3.19.0
- Harmony bsp repository, v3.20.0
- Harmony crypto repository, v3.8.1
- Harmony usb repository, v3.13.0 for demos requiring USB
- wolfSSL  v5.4.0 (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v5.4.0) for demos requiring wolfSSL encryption
- wolfMQTT v1.11.1 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.11.1) for demos requiring wolfMQTT support
- CMSIS-FreeRTOS v10.5.1 (https://github.com/ARM-software/CMSIS-FreeRTOS/tree/v10.5.1) for demos requiring FreeRTOS support


### KNOWN ISSUES
* None.


## Net Release v3.11.1 (April, 2024)
### ADDITIONS AND UPDATES FOR  3.11.1:

- **New features**

    - The release provides various bug fixes.

- **Applications**
  - MPLAB Harmony provides large number of application examples to accelerate learning and reduce the development cycles for your embedded systems with reusable software components. The applications examples are moved to the [product family specific repository](apps/readme.md).


- **New Features and Bug Fixes**

The following table provides the list of fixes in the 3.11.1 release:

| Module                | Description     |
| ------ | -------- |
| ENCX24J600 driver| Fix for ENC transmit of TCP buffers |
| GMAC driver      | Fixed the unstable behaviour after a link loss |
|                  | Fixed the GMAC Descriptor start Address for devices with smaller internal RAM |
| ETHMAC driver    | Bug fix for the ETHMAC |
| TCPIP_Helper     | Fixed 'M0 Devices need particular handling on TCPIP_Helper_Memcpy'  |
| PHY driver       | Support the PHY dummy driver |
|                  | Fixed the DRV_ETHPHY_CFG_DEFAULT flag |
|                  | Implemented Customer Feedback - PHY pins 'RESET' and 'IRQ' pin usage indication |
| TCP              | Fixed the issue when 'Disabling crypto TCP dependency' |
| DHCP             | Extended DHCP event for offer with improved event info |
| ARP              | Fixed the command for insertion of permanent entries in the ARP cache |

### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.20](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.35](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, v3.11.1
- Harmony net demo apps repositories
- MPLAB Code Configurator (MCC), v5.5.0 or later

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, v3.13.3
- Harmony csp repository, v3.18.4
- Harmony bsp repository, v3.18.0
- Harmony crypto repository, v3.8.1
- Harmony usb repository, v3.12.0 for demos requiring USB
- wolfSSL  v5.4.0 (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v5.4.0) for demos requiring wolfSSL encryption
- wolfMQTT v1.11.1 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.11.1) for demos requiring wolfMQTT support
- CMSIS-FreeRTOS v10.5.1 (https://github.com/ARM-software/CMSIS-FreeRTOS/tree/v10.5.1) for demos requiring FreeRTOS support


### KNOWN ISSUES
* See the applications release notes in [product family specific repository](apps/release_notes.md)


## Net Release v3.11.0 (December, 2023)
### ADDITIONS AND UPDATES FOR  3.11.0:

- **New features**

    - The release provides various bug fixes.
    - Improvements of the iperf throughput on GMAC SAM devices - GMAC checksum offload support.
    - The release adds the serial PPP MAC driver.

- **Applications**
  - MPLAB Harmony provides large number of application examples to accelerate learning and reduce the development cycles for your embedded systems with reusable software components. The applications examples are moved to the [product family specific repository](apps/readme.md).


- **New Features and Bug Fixes**

The following table provides the list of fixes in the 3.11.0 release:

| Module                | Description     |
| ------ | -------- |
| GMAC driver   | Added support for MAC RX/TX Checksum offload feature |
|               | iperf throughput improvement |
|               | RGMII/GMII 10/100 Support - for 9x7, A7G5, PIC32CZ_CA platforms |
|               | Fixed issue with critical section access |
|               | Fixed invalid use of FreeRTOS APIs in GMAC code |
| MAC driver    | Added the serial link PPP support as a virtual MAC |
| MIIM driver   | Corrections for the usage of the MIIM driver |
| PHY driver    | ETHPHY Driver function to write to ETHPHY registers and wait for the completion |
|               | Corrected symbol DRV_ETHPHY_OBJECT_BASE_lan9303 case |
|               | Updates for 1000 Mbps support |
| BSD module    | Fixes for the bind operation |
| MCC           | MCC Net plugin various fixes |
|               | Added support for Wi-Fi modules part numbers |

### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.15](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.35](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, 3.11.0
- Harmony net demo apps repositories
- MPLAB Code Configurator (MCC), 5.3.7 or later

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, v3.13.1
- Harmony csp repository, v3.18.0
- Harmony bsp repository, v3.17.0
- Harmony dev\_packs repository, v3.18.1
- Harmony crypto repository, v3.8.1
- Harmony usb repository, 3.11.0 for demos requiring USB
- wolfSSL  v5.4.0 (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v5.4.0) for demos requiring wolfSSL encryption
- wolfMQTT v1.11.1 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.11.1) for demos requiring wolfMQTT support
- CMSIS-FreeRTOS v10.5.1 (https://github.com/ARM-software/CMSIS-FreeRTOS/tree/v10.5.1) for demos requiring FreeRTOS support


### KNOWN ISSUES
* See the applications release notes in [product family specific repository](apps/release_notes.md)

## Net Release v3.10.1 (August, 2023)
### ADDITIONS AND UPDATES FOR  3.10.1:

- **New features**

    - The release provides various bug fixes
    - The release provides some additions for G3-PLC support

- **Applications**
  - MPLAB Harmony provides large number of application examples to accelerate learning and reduce the development cycles for your embedded systems with reusable software components. The applications examples are moved to the [product family specific repository](apps/readme.md).


- **New Features and Bug Fixes**

The following table provides the list of fixes in the 3.10.1 release:

| Module                | Description     |
| ------ | -------- |
| MIIM driver   | Corrections for the usage of the MIIM driver |
|               | Updated the MIIM and PHY driver to work without the Harmony TCP/IP stack |
| GMAC driver   | iperf throughput improvement |
| DHCPv6 server | Fixed the purging of the Tx message list |
|               | Added debug mask for notification event |
| UDP           | Fixed the UDP socket matching to use the correct type of the destination address |
| IPv6          | Added G3 ADP MAC interface |
|               | Added G3-PLC support |


### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.15](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.20](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, 3.10.1
- Harmony net demo apps repositories
- MPLAB Code Configurator (MCC), 5.3.7 or later

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, v3.13.1
- Harmony csp repository, v3.17.0
- Harmony bsp repository, v3.16.1
- Harmony dev\_packs repository, v3.17.0
- Harmony crypto repository, 3.8.0
- Harmony usb repository, 3.11.0-E2 for demos requiring USB
- wolfSSL  v5.4.0 (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v5.4.0) for demos requiring wolfSSL encryption
- wolfMQTT v1.11.1 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.11.1) for demos requiring wolfMQTT support
- CMSIS-FreeRTOS v10.5.1 (https://github.com/ARM-software/CMSIS-FreeRTOS/tree/v10.5.1) for demos requiring FreeRTOS support


### KNOWN ISSUES
* See the applications release notes in [product family specific repository](apps/release_notes.md)



## Net Release v3.10.0 (July, 2023)
### ADDITIONS AND UPDATES FOR  3.10.0:

- **New features**

    - The release adds support for PIC32CX\_SG41.
    - The release adds new PHY support: LAN9354 and LAN8770 
    - The release updates the MCC Net plugin.

- **Applications**
  - MPLAB Harmony provides large number of application examples to accelerate learning and reduce the development cycles for your embedded systems with reusable software components. The applications examples are moved to the [product family specific repository](apps/readme.md).


- **New Features and Bug Fixes**

The following table provides the list of new features and fixes in the 3.10.0 release:

| Module                | Description     |
| ------ | -------- |
| MCC net plugin| Support for the PIC32CX\_SG41, PIC32CK\_GC configuration has been added |
|               | Configured heap size is set to blank when heap type is other than external. |
|               | Available components and active components are sorted in alphabetical order in Layers, Overview and configuration Summary |
| MIIM driver   | Provided extended operation support |
|               | Solved MIIM build issue for PIC32CZ\_CA demos |
| PHY driver    | Add LAN9354, LAN8770 support |
|               | Removed obsolete SMSC naming from the PHY files |
| GMAC driver   | Add drivers for PIC32CK\_GC, PIC32CX\_SG41 GMAC |                
| DHCP server   | Allowed serving of errors and client events simultaneously |
| Manager       | MAC check link frequency has been added as a parameter |
|               | APIs for modules de-initialization and running status have been added to the stack manager |
| TCP           | Fixed application lock-up  when remote window == 0 and there's data to be sent |
|               | Added re-transmission timers for the unacknowledged data to the TCP state machine |
| HTTP          | Beta support for HTTP server to support multiple instances with multiple ports and access/redirection rules |
| IPv6          | ICMPv6 template is corrected when ICMPV6 console command is not enabled |
| IPv4          | Fixed the calculation of leading ones/trailing zeroes in the forwarding table|


### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.10](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.30](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, 3.10.0
- Harmony net demo apps repositories
- MPLAB Code Configurator (MCC), 5.3.7 or later

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, v3.13.0
- Harmony csp repository, v3.17.0
- Harmony bsp repository, v3.16.0
- Harmony dev\_packs repository, v3.17.0
- Harmony crypto repository, 3.8.0
- Harmony usb repository, 3.11.0-E2 for demos requiring USB
- wolfSSL  v5.4.0 (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v5.4.0) for demos requiring wolfSSL encryption
- wolfMQTT v1.11.1 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.11.1) for demos requiring wolfMQTT support
- CMSIS-FreeRTOS v10.5.1 (https://github.com/ARM-software/CMSIS-FreeRTOS/tree/v10.5.1) for demos requiring FreeRTOS support


### KNOWN ISSUES
* See the applications release notes in [product family specific repository](apps/release_notes.md)

## Net Release v3.9.2 (May, 2023)
### ADDITIONS AND UPDATES FOR  3.9.2:

- **New features**

    - This release updates the documentation by adding the **'Create your first TCP/IP Application'** topic.

- **Applications**
  - MPLAB Harmony provides large number of application examples to accelerate learning and reduce the development cycles for your embedded systems with reusable software components. The applications examples are moved to the [product family specific repository](apps/readme.md).


- **New Features and Bug Fixes**

The following table provides the list of new features in the 3.9.2 release:

| Module                | Description     |
| ------ | -------- |
| Documentation| Updated the documentation and added the 'Create your first TCP/IP Application' |

### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.05](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.20](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, 3.9.2
- Harmony net demo apps repositories
- MPLAB Code Configurator (MCC), 5.2.1 or later

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, v3.12.0-E1
- Harmony csp repository, v3.15.0
- Harmony bsp repository, v3.15.0
- Harmony dev\_packs repository, v3.15.1
- Harmony crypto repository, 3.7.6
- Harmony usb repository, 3.11.0-E1 for demos requiring USB
- wolfSSL  v4.7.0-stable (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v4.7.0-stable) for demos requiring wolfSSL encryption
- wolfMQTT v1.11.1 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.11.1) for demos requiring wolfMQTT support
- CMSIS-FreeRTOS v10.4.6 (https://github.com/Microchip-MPLAB-Harmony/CMSIS-FreeRTOS/tree/v10.4.6) for demos requiring FreeRTOS support


### KNOWN ISSUES
* See the applications release notes in [product family specific repository](apps/release_notes.md)

## Net Release v3.9.1 (March, 2023)
### ADDITIONS AND UPDATES FOR  3.9.1:

- **New features**

    - This release adds support for the LAN865x 10BASE-T1S.
    It also updates the usage of the 9X70 registers.

- **Applications**
  - MPLAB Harmony provides large number of application examples to accelerate learning and reduce the development cycles for your embedded systems with reusable software components. The applications examples are moved to the [product family specific repository](apps/readme.md).


- **New Features and Bug Fixes**

The following table provides the list of new features and bug fixes in the 3.9.1 release:

| Module                | Description     |
| ------ | -------- |
| MAC Driver| Updated the Ack function return type |
| PHY Driver| Added support for the VSC8540 Ethernet PHY |
| Manager   | Added global second and millisecond time keeping to the stack |
| MAC Driver| Added LAN865x 10BASE-T1S MAC-PHY Support |
| MAC Driver| Removed bit MIIONRGMII from SAM9X7 GMAC Driver |
| Net Pres  | Added Support for "More Client Certificates" in the NetPres MCC component |
| Net Pres  | Added API for setting client certificate at run time |
| MAC Driver| Fixed incorrect cache management in the V7x GMAC driver |
|All modules| Updated net repo license and the file disclaimer |

### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.05](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.20](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, 3.9.1
- Harmony net demo apps repositories
- MPLAB Code Configurator (MCC), 5.2.1 or later

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, v3.12.0-E1
- Harmony csp repository, v3.15.0
- Harmony bsp repository, v3.15.0
- Harmony dev\_packs repository, v3.15.1
- Harmony crypto repository, 3.7.6
- Harmony usb repository, 3.11.0-E1 for demos requiring USB
- wolfSSL  v4.7.0-stable (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v4.7.0-stable) for demos requiring wolfSSL encryption
- wolfMQTT v1.11.1 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.11.1) for demos requiring wolfMQTT support
- CMSIS-FreeRTOS v10.4.6 (https://github.com/Microchip-MPLAB-Harmony/CMSIS-FreeRTOS/tree/v10.4.6) for demos requiring FreeRTOS support


### KNOWN ISSUES
* See the applications release notes in [product family specific repository](apps/release_notes.md)

## Net Release v3.9.0 (December, 2022)
### ADDITIONS AND UPDATES FOR  3.9.0:

- **New features**

    - This release adds Beta support for the PIC32CZ_CA family of devices

- **Applications**
  - MPLAB Harmony provides large number of application examples to accelerate learning and reduce the development cycles for your embedded systems with reusable software components. The applications examples are moved to the [product family specific repository](apps/readme.md).


- **New Features and Bug Fixes**

The following table provides the list of new features and bug fixes in the 3.9.0 release:

| Module                | Description     |
| ------ | -------- |
| Manager   | TCP/IP stack option to start only the modules presented in the initialization data |
| PHY Driver| Added LAN8742A and KSZ9031 support files |
| MAC Driver| Updates to the EMAC, ETHMAC and GMAC drivers |
| UDP       | Added fixed destination address/port for the socket |
| UDP       | Added socket sticky  options |
| IPv4      | Updates for IPv4 checking the TX packet result |
| Manager   | Added global assert/condition functions that could be used by all modules |
| Manager   | When the heap delete operation fails - at stack deinit - and the DRAM debugging is enabled, display the offending module |
| Manager   | The heap debug now stores the number of allocation/free operations |
| Manager   | Added checking of the running services for the AddressServiceSelect functionality |
| DHCPv6    | Added DHCPv6 Enable/Disable API |
| DHCPv6    | Fix for DHCPv6 confirm/not-on-link reply |
| DHCPv6    | Updated the reply processing to separately check for the transaction ID |
| DHCPv6    | Various other fixes |
| iperf     | Updated the iperfs command to display the RX/TX buffer size |
| HTTP_NET  | Added the listening port to the http info command |


### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.05](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.20](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, 3.9.0
- Harmony net demo apps repositories, 3.9.0
- MPLAB Code Configurator (MCC), 5.2.1

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, 3.11.1
- Harmony csp repository, 3.14.0
- Harmony bsp repository, 3.14.0
- Harmony dev_packs repository, 3.14.0
- Harmony crypto repository, 3.7.6
- Harmony usb repository, 3.10.0 for demos requiring USB
- wolfSSL  v4.7.0-stable (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v4.7.0-stable) for demos requiring wolfSSL encryption
- wolfMQTT v1.11.1 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.11.1) for demos requiring wolfMQTT support
- CMSIS-FreeRTOS 10.3.1 (https://github.com/Microchip-MPLAB-Harmony/CMSIS-FreeRTOS/tree/v10.3.1) for demos requiring FreeRTOS support



### KNOWN ISSUES
* See the applications release notes in [product family specific repository](apps/release_notes.md)


## Net Release v3.8.0 (July, 2022)
### ADDITIONS AND UPDATES FOR  3.8.0:

- **New features**

The following table provides the list of the new features included in the 3.8.0 release:
- All applications have been migrated from MHC to MCC
- Migration of select applications to the Net Plugin (beta)
    - tcpip_tcp_server net plugin application
    - berkeley_udp_client net plugin application
    - web_net_server_sdcard_fatfs net plugin application
- Added configuration help for all modules and drivers


| Module                | Description                          |
| ------ | ------ |
| DHCP Server V2  | A new implementation of the DCP server. Improved functionality over multiple interfaces and configuration. Beta release|
| HTTP NET        | Updated the HTTP de-register call to abort the existing connections |
| MQTT            | Updated the default broker to test.mosquitto.org |
| IPv4            | Added IPv4 Forwarding Statistics to MHC/MCC |

- **New Applications**

The following table provides the list of the new applications including bare metal and FreeRTOS configurations:

| Application                 | Platform          | Description                                                                                      |
| ------------ | ------------ |  ------------     |
| tcpip_tcp_server_bridge   | PIC32MZ  | MPLABX Layer 2 bridge application for PIC32MZ platform |
| tcpip_tcp_server_ipv4_fwd | PIC32MZ  | MPLABX IPv4 forwarding/routing application for PIC32MZ platform |
| tcpip_tcp_client          | SAM9X60, SAMA5D2  | MPLABX migrated applications |
| tcpip_tcp_client_server   | SAM9X60, SAMA5D2  | MPLABX migrated applications |
| tcpip_tcp_server          | SAM9X60, SAMA5D2  | MPLABX migrated applications |
| tcpip_udp_client          | SAM9X60, SAMA5D2  | MPLABX migrated applications |
| tcpip_udp_client_server   | SAM9X60, SAMA5D2  | MPLABX migrated applications |
| tcpip_udp_server          | SAM9X60, SAMA5D2  | MPLABX migrated applications |
| web_net_server_qspi_mpfs  | SAM9X60, SAMA5D2  | MPLABX migrated applications |
| web_net_server_sdcard_fatfs| SAM9X60, SAMA5D2 | MPLABX migrated applications |
| wolfssl_tcp_client        | SAM9X60, SAMA5D2  | MPLABX migrated applications |
| tcpip_client_server       | SAMA5D2           | MPLABX migrated application |
| wolfssl_tcp_server        | SAMA5D2           | MPLABX migrated application |

- **Updated Applications**

    - All applications have been regenerated and tested to work with the latest repositories the TCP/IP stack depends on: core, csp, crypto, etc.

- **Bug Fixes**

The following table provides the list of bug fixes in the 3.8.0 release:

| Module                | Description                                             |
| ------ | -------- |
| KSZ8091 PHY driver    | Updated the KSZ8091 PHY Auto MDIX setting |
| IPv6                  | Fixed compilation issue for IPv6 only demos|
| Net Utilities         | Updated the mpfs utility to generate HTTP_APP_DYNVAR_BUFFER for http_net_print.h file |
| ICMP                  | Updated ICMP to properly release a request once the request is done |
| FTP Server            | Updated the formatting for the file command list |
| IPV6                  | TCPIP_ICMPV6_CLIENT_USER_NOTIFICATION updated to TCPIP_ICMPV6_CLIENT_CONSOLE_CMD |
| SAMA5D2 MAC driver    | Added extra checks and updates to the A5D2 MAC driver |
| Manager               | Added a more explicit initialization failure message |
| DHCP client           | ARP check timeout is MHC/MCC configurable now |
| SMTPC                 | Increased the default socket buffer size to 2 KB |
| SYS_Random adapter    | Added multi-threaded protection for the SYS_Random adapter |
| All modules           | Fixes for MISRA violations |


### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v6.00](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v4.00](https://www.microchip.com/mplab/compilers) or later
- Harmony net repository, 3.8.0
- Harmony net demo apps repositories, 3.8.0
- MPLAB Code Configurator (MCC), 5.1.2
- MCC Harmony Core, 1.1.0

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, 3.10.1
- Harmony csp repository, 3.11.1
- Harmony bsp repository, 3.11.1
- Harmony dev_packs repository, 3.11.1
- Harmony crypto repository, 3.7.6
- Harmony usb repository, 3.8.1 for demos requiring USB
- wolfSSL  v4.7.0-stable (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v4.7.0-stable) for demos requiring wolfSSL encryption
- wolfMQTT v1.11.1 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.11.1) for demos requiring wolfMQTT support
- CMSIS-FreeRTOS 10.3.1 (https://github.com/Microchip-MPLAB-Harmony/CMSIS-FreeRTOS/tree/v10.3.1) for demos requiring FreeRTOS support


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
* IAR projects are not supported in this release

* SAM9X60/SAMA5D2 MPLABX demos regeneration should **NOT** overwrite the mpfs_net_img.c file.
    - The symbols used in this automatically generated file are not present in the project.

* wolfSSL/wolfCrypto PIC32M projects should be built with xc32 version >= 3.01.
    - Otherwise the build will fail.

* HTTP_NET projects using https with wolfSSL/wolfCrypt need to use xc32 v3.01.
    - A crash occurrs with xc32 v4.0 for PIC32MZ, SAME54.
    - Issue is under investigation.

* HTTP_NET FreeRTOS demos need a heap size of 200K when using the XC32 v4.0.0 compiler.

* SAMA5D2 MPLABX demos should **NOT** overwrite the linker script ddr.ld when regenerated.
    - Otherwise the GMAC descriptors: gmac_dcpt_array will not be placed in the '.region_nocache'

* SAMA5D2 projects should be built with -O0 optimization and at least 16 descriptors.
    - Higher optimization may cause GMAC driver lock up.
    - Investigation is in progress.

* The SAM9x60 MPLABX should **NOT** overwrite the linker script ddram.ld when regenerated.
    - Otherwise the EMAC descriptors: EMAC0_DcptArray will not be placed in the '.region_nocache'

* wolfMQTT demos need to have the file third_party/wolfMQTT/mqtt_socket.c built without xc32 option 'Make warnings into errors'
    - That is because it uses the obsolete #include <sys/errno.h>
    - Applies to PIC32MZ, SAME70, SAMV71 platforms

* Net Plugin issues:
    - Issue 1: The connection between MAC and NETCONFIG is missing in MCC project graph.
        - This happens for Net demos not migrated to TCP/IP Configurator. This issue will not impact code regeneration.
        - Reason: The MAC components updated for multi-capability support (MH3-52292). 
        - Workaround: Open the project graph in MCC. Open 'TCP/IP Configuration' from Plugins drop-down menu. Go to 'Data Link' Layer. Connect nodes between NETCONFIG and MAC.

    - Issue 2: Cannot remove dependency node on group-box once added.
        - This issue happens when multiple instances of external ethernet controller are added to project and one instance is removed later
        - Reason: New python interface API needed (CFW-1183) 
        - Workaround: No work around available

    - Issue 3: While starting the TCP/IP demo projects, the ‘Number of Instances Using UART’ field in SYS_CONSOLE shows value ‘2’.
        - This happens when only one UART is connected to SYS_CONSOLE, and the above field should be ‘1’.
        - Reason: Under investigation
        - Workaround: Remove SYS_CONSOLE from project graph. Add the SYS_CONSOLE back to project graph. Connect the dependencies.

* Stack down/up issues:
    - When using xc32 v4.00 there is the possibility that executing the sequence stack down and then up, the stack up fails because of an allocation error.
    - This occurs even if the stack down was successful and all the memory was released.
    - The issue is currently under investigation.
    - Use xc32 v3.01 as a workaround.

## Net Release v3.7.4 (October, 2021)
### ADDITIONS AND UPDATES FOR  3.7.4:

- **Important functionality update**

This release provides support for Microchip Wi-Fi demos.  
As with the Release v3.7.3 this release uses updated MAC drivers to work with the new packet allocation and layout.  

**Regular Harmony networking demos should use the previous v3.7.2 release!**  
Complete testing with the new allocation scheme will be part of the net v3.8.0 release.


- **New features**

The following table provides the list of the new features included in the 3.7.4 dot release:

| Module                | Description                          |
| ------ | ------ |
| IPv4          | Added dynamic routing API |
|               | Allowed internal routing of broadcast packets with source IP address == 0 |
| MAC bridge    | Added bridge ports glue code for ARP and IPv4 |
|               | Update the MAC bridge to do its own time keeping |
| NET_PRES      | Updates for support of SNI and ALPN features for TLS connection |
| DHCP Server   | Added ICMP support, renewal and rebind option support |
| TCP, UDP      | Added run time debug checkpoints |
| ETHMAC driver | Updated synchronization to protect only the driver queues |

- **Bug Fixes**

The following table provides the list of bug fixes in the 3.7.4 dot release:

| Module                | Description                                             |
| ------ | -------- |
| GMAC driver   | Fix for incorrect use of _dataOffsetMask |
|               | DRV_GMAC_PacketTx properly verifies the packet priority field |
| Manager       | Fixed the clearing of the packet priority |
| ETHMAC driver | Updated the ETH MAC statistics registers to return 32 bits |
| DHCP server   | Run time exception issue resolved |
| MHC           | Fixed Incorrect definition of "TCPIP_IF_PIC32MZW_ETHMAC" for PIC32M projects |
|               | Add processor condition check for SAMRH71 macName |
|               | Fixed MHC failure on devices without internal mac when tcp/ip stack enabled |
| All net modules| Updates and fixes for MISRA build failures |


### TESTED WITH:

#### Software Dependencies

- [MPLAB® X IDE v5.45](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v2.50](https://www.microchip.com/mplab/compilers) or later
- See package.xml for the required dependency repo versions.


- **Release notes for v3.7.3 apply**

## Net Release v3.7.3 (July, 2021)
### ADDITIONS AND UPDATES FOR  3.7.3:

- **Important functionality update**

This release provides support for Microsoft NetxDuo and Azure IoT demos.  
This release changes the allocation of the MAC packets and the payload alignment.  
Protocol structures are now 32 bit boundary aligned.  
MAC drivers have been updated to work with the new packet layout.  

**Regular Harmony networking demos should use the previous v3.7.2 release!**  
Complete testing with the new allocation scheme will be part of the net v3.8.0 release.


- **New features**

The following table provides the list of the new features included in the 3.7.3 dot release:

| Module                | Description                          |
| ------ | ------ |
| MAC drivers           | Updated the MC packet structure for a more efficient payload alignment. Protocol structures are now 32 bit aligned |
| MAC drivers           | Updated the MAC TCPIP_MAC_PacketRx() signature |
| MHC                   | Added support for Azure IoT configuration |
| Announce              | Updated announce to advertise a static IPv4 address too |
| HTTP_NET              | Defaulted the HTTP_NET allocation functions to TCP/IP heap |

- **Bug Fixes**

The following table provides the list of bug fixes in the 3.7.3 dot release:

| Module                | Description                                             |
| ------ | -------- |
| GMAC driver           | Prevent multiple freeing of array of pointers for rx packet in _MacRxFreePacket |
| ENC 28J60 driver      | Extended Receive Status Vector buffer to avoid overwrite and exception |
| ETHMAC, GMAC driver   | Cleared the initialization flag so that the _MACDeinit is called only once |

### TESTED WITH:

#### Software Dependencies

- [MPLAB® X IDE v5.45](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v2.50](https://www.microchip.com/mplab/compilers) or later
- See package.xml for the required dependency repo versions.


- **Release notes for v3.7.2 apply**

## Net Release v3.7.2 (April, 2021)
### ADDITIONS AND UPDATES FOR  3.7.2:

- **New features**

The following table provides the list of the new features included in the 3.7.2 dot release:

| Module                | Description                          |
| ------ | ------ |
| FTP server            | Added lower case commands support |
| FTP server            | Command socket disconnect when client connection is not present |
| Packet logger         | Support for just one of the and/or commands |
| DHCP Client           | Use of extended ID for virtual interfaces only |

- **Bug Fixes**

The following table provides the list of bug fixes in the 3.7.2 dot release:

| Module                | Description                                             |
| ------ | -------- |
| All net modules       | Various fixes for build with xc32 v3.0 |
| IPv4                  | Updated TCPIP_IPV4_PacketTransmit() to acknowledge only the fragments it created |
| ICMP                  | Discarded all ICMP fragments when TCPIP_IPV4_PktTx() failed |
|                       | Updated the ICMP response to follow the IPv4 forwarding flow |
| DNS client            | Updated the selection of the DNS server index and interface |
| GMAC driver           | Fixed wrong index type in DRV_PIC32CGMAC_LibTxInit |
| MHC                   | Set Display Type of all TCP/IP related component to identify the layer |

- **Release notes for v3.7.1 apply**

## Net Release v3.7.1 (March, 2021)
### ADDITIONS AND UPDATES FOR  3.7.1:

- **New features**

The following table provides the list of the new features included in the 3.7.1 dot release:

| Module                | Description                          |
| ------ | ------ |
| FTP server            | Added the use of NetPres API |
| TFTP Client           | Added IPv4/IPv6 Selection |
| FTP Server            | Added IPv4/IPv6 Selection |
| DHCPV6 client         | Added MHC support |
| BSD                   | Updated connect procedure |
| NET_PRES              | Trust&Go Client Certificate updates | 
| PHY driver            | Added PHY vendor detection procedure |
| SNTP                  | Added run time enable/disable functionality |
| TCP                   | Allowed disabling of the Crypt RNG usage |
| Manager               | Added support for power mode == 'down' |
|                       | Allow network interfaces to start in 'down' state |

- **Bug Fixes**

The following table provides the list of bug fixes in the 3.7.1 dot release:

| Module                | Description                                             |
| ------ | -------- |
| FTP, UDP, Commands    | Updated Deinitialize routine to perform action only if already properly initialized |
| TFTP client           | PUT command large file transfer issue resolved |
| ENCx24j600            | Fixed MAC driver failing to release all the allocated resources |
| GMAC driver           | Fixed not transmitting with 1 Tx Descriptor |
|                       | Enabled DCache Clean by Address |
| SNTP                  | Fixed test for TCPIP_UDP_TxPutIsReady() |
| MAC bridge            | Fixed corner case resulting in an orphaned packet |
| Manager               | Removed address 0 from duplicate check |

| MHC                   | Fixed wolmqtt regeneration failure due to missing 'user_settings.h' |
|                       | Fixed issues with MAC bridging support |
|                       | Auto activated SNTP Component when 'Encryption' in net_Pres component is enabled |
|sys_adapter            | Fixed memory leaks in the random adapter |

- **Release notes for v3.7.0 apply**


## Net Release v3.7.0 (December, 2020)
### ADDITIONS AND UPDATES FOR  3.7.0:

- **New Application Repos**

The following table provides the list of the new applications repositories for the net distribution:

| Repository                  | Platform          | Description                                         |
| ------------ | ------------ |  ------------     |
| net_apps_pic32mx      | PIC32MX                 | Networking applications for PIC32MX |
| net_apps_pic32mz      | PIC32MZDA, PIC32MZEF    | Networking applications for PIC32MZDA and PIC32MZEF |
| net_apps_sam_9x60     | SAM9X60                 | Networking applications for SAM9X60 |
| net_apps_sam_a5d2     | SAMA5D2                 | Networking applications for SAMA5D2 |
| net_apps_sam_e5x      | SAME54                  | Networking applications for SAME54 |
| net_apps_sam_e70_v71  | SAME70, SAMV71          | Networking applications for SAME70 and SAMV71 |

- **New features**

The following table provides the list of the new features included in the 3.7.0 release:

| Module                | Description                          |
| ------ | ------ |
| IPv4            | Added IP static forwarding support. For multiple interfaces hosts the TCP/IP stack provides routing functionality |
| Layer 2 Bridge  | Added interfaces bridging at the MAC level. Hosts with multiple interfaces can bridge them to create a network aggregate |
| MAC driver      | The packet allocation has been updated so that all the MAC driver use a unique data packet layout accross all architectures. This allows packets to be dynamically exchanged between different interfaces |
| ZCLL            | Updated ZCLL to announce its own address and use a better pseudo-random algorithm. Thanks to *jdeguire* on the Forum for his contribution |
| MHC             | The NET_PRES module configuration has been simplified and it belongs now to the TCP/IP stack. It is now part of *Libraries->TCPIP->Layer6-PRESENTATION->Presentation Layer*. The demos have been updated to use this new configuration module |
| TCP/IP MHC      | All TCP/IP stack modules settings have been divided into *basic* and *advanced* settings to simplify the module configuration. The *advanced* settings contain not frequently used parameters and are hidden by default |
| GMAC Driver     | Support for Priority Queuing of Tx and Rx packets has been added |


- **Addressed Security Vulnerabilities**

The following table provides the list of the security vulnerabilities addressed in the 3.7.0 release:

| Vulnerability   | Description                          |
| ------ | ------ |
| CVE-2020-17439 (FSCT-2020-0017) | DNS Cache Poisoning |
| CVE-2020-17441 (FSCT-2020-0014) |	IPv6 Payload Validation - DoS |


- **New Applications**

The following table provides the list of the new applications including bare metal and FreeRTOS configurations:

| Application                 | Platform          | Description                                                                                      |
| ------------ | ------------ |  ------------     |
| tcpip_tcp_client | SAM9X60  | MPLABX TCP client demo application for SAM9X60 platform |

- **Updated Applications**

    - All applications have been regenerated and tested to work with the latest repositories the TCP/IP stack depends on: core, csp, crypto, etc.


- **Bug Fixes**

The following table provides the list of bug fixes in the 3.7.0 release:

| Module                | Description                                             |
| ------ | -------- |
| SAM9X60 MAC driver | Fixed bug in the driver receive functionality using clean instead of invalidate operation |
| SAMA5D2 MAC driver | Updated the MAC driver to replace the global cache operations with line oriented ones |
| ENC MAC driver     | Adjusted the packet segment length, removing the FCS and Ethernet header length |
| PHY driver         | Updated the PHY driver operation to handle the MIIM reported errors |
| PHY driver         | Removed PHY hardware configuration dependency of xc32 version |
| DNS client         | Added RR checks for DNS to match the query name with the answers. Fixed bug when extracting the RR name |


### TESTED WITH:

#### Software Dependencies

Before using MPLAB Harmony Net, ensure that the following are installed:

- [MPLAB® X IDE v5.45](https://www.microchip.com/mplab/mplab-x-ide) or later
- [MPLAB® XC32 C/C++ Compiler v2.50](https://www.microchip.com/mplab/compilers) or later
- [IAR Embedded Workbench for ARM - version 8.50.1](https://www.iar.com/iar-embedded-workbench/) or later for SAMA5D2, SAM9X60 projects
- Harmony net repository, 3.7.0
- Harmony net demo apps repositories, 3.7.0

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see net/package.xml):

- Harmony core repository, 3.8.1
- Harmony csp repository, 3.8.2
- Harmony bsp repository, 3.8.2
- Harmony dev_packs repository, 3.8.0
- Harmony crypto repository, 3.6.1
- Harmony usb repository, 3.7.0 for demos requiring USB
- wolfSSL  v4.5.0-stable (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v4.5.0-stable) for demos requiring wolfSSL encryption
- wolfMQTT v1.2 (https://github.com/Microchip-MPLAB-Harmony/wolfMQTT/tree/v1.2) for demos requiring wolfMQTT support
- CMSIS-FreeRTOS 10.3.1 (https://github.com/Microchip-MPLAB-Harmony/CMSIS-FreeRTOS/tree/v10.3.1) for demos requiring FreeRTOS support
- MPLAB Harmony Configurator (MHC) v.3.6.2
- MPLAB® Harmony Configurator (MHC) plug-in, v3.6.2


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

* When regenerating the FreeRTOS projects containing wolfSSL/wolfCrypt do **NOT** override the changes in the *config.h* if this adds the inclusion of *definitions.h*.
    - This header is needed for accessing the OSAL/FreeRTOS allocation functions: pvPortMalloc and vPortFree

* Layer 2 Bridge documentation is not part of the current release
    - It will be added in a future release
    
* Application for demonstrating the Layer 2 Bridge functionality is not part of the current release
    - It will be added in a future release

* Application for demonstrating the IPv4 static forwarding/routing functionality is not part of the current release
    - It will be added in a future release


- **Release notes for v3.6.4 apply**

## Net Release v3.6.4 (October, 2020)
### ADDITIONS AND UPDATES FOR  3.6.4:

- **Updated functionality**

The following table provides the list of the updates included in the 3.6.4 dot release:

| Module                | Description                          |
| ------ | ------ |
| sys_adapter           | The sys_random_h2_adapter has been updated to include the cryptographically secure random number generators. The sys_random_h2_adapter.c has been added to the net repo.|
| TCP                   | The calculation of the Initial Sequence Number (ISN) has been updated to include a strong random key |



### KNOWN ISSUES

The updates in this release affect the way in which the TCP sockets calculate the ISN.
The calculation has been updated to include the crypto library in order to make the guessing of the ISN very diffcult.

* The updates have been applied to the NET TCP/IP core TCP files.

* The demos that are part of the release and contain the TCP module have **NOT** been updated! The demos need to be regenerated in order to apply this update

* For proper regeneration of the demos, run the MPLAB Harmony Configurator (MHC) for the selected demo and configuration
    - Select the *TCP/IP STACK* component and go to the *TRANSPORT LAYER*
    - In the *TCP/IP Transport Layer Configurator* uncheck the *TCP* module checkbox and then add it back
    - If the *Crypto Library* was not part of the project it will be added. Accept all the requested changes when prompted
    - The *Crypto Library* and *wolfCrypt Library* components will be now part of the root project graph
    - Regenerate the demo and the updates will be included

- **Release notes for v3.6.1 apply**


## Net Release v3.6.3 (October, 2020)
### ADDITIONS AND UPDATES FOR  3.6.3:

- **New features**

The following table provides the list of the new features included in the 3.6.3 dot release:

| Module                | Description                          |
| ------ | ------ |
| sys_adapter           |Removed the sys_reset_h2_adapter from net repo. This is handled by the core system services|


- **Release notes for v3.6.1 apply**


## Net Release v3.6.2 (September, 2020)
### ADDITIONS AND UPDATES FOR  3.6.2:

- **New features**

The following table provides the list of the new features included in the 3.6.2 dot release:

| Module                | Description                          |
| ------ | ------ |
| NET_PRES              |Added MHC support for TLS mutual auth for NET PRES Client | 
|                       |Added function to retrieve Client TLS parameters to certificate store files |
|                       |Added new dropdown options to select X509 file formats for certificates and Private key (i.e. PEM or ASN1) |
|                       |Added mutual auth empty function definition to certificate stub section for uniformity |
|                       |CA cert format and Server cert format set to ASN1 in netPres.py to maintain backward compatibility |
|                       |Changed MHC GUI field names for Client Certificate to Client CA Certififace |
| sys_adapter           |Added sys_reset_h2_adapter handling of PIC32M devices|


- **Bug Fixes**

The following table provides the list of bug fixes in the 3.6.2 dot release:

| Module                | Description                                             |
| ------ | -------- |
| Demo apps         | Added missing atsame54p20a_compat.h file: web_net_server_nvm_mpfs, web_server_nvm_mpfs, wolfmqtt_demo|
|                   | Removed from manifest.yml the files not in package.xml|
| PHY driver        | Removed PHY hardware configuration dependency of xc32 version |
| MHC               | Fixed ICMP file not included in project when ICMP Server/Client unchecked |
|                   | Fixed tcpip_helper_c32.S added by MH3 configurator for non MIPS projects |
|                   | Fixed netPresBlobClientSupport name used for 2 different symbols|
|                   | Fixed wolfMQTT NET Glue script not generating proper code | 
| NET_PRES          | Updated the NET_PRES glue code to use sys_debug service |
| TCP/IP Manager    | Updated the SYS_ERROR_PRINT format parameters |
| SYS_FS shell      | Updated the Shell_FileDelete return type |
| IPv6              | Moved the flexible array IPV6_DATA_SEGMENT_HEADER structure to the end of the IPV6_PACKET definition |
| UDP, TCP          | Added comments for sockets multi-threaded usage |
| DHCPv4            | Updated the default DHCP client timeout to 10 seconds|
| HTTP_NET          | Updated the ledSelected custom code to properly use the dynamic STRING type|
|                   | Fixed the decrement of negative numbers |
|                   | Updated the html pages copyright date|


- **Release notes for v3.6.1 apply**



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
















