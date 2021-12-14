# Telnet Server  Configurations

**Telnet Specific User Configurations**


- **Maximum Connections:**
    - Maximum number of Telnet connections
    - Default value: 2
    - Symbol TCPIP_TELNET_MAX_CONNECTIONS


- **Default Socket TX Buffer Size:**
    - Size of the telnet socket TX buffer
    - If long messages are sent over the telnet connection the socket should have a large enough buffer to write the data at once
    - Leave 0 for default TCP TX buffer size
    - Default value: 0
    - Symbol TCPIP_TELNET_SKT_TX_BUFF_SIZE


- **Default Socket RX Buffer Size:**
    - Size of the telnet socket RX buffer
    - If long messages are sent over the telnet connection the socket should have a large enough buffer to receive the data
    - Leave 0 for default TCP RX buffer size
    - Default value: 0
    - Symbol TCPIP_TELNET_SKT_RX_BUFF_SIZE


- **Telnet Server Listening Port:**
    - Port on which the telnet server is listening
    - Default is TCPIP_TELNET_SERVER_PORT (23) or TCPIP_TELNET_SERVER_SECURE_PORT (992) 
    - Adjust as needed
    - Default value: 23
    - Symbol TCPIP_TELNET_LISTEN_PORT


- **Telnet Task Rate (in msec):**
    - Telnet task processing rate, in milliseconds.
    - The Telnet module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 100
    - Symbol TCPIP_TELNET_TASK_TICK_RATE

- **Size of the Internal Print Buffer:**
    - Size of the internal print buffer
    - This buffer is used when the telnet console printf style function is called
    - Adjust based on the length of the messages to be formatted with print statements
    - This buffer is created in the automatic stack.
    - Make sure that there is enough stack space for this buffer!
    - Default value: 200
    - Symbol TCPIP_TELNET_PRINT_BUFF_SIZE


- **Size of the Internal Line Buffer:**
    - Size of the internal line buffer
    - This buffer is used for receiving and assembling the password, authentication and regular characters
    - Adjust based on the length of the commands sent to the telnet server
    - This buffer is created in the automatic stack.
    - Make sure that there is enough stack space for this buffer!
    - Default value: 80
    - Symbol TCPIP_TELNET_LINE_BUFF_SIZE

- **Size of the Internal User Name Buffer:**
    - Maximum size of the internal buffer to store the user name
    - Adjust based on the length of the user names allowed for the telnet connections
    - A buffer of this size if allocated for each connection to store the login user name 
    - Longer user names will be truncated to this size
    - Default value: 15
    - Symbol TCPIP_TELNET_USERNAME_SIZE


- **Create Telnet Sockets with NO-DELAY option:**
    - Create the telnet sockets with NO_DELAY option
    - Default value: false
    - Symbol TCPIP_TELNET_CONFIG_FLAG_NO_DELAY, TCPIP_TELNET_FLAG_NO_DELAY

- **Pass Control Characters to Console:**
    - Pass the telnet commands, options, controls, etc. within the messages to the console
    - By default they are removed and only printable characters are sent to the console
    - Enable only for debug or special purposes.
    - Default value: false
    - Symbol TCPIP_TELNET_CONFIG_PASS_CONTROL_CHARS, TCPIP_TELNET_FLAG_PASS_CONTROL_CHARS

- **Select telnet Authentication:**
    - Select the type of telnet authentication: run time or build time (obsolete)
    - Runtime authentication: this is the default and the PREFERRED case!
        - The telnet module will call a registrated callback to authenticate the users.
        - It is the preferred authentication method.
        - It requires the run time registration of an authentication handler
        - Until the moment the authentication handler is registered any login attempts will fail!

    - The old OBSOLETE build time authentication method
        - TCPIP_TELNET_USERNAME and TCPIP_TELNET_PASSWORD is used
        - This method is DEPRECATED and it will be eventually removed!
        - It is recommended that you leave the setting to false!
    - Note: the build time authentication stores the telnet user and password in clear in the code and it should be avoided!
    - Default value: false (Run time Authentication)
    - Symbol TCPIP_TELNET_OBSOLETE_AUTHENTICATION, TCPIP_TELNET_AUTH_CONFIG


- **Connection Info Used for Authentication:**
    - The connection info is passed to the authentication handler
    - The handler has access at data showing which IP address requested the connection
    - Used only for run time authentication
    - If false no connection info data is provided to the authentication callback
        - This is useful for the default case when all connections will use the same username/password.
    - If true the authentication callback will contain the connection info data to allow different processing based on the source of the authentication request
    - Note: This flag is relevant only when TCPIP_TELNET_OBSOLETE_AUTHENTICATION == false
    - Default value: true
    - Symbol TCPIP_TELNET_AUTHENTICATION_CONN_INFO



- **User Name:**
    - Default Telnet user name
    - DEPRECATED. This is used only when there is no authentication callback defined!
    - Use only when TCPIP_TELNET_OBSOLETE_AUTHENTICATION == true
    - Default value: "admin"
    - Symbol TCPIP_TELNET_USERNAME

- **Password:**
    - Default Telnet password
    - DEPRECATED. This is used only when there is no authentication callback defined!
    - Use only when TCPIP_TELNET_OBSOLETE_AUTHENTICATION == true
    - Default value: "microchip"
    - Symbol TCPIP_TELNET_PASSWORD

