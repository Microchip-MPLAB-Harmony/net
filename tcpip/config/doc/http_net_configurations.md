# HyperText Transfer Protocol (HTTP) Net  Configurations

**HTTP_NET Specific User Configurations**


- **HTTP NET Socket TX Buffer Size:**
    - Define the size of the TX buffer for the HTTP socket
    - Use 0 for default TCP socket value
    - The default recommended value for high throughput is > 2MSS (3 KB).
    - The performance of a socket is highly dependent on the size of its buffers so it is a good idea to use as large as possible buffers for the sockets that need high throughput. 
    - Default value: 1024
    - Symbol TCPIP_HTTP_NET_SKT_TX_BUFF_SIZE

- **HTTP NET Socket RX Buffer Size:**
    - Define the size of the RX buffer for the HTTP socket
    - Use 0 for default TCP socket value
    - The default recommended value for high throughput is > 2MSS (3 KB).
    - The performance of a socket is highly dependent on the size of its buffers so it is a good idea to use as large as possible buffers for the sockets that need high throughput. 
    - Default value: 1024
    - Symbol TCPIP_HTTP_NET_SKT_RX_BUFF_SIZE

- **Maximum Number of Simultaneous Connections:**
    - Maximum numbers of simultaneous supported HTTP connections.
    - Default value: 4
    - Symbol TCPIP_HTTP_NET_MAX_CONNECTIONS

- **HTTP NET Listening Port:**
    - HTTP listening port: 80, 443, etc.
    - Default value: 80
    - Symbol TCPIP_HTTP_NET_LISTEN_PORT


- **Web Pages Directory:**
    - Web pages directory
    -  The HTTP server will serve pages located under this top directory.
    -  No access is possible above this directory.
    -  As a good practice to follow, use something like: "/mnt_point/webpages/" to limit an external user access to this only directory in the file system when using the Web server
    -  NOTE: to allow access to the whole file system use the root directory: "/mnt_point/" or "/" depending on your SYS_FS settings/mounting
    -  But this usage should be discouraged because it gives HTTP access to all system files.
    - Default value: "/mnt/mchpSite1/"
    - Symbol TCPIP_HTTP_NET_WEB_DIR


- **Enable Console Commands:**
    - Enable the HTTP_NET TCP/IP Console Commands
    - Default value: false
    - Symbol: TCPIP_HTTP_NET_CONSOLE_CMD

- **Include HTTP NET Custom Template:**
    - Add the HTTP_NET template to the project
    - This is a model of web pages and processing
    - Default value: true
    - Symbol TCPIP_HTTP_NET_CUSTOM_TEMPLATE

- **Web pages source directory path:**
    - Location of the web pages source
    - Default value: web_pages_directory + '\web_pages'
    - Symbol TCPIP_HTTP_NET_WEBPAGE_DIRECTORY_PATH

- **Web pages destination directory path:**
    - Destination of the web pages in the project
    - Default value: "<project_path>/firmware/src/web_pages"
    - Symbol TCPIP_HTTP_NET_DEST_WEBPAGE_DIRECTORY_PATH


- **HTTP_NET Task Rate (in msec):**
    - HTTP_NET task processing rate, in milliseconds.
    - The HTTP_NET module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Default value: 33
    - Symbol TCPIP_HTTP_NET_TASK_RATE

- **:Maximum Data Length (bytes) for Reading Cookie and GET/POST Arguments**
    - Define the maximum size of the buffer for reading cookie and GET/POST arguments (bytes)
    - Default value: 100
    - Symbol TCPIP_HTTP_NET_MAX_DATA_LEN


- **Buffer Size for Sending Response Messages to the Client:**
    - Size of the buffer used for sending the response messages to the client.
    - Should be able to accommodate the longest server response
    - Default setting should be 300 bytes
    - Default value: 300
    - Symbol TCPIP_HTTP_NET_RESPONSE_BUFFER_SIZE

- **Buffer Size for Sending Cookies to the Client:**
    - Size of the buffer used for sending the cookies to the client.
    - Should be able to accommodate the longest cookie response. Otherwise the cookies will be truncated.
    - Default value: 200
    - Symbol TCPIP_HTTP_NET_COOKIE_BUFFER_SIZE


- **Peek Buffer Size for Performing Searches:**
    - Size of the peek buffer to perform searches into. 
    - If the underlying transport layer supports peak operation with a offset, the value could be smaller (80 characters, for example);
    - Otherwise, a one time peek is required and the buffer should be larger:
        - recommended to be close to the size of the socket RX buffer.
    - Note - if upload is enabled (TCPIP_HTTP_NET_FILE_UPLOAD_ENABLE) then it is recommended for the peek buffer size to match the size of the socket RX buffer: TCPIP_HTTP_NET_SKT_RX_BUFF_SIZE  
    - Note that this is an automatic buffer (created on the stack) and enough stack space should be provided for the application. 
    - If left to 0 it will use the size of the socket RX buffer - this should be the default
    - Default value: 0
    - Symbol TCPIP_HTTP_NET_FIND_PEEK_BUFF_SIZE

- **File Processing Buffer Size:**
    - Size of the buffer used for processing HTML, dynamic variable and binary files.
    - For dynamic variable files it should be able to accommodate the longest HTML line size, including CRLF!
    - Default value: 512
    - Symbol TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_SIZE

- **Number of File Buffers to be Created:**
    - Number of file buffers to be created;
    - These buffers are used to store data while file processing is done
    - They are organized in a pool
    - Each file being processed needs a file buffer and tries to get it from the pool
    - If a buffer is not available, the HTTP conenction will wait for one to become available.
    - Once the file is done the file buffer is released and could be used by a different file
    - The number depends on the number of files that are processed in parallel
    - To avoid deadlock the number should be >= than the number of maximum files that can be open simultaneously:
    - i.e. for file1 ->include file2 -> include file3 you will need >= 3 file process buffers
    - Default value: 4
    - Symbol TCPIP_HTTP_NET_FILE_PROCESS_BUFFERS_NUMBER


- **Retry Limit for Allocating a File Buffer from the Pool:**
    - Retry limit for allocating a file buffer from the pool.
    - If more retries are not successful the operation will be aborted.
    - Default value: 10
    - Symbol TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_RETRIES

- **Number of Chunks to be Created:**
    - Number of chunks that are created
    - It depends on the TCPIP_HTTP_NET_MAX_RECURSE_LEVEL and on the number of connections
    - Maximum number should be TCPIP_HTTP_NET_MAX_CONNECTIONS * TCPIP_HTTP_NET_MAX_RECURSE_LEVEL
        - i.e. TCPIP_HTTP_NET_MODULE_CONFIG::nConnections * TCPIP_HTTP_NET_MODULE_CONFIG::nChunks
    - All the chunks are in a pool and are used by all connections
    - Default value: 10
    - Symbol TCPIP_HTTP_NET_CHUNKS_NUMBER

- **Retry Limit for Allocating a Chunk from the Pool:**
    - Retry limit for allocating a chunk from the pool
    - If retries are not successful the operation will be aborted
    - Default value: 10
    - Symbol TCPIP_HTTP_NET_CHUNK_RETRIES

- **Maximum Depth of Recursive Calls for Serving a Web Page:**
    - The maximum depth of recursive calls for serving a web page:
        - no dynvars files: 1
        - file including a file: 2
        - if the include file includes another file: +1
        - if a dyn variable: +1
    - Default value: 3
    - Symbol TCPIP_HTTP_NET_MAX_RECURSE_LEVEL

- **Maximum Header Length:**
    - The length of longest header string that can be parsed
    - Default value: 15
    - Symbol TCPIP_HTTP_NET_MAX_HEADER_LEN

- **Maximum Lifetime of Static Responses (in sec):**
    - Max lifetime (sec) of static responses as string
    - Value is in seconds (string)
    - Default value: "600" 
    - Symbol TCPIP_HTTP_NET_CACHE_LEN

- **Socket Disconnect Timeout (in sec):**
    - Max time (sec) to await for more HTTP client data in an active connection state before timing out and disconnecting the socket
    - Value is in seconds
    - Default value: 45
    - Symbol TCPIP_HTTP_NET_TIMEOUT


- **Default HTTP NET File:**
    - Indicate what HTTP file to serve when no specific one is requested
    - Default value: "index.htm"
    - Symbol TCPIP_HTTP_NET_DEFAULT_FILE


- **Maximum Size of a HTTP File Name:**
    - Maximum size of a HTTP file name with the path removed from the file name
    - One extra char added for the string terminator
    - Default value: 25
    - Symbol TCPIP_HTTP_NET_FILENAME_MAX_LEN

- **Enable MPFS Update via HTTP NET:**
    - Configure MPFS over HTTP updating
    - Enable/disable updating via HTTP
    - Default value: false
    - Symbol TCPIP_HTTP_NET_FILE_UPLOAD_ENABLE

- **MPFS Upload Page Name:**
    - The default MPFS upload page name
    - Default value: "mpfsupload"
    - Symbol TCPIP_HTTP_NET_FILE_UPLOAD_NAME

- **MPFS NVM Mount Path:**
    - NVM mount path for a file upload
    - Default value: "/mnt/mchpSite1"
    - Symbol TCPIP_HTTP_NET_MPFS_NVM_PATH

- **MPFS NVM Disk Path:**
    - NVM disk path for a file upload
    - Default value: "/dev/nvma1"
    - Symbol TCPIP_HTTP_NET_MPFS_NVM_VOL

- **MPFS NVM Disk Number:**
    - NVM disk number for a file upload
    - Default value: 0
    - Symbol TCPIP_HTTP_NET_MPFS_NVM_NUM

- **Enable POST Support:**
    - Enable POST support
    - Default value: true
    - Symbol TCPIP_HTTP_NET_USE_POST

- **Enable Cookie Support:**
    - Enable cookie processing and support
    - Default value: true
    - Symbol TCPIP_HTTP_NET_USE_COOKIES

- **Use Base 64 Decode:**
    - Enable the usage of Bas64 encode/decode
    - Default value: false
    - Symbol TCPIP_HTTP_NET_USE_BASE64_DECODE

- **Enable Basic Authentication Support:**
    - Enable HTTP basic authentication support
    - Parses the "Authorization:" header for a request and verifies the credentials.
    - Default value: true
    - Symbol TCPIP_HTTP_NET_USE_AUTHENTICATION

- **Use non-persistent connections:**
    - Use non-persistent connections
    - This flag will cause the HTTP connections to be non-persistent and closed after serving each request to the client
    - By default the HTTP connections are persistent
    - Default value: false
    - Symbol TCPIP_HTTP_NET_CONFIG_FLAG_NON_PERSISTENT, TCPIP_HTTP_NET_CONFIG_FLAGS

- **HTTP sockets created with NO-DELAY option:**
    - Create the HTTP sockets with NO-DELAY option.
    - The socket will flush data as soon as possible.
    - Default value: false
    - Symbol TCPIP_HTTP_NET_CONFIG_FLAG_NO_DELAY, TCPIP_HTTP_NET_CONFIG_FLAGS

- **All HTTP connections have to be secure:**
    - All HTTP connections have to be secure (supposing the network presentation layer supports encryption)
    - Cannot be used together with TCPIP_HTTP_NET_MODULE_FLAG_SECURE_OFF
    - Default value: false
    - Symbol TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_ON, TCPIP_HTTP_NET_CONFIG_FLAGS


- **All HTTP connections have to be non-secure:**
    - HTTP connections will be non-secure
    - Cannot be used together with TCPIP_HTTP_NET_MODULE_FLAG_SECURE_ON
    - Default value: false
    - Symbol TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_OFF, TCPIP_HTTP_NET_CONFIG_FLAGS


- **HTTP security is based on the port numbers:**
    - HTTP security setting is dictated by the port numbers
    - Default value: true
    - Symbol TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_DEFAULT, TCPIP_HTTP_NET_CONFIG_FLAGS


- **Enable the Processing of Dynamic Variables:**
    - This symbol enables the processing of dynamic variables
    - Make it evaluate to false (0) if dynamic variables are not needed
    - All the following symbols referring to dynamic variables are relevant only when TCPIP_HTTP_NET_DYNVAR_PROCESS != 0
    - Default value: true
    - Symbol TCPIP_HTTP_NET_DYNVAR_PROCESS

- **Number of the Descriptors for Dynamic Variables Processing:**
    - How many buffers descriptors for dynamic variable processing
    - They are independent of the HTTP connection number
    - All the HTTP connections use from the dynamic descriptors pool
    - Default value: 10
    - Symbol TCPIP_HTTP_NET_DYNVAR_DESCRIPTORS_NUMBER


- **Maximum Size for a Complete Dynamic Variable: Name + Args:**
    - Maximum size for a complete dynamic variable: name + args
    - Must be <= TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_SIZE!
    - If it is much larger than needed then inefficiency occurs when reading data from the file and then discarding it because a much larger than needed data buffer was read
    - Default value: 50
    - Symbol TCPIP_HTTP_NET_DYNVAR_MAX_LEN

- **Maximum Number of Arguments for a Dynamic Variable:**
    - Maximum number of arguments for a dynamic variable
    - Default value: 10
    - Symbol TCPIP_HTTP_NET_DYNVAR_ARG_MAX_NUMBER

- **Retry Limit for a Dynamic Variable Processing:**
    - Retry limit for a dynamic variable processing
    - This puts a limit on the number of times a dynamic variable "dynamicPrint" function can return TCPIP_HTTP_DYN_PRINT_RES_AGAIN/TCPIP_HTTP_DYN_PRINT_RES_PROCESS_AGAIN and avoids having the HTTP code locked up forever.
    - If more retries are attempted the processing will be considered done and dynamicPrint function will not be called again
    - Default value: 10
    - Symbol TCPIP_HTTP_NET_DYNVAR_PROCESS_RETRIES

