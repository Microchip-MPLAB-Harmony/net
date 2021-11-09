"""*****************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************"""

################## DHCP Server Instances ##############################
tcpipDhcpServerInstanceRangePrev = 1
tcpipDhcpServerInstancesNumPrev = 1
tcpipDhcpServerInstance = []
tcpipDhcpServerAddrRangeStart = []
tcpipDhcpServerNetMaskAddr = []
tcpipDhcpServerGatewayAddr = []
tcpipDhcpServerPrimDnsAddr = []
tcpipDhcpServerSecDnsAddr = []
tcpipDhcpServerIfIdx = []
tcpipDhcpServerPoolEn = []

tcpipDhcpServerLeaseEntryMaxNum = []
tcpipDhcpServerLeaseDurationDflt = []
tcpipDhcpServerIpAddr = []
tcpipDhcpServerStartIPAddr = []
tcpipDhcpServerLeadBitNum = []
tcpipDhcpServerClientOptMenu = []
tcpipDhcpServerRouterIPAddr = []
tcpipDhcpServerDnsIPAddr = []
tcpipDhcpServerTimeServerIPAddr = []
tcpipDhcpServerNameServerIPAddr = []
tcpipDhcpServerNTPServerIPAddr = []
tcpipDhcpServerInstanceAdvMenu = []
tcpipDhcpServerConfigFlagMenu = []
tcpipDhcpServerConfigFlag1 = []
tcpipDhcpServerConfigFlag2 = []
tcpipDhcpServerLeaseDurationMin = []
tcpipDhcpServerLeaseDurationMax = []
tcpipDhcpServerUnreqTimeout = []

