# Simple Network Management Protocol (SNMPv3)  Configurations

**SNMPv3 Specific User Configurations**

- **Maximum Security Entries:**
    - Maximum number of SNMPv3 users.
    - User Security Model should have at least 1 user.
    - Default value: 3
    - Symbol TCPIP_SNMPV3_USM_MAX_USER

- **User Name:**
    - User name for SNMPv3 entry
    - Default value: "microchip"/"SnmpAdmin"/"root"
    - Symbol TCPIP_SNMPV3_STACK_USM_NAME_IDXn

- **Security Level:**
    - Security level for SNMPv3 entry
    - Default value: "AUTH_PRIV"/"AUTH_NO_PRIV"/"NO_AUTH_NO_PRIV"
    - Symbol TCPIP_SNMPV3_STACK_SECURITY_LEVEL_IDXn

- **Authentication Protocol:**
    - SNMPv3 authentication protocol
    - Default value: "SNMPV3_HMAC_MD5"/"SNMPV3_HMAC_SHA1"/"SNMPV3_NO_HMAC_AUTH"
    - Symbol TCPIP_SNMPV3_STACK_AUTH_PROTOCOL_IDXn

- **Authentication Protocol Password:**
    - Authentication password
    - Default value: "auth12345"/"ChandlerUS"/""
    - Symbol TCPIP_SNMPV3_STACK_AUTH_PASSWORD_IDXn

- **Privacy Protocol:**
    - SNMPv3 privacy protocol
    - Default value: "SNMPV3_AES_PRIV"/"SNMPV3_NO_PRIV"/"SNMPV3_NO_PRIV"
    - Symbol TCPIP_SNMPV3_STACK_PRIV_PROTOCOL_IDXn

- **Privacy Protocol Password:**
    - SNMPv3 privacy protocol password
    - Default value: "priv12345"/""/""
    - Symbol TCPIP_SNMPV3_STACK_PRIV_PASSWORD_IDXn

- **Target Entry Message Protocol Type:**
    - SNMPv3 message protocol type
    - Default value: "SNMPV3_MSG_PROCESSING_MODEL"
    - Symbol TCPIP_SNMPV3_TARGET_ENTRY_MESSAGE_PROTOCOL_TYPE_IDXn

- **Target Entry Security Model Type:**
    - SNMPv3 target entry model type
    - Default value: "SNMPV3_USM_SECURITY_MODEL"
    - Symbol TCPIP_SNMPV3_TARGET_ENTRY_SEC_MODEL_TYPE_IDXn

- **V1 and V2 Trap Support:**
    - Enable V1 and V2 trap support
    - Default value: true
    - Symbol TCPIP_SNMPV3_STACK_USE_V1_V2_TRAP

- **User Security Name Length:**
    - Maximum size for SNMPv3 User Security Name length.
    - Default value: 16
    - Symbol TCPIP_SNMPV3_USER_SECURITY_NAME_LEN

- **Localized Password Key Length:**
    - SNMPv3 Authentication Localized password key length size
    - Default value: 20
    - Symbol TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN

- **Privacy Password Key Length:**
    - SNMPv3 Privacy Password key length size
    - Default value: 20
    - Symbol TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN

