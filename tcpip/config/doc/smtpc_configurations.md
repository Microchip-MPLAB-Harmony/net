# Simple Mail Transfer Protocol Client (SMTPC)  Configurations

**SMTPC Specific User Configurations**


- **Number of Mail Connections to be Created:**
    - Number of mail connections (sockets) to be created
    - Each mail message requires a mail connection
    - Adjust based on the number of simultaneous mail messages in transit
    - Default value: 3
    - Symbol TCPIP_SMTPC_MAIL_CONNECTIONS

- **SMTPC Socket TX Buffer Size:**
    - Define the size of the TX buffer for the SMTPC socket
    - Use 0 for default TCP socket value
    - The SMTPC TX does not need high throughput so the default value is usually fine.
    - For transferring large files increase this value as needed.
    - Default value: 0
    - Symbol TCPIP_SMTPC_SKT_TX_BUFF_SIZE

- **SMTPC Socket RX Buffer Size:**
    - Define the size of the RX buffer for the SMTPC socket
    - Use 0 for default TCP socket value
    - The SMTPC RX does not need high throughput so the default value is usually fine.
    - Default value: 0
    - Symbol TCPIP_SMTPC_SKT_RX_BUFF_SIZE

- **Number of Retries for Sending a Mail Message:**
    - How many times to retry sending a mail message
    - Retries occur only for server reported transient errors or for internal temporary errors (DNS, socket errors, etc.)
    - Default value: 3
    - Symbol TCPIP_SMTPC_MAIL_RETRIES

- **SMTPC Client Mail Date String:**
    - Default string that identifies the SMTPC client mail date
    - Currently there is no date/time service available
    - The mail server will update with the current date 
    - Keep the current data/time format that is accepted by the SMTP servers
    - Default value: "Wed, 20 July 2016 14:55:06 -0600" 
    - Symbol TCPIP_SMTPC_CLIENT_MESSAGE_DATE

- **Enable Console Commands:**
    - Enable the SMTPC TCP/IP Console Commands
    - Use the sample tcpip console "mail" command for sending email from the console
    - Default value: false
    - Symbol: TCPIP_SMTPC_USE_MAIL_COMMAND


- **SMTPC Task Rate (in msec):**
    - SMTPC task processing rate, in milliseconds.
    - The SMTPC module will process a timer event with this rate for maintaining its own queues, processing timeouts, etc.
    - The lower the rate (higher the frequency) the higher the module priority and higher module performance can be obtained
    - The value cannot be lower than the TCPIP_STACK_TICK_RATE.
    - Value is in milliseconds
    - Default value: 55
    - Symbol TCPIP_SMTPC_TASK_TICK_RATE

- **General Server Response Timeout (in sec):**
    - General server response timeout, seconds
    - Should be adjusted according to the server responsivity
    - RFC specifies it as 5 min    
    - Default is 1 minute, which should normally be enough
    - Value is in seconds
    - Default value: 60
    - Symbol TCPIP_SMTPC_SERVER_REPLY_TIMEOUT

- **Timeout for Server Acknowledgment of Mail Data (in sec):**
    - Server acknowledgment of the mail data: body, attachments et all
    - Should be adjusted according to the server responsivity
    - RFC specifies it as 10 min    
    - Default is 1 minute, which should normally be enough
    - Value is in seconds
    - Default value: 60
    - Symbol TCPIP_SMTPC_SERVER_DATA_TIMEOUT

- **Timeout for the TLS Handshake to Complete (in sec):**
    - The timeout for the TLS handshake to complete
    - When a secure connection is established to the mail server
    - Adjust based on the processing speed, key size, etc.
    - Value is in seconds
    - Default value: 10
    - Symbol TCPIP_SMTPC_TLS_HANDSHAKE_TIMEOUT

- **Server Transient Error Retry Interval (in sec):**
    - The retry interval because of a server transient error
    - RFC specifies it should be at least 30 min!
    - Value is in seconds
    - Default value: 600
    - Symbol TCPIP_SMTPC_SERVER_TRANSIENT_RETRY_TIMEOUT

- **SMTPC Temporary Error Retry Interval (in sec):**
    - The retry interval because of a SMTPC temporary error
    - These include temporary errors related to: DNS, socket connection, TLS.
    - Value is in seconds
    - Default value: 10
    - Symbol TCPIP_SMTPC_INTERNAL_RETRY_TIMEOUT

- **Server Reply Processing RX Buffer Size:**
    - Size of the RX buffer for processing the server replies
    - Usually the server replies are not very long
    - Note that some serverz expect a window > 1400 bytes
    - Default value: 2048
    - Symbol TCPIP_SMTPC_SERVER_REPLY_BUFFER_SIZE

- **SMTP Client Authentication Buffer Size:**
    - Size of a buffer that can hold the 2* username and password
    - Note: this buffer is automatic, created on the stack
    - Check your project stack settings if you need a large buffer here!
    - If buffer not long enough this could generate TCPIP_SMTPC_RES_MESSAGE_AUTH_LEN_ERROR.
    - Default value: 100
    - Symbol TCPIP_SMTPC_CLIENT_AUTH_BUFFER_SIZE

- **Email Address Buffer Size:**
    - Size of a buffer that can hold an email address: user@domain.smth
    - Note: this buffer is automatic, created on the stack
    - Check your project stack settings if you need a large buffer here!
    - If buffer not long enough this could generate TCPIP_SMTPC_RES_MESSAGE_ADDR_LEN_ERROR.
    - Default value: 80
    - Symbol TCPIP_SMTPC_CLIENT_ADDR_BUFFER_SIZE

- **Plain Email Text Line Buffer Size:**
    - Size of an email line when sending the email body as plain text
    - SMTP lines are recommended to be 78 chars long and MUST not exceed 998 characters!
    - This implementation limits the line size to TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE (usually 256 or 512).
    - So any mail line longer that this size will have inserted an artificial end of line sequence (CRLF) after this many characters.
    - Note: this buffer is automatic, created on the stack
    - Check your project stack settings if you need a large buffer here!
    - Note: Lines are sent out as a whole so the line cannot be longer than the TCP socket TX buffer!
    - SMTPC will ensure that this symbol does not exceed the size of the socket TX buffer by allocating
    - a larger TX buffer, if needed
    - Default value: 256
    - Symbol TCPIP_SMTPC_PLAIN_LINE_BUFF_SIZE