- **Enable the Processing of SSI Commands:**
    - This symbol enables the processing of SSI commands
    - Make it evaluate to false (0) if SSI commands are not needed
    - All the following symbols referring to SSI commands are relevant only when TCPIP_HTTP_NET_SSI_PROCESS != 0
    - Default value: true
    - Symbol TCPIP_HTTP_NET_SSI_PROCESS


- **Maximum Number of Attributes for a SSI Command:**
    - Maximum number of attributes for a SSI command
    - Most SSI commands take just one attribute/value pair per line but multiple attribute/value pairs on the same line are allowed where it makes sense
    - Default value: 4
    - Symbol TCPIP_HTTP_NET_SSI_ATTRIBUTES_MAX_NUMBER

- **Number of Static Attributes Associated to a SSI Command:**
    - Number of static attributes associated to a SSI command
    - If the command has more attributes than this number the excess will be allocated dynamically 
    - Default value: 2
    - Symbol TCPIP_HTTP_NET_SSI_STATIC_ATTTRIB_NUMBER

- **Maximum Size for a SSI Command Line: Command + Attribute/Value Pairs:**
    - Maximum size for a SSI command line: command + attribute/value pairs
    - Must be <= TCPIP_HTTP_NET_FILE_PROCESS_BUFFER_SIZE!
    - If it is much larger than needed then inefficiency occurs when reading data from the file and then discarding it because a much larger than needed data buffer was read
    - Default value: 100
    - Symbol TCPIP_HTTP_NET_SSI_CMD_MAX_LEN


- **Maximum Number of SSI Variables that Can Be Created at Runtime:**
    - Maximum number of SSI variables that can be created at run time
    - These variables are stored in an internal hash.
    - For max. efficiency this number should be a prime.
    - Default value: 13
    - Symbol TCPIP_HTTP_NET_SSI_VARIABLES_NUMBER

- **Maximum Length of a SSI Variable Name:**
    - Maximum length of a SSI variable name
    - Any excess characters will be truncated
    - Note that this can result in multiple variables being represented as one SSI variable 
    - Default value: 10
    - Symbol TCPIP_HTTP_NET_SSI_VARIABLE_NAME_MAX_LENGTH

- **Maximum Size of a SSI String Variable Value:**
    - Maximum size of a SSI string variable value
    - Any excess characters will be truncated
    - Note that the variable value requires SSI storage that is allocated dynamically
    - Also, this value determines the size of an automatic (stack) buffer when the variable is echoed.
    - If this value is large, make sure you have enough stack space.
    - Default value: 10
    - Symbol TCPIP_HTTP_NET_SSI_VARIABLE_STRING_MAX_LENGTH


- **Message to Echo when Echoing a Not Found Variable:**
    - Message to echo when echoing a not found variable 
    - Default value: "SSI Echo - Not Found: "
    - Symbol TCPIP_HTTP_NET_SSI_ECHO_NOT_FOUND_MESSAGE


- **Persistent Connection Idle Timeout (in sec):**
    - Persistent connection idle timeout, in seconds
    - If a persistent connection is idle for more that this timeout the server will close it.
    - Usually the client closes the connection.
    - Using this timeout value the server can close the its side of the connection to avoid all connections to remain indefinitely open if the client misbehaves.
    - Use 0 to never timeout.
    - The timeout value has to be <= 32767 seconds.
    - Value is in seconds
    - Default value: 0
    - Symbol TCPIP_HTTP_NET_CONNECTION_TIMEOUT

- **HTTP NET allocation function, malloc style:**
    - HTTP allocation function, malloc style
    - This is the function the HTTP will call to allocate memory needed for: dynamic variables, SSI or file uploads.
    - Use standard C library 'malloc' or 0 as a default
    - If it is 0, HTTP will use the allocation functions passed in at the stack initialization
    - Note: the used function should be multi-thread safe in an RTOS environment!
    - Default value: "malloc"
    - Symbol TCPIP_HTTP_NET_MALLOC_FUNC


- **HTTP NET deallocation function, free style:**
    - HTTP corresponding deallocation function, free style
    - This is the function the HTTP will call for freeing the allocated memory
    - Use standard C library 'free' or 0 as a default
    - If it is 0, HTTP will use the allocation functions passed in at the stack initialization
    - Note: the used function should be multi-thread safe in an RTOS environment!
    - Default value: "free"
    - Symbol TCPIP_HTTP_NET_FREE_FUNC


