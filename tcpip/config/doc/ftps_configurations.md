# File Transfer Protocol (FTP)  Configurations

**FTP Server Specific User Configurations**

- **Maximum Number of FTP Connections Allowed per Interface:**
    - Maximum number of FTP connections allowed
    - Default value: 1
    - Symbol TCPIP_FTP_MAX_CONNECTIONS

- **FTP Data Socket TX Buffer Size:**
    - Define the size of the TX buffer for the FTP Data socket
    - Use 0 for default TCP TX buffer size.
    - The default recommended value for high throughput is > 2MSS (3 KB).
    - The performance of a socket is highly dependent on the size of its buffers so it is a good idea to use as large as possible buffers for the sockets that need high throughput. 
    - Default value: 0
    - Symbol TCPIP_FTP_DATA_SKT_TX_BUFF_SIZE

- **FTP Data Socket RX Buffer Size:**
    - Define the size of the RX buffer for the FTP Data socket
    - Use 0 for default TCP RX buffer size.
    - The default recommended value for high throughput is > 2MSS (3 KB).
    - The performance of a socket is highly dependent on the size of its buffers so it is a good idea to use as large as possible buffers for the sockets that need high throughput. 
    - Default value: 0
    - Symbol TCPIP_FTP_DATA_SKT_RX_BUFF_SIZE

- **FTP Server Root Directory Path:**
    - The root of the FTP server directory
    - The server will have access only to the files beneath this path
    - Default value: none
    - Symbol TCPIP_FTP_MOUNT_POINT

- **FTP Task Rate (in msec):**
    - FTP task processing rate, in milliseconds.
    - The FTP module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 33
    - Symbol TCPIP_FTPS_TASK_TICK_RATE

- **Command Listening Port:**
    - FTP Server command listening port
    - Default value: 21
    - Symbol TCPIP_FTPS_COMMAND_LISTEN_PORT

- **Data Listening Port:**
    - FTP Server data listening port
    - Default value: 20
    - Symbol TCPIP_FTPS_DATA_LISTEN_PORT


- **FTP Server timeout (in sec):**
    - FTP server timeout
    - Value is in seconds
    - Default value: 180
    - Symbol TCPIP_FTP_TIMEOUT

- **FTP File PUT Command:**
    - Enable/disable FTP Put command
    - Note: the mounted file system has to support write operations
    - Default value: true
    - Symbol TCPIP_FTP_PUT_ENABLED

- **Select FTP Authentication:**
    - Select the type of FTP authentication: run time or build time (obsolete)
    - Note: the build time authentication stores the FTP user and password in clear in the code and it should be avoided!
    - Default value: Run time Authentication
    - Symbol TCPIP_FTPS_AUTH_CONFIG

- **Connection Info Used for Authentication:**
    - The connection info is passed to the authentication handler
    - The handler has access at data showing which IP address requested the connection
    - Used only for run time authentication
    - Default value: true
    - Symbol TCPIP_FTPS_AUTHENTICATION_CONN_INFO

- **FTP Login User Name:**
    - Obsolete build time login user name
    - Default value: none
    - Symbol TCPIP_FTP_USER_NAME

- **Maximum Length for User Name:**
    - Specifies the max length for user name
    - Default value: 10
    - Symbol TCPIP_FTP_USER_NAME_LEN


- **FTP Login Password:**
    - Obsolete build time login password
    - Default value: none
    - Symbol TCPIP_FTP_PASSWORD

- **Maximum Length of FTP Login Password:**
    - Obsolete build time login password
    - Specifies the max length of FTP login password
    - Default value: 10
    - Symbol TCPIP_FTP_PASSWD_LEN








