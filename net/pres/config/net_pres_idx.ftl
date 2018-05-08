config NET_PRES_INST_GT_${INSTANCE+1}
    bool
    depends on NET_PRES_USE
<#if INSTANCE != 0>
    default n if NET_PRES_INST_GT_${INSTANCE} = n
</#if>
    default n if NET_PRES_INSTANCES = ${INSTANCE+1}
    default y
    
config NET_PRES_IDX${INSTANCE}
    depends on NET_PRES_USE
<#if INSTANCE != 0>
        && NET_PRES_INST_GT_${INSTANCE}
</#if>
    bool "Net Presentation Instance ${INSTANCE}"
    default y
    
ifblock NET_PRES_IDX${INSTANCE}

config NET_PRES_CONFIG_NAME_IDX${INSTANCE}
    string "Name of Presentation Instance?"
    default "NET_PRES_${INSTANCE}"

config NET_PRES_TRANSPORT_AS_TCPIP_IDX${INSTANCE}
    bool "Use MPLAB Harmony TCP/IP as Transport Layer?"
    default y
    
config NET_PRES_SUPPORT_STREAM_IDX${INSTANCE}
    bool "Support Stream Connections?"
    default y if NET_PRES_TRANSPORT_AS_TCPIP_IDX${INSTANCE} && TCPIP_USE_TCP
    default n if NET_PRES_TRANSPORT_AS_TCPIP_IDX${INSTANCE} && !TCPIP_USE_TCP
    default y
    
config NET_PRES_SUPPORT_DATAGRAM_IDX${INSTANCE}
    bool "Support Data-gram Connections?"
    default y if NET_PRES_TRANSPORT_AS_TCPIP_IDX${INSTANCE} && TCPIP_USE_UDP
    default n if NET_PRES_TRANSPORT_AS_TCPIP_IDX${INSTANCE} && !TCPIP_USE_UDP
    default y
    
    
config NET_PRES_SUPPORT_SERVER_IDX${INSTANCE}
    bool "Support Server Connections?"
    default y
    
config NET_PRES_SUPPORT_CLIENT_IDX${INSTANCE}
    bool "Support Client Connections?"
    default y    

config NET_PRES_SUPPORT_ENCRYPTION${INSTANCE}
    bool "Support Encryption?"
    default n

ifblock NET_PRES_SUPPORT_ENCRYPTION${INSTANCE}

config NET_PRES_USE_WOLF_SSL_IDX${INSTANCE}
    bool "Use wolfSSL as Encryption Provider?"
    depends on USE_3RDPARTY_WOLFSSL
    depends on !NET_PRES_GENERATE_ENC_STUBS_IDX${INSTANCE}
    default y
	
config NET_PRES_USE_WOLF_SSL_DEBUG_LOG_IDX${INSTANCE}
    bool "Add system console logging for wolf ssl?"
	depends on NET_PRES_USE_WOLF_SSL_IDX${INSTANCE}
	depends on WOLFSSL_DEBUG_SUPPORT
	depends on USE_SYS_CONSOLE
	default n
	
config NET_PRES_USE_WOLF_SSL_DEBUG_LOG_BUFFERS_IDX${INSTANCE}
	int "Number of log message buffers"
    depends on NET_PRES_USE_WOLF_SSL_DEBUG_LOG_IDX${INSTANCE}
	default 80
	
config NET_PRES_GENERATE_ENC_STUBS_IDX${INSTANCE}
    bool "Generate Encryption Provider Stub Code?"
    depends on !NET_PRES_USE_WOLF_SSL_IDX${INSTANCE}
    default n

config NET_PRES_SUPPORT_STREAM_ENC_IDX${INSTANCE}
    bool "Support Stream Encryption?"
    depends on NET_PRES_SUPPORT_STREAM_IDX${INSTANCE}
    default y
    
config NET_PRES_SUPPORT_DATAGRAM_ENC_IDX${INSTANCE}
    bool "Support Data-gram Encryption?"
    depends on NET_PRES_SUPPORT_DATAGRAM_IDX${INSTANCE}
    default n

config NET_PRES_SUPPORT_SERVER_ENC_IDX${INSTANCE}
    bool "Support Server Encryption?"
    depends on NET_PRES_SUPPORT_SERVER_IDX${INSTANCE}
    default n
    
config NET_PRES_SUPPORT_CLIENT_ENC_IDX${INSTANCE}
    bool "Support Client Encryption?"
    depends on NET_PRES_SUPPORT_CLIENT_IDX${INSTANCE}
    default y    
    
endif
        
endif
