# Migrate MHC based TCP/IP Demos to TCP/IP Configurator

Follow the steps to migrate existing TCP/IP application demo to work with TCP/IP Configurator.

-   Open TCP/IP application demo to migrate, in MPLAB X IDE

-   Open MCC project graph

-   \(Only for SAME70/V71 projects\) Clear user setting of GMAC descriptor for MPU in System configurations

-   Remove all TCP/IP Components in Driver Layer Group box \(Note down any user configurations made in the removed components. This is needed when adding these modules using TCP/IP Configurator\)

-   Remove Driver Layer Group box

-   Remove all TCP/IP Components in Basic Configuration Layer Group box. \(Note down any user configurations made in the removed components. This is needed when adding these modules using TCP/IP Configurator\)

-   Open group boxes of TCP/IP Stack and remove 'Configurators': for example, ‘TCP/IP Application Layer Configurator’ from TCPIP STACK/APPLICATION LAYER

-   Open TCP/IP Configurator from plugin drop-down menu

-   In Basic Config layer, add TCP/IP Core and TCP/IP CMD modules. Apply user settings to the modules, if any.

-   In Data Link layer, add Network Interface by clicking '+'. Drag and drop MAC and PHY modules. Apply user settings to the modules, if any

-   Make connections between Network Interface, MAC and PHY

-   Open project graph in MCC

-   In root layer of project graph, connect Console dependency on TCP/IP STACK to SYS\_CONSOLE module.

-   Save Configuration


**Parent topic:**[TCPIP Configurator](GUID-F4E4B0BB-BCBC-45A1-8F5B-7E4D76C83CF3.md)

