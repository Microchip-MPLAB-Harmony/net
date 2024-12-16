![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)

# MPLAB® Harmony 3 Net Support Package

MPLAB Harmony 3 is an extension of the MPLAB® ecosystem for creating
embedded firmware solutions for Microchip 32-bit SAM and PIC microcontroller
and microprocessor devices.  Refer to the following links for more information.

 - [Microchip 32-bit MCUs](https://www.microchip.com/design-centers/32-bit)
 - [Microchip 32-bit MPUs](https://www.microchip.com/design-centers/32-bit-mpus)
 - [Microchip MPLAB X IDE](https://www.microchip.com/mplab/mplab-x-ide)
 - [Microchip MPLAB Harmony](https://www.microchip.com/mplab/mplab-harmony)
 - [Microchip MPLAB Harmony Pages](https://microchip-mplab-harmony.github.io/)

This repository contains the MPLAB® Harmony 3 Network Package (Net).  The
Net repo provides a free fast to market TCP/IP stack for Microchip 32-bit SAM and PIC microprocessor
devices. The repo contains multiple applications that demonstrate communication over TCP/IP
using well known protocols like TCP, UDP, HTTP, SMTP, etc.
Refer to the following links for release notes, training materials, and interface
reference information.

 - [Release Notes](./release_notes.md)
 - [Microchip Software License Agreement](Microchip_SLA001.md)
 - [MPLAB Harmony 3 Net Wiki](https://github.com/Microchip-MPLAB-Harmony/net/wiki)
 - [MPLAB Harmony 3 Net API Help](https://microchip-mplab-harmony.github.io/net)

# Contents Summary

| Folder     | Description                                               |
|------------|-----------------------------------------------------------|
| apps       | Example applications for Net library components           |
| docs       | Net library help documentation                            |
| driver     | Drivers used by the TCP/IP stack                          |
| tcpip      | The TCP/IP stack                                          |
| net pres   | The network presentation layer used by the TCP/IP stack   |

## Documentation

Click [here](https://onlinedocs.microchip.com/v2/keyword-lookup?keyword=MH3_net&redirect=true) to view the Net library help documentation.

To view the documentation offline, follow these steps:
 - Download the publication as a zip file from [here](https://onlinedocs.microchip.com/download/GUID-B1AF6B2B-BC62-4D92-B329-A50140DF3437?type=webhelp).
 - Extract the zip file into a folder.
 - Navigate to the folder and open **index.html** in a web browser of your choice.

____

## MPLAB® Harmony 3 TCP/IP Stack Library:

| Category                                 | Item                | Description | Release Status |
| ---------------------------------------- | ------------------- | ----------- |--------------- |
| TCP/IP Stack Plugin                      | TCP/IP Configurator | The TCP/IP Stack Configurator Plugin UI | ![app-beta](https://img.shields.io/badge/plugin-production-green?style=plastic) |
| TCP/IP Library General                   |  TCPIP CORE         | The core functions for the working of MPLAB® Harmony TCP/IP Stack is part of this Module | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  NETCONFIG          | TCP/IP Stack Network Configuration                                                       | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  TCPIP CMD          | TCP/IP Stack Commands                                                                    | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
| TCP/IP Library Application Layer         |  ANNOUNCE           | TCP/IP Stack Library Announce Module for Microchip Devices                | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  Berkeley API       | The Berkeley_Socket_Distribution (BSD) APIs provide a BSD wrapper to the native Microchip TCP/IP Stack APIs.                   | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  DDNS               | The Dynamic DNS Client module provides a method for updating a dynamic IP address to a public DDNS service.                  | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  DHCP CLIENT        | The DHCP client module will allow your application to dynamically obtain an IP address and a subnet mask from a DHCP server on the same network.                  | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  DHCP SERVER        | The DHCP Server module is used to assign IP addresses to DHCP clients from the configured IP address database.                  | ![app-beta](https://img.shields.io/badge/middleware-obsolete-red?style=plastic) |
|                                          |  DHCP SERVER v2     | The DHCP Server module is used to assign IP addresses to DHCP clients from the configured IP address database.                  | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  DHCPV6 CLIENT      | The DHCPv6 client module will allow your application to dynamically obtain an IPv6 address from a DHCPv6 server.                   | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  DNS CLIENT         | The DNS(Domain Name Service) Client, also known as the DNS Resolver                  | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  DNS SERVER         | The DNS(Domain Name Service) Server                | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  FTP CLIENT         | The File Transfer Protocol (FTP) Client module will allow your application to upload the files to, and download the files from, any FTP server.               | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  FTP SERVER         | FTP server capability facilitates the uploading of files to, and downloading of files from, an embedded device.               | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  HTTPNET SERVER     | The HyperText Transfer Protocol (HTTP) Net Server module allows a development board to act as a Web server.               | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  HTTP SERVER v2     | The HyperText Transfer Protocol (HTTP) Server module allows a development board to act as a Web server, version 2.              | ![app-beta](https://img.shields.io/badge/middleware-beta-yellow?style=plastic) |
|                                          |  HTTP SERVER        | The HyperText Transfer Protocol (HTTP) Server module allows a development board to act as a Web server.               | ![app-beta](https://img.shields.io/badge/middleware-obsolete-red?style=plastic) |
|                                          |  IPERF              | Iperf is a networking tool that helps to measure networking bandwidth and performance.               | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  NBNS               | The NetBIOS Name Service protocol associates host names with IP addresses, similarly to DNS, but on the same IP subnet.               | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  SMTP CLIENT        | The SMTPC module in the TCP/IP Stack lets applications send e-mails to any recipient worldwide.              | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  SNMP               | Simple Network Management Protocol             | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  SNMPV3             | Simple Network Management Protocol V3             | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  SNTP               | The SNTP module implements the Simple Network Time Protocol.             | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  TELNET             | Telnet, which runs over a TCP connection, provides bidirectional, interactive communication between two nodes on the Internet or on a Local Area Network.             | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  TFTP CLIENT        | The TCP/IP Stack Library Trivial File Transfer Protocol (TFTP) module is a simple protocol used to transfer files.           | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  TFTP SERVER        | The TCP/IP Stack Library Trivial File Transfer Protocol (TFTP) module is a simple protocol used to transfer files.           | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  ZEROCONF           | Zero configuration (Zeroconf), provides a mechanism to ease the configuration of a device on a network.          | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
| TCP/IP Library Presentation Layer        |  Presentation Layer | The MPLAB Harmony Networking Presentation Layer is an abstracted middleware layer that provides an encrypted channel.                 | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
| TCP/IP Library Transport Layer           |  TCP                | Transmission Control Protocol                | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  UDP                | User Datagram Protocol                  | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
| TCP/IP Library Network Layer             |  ARP                | The Address Resolution Protocol, or ARP, is a foundation layer of TCP/IP                 | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  ICMPv4             | The Internet Control Message Protocol is used to send error and status messages and requests                 | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  IGMP               | The IGMP module implements the IGMPv3 protocol. This protocol is used by hosts and routers to establish multicast group memberships.                 | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  IPv4               | IPv4 is the workhorse protocol of the TCP/IP protocol suite                 | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  IPv6               | IPv6 is the workhorse protocol of the TCP/IP protocol suite                 | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  ICMPv6             | ICMPv6 provides software abstraction of the IPv6 module existent in any TCP/IP Stack implementation. It provides error reporting and diagnostics to the TCP/IP stack.                 | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
|                                          |  NDP                | The NDP (Neighbor Discovery Protocol) for IP version 6 is used by IPv6 nodes on the same link to discover each other's presence.                  | ![app-beta](https://img.shields.io/badge/middleware-production-green?style=plastic) |
| TCP/IP Library Datalink & Physical Layer |  GMAC               | The Ethernet Media Access Controller (GMAC) module                 | ![app-beta](https://img.shields.io/badge/driver-production-green?style=plastic) |
|                                          |  ETH                | The Ethernet Media Access Controller (ETH) module                 | ![app-beta](https://img.shields.io/badge/driver-production-green?style=plastic) |
|                                          |  EMAC               | The Ethernet EMAC Driver module                 | ![app-beta](https://img.shields.io/badge/driver-production-green?style=plastic) |
|                                          |  ETHMAC             | The PIC32 Ethernet Controller module                 | ![app-beta](https://img.shields.io/badge/driver-production-green?style=plastic) |
|                                          |  PPP                | The Point-to-Point Protocol (PPP) module                 | ![app-beta](https://img.shields.io/badge/driver-production-green?style=plastic) |
|                                          |  MIIM Driver        | The MIIM driver clients access PHY registers using the MIIM Driver API                 | ![app-beta](https://img.shields.io/badge/driver-production-green?style=plastic) |
| TCP/IP Library Ethernet PHY Driver       |                     | This library provides a software abstraction for configuring external Ethernet PHY devices for use with the on-chip Ethernet Controller.<br> The supported Ethernet PHYs are: VSC8540, KSZ8041, KSZ8061, KSZ8081, KSZ8091, KSZ8863, KSZ9031, KSZ9131, LAN8700, LAN8720, LAN8740, LAN8742A, LAN9303, DP83640, DP83848, IP101GR, LAN9354, LAN8770, LAN8840 & Dummy PHY | ![app-beta](https://img.shields.io/badge/driver-production-green?style=plastic) |
| TCP/IP Library Ethernet Controller       |  ENCX24J600         | The ENCx24J600 External MAC is an external module to the PIC32 that is connected through a SPI or PSP interface.                 | ![app-beta](https://img.shields.io/badge/driver-production-green?style=plastic) |
|                                          |  ENC28J60           | The ENC28J60 External MAC and PHY is an external module connected through a Serial Peripheral Interface (SPI) to host microcontroller.                 | ![app-beta](https://img.shields.io/badge/driver-production-green?style=plastic) |

____

[![License](https://img.shields.io/badge/license-Harmony%20license-orange.svg)](https://github.com/Microchip-MPLAB-Harmony/net/blob/master/Microchip_SLA001.md)
[![Latest release](https://img.shields.io/github/release/Microchip-MPLAB-Harmony/net.svg)](https://github.com/Microchip-MPLAB-Harmony/net/releases/latest)
[![Latest release date](https://img.shields.io/github/release-date/Microchip-MPLAB-Harmony/net.svg)](https://github.com/Microchip-MPLAB-Harmony/net/releases/latest)
[![Commit activity](https://img.shields.io/github/commit-activity/y/Microchip-MPLAB-Harmony/net.svg)](https://github.com/Microchip-MPLAB-Harmony/net/graphs/commit-activity)
[![Contributors](https://img.shields.io/github/contributors-anon/Microchip-MPLAB-Harmony/net.svg)]()

____

[![Developer Help](https://img.shields.io/badge/Youtube-Developer%20Help-red.svg)](https://www.youtube.com/MicrochipDeveloperHelp)
[![Developer Help](https://img.shields.io/badge/XWiki-Developer%20Help-torquiose.svg)](https://developerhelp.microchip.com/xwiki/bin/view/software-tools/harmony/)
[![Follow us on Youtube](https://img.shields.io/badge/Youtube-Follow%20us%20on%20Youtube-red.svg)](https://www.youtube.com/user/MicrochipTechnology)
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Follow%20us%20on%20LinkedIn-blue.svg)](https://www.linkedin.com/company/microchip-technology)
[![Follow us on Facebook](https://img.shields.io/badge/Facebook-Follow%20us%20on%20Facebook-blue.svg)](https://www.facebook.com/microchiptechnology/)
[![Follow us on Twitter](https://img.shields.io/twitter/follow/MicrochipTech.svg?style=social)](https://twitter.com/MicrochipTech)

[![](https://img.shields.io/github/stars/Microchip-MPLAB-Harmony/net.svg?style=social)]()
[![](https://img.shields.io/github/watchers/Microchip-MPLAB-Harmony/net.svg?style=social)]()