tcpipNetConfigNumMax = 5  # hard coded value
############################################################################
def instantiateComponent(tcpipDhcpServerComponent):
    print(tcpipNetConfigNumMax)
    print("TCPIP DHCP Server Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    # tcpipDhcpServerICMPClientEnable()
    # Enable DHCP Server
    tcpipDhcpServer = tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_STACK_USE_DHCP_SERVER", None)
    tcpipDhcpServer.setLabel("DHCP Server")
    tcpipDhcpServer.setVisible(False)
    tcpipDhcpServer.setDescription("Enable DHCP Server")
    tcpipDhcpServer.setDefaultValue(True)    

######################################################################################################################################
    if(Database.getComponentByID("tcpipNetConfig") != None):
        dhcpsIntrfcCount = Database.getSymbolValue("tcpipNetConfig", "TCPIP_STACK_NETWORK_INTERAFCE_COUNT")
    else:
        dhcpsIntrfcCount = 1
    
    # if (dhcpsIntrfcCount==None):
        # dhcpsIntrfcCount = 1
        # print ("interface error")
    # Maximum Number of Interfaces supported by DHCP Server
    tcpipDhcpServerMaxIntrfcNum = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_MAX_INTERFACES", None)
    tcpipDhcpServerMaxIntrfcNum.setLabel("Maximum Number of Interfaces")
    tcpipDhcpServerMaxIntrfcNum.setVisible(True)
    tcpipDhcpServerMaxIntrfcNum.setDescription("Maximum Number of Interfaces supported by DHCP Server")
    tcpipDhcpServerMaxIntrfcNum.setDefaultValue(int(dhcpsIntrfcCount))
    tcpipDhcpServerMaxIntrfcNum.setMin(0)
    tcpipDhcpServerMaxIntrfcNum.setReadOnly(True)
    tcpipDhcpServerMaxIntrfcNum.setDependencies(tcpipDhcpServerMaxInstCount, ["tcpipNetConfig.TCPIP_STACK_NETWORK_INTERAFCE_COUNT"])

    # Maximum Number Leases per Interface
    tcpipDhcpServerMaxLeaseNum = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_MAX_LEASES", None)
    tcpipDhcpServerMaxLeaseNum.setLabel("Maximum Number Leases per Interface")
    tcpipDhcpServerMaxLeaseNum.setVisible(True)
    tcpipDhcpServerMaxLeaseNum.setDescription("Maximum Number Leases per Interface")
    tcpipDhcpServerMaxLeaseNum.setDefaultValue(32)
    tcpipDhcpServerMaxLeaseNum.setMin(0)

    # Number of ICMP echo probes 
    tcpipDhcpServerICMPProbeNum = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_ICMP_PROBES", None)
    tcpipDhcpServerICMPProbeNum.setLabel("Number of ICMP Echo Probes")
    tcpipDhcpServerICMPProbeNum.setVisible(True)
    tcpipDhcpServerICMPProbeNum.setDescription("Number of ICMP Echo Probes")
    tcpipDhcpServerICMPProbeNum.setDefaultValue(1)
    tcpipDhcpServerICMPProbeNum.setMin(0)
    
    # Number of Attempts for new IP Address in case of conflict detection
    tcpipDhcpServerIPAttemptNum = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_CONFLICT_ATTEMPTS", None)
    tcpipDhcpServerIPAttemptNum.setLabel("Number of Attempts for new IP Address")
    tcpipDhcpServerIPAttemptNum.setVisible(True)
    tcpipDhcpServerIPAttemptNum.setDescription("Number of Attempts for new IP Address in case of conflict detection")
    tcpipDhcpServerIPAttemptNum.setDefaultValue(1)
    tcpipDhcpServerIPAttemptNum.setMin(0)
    
    # Number of DHCP Server Driver Instances
    tcpipDhcpServerInstancesNum = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_INSTANCES_NUMBER", None)
    tcpipDhcpServerInstancesNum.setLabel("Number of DHCP Server Instances")
    tcpipDhcpServerInstancesNum.setMax(tcpipNetConfigNumMax)
    tcpipDhcpServerInstancesNum.setMin(1)
    tcpipDhcpServerInstancesNum.setVisible(True)
    tcpipDhcpServerInstancesNum.setDescription("Number of DHCP Server Instances")
    tcpipDhcpServerInstancesNum.setDefaultValue(1)
    tcpipDhcpServerInstancesNum.setDependencies(tcpipDhcpServerInstnNumVisible, ["TCPIP_DHCPS_INSTANCES_NUMBER"])

    for index in range(0,tcpipNetConfigNumMax): 
        tcpipDhcpServerInstance.append(tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_IDX"+str(index),tcpipDhcpServerInstancesNum))
        tcpipDhcpServerInstance[index].setLabel("DHCP Server Instance "+ str(index))
        if (index < tcpipDhcpServerInstancesNum.getValue()):
            tcpipDhcpServerInstance[index].setVisible(True)
            tcpipDhcpServerInstance[index].setDefaultValue(True)
        else:
            tcpipDhcpServerInstance[index].setDefaultValue(False)
            tcpipDhcpServerInstance[index].setVisible(False) 
        # tcpipDhcpServerInstance[index].setVisible(True)
        # tcpipDhcpServerInstance[index].setDefaultValue(True)
        tcpipDhcpServerInstance[index].setReadOnly(True)
        tcpipDhcpServerInstance[index].setDependencies(tcpipDhcpServerEnableInstance, ["TCPIP_DHCPS_INSTANCES_NUMBER"])      
    
        # DHCP Server interface index
        tcpipDhcpServerIfIdx.append(tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_INTERFACE_INDEX_IDX" + str(index), tcpipDhcpServerInstance[index]))
        tcpipDhcpServerIfIdx[index].setLabel("Interface Index for DHCP Server")
        tcpipDhcpServerIfIdx[index].setVisible(True)
        tcpipDhcpServerIfIdx[index].setDefaultValue(index)
        tcpipDhcpServerIfIdx[index].setReadOnly(True)
        # tcpipDhcpServerIfIdx[index].setDependencies(tcpipDhcpServerInstnIfIdxMenu, [tcpipDhcpServerInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])

        # Maximum Number of Leases
        tcpipDhcpServerLeaseEntryMaxNum.append(tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_MAX_LEASE_NUM_IDX" + str(index), tcpipDhcpServerInstance[index]))
        tcpipDhcpServerLeaseEntryMaxNum[index].setLabel("Maximum Number of Leases")
        tcpipDhcpServerLeaseEntryMaxNum[index].setVisible(True)
        tcpipDhcpServerLeaseEntryMaxNum[index].setDefaultValue(32)

        # Default Lease Duration
        tcpipDhcpServerLeaseDurationDflt.append(tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_LEASEDURATION_DFLT_IDX" + str(index), tcpipDhcpServerInstance[index]))
        tcpipDhcpServerLeaseDurationDflt[index].setLabel("Default Lease Duration(in Sec)")
        tcpipDhcpServerLeaseDurationDflt[index].setVisible(True)
        tcpipDhcpServerLeaseDurationDflt[index].setDefaultValue(28800) # 8 Hours - 8 x 60 x 60
        
        # Server IP Address
        tcpipDhcpServerIpAddr.append(tcpipDhcpServerComponent.createStringSymbol("TCPIP_DHCPS_SERVER_IP_ADDRESS_IDX" + str(index), tcpipDhcpServerInstance[index]))
        tcpipDhcpServerIpAddr[index].setLabel("DHCPS Server IP Address")
        tcpipDhcpServerIpAddr[index].setVisible(True)
        tcpipDhcpServerIpAddr[index].setDefaultValue("192.168.1." + str(index + 1))
        # tcpipDhcpServerIpAddr[index].setDependencies(tcpipDhcpServerInstnIpAddrMenu, [tcpipDhcpServerInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])
        
        # Lease Start IP Address
        tcpipDhcpServerStartIPAddr.append(tcpipDhcpServerComponent.createStringSymbol("TCPIP_DHCPS_START_IP_ADDR_IDX" + str(index), tcpipDhcpServerInstance[index]))
        tcpipDhcpServerStartIPAddr[index].setLabel("Start Lease Address")
        tcpipDhcpServerStartIPAddr[index].setVisible(True)
        tcpipDhcpServerStartIPAddr[index].setDefaultValue("192.168.1.100")
        # tcpipDhcpServerAddrRangeStart[index].setDependencies(tcpipDhcpServerInstnAddrRangeMenu, [tcpipDhcpServerInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])
        
        # Number of Leading Bits in Network Mask
        tcpipDhcpServerLeadBitNum.append(tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_MASK_LEAD_BIT_NUM_IDX" + str(index), tcpipDhcpServerInstance[index]))
        tcpipDhcpServerLeadBitNum[index].setLabel("Number of Leading 1 Bits in Network Mask ")
        tcpipDhcpServerLeadBitNum[index].setVisible(True)
        tcpipDhcpServerLeadBitNum[index].setMin(0)
        tcpipDhcpServerLeadBitNum[index].setMax(32)
        tcpipDhcpServerLeadBitNum[index].setDefaultValue(24) 
        
        # Client Options        
        tcpipDhcpServerClientOptMenu.append(tcpipDhcpServerComponent.createMenuSymbol("TCPIP_DHCPS_CLIENT_OPT_MENU_IDX" + str(index), tcpipDhcpServerInstance[index]))
        tcpipDhcpServerClientOptMenu[index].setLabel("Client Options")
        tcpipDhcpServerClientOptMenu[index].setVisible(True)
        
        # Router IP Address
        tcpipDhcpServerRouterIPAddr.append(tcpipDhcpServerComponent.createStringSymbol("TCPIP_DHCPS_ROUTER_IP_ADDR_IDX" + str(index), tcpipDhcpServerClientOptMenu[index]))
        tcpipDhcpServerRouterIPAddr[index].setLabel("Router IP Address")
        tcpipDhcpServerRouterIPAddr[index].setVisible(True)
        tcpipDhcpServerRouterIPAddr[index].setDefaultValue("192.168.1.1")

        # DNS IP Address
        tcpipDhcpServerDnsIPAddr.append(tcpipDhcpServerComponent.createStringSymbol("TCPIP_DHCPS_DNS_IP_ADDR_IDX" + str(index), tcpipDhcpServerClientOptMenu[index]))
        tcpipDhcpServerDnsIPAddr[index].setLabel("DNS IP Address")
        tcpipDhcpServerDnsIPAddr[index].setVisible(True)
        tcpipDhcpServerDnsIPAddr[index].setDefaultValue("192.168.1.1")

        # Time Server IP Address
        tcpipDhcpServerTimeServerIPAddr.append(tcpipDhcpServerComponent.createStringSymbol("TCPIP_DHCPS_TIMESERVER_IP_ADDR_IDX" + str(index), tcpipDhcpServerClientOptMenu[index]))
        tcpipDhcpServerTimeServerIPAddr[index].setLabel("Time Server IP Address")
        tcpipDhcpServerTimeServerIPAddr[index].setVisible(True)

        # Name Server IP Address
        tcpipDhcpServerNameServerIPAddr.append(tcpipDhcpServerComponent.createStringSymbol("TCPIP_DHCPS_NAMESERVER_IP_ADDR_IDX" + str(index), tcpipDhcpServerClientOptMenu[index]))
        tcpipDhcpServerNameServerIPAddr[index].setLabel("Name Server IP Address")
        tcpipDhcpServerNameServerIPAddr[index].setVisible(True)

        # NTP Server IP Address
        tcpipDhcpServerNTPServerIPAddr.append(tcpipDhcpServerComponent.createStringSymbol("TCPIP_DHCPS_NTPSERVER_IP_ADDR_IDX" + str(index), tcpipDhcpServerClientOptMenu[index]))
        tcpipDhcpServerNTPServerIPAddr[index].setLabel("NTP Server IP Address")
        tcpipDhcpServerNTPServerIPAddr[index].setVisible(True)
        
        
        # Instance Advanced menu       
        tcpipDhcpServerInstanceAdvMenu.append(tcpipDhcpServerComponent.createMenuSymbol("TCPIP_DHCPS_INSTN_ADV_MENU_IDX" + str(index), tcpipDhcpServerInstance[index]))
        # tcpipDhcpServerInstanAdvMenu[index].setLabel("Config Flags")
        tcpipDhcpServerInstanceAdvMenu[index].setLabel("Instance "+ str(index) + " Advanced Setting")
        tcpipDhcpServerInstanceAdvMenu[index].setVisible(True)

        # Config Flags menu       
        tcpipDhcpServerConfigFlagMenu.append(tcpipDhcpServerComponent.createMenuSymbol("TCPIP_DHCPS_CONFIG_MENU_IDX" + str(index), tcpipDhcpServerInstanceAdvMenu[index]))
        tcpipDhcpServerConfigFlagMenu[index].setLabel("Config Flags")
        tcpipDhcpServerConfigFlagMenu[index].setVisible(True)
        
        # Config Flag 1 
        tcpipDhcpServerConfigFlag1.append(tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_CONFIG_FLAG1_IDX"+str(index),tcpipDhcpServerConfigFlagMenu[index]))
        tcpipDhcpServerConfigFlag1[index].setLabel("Flag 1")
        tcpipDhcpServerConfigFlag1[index].setVisible(True)
        tcpipDhcpServerConfigFlag1[index].setDefaultValue(False)
                
        # Config Flag 2 
        tcpipDhcpServerConfigFlag2.append(tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_CONFIG_FLAG2_IDX"+str(index),tcpipDhcpServerConfigFlagMenu[index]))
        tcpipDhcpServerConfigFlag2[index].setLabel("Flag 2")
        tcpipDhcpServerConfigFlag2[index].setVisible(True)
        tcpipDhcpServerConfigFlag2[index].setDefaultValue(False)        


       
        # Minimum Lease Duration
        tcpipDhcpServerLeaseDurationMin.append(tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_LEASEDURATION_MIN_IDX" + str(index), tcpipDhcpServerInstanceAdvMenu[index]))
        tcpipDhcpServerLeaseDurationMin[index].setLabel("Minimum Lease Duration(in Sec)")
        tcpipDhcpServerLeaseDurationMin[index].setVisible(True)
        tcpipDhcpServerLeaseDurationMin[index].setDefaultValue(60)
       
        # Maximum Lease Duration
        tcpipDhcpServerLeaseDurationMax.append(tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_LEASEDURATION_MAX_IDX" + str(index), tcpipDhcpServerInstanceAdvMenu[index]))
        tcpipDhcpServerLeaseDurationMax[index].setLabel("Maximum Lease Duration(in Sec)")
        tcpipDhcpServerLeaseDurationMax[index].setVisible(True)
        tcpipDhcpServerLeaseDurationMax[index].setDefaultValue(0)
       
        # Unrequested Offer Timeout
        tcpipDhcpServerUnreqTimeout.append(tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_UNREQ_TMO_IDX" + str(index), tcpipDhcpServerInstanceAdvMenu[index]))
        tcpipDhcpServerUnreqTimeout[index].setLabel("Unrequested Offer Timeout(in Sec)")
        tcpipDhcpServerUnreqTimeout[index].setVisible(True)
        tcpipDhcpServerUnreqTimeout[index].setDefaultValue(0)
        
        # # DHCP Server Net Mask Address
        # tcpipDhcpServerNetMaskAddr.append(tcpipDhcpServerComponent.createStringSymbol("TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS_IDX" + str(index), tcpipDhcpServerInstance[index]))
        # tcpipDhcpServerNetMaskAddr[index].setLabel("DHCPS Netmask")
        # tcpipDhcpServerNetMaskAddr[index].setVisible(True)
        # tcpipDhcpServerNetMaskAddr[index].setDefaultValue("255.255.255.0")
        # tcpipDhcpServerNetMaskAddr[index].setDependencies(tcpipDhcpServerInstnNetMaskAddrMenu, [tcpipDhcpServerInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])
    
        # # DHCP Server Default Gateway Address
        # tcpipDhcpServerGatewayAddr.append(tcpipDhcpServerComponent.createStringSymbol("TCPIP_DHCPS_DEFAULT_SERVER_GATEWAY_ADDRESS_IDX" + str(index), tcpipDhcpServerInstance[index]))
        # tcpipDhcpServerGatewayAddr[index].setLabel("Default Gateway")
        # tcpipDhcpServerGatewayAddr[index].setVisible(True)
        # tcpipDhcpServerGatewayAddr[index].setDefaultValue("192.168.1.1")
        # tcpipDhcpServerGatewayAddr[index].setDependencies(tcpipDhcpServerInstnGatewayAddrMenu, [tcpipDhcpServerInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"]) 
    
        # # DHCP Server Primary DNS Server Address
        # tcpipDhcpServerPrimDnsAddr.append(tcpipDhcpServerComponent.createStringSymbol("TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS_IDX" + str(index), tcpipDhcpServerInstance[index]))
        # tcpipDhcpServerPrimDnsAddr[index].setLabel("Primary DNS Server Address")
        # tcpipDhcpServerPrimDnsAddr[index].setVisible(True)
        # tcpipDhcpServerPrimDnsAddr[index].setDefaultValue("192.168.1.1")
        # tcpipDhcpServerPrimDnsAddr[index].setDependencies(tcpipDhcpServerInstnPrimDnsAddrMenu, [tcpipDhcpServerInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"]) 
    
        # # DHCP Server Secondary DNS Server Address
        # tcpipDhcpServerSecDnsAddr.append(tcpipDhcpServerComponent.createStringSymbol("TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS_IDX" + str(index), tcpipDhcpServerInstance[index]))
        # tcpipDhcpServerSecDnsAddr[index].setLabel("Secondary DNS Server Address")
        # tcpipDhcpServerSecDnsAddr[index].setVisible(True)
        # tcpipDhcpServerSecDnsAddr[index].setDefaultValue("192.168.1.1")
        # tcpipDhcpServerSecDnsAddr[index].setDependencies(tcpipDhcpServerInstnSecDnsAddrMenu, [tcpipDhcpServerInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])   
    
        # DHCP Server Pool enabled
        # tcpipDhcpServerPoolEn.append(tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_POOL_ENABLED_IDX" + str(index), tcpipDhcpServerInstance[index]))
        # tcpipDhcpServerPoolEn[index].setLabel("Pool Enabled")
        # tcpipDhcpServerPoolEn[index].setVisible(True)
        # tcpipDhcpServerPoolEn[index].setDefaultValue(True)
        # tcpipDhcpServerPoolEn[index].setDependencies(tcpipDhcpServerInstnPoolEnMenu, [tcpipDhcpServerInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])   
    
    ######################################################################################################################################    

    # Advanced Settings
    tcpipDhcpServerAdvSettings = tcpipDhcpServerComponent.createMenuSymbol("TCPIP_DHCPS_ADV_SETTING", None)
    tcpipDhcpServerAdvSettings.setLabel("Advanced Settings")
    tcpipDhcpServerAdvSettings.setDescription("Advanced Settings")
    tcpipDhcpServerAdvSettings.setVisible(True)

    # DHCP Server Task Rate in msec
    tcpipDhcpServerTaskRate = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_TASK_PROCESS_RATE", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerTaskRate.setLabel("DHCP Server Task Rate (in msec)")
    tcpipDhcpServerTaskRate.setVisible(True)
    tcpipDhcpServerTaskRate.setDescription("DHCP Server Task Rate in msec")
    tcpipDhcpServerTaskRate.setDefaultValue(200) 
    tcpipDhcpServerTaskRate.setMin(0)    

    # Client ID Maximum Size
    tcpipDhcpServerClientIDMaxSize = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_CLIENT_ID_MAX_SIZE", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerClientIDMaxSize.setLabel("Client ID Maximum Size")
    tcpipDhcpServerClientIDMaxSize.setVisible(True)
    tcpipDhcpServerClientIDMaxSize.setDescription("Client ID Maximum Size")
    tcpipDhcpServerClientIDMaxSize.setDefaultValue(16)
    tcpipDhcpServerClientIDMaxSize.setMin(0)

    # ICMP Echo Request Buffer Size
    tcpipDhcpServerICMPEchoBuffSize = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_ICMP_ECHO_DATA_SIZE", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerICMPEchoBuffSize.setLabel("ICMP Echo Request Buffer Size")
    tcpipDhcpServerICMPEchoBuffSize.setVisible(True)
    tcpipDhcpServerICMPEchoBuffSize.setDescription("ICMP Echo Request Buffer Size")
    tcpipDhcpServerICMPEchoBuffSize.setMax(32)
    tcpipDhcpServerICMPEchoBuffSize.setMin(8)
    tcpipDhcpServerICMPEchoBuffSize.setDefaultValue(16) 

    # Number of ICMP Request Retries 
    tcpipDhcpServerICMPRetryNum = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_ICMP_ECHO_RETRIES", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerICMPRetryNum.setLabel("Number of ICMP Request Retries")
    tcpipDhcpServerICMPRetryNum.setVisible(True)
    tcpipDhcpServerICMPRetryNum.setDescription("Number of ICMP Request Retries")
    tcpipDhcpServerICMPRetryNum.setDefaultValue(2)
    tcpipDhcpServerICMPRetryNum.setMin(0)

    # Maximum number of values for the options to be sent to clients 
    
    # Maximum number of Router Value Options
    tcpipDhcpServerOptionRouterValNum = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_OPTION_ROUTER_VALUES", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerOptionRouterValNum.setLabel("Maximum Number of Router Value Options")
    tcpipDhcpServerOptionRouterValNum.setVisible(True)
    tcpipDhcpServerOptionRouterValNum.setDescription("Maximum Number of Router Value Options")
    tcpipDhcpServerOptionRouterValNum.setDefaultValue(1)  
    tcpipDhcpServerOptionRouterValNum.setMin(0)
    tcpipDhcpServerOptionRouterValNum.setMax(2)
    
    # Maximum number of DNS Value Options
    tcpipDhcpServerOptionDNSValNum = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_OPTION_DNS_VALUES", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerOptionDNSValNum.setLabel("Maximum Number of DNS Value Options")
    tcpipDhcpServerOptionDNSValNum.setVisible(True)
    tcpipDhcpServerOptionDNSValNum.setDescription("Maximum Number of DNS Value Options")
    tcpipDhcpServerOptionDNSValNum.setDefaultValue(2)  
    tcpipDhcpServerOptionDNSValNum.setMin(0)
    tcpipDhcpServerOptionDNSValNum.setMax(2)
    
    # Maximum number of Time Server Value Options
    tcpipDhcpServerOptionTimeServerValNum = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerOptionTimeServerValNum.setLabel("Maximum Number of Time Server Value Options")
    tcpipDhcpServerOptionTimeServerValNum.setVisible(True)
    tcpipDhcpServerOptionTimeServerValNum.setDescription("Maximum Number of Time Server Value Options")
    tcpipDhcpServerOptionTimeServerValNum.setDefaultValue(1)
    tcpipDhcpServerOptionTimeServerValNum.setMin(0)
    tcpipDhcpServerOptionTimeServerValNum.setMax(2)
    
    # Maximum number of Name Server Value Options
    tcpipDhcpServerOptionNameServerValNum = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerOptionNameServerValNum.setLabel("Maximum Number of Name Server Value Options")
    tcpipDhcpServerOptionNameServerValNum.setVisible(True)
    tcpipDhcpServerOptionNameServerValNum.setDescription("Maximum Number of Name Server Value Options")
    tcpipDhcpServerOptionNameServerValNum.setDefaultValue(1)
    tcpipDhcpServerOptionNameServerValNum.setMin(0)
    tcpipDhcpServerOptionNameServerValNum.setMax(2)
    
    # Maximum number of NTP Server Value Options
    tcpipDhcpServerOptionNTPServerValNum = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerOptionNTPServerValNum.setLabel("Maximum Number of NTP Server Value Options")
    tcpipDhcpServerOptionNTPServerValNum.setVisible(True)
    tcpipDhcpServerOptionNTPServerValNum.setDescription("Maximum Number of NTP Server Value Options")
    tcpipDhcpServerOptionNTPServerValNum.setDefaultValue(1)
    tcpipDhcpServerOptionNTPServerValNum.setMin(0)
    tcpipDhcpServerOptionNTPServerValNum.setMax(2)    
    
    # Suppress T1 Renewal and T2 Rebind
    tcpipDhcpServerOptionT1T2Suppress = tcpipDhcpServerComponent.createBooleanSymbol("o TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS ", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerOptionT1T2Suppress.setLabel("Suppress T1 Renewal and T2 Rebind")
    tcpipDhcpServerOptionT1T2Suppress.setVisible(True)
    tcpipDhcpServerOptionT1T2Suppress.setDescription("Suppress T1 Renewal and T2 Rebind")
    tcpipDhcpServerOptionT1T2Suppress.setDefaultValue(True)

    # Events Report
    
    # Maximum Number of Event Registrations
    tcpipDhcpServerEventRegMaxNum = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_MAX_EVENT_REGISTRATIONS", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerEventRegMaxNum.setLabel("Maximum Number of Event Registrations")
    tcpipDhcpServerEventRegMaxNum.setVisible(True)
    tcpipDhcpServerEventRegMaxNum.setDescription("Maximum Number of Event Registrations")
    tcpipDhcpServerEventRegMaxNum.setDefaultValue(1)
    tcpipDhcpServerEventRegMaxNum.setMin(0)
    tcpipDhcpServerEventRegMaxNum.setMax(4)
    
    # Enable Error Events Report
    tcpipDhcpServerEventErrReport = tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_REPORT_ERROR_EVENT", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerEventErrReport.setLabel("Enable Error Events Report")
    tcpipDhcpServerEventErrReport.setVisible(True)
    tcpipDhcpServerEventErrReport.setDescription("Enable Error Events Report")
    tcpipDhcpServerEventErrReport.setDefaultValue(True)
    
    # Enable Client Events Report
    tcpipDhcpServerEventClientReport = tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_REPORT_CLIENT_EVENT", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerEventClientReport.setLabel("Enable Client Events Report")
    tcpipDhcpServerEventClientReport.setVisible(True)
    tcpipDhcpServerEventClientReport.setDescription("Enable Client Events Report")
    tcpipDhcpServerEventClientReport.setDefaultValue(True)
    
    # Enable Statistics
    tcpipDhcpServerEventStatistics = tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_ENABLE_STATISTICS", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerEventStatistics.setLabel("Enable Statistics")
    tcpipDhcpServerEventStatistics.setVisible(True)
    tcpipDhcpServerEventStatistics.setDescription("Enable Statistics")
    tcpipDhcpServerEventStatistics.setDefaultValue(False)
    
    # Enable Dynamic DB Manipulation
    tcpipDhcpServerEventDynDB = tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_DYNAMIC_DB_ACCESS", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerEventDynDB.setLabel("Enable Dynamic DB Manipulation")
    tcpipDhcpServerEventDynDB.setVisible(True)
    tcpipDhcpServerEventDynDB.setDescription("Enable Dynamic DB Manipulation")
    tcpipDhcpServerEventDynDB.setDefaultValue(False)
    
    # Enable Multi-Threaded Access
    tcpipDhcpServerEventMultThread = tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_MULTI_THREADED_ACCESS", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerEventMultThread.setLabel("Enable Multi-Threaded Access")
    tcpipDhcpServerEventMultThread.setVisible(True)
    tcpipDhcpServerEventMultThread.setDescription("Enable Multi-Threaded Access")
    tcpipDhcpServerEventMultThread.setDefaultValue(False)    

    # Time-out for a Solved Entry in the Cache in Seconds.
    tcpipDhcpServerLeaseSolvedEntryTimeout = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_LEASE_SOLVED_ENTRY_TMO", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerLeaseSolvedEntryTimeout.setLabel("Timeout for a Solved Entry in the Cache (in sec)")
    tcpipDhcpServerLeaseSolvedEntryTimeout.setVisible(True)
    tcpipDhcpServerLeaseSolvedEntryTimeout.setDescription("Time-out for a Solved Entry in the Cache in Seconds.")
    tcpipDhcpServerLeaseSolvedEntryTimeout.setDefaultValue(1200)

    # Time-out for a Solved Entry in the Cache in Seconds.
    tcpipDhcpServerLeaseUnsolvedEntryTimeout = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_LEASE_REMOVED_BEFORE_ACK", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerLeaseUnsolvedEntryTimeout.setLabel("Timeout for an Unsolved Entry (in sec)")
    tcpipDhcpServerLeaseUnsolvedEntryTimeout.setVisible(True)
    tcpipDhcpServerLeaseUnsolvedEntryTimeout.setDescription("Time-out for an Unsolved Entry in Seconds")
    tcpipDhcpServerLeaseUnsolvedEntryTimeout.setDefaultValue(5)

    # Delete Old Entries
    tcpipDhcpServerOldEntryDelete = tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCP_SERVER_DELETE_OLD_ENTRIES", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerOldEntryDelete.setLabel("Delete Old Entries")
    tcpipDhcpServerOldEntryDelete.setVisible(True)
    tcpipDhcpServerOldEntryDelete.setDescription("Delete Old Entries")
    tcpipDhcpServerOldEntryDelete.setDefaultValue(True)    

    tcpipDhcpServerheapdependency = ["TCPIP_DHCPS_INSTANCES_NUMBER", "TCPIP_DHCPS_LEASE_ENTRIES_DEFAULT", "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]    
        
    # # DHCP Server Heap Size
    # tcpipDhcpServerHeapSize = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCP_SERVER_HEAP_SIZE", None)
    # tcpipDhcpServerHeapSize.setLabel("DHCP Server Heap Size (bytes)")  
    # tcpipDhcpServerHeapSize.setVisible(False)
    # # tcpipDhcpServerHeapSize.setDefaultValue(tcpipDhcpServerHeapCalc())
    # tcpipDhcpServerHeapSize.setReadOnly(True)
    # tcpipDhcpServerHeapSize.setDependencies(tcpipDhcpServerHeapUpdate, tcpipDhcpServerheapdependency)      
    
    #Add to system_config.h
    tcpipDhcpServerHeaderFtl = tcpipDhcpServerComponent.createFileSymbol(None, None)
    tcpipDhcpServerHeaderFtl.setSourcePath("tcpip/config/dhcps.h.ftl")
    tcpipDhcpServerHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipDhcpServerHeaderFtl.setMarkup(True)
    tcpipDhcpServerHeaderFtl.setType("STRING")

    # Add dhcps.c file
    tcpipDhcpSourceFile = tcpipDhcpServerComponent.createFileSymbol(None, None)
    tcpipDhcpSourceFile.setSourcePath("tcpip/src/dhcps.c")
    tcpipDhcpSourceFile.setOutputName("dhcps.c")
    tcpipDhcpSourceFile.setOverwrite(True)
    tcpipDhcpSourceFile.setDestPath("library/tcpip/src/")
    tcpipDhcpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipDhcpSourceFile.setType("SOURCE")
    tcpipDhcpSourceFile.setEnabled(True)
    tcpipDhcpSourceFile.setDependencies(tcpipDhcpServerGenSourceFile, ["TCPIP_STACK_USE_DHCP_SERVER"])
    
#############################################################################################################

# def tcpipDhcpServerHeapCalc(): 
    # nDHCPInterfaces = Database.getSymbolValue("tcpipDhcpServer","TCPIP_DHCPS_INSTANCES_NUMBER")
    # leaseEntries = Database.getSymbolValue("tcpipDhcpServer","TCPIP_DHCPS_LEASE_ENTRIES_DEFAULT")
    # heap_size = (nDHCPInterfaces * 32) + (40 + (leaseEntries * 28))
    # return heap_size    
    
# def tcpipDhcpServerHeapUpdate(symbol, event): 
    # heap_size = tcpipDhcpServerHeapCalc()
    # symbol.setValue(heap_size)
    # if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        # symbol.setVisible(event["value"])
    
# def tcpipDhcpServerEnableInstance(tcpipNetDependentSymbol, event):
    # global tcpipDhcpServerInstancesNumPrev
    # print("Start tcpipDhcpServerEnableInstance")
    
    # if(event["id"] == "TCPIP_STACK_USE_DHCP_SERVER" ):
        # print("USE DHCP Server")
        # tcpipDhcpServerEnable = Database.getSymbolValue("tcpipDhcpServer","TCPIP_STACK_USE_DHCP_SERVER")
        # print("DHCP Server Enabled: " + str(tcpipDhcpServerEnable) )
        # tcpipDhcpServerIndex = int(tcpipNetDependentSymbol.getID().strip("TCPIP_DHCP_SERVER_IDX"))
        # print("DHCP Server Index: " + str(tcpipDhcpServerIndex) )
        # print(tcpipDhcpServerInstancesNumPrev)
        # if(tcpipDhcpServerEnable == True):
            # if(tcpipDhcpServerIndex < tcpipDhcpServerInstancesNumPrev ):
                # tcpipNetDependentSymbol.setVisible(True)
        # else:
            # tcpipNetDependentSymbol.setVisible(False)
    
    # else:   
        # #tcpipNetDependentSymbol.setVisible(True)
        # if(event["id"] == "TCPIP_STACK_NETWORK_CONFIG_NUMBER" ):
            # tcpipDhcpServerNetConfigNum = int(Database.getSymbolValue("tcpipNetConfig","TCPIP_STACK_NETWORK_CONFIG_NUMBER"))
            # print("Net Config Num:" + str(tcpipDhcpServerNetConfigNum))
            # tcpipNetDependentSymbol.getComponent().getSymbolByID("TCPIP_DHCP_SERVER_INSTANCES_NUMBER").setMax(tcpipDhcpServerNetConfigNum)
        # else:           
            # print(tcpipNetDependentSymbol.getID())
            # tcpipDhcpServerInstanceNumberValue = event["value"]
            # print("Max No:" + str(tcpipNetDependentSymbol.getComponent().getSymbolByID("TCPIP_DHCP_SERVER_INSTANCES_NUMBER").getMax()))
            # print(tcpipDhcpServerInstanceNumberValue)
            # print(tcpipDhcpServerInstancesNumPrev)
            # if(tcpipDhcpServerInstanceNumberValue > tcpipDhcpServerInstancesNumPrev ):
                # tcpipDhcpServerInstance[tcpipDhcpServerInstancesNumPrev].setVisible(True)
                # tcpipDhcpServerInstance[tcpipDhcpServerInstancesNumPrev].setValue(True, 1)
                # print("Set TRUE"+ str(tcpipDhcpServerInstancesNumPrev))
                # tcpipDhcpServerInstancesNumPrev = tcpipDhcpServerInstancesNumPrev + 1
                # #Add more network configurations
            # else:
                # if(tcpipDhcpServerInstanceNumberValue < tcpipDhcpServerInstancesNumPrev ):
                    # #Reduce network configurations
                    # tcpipDhcpServerInstancesNumPrev = tcpipDhcpServerInstancesNumPrev - 1
                    # tcpipDhcpServerInstance[tcpipDhcpServerInstancesNumPrev].setVisible(False)
                    # tcpipDhcpServerInstance[tcpipDhcpServerInstancesNumPrev].setValue(False, 1)
                    # print("Set FALSE"+ str(tcpipDhcpServerInstancesNumPrev))
                    
                # #else:
                    # #Do Nothing
    # print("END tcpipDhcpServerEnableInstance")
    
# def tcpipDhcpServerInstnNumVisible(symbol, event):   
    # if(event["id"] == "TCPIP_STACK_NETWORK_CONFIG_NUMBER" ):
        # eventValue = event["value"]
        # symbol.setValue(int(eventValue),1)
    # else:
        # if (event["value"] == True):
            # tcpipDhcpServerNetConfigNum = int(Database.getSymbolValue("tcpipNetConfig","TCPIP_STACK_NETWORK_CONFIG_NUMBER"))
            # symbol.getComponent().getSymbolByID("TCPIP_DHCP_SERVER_INSTANCES_NUMBER").setMax(tcpipDhcpServerNetConfigNum)        
            # symbol.setVisible(True)
        # else:       
            # symbol.setVisible(False)

##############################################################################################################

# make DHCP Server option visible
# def tcpipDhcpServerMenuVisible(tcpipDependentSymbol, tcpipIPSymbol):
    # tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
    # tcpipUdp = Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP")

    # if(tcpipIPv4 and tcpipUdp):
        # tcpipDependentSymbol.setVisible(True)
    # else:
        # tcpipDependentSymbol.setVisible(False)

# def tcpipDhcpServerICMPClientEnable():
    # tcpipIcmpComp = Database.getComponentByID("tcpipIcmp")
    # print(tcpipIcmpComp)
    # if(tcpipIcmpComp == None):
        # # Enable ICMP Dependency 
        # if(Database.getComponentByID("tcpip_network_config") == None):
            # res = Database.activateComponents(["tcpip_network_config"])
        # if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ICMPv4") != True):
            # setVal("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ICMPv4", True)
        # else:
            # tcpipIcmpComp = Database.activateComponents(["tcpipIcmp"])
            # print(tcpipIcmpComp)

        # tcpipIcmpComp = Database.getComponentByID("tcpipIcmp")
        # print(tcpipIcmpComp)
    # if(tcpipIcmpComp != None):
        # tcpipICMPClientSymbol = tcpipIcmpComp.getSymbolByID("TCPIP_STACK_USE_ICMP_CLIENT")
        # print(tcpipICMPClientSymbol)
        # print("DHCP Server ICMP Client Menu visible.")
    # #if(tcpipIcmpClient):
    # #tcpipIcmpClient.setVisible(True)
    # #tcpipIcmpClient.setEnabled(True)
        # tcpipICMPClientSymbol.setValue(True)
    # print("DHCP Server ICMP Client Enabled.")
    
    
  
# def tcpipDhcpServerMenuVisibleSingle(symbol, event):
    # if (event["value"] == True):
        # print("DHCP Server Menu Visible.")      
        # symbol.setVisible(True)
    # else:
        # print("DHCP Server Menu Invisible.")
        # symbol.setVisible(False)    

# def tcpipDhcpServerInstnAddrRangeMenu(symbol, event):
    # print("DHCP Server Instance Start.")
    # tcpipDhcpServerInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START_IDX"))
    # print(tcpipDhcpServerInstnIndex)
    # tcpipDhcpServerInstnEnable = tcpipDhcpServerInstance[tcpipDhcpServerInstnIndex].getValue()
    # tcpipDhcpServerEnable = Database.getSymbolValue("tcpipDhcpServer","TCPIP_STACK_USE_DHCP_SERVER")
    # print(tcpipDhcpServerEnable)
    
    # if(tcpipDhcpServerInstnEnable and tcpipDhcpServerEnable):
        # symbol.setVisible(True)
    # else:
        # symbol.setVisible(False)

# def tcpipDhcpServerInstnIpAddrMenu(symbol, event):
    # print("DHCP Server Instance Start.")
    # tcpipDhcpServerInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS_IDX"))
    # print(tcpipDhcpServerInstnIndex)
    # tcpipDhcpServerInstnEnable = tcpipDhcpServerInstance[tcpipDhcpServerInstnIndex].getValue()
    # tcpipDhcpServerEnable = Database.getSymbolValue("tcpipDhcpServer","TCPIP_STACK_USE_DHCP_SERVER")
    # print(tcpipDhcpServerEnable)
    
    # if(tcpipDhcpServerInstnEnable and tcpipDhcpServerEnable):
        # symbol.setVisible(True)
    # else:
        # symbol.setVisible(False)

# def tcpipDhcpServerInstnNetMaskAddrMenu(symbol, event):
    # print("DHCP Server Instance Start.")
    # tcpipDhcpServerInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS_IDX"))
    # print(tcpipDhcpServerInstnIndex)
    # tcpipDhcpServerInstnEnable = tcpipDhcpServerInstance[tcpipDhcpServerInstnIndex].getValue()
    # tcpipDhcpServerEnable = Database.getSymbolValue("tcpipDhcpServer","TCPIP_STACK_USE_DHCP_SERVER")
    # print(tcpipDhcpServerEnable)

    # if(tcpipDhcpServerInstnEnable and tcpipDhcpServerEnable):
        # symbol.setVisible(True)
    # else:
        # symbol.setVisible(False)
    
# def tcpipDhcpServerInstnGatewayAddrMenu(symbol, event):
    # print("DHCP Server Instance Start.")
    # tcpipDhcpServerInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_SERVER_GATEWAY_ADDRESS_IDX"))
    # print(tcpipDhcpServerInstnIndex)
    # tcpipDhcpServerInstnEnable = tcpipDhcpServerInstance[tcpipDhcpServerInstnIndex].getValue()
    # tcpipDhcpServerEnable = Database.getSymbolValue("tcpipDhcpServer","TCPIP_STACK_USE_DHCP_SERVER")
    # print(tcpipDhcpServerEnable)

    # if(tcpipDhcpServerInstnEnable and tcpipDhcpServerEnable):
        # symbol.setVisible(True)
    # else:
        # symbol.setVisible(False)    

# def tcpipDhcpServerInstnPrimDnsAddrMenu(symbol, event):
    # print("DHCP Server Instance Start.")
    # tcpipDhcpServerInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS_IDX"))
    # print(tcpipDhcpServerInstnIndex)
    # tcpipDhcpServerInstnEnable = tcpipDhcpServerInstance[tcpipDhcpServerInstnIndex].getValue()
    # tcpipDhcpServerEnable = Database.getSymbolValue("tcpipDhcpServer","TCPIP_STACK_USE_DHCP_SERVER")
    # print(tcpipDhcpServerEnable)

    # if(tcpipDhcpServerInstnEnable and tcpipDhcpServerEnable):
        # symbol.setVisible(True)
    # else:
        # symbol.setVisible(False)    

# def tcpipDhcpServerInstnSecDnsAddrMenu(symbol, event):
    # print("DHCP Server Instance Start.")
    # tcpipDhcpServerInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS_IDX"))
    # print(tcpipDhcpServerInstnIndex)
    # tcpipDhcpServerInstnEnable = tcpipDhcpServerInstance[tcpipDhcpServerInstnIndex].getValue()
    # tcpipDhcpServerEnable = Database.getSymbolValue("tcpipDhcpServer","TCPIP_STACK_USE_DHCP_SERVER")
    # print(tcpipDhcpServerEnable)

    # if(tcpipDhcpServerInstnEnable and tcpipDhcpServerEnable):
        # symbol.setVisible(True)
    # else:
        # symbol.setVisible(False)    

# def tcpipDhcpServerInstnIfIdxMenu(symbol, event):
    # print("DHCP Server Instance Start.")
    # tcpipDhcpServerInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_INTERFACE_INDEX_IDX"))
    # print(tcpipDhcpServerInstnIndex)
    # tcpipDhcpServerInstnEnable = tcpipDhcpServerInstance[tcpipDhcpServerInstnIndex].getValue()
    # tcpipDhcpServerEnable = Database.getSymbolValue("tcpipDhcpServer","TCPIP_STACK_USE_DHCP_SERVER")
    # print(tcpipDhcpServerEnable)

    # if(tcpipDhcpServerInstnEnable and tcpipDhcpServerEnable):
        # symbol.setVisible(True)
    # else:
        # symbol.setVisible(False)    

# def tcpipDhcpServerInstnPoolEnMenu(symbol, event):
    # print("DHCP Server Instance Start.")
    # tcpipDhcpServerInstnIndex = int(symbol.getID().strip("TCPIP_DHCP_SERVER_POOL_ENABLED_IDX"))
    # print(tcpipDhcpServerInstnIndex)
    # tcpipDhcpServerInstnEnable = tcpipDhcpServerInstance[tcpipDhcpServerInstnIndex].getValue()
    # tcpipDhcpServerEnable = Database.getSymbolValue("tcpipDhcpServer","TCPIP_STACK_USE_DHCP_SERVER")
    # print(tcpipDhcpServerEnable)

    # if(tcpipDhcpServerInstnEnable and tcpipDhcpServerEnable):
        # symbol.setVisible(True)
    # else:
        # symbol.setVisible(False) 
        
def tcpipDhcpServerGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])




#########################################################################################################################
#########################################################################################################################
def tcpipDhcpServerMaxInstCount(symbol, event):   
    # print "Netconfig interface Count : " + str (event["value"])
    symbol.setValue(int(event["value"]))
    symbol.getComponent().getSymbolByID("TCPIP_DHCPS_INSTANCES_NUMBER").setMax(int(event["value"])) 

def tcpipDhcpServerInstnNumVisible(symbol, event):   
    global tcpipDhcpServerInstanceRangePrev  

    for index in range(event["value"]):
        tcpipDhcpServerInstance[index].setVisible(True)
        tcpipDhcpServerInstance[index].setValue(True)
    
    if tcpipDhcpServerInstanceRangePrev > event["value"]:
        for index in range(event["value"], tcpipDhcpServerInstanceRangePrev):
            tcpipDhcpServerInstance[index].setVisible(False)
            tcpipDhcpServerInstance[index].setValue(False)
            
    tcpipDhcpServerInstanceRangePrev = event["value"]

            
def tcpipDhcpServerEnableInstance(tcpipNetDependentSymbol, event):
    global tcpipDhcpServerInstancesNumPrev
    print("Start tcpipDhcpServerEnableInstance")
    
    tcpipDhcpServerIndex = int(tcpipNetDependentSymbol.getID().strip("TCPIP_DHCPS_IDX"))
    print str(tcpipDhcpServerIndex)
    tcpipDhcpServerEnable = Database.getSymbolValue("tcpipDhcpServer","TCPIP_STACK_USE_DHCP_SERVER")
    
    
    # if(event["id"] == "TCPIP_STACK_USE_DHCP_SERVER" ):
        # print("USE DHCP Server")
        # tcpipDhcpServerEnable = Database.getSymbolValue("tcpipDhcpServer","TCPIP_STACK_USE_DHCP_SERVER")
        # print("DHCP Server Enabled: " + str(tcpipDhcpServerEnable) )
        # tcpipDhcpServerIndex = int(tcpipNetDependentSymbol.getID().strip("TCPIP_DHCP_SERVER_IDX"))
        # print("DHCP Server Index: " + str(tcpipDhcpServerIndex) )
        # print(tcpipDhcpServerInstancesNumPrev)
        # if(tcpipDhcpServerEnable == True):
            # if(tcpipDhcpServerIndex < tcpipDhcpServerInstancesNumPrev ):
                # tcpipNetDependentSymbol.setVisible(True)
        # else:
            # tcpipNetDependentSymbol.setVisible(False)
    
    # else:   
        # #tcpipNetDependentSymbol.setVisible(True)
        # if(event["id"] == "TCPIP_STACK_NETWORK_CONFIG_NUMBER" ):
            # tcpipDhcpServerNetConfigNum = int(Database.getSymbolValue("tcpipNetConfig","TCPIP_STACK_NETWORK_CONFIG_NUMBER"))
            # print("Net Config Num:" + str(tcpipDhcpServerNetConfigNum))
            # tcpipNetDependentSymbol.getComponent().getSymbolByID("TCPIP_DHCP_SERVER_INSTANCES_NUMBER").setMax(tcpipDhcpServerNetConfigNum)
        # else:           
            # print(tcpipNetDependentSymbol.getID())
            # tcpipDhcpServerInstanceNumberValue = event["value"]
            # print("Max No:" + str(tcpipNetDependentSymbol.getComponent().getSymbolByID("TCPIP_DHCP_SERVER_INSTANCES_NUMBER").getMax()))
            # print(tcpipDhcpServerInstanceNumberValue)
            # print(tcpipDhcpServerInstancesNumPrev)
            # if(tcpipDhcpServerInstanceNumberValue > tcpipDhcpServerInstancesNumPrev ):
                # tcpipDhcpServerInstance[tcpipDhcpServerInstancesNumPrev].setVisible(True)
                # tcpipDhcpServerInstance[tcpipDhcpServerInstancesNumPrev].setValue(True, 1)
                # print("Set TRUE"+ str(tcpipDhcpServerInstancesNumPrev))
                # tcpipDhcpServerInstancesNumPrev = tcpipDhcpServerInstancesNumPrev + 1
                # #Add more network configurations
            # else:
                # if(tcpipDhcpServerInstanceNumberValue < tcpipDhcpServerInstancesNumPrev ):
                    # #Reduce network configurations
                    # tcpipDhcpServerInstancesNumPrev = tcpipDhcpServerInstancesNumPrev - 1
                    # tcpipDhcpServerInstance[tcpipDhcpServerInstancesNumPrev].setVisible(False)
                    # tcpipDhcpServerInstance[tcpipDhcpServerInstancesNumPrev].setValue(False, 1)
                    # print("Set FALSE"+ str(tcpipDhcpServerInstancesNumPrev))
                    
                # #else:
                    # #Do Nothing
    print("END tcpipDhcpServerEnableInstance")



#########################################################################################################################
#########################################################################################################################

#Set symbols of other components
def setVal(component, symbol, value):
    triggerDict = {"Component":component,"Id":symbol, "Value":value}
    if(Database.sendMessage(component, "SET_SYMBOL", triggerDict) == None):
        print "Set Symbol Failure" + component + ":" + symbol + ":" + str(value)
        return False
    else:
        return True

#Handle messages from other components
def handleMessage(messageID, args):
    retDict= {}
    if (messageID == "SET_SYMBOL"):
        print "handleMessage: Set Symbol"
        retDict= {"Return": "Success"}
        Database.setSymbolValue(args["Component"], args["Id"], args["Value"])
    else:
        retDict= {"Return": "UnImplemented Command"}
    return retDict
        

def destroyComponent(component):
    Database.setSymbolValue("tcpipDhcpServer", "TCPIP_STACK_USE_DHCP_SERVER", False, 2)