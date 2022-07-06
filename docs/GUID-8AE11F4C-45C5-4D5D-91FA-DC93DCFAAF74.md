# TCPIP\_MDNS\_ServiceRegister Function

**Parent topic:**[Zeroconf Module](GUID-1076C2EA-7DC5-419E-A3E7-BBA92CFF5CFB.md)

## C

```
MDNSD_ERR_CODE TCPIP_MDNS_ServiceRegister(
    TCPIP_NET_HANDLE netH, 
    const char * srv_name, 
    const char * srv_type, 
    uint16_t port, 
    const uint8_t * txt_record, 
    uint8_t auto_rename, 
    void (*call_back)(char *name, MDNSD_ERR_CODE err, void *context), 
    void * context
);
```

## Description

This function is used by end-user application to announce its service on local network. All peer machines that are compliant with Multicast DNS and DNS Service Discovery protocol can detect the announcement and lists out an entry in the Service Browser list. The end-user selects an entry to connect to this service. Ultimately this is an aid to end-user to discover any service of interest on a local network.

This is the first function that needs to be invoked by end-user application. Presently Multicast-DNS and Service discovery stack supports only single service advertisement. Once the application wants to terminate the service it has to invoke TCPIP\_MDNS\_ServiceDeregister\(\) function to free the DNS SD stack for some other application.

## Preconditions

None.

## Parameters

|Parameters|Description|
|----------|-----------|
|netH|Handle of the network to be registered.|
|srv\_name|Service name, which is being advertised.|
|srv\_type|For a HTTP-Service it's "\_\_http.\_tcp.local".|
|\_\_http|Application protocol proceeded with an underscore.|
|\_tcp|Lower-layer protocol on which service runs.|
|local|To represent service is on local-network. For iTunes Music Sharing "\_daap.\_tcp.local" For a Printing Service "\_ipp.\_tcp.local" Refer to http://www.dns-sd.org/ServiceTypes.html for more service types.|
|port|Port number on which service is running.|
|txt\_len|For additional information about service like default page \(e.g., "index.htm"\) for HTTP-service. Length of such additional information.|
|txt\_record|String of additional information \(e.g., "index.htm"\) for HTTP service.|
|auto\_rename|A flag to indicate DNS-SD stack, whether to rename the service automatically or not. If this is set to '0' Callback parameter will be used to indicate the conflict error and user has to select different name and re-register with this function. If this is set to '1' service-name will be automatically renamed with numerical suffix.|
|callback|Callback function, which is user-application defined. This callback gets invoked on completion of service advertisement. If an service name-conflict error is detected and auto\_rename is set to '0' callback gets invoked with MDNSD\_ERR\_CONFLICT as error-code.|
|context|Opaque context \(pointer to opaque data\), which needs to be used in callback function.|

## Returns

-   MDNSD\_ERR\_CODE - Returns Error-code to indicate whether or not registration is successful:

    -   MDNSD\_SUCCESS - returns on success of call.

    -   MDNSD\_ERR\_BUSY - When already some other service is being advertised using this DNS SD stack.

    -   MDNSD\_ERR\_INVAL - Invalid parameter.


