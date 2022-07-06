# Zeroconf Module

Zero configuration \(Zeroconf\), provides a mechanism to ease the configuration of a device on a network. It also provides for a more human-like naming convention, instead of relying on IP addresses alone. Zeroconf also goes by the names Bonjour \(Apple\) and Avahi \(Linux\), and is an IETF standard

**Abstraction Model**

Zeroconf Software Abstraction Model:

![TCPIP ZEROCONF Abstraction Model](GUID-5CBF2A9D-5D30-4DC7-B0E9-A64478F5F1E0-low.png)

Bonjour Service Model:

![TCPIP ZEROCONF Bonjour Service](GUID-3E062FBA-C554-4C42-9E29-B0FB928BE33C-low.png)

**Enabling**

Zeroconf can be enabled in the MPLAB Harmony Configurator, or by setting the following two defines in system\_config.h file:

-   TCPIP\_STACK\_USE\_ZEROCONF\_LINK\_LOCAL

-   TCPIP\_STACK\_USE\_ZEROCONF\_MDNS\_SD


**Link Local \(Zeroconf\)**

The first component of Zeroconf is the ability to self-assign an IP address to each member of a network. Normally, a DHCP server would handle such situations. However, in cases where no DHCP server exists, Zeroconf enabled devices negotiate unique IP addresses amongst themselves.

**mDNS**

The second component of Zeroconf is the ability to self-assign human-readable hostnames for themselves. Multicast DNS provides a local network the ability to have the features of a DNS server. Users can use easily remembered hostnames to accesses the devices on the network. In the event that devices elect to use the same hostname, as in the IP address resolution, each of the devices will auto-negotiate new names for themselves \(usually by appending a number to the end of the name\).

**Service Discovery**

The last component of Zeroconf is service discovery. All Zeroconf devices can broadcast what services they provide. For instance, a printer can broadcast that it has printing services available. A thermostat can broadcast that it has an HVAC control service. Other interested parties on the network who are looking for certain services can then see a list of devices that have the capability of providing the service, and\_connect\_directly to it. This further eliminates the need to know whether something exists on a network \(and what its IP or hostname is\). As an end-user, all you would need to do is query the network if a certain service exists, and easily\_connect\_to it.

**Zeroconf Enabled Environments**

All Apple products have Zeroconf enabled by default. On Windows, you'll need to download the Safari web browser, and during the install, enable support for Bonjour. Note that in the Safari browser, you can browse and see a list of all Bonjour enabled devices, and click through to them automatically.

**Library Interface**

|Name|Description|
|----|-----------|
|**Multicast DNS Functions**||
|TCPIP\_MDNS\_ServiceDeregister|DNS Service Discovery function for end-user to deregister a service advertisement, which was previously registered with the TCPIP\_MDNS\_ServiceRegister function.|
|TCPIP\_MDNS\_ServiceRegister|DNS Service Discovery function for end-users to register a service advertisement. The service is associated with all interfaces.|
|TCPIP\_MDNS\_ServiceUpdate|DNS-Service Discovery function for end-user to update the service advertisement, which was previously registered with TCPIP\_MDNS\_ServiceRegister.|
|TCPIP\_MDNS\_Task|Standard TCP/IP stack module task function.|
|**Link Local Functions**||
|TCPIP\_ZCLL\_Disable|Disables Zero Configuration on the specified interface.|
|TCPIP\_ZCLL\_Enable|Enables Zero Configuration on the specified interface.|
|TCPIP\_ZCLL\_IsEnabled|Returns whether or not an interface is enabled for zero configuration|
|TCPIP\_ZCLL\_Task|Standard TCP/IP stack module task function.|
|**Data Types and Constants**||
|ZCLL\_MODULE\_CONFIG|Placeholder for Zero Configuration Link Layer module configuration.|
|MDNSD\_ERR\_CODE|void DisplayHostName\(uint8\_t \*HostName\);|

-   **[MDNSD\_ERR\_CODE Enumeration](GUID-890B63B7-0798-40C8-90F2-75C029CC66FC.md)**  

-   **[TCPIP\_MDNS\_ServiceDeregister Function](GUID-9DF34C88-1362-40AF-96CB-91D235E15F99.md)**  

-   **[TCPIP\_MDNS\_ServiceRegister Function](GUID-8AE11F4C-45C5-4D5D-91FA-DC93DCFAAF74.md)**  

-   **[TCPIP\_MDNS\_ServiceUpdate Function](GUID-AB645995-18DD-49C5-8A13-52F35B0D6E60.md)**  

-   **[TCPIP\_MDNS\_Task Function](GUID-6CA1CA3F-29AE-487B-BD5E-5803D9E9A04E.md)**  

-   **[TCPIP\_ZCLL\_Disable Function](GUID-13542F9F-FB82-4857-A222-F39F35DA80C7.md)**  

-   **[TCPIP\_ZCLL\_Enable Function](GUID-BEC868B6-5E68-43CA-B0BD-06EA115D4339.md)**  

-   **[TCPIP\_ZCLL\_IsEnabled Function](GUID-CA63B30E-EABD-4DDB-B676-1416AB83A279.md)**  

-   **[TCPIP\_ZCLL\_Task Function](GUID-CA22AE73-6689-44DB-8A43-36DF320C8ECC.md)**  

-   **[ZCLL\_MODULE\_CONFIG Structure](GUID-E315EB00-D610-404F-9F5C-F62B206F19B5.md)**  


**Parent topic:**[MPLAB® Harmony TCP/IP Library](GUID-01A0A1D8-EC9B-4EFF-B8E4-D154B555FEF2.md)

