# Ethernet PHY Driver  Configurations

**PHY Driver Specific User Configurations**

- **PHY Address:**
    - The address of this PHY on the MIIM bus
    - Default value: 0
    - Symbol TCPIP_INTMAC_PHY_ADDRESS

- **Use a Function to be called at PHY Reset:**
    - Use a callback function when the PHY is reset
    - Default value: false
    - Symbol DRV_ETHPHY_USE_RESET_CALLBACK

- **App Function:**
    - The reset callback function name
    - Default value: "AppPhyResetFunction"
    - Symbol DRV_ETHPHY_RESET_CALLBACK

- **RMII Data Interface:**
    - Use RMII data interface for the MAC <-> PHY connection
    - Otherwise it will be MII
    - Default value: depends on platform
    - Symbol TCPIP_INTMAC_PHY_CONFIG_RMII

- **Configuration Fuses Is ALT:**
    - For platforms that support multiple I/O configuration fuses for the Ethernet pins this will select an alternate I/O pin setting.
    - Otherwise is the default I/O setting
    - Default value: false
    - Symbol TCPIP_INTMAC_PHY_CONFIG_ALTERNATE

- **Use The Fuses Configuration:**
    - For platforms that support multiple Ethernet I/O pins and MII/RMII configuration fuses this setting will instruct the PHY driver to use the duse settings for configuring the PHY
    - Default value: true
    - Symbol TCPIP_INTMAC_PHY_CONFIG_AUTO

- **Delay for the Link Initialization - ms:**
    - The value of the delay for the link initialization
    - This insures that the PHY is ready to transmit after it is reset
    - A usual value is > 500 ms up to 2000 ms.
    - The longer the delay, the less chance that the PHY drops packets when the link is established
    - Adjust to your needs.
    - Value is in milliseconds
    - Default value: 500
    - Symbol TCPIP_INTMAC_PHY_LINK_INIT_DELAY, TCPIP_EMAC_PHY_LINK_INIT_DELAY

- **PHY Negotiation Time-out - ms:**
    - Value of the PHY negotiation initiation time out as per IEEE 802.3 spec.
    - Value is in milliseconds
    - Default value: 1
    - Symbol DRV_ETHPHY_NEG_INIT_TMO

- **PHY Negotiation Done Time-out - ms:**
    - Value of the PHY negotiation complete time out as per IEEE 802.3 spec.
    - This definition sets the time out of the PHY negotiation complete
    - Value is in milliseconds
    - Notes:
        - See IEEE 802.3 Clause 28 Table 28-9 autoneg_wait_timer value (max 1s).
    - Default value: 2000
    - Symbol DRV_ETHPHY_NEG_DONE_TMO

- **PHY Reset Clear Time-out - ms:**
    - Value of the PHY Reset self clear time out as per IEEE 802.3 spec.
    - This definition sets the time out of the PHY Reset self clear.
    - Note:
        - See IEEE 802.3 Clause 22 Table 22-7 and paragraph "22.2.4.1.1 Reset" (max 0.5s)
    - Value is in milliseconds
    - Default value: 500
    - Symbol DRV_ETHPHY_RESET_CLR_TMO

- **PHY Instances Number:**
    - Selects the maximum number of hardware instances that can be supported by the dynamic driver.
    - Note:
        - This value is currently read-only
    - Default value: 1
    - Symbol DRV_ETHPHY_INSTANCES_NUMBER

- **PHY Clients Number:**
    - Ethernet PHY Maximum Number of Clients
    - This definition selects the maximum number of clients that the Ethernet PHY driver can support at run time.
    - Notes:
        - The MAC driver is the only client of the PHY driver and the number of clients should always be 1.
        - This value is currently read-only
    - Default value: 1
    - Symbol DRV_ETHPHY_CLIENTS_NUMBER

- **PHY Peripheral Index Number:**
    - This definition selects the Ethernet PHY static index for the driver object reference.
    - Notes:
        - The only value currently supported is 1
        - This value is currently read-only
    - Default value: 1
    - Symbol DRV_ETHPHY_INDEX

- **PHY Peripheral ID:**
    - Defines the value of the peripheral ID.
    - Notes:
        - The only value currently supported is 1
        - This value is currently read-only
    - Default value: 1
    - Symbol DRV_ETHPHY_PERIPHERAL_ID


