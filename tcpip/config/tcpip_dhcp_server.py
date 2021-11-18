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
tcpipDhcpServerInstance = []
tcpipDhcpServerIfIdx = []
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
tcpipDhcpServerStartDisable = []
tcpipDhcpServerDelLeaseInfo = []
tcpipDhcpServerCoflictDetectDisable = []
tcpipDhcpServerLeaseExtDisable = []
tcpipDhcpServerKeepInfoUnreq = []
tcpipDhcpServerProbeFailAbort = []
tcpipDhcpServerKeepSilent = []
           
tcpipDhcpServerLeaseDurationMin = []
tcpipDhcpServerLeaseDurationMax = []
tcpipDhcpServerUnreqTimeout = []

tcpipDhcpServerT1RenewalTimeMenu = [] 
tcpipDhcpServerT1RenewMultFactor = []       
tcpipDhcpServerT1RenewDivFactor = []
tcpipDhcpServerT2RebindTimeMenu = []
tcpipDhcpServerT2RebindMultFactor = []
tcpipDhcpServerT2RebindDivFactor = []

tcpipDhcpServerInstanceRangePrev = 1
tcpipNetConfigNumMax = 5  # hard coded value
############################################################################
def instantiateComponent(tcpipDhcpServerComponent):
    print("TCPIP DHCP Server V2 Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Enable DHCP Server
    tcpipDhcpServer = tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_STACK_USE_DHCP_SERVER_V2", None)
    tcpipDhcpServer.setLabel("DHCP Server")
    tcpipDhcpServer.setVisible(False)
    tcpipDhcpServer.setDescription("Enable DHCP Server")
    tcpipDhcpServer.setDefaultValue(True)    

######################################################################################################################################
    if(Database.getComponentByID("tcpipNetConfig") != None):
        dhcpsIntrfcCount = Database.getSymbolValue("tcpipNetConfig", "TCPIP_STACK_NETWORK_INTERAFCE_COUNT")
    else:
        dhcpsIntrfcCount = 1

    # Maximum Number of Interfaces supported by DHCP Server
    tcpipDhcpServerMaxIntrfcNum = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_INTERFACE_MAX_COUNT", None)
    tcpipDhcpServerMaxIntrfcNum.setLabel("Maximum Number of Interfaces")
    tcpipDhcpServerMaxIntrfcNum.setVisible(False)
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

    tcpipDhcpServerInstnDependency = ["TCPIP_DHCPS_INTERFACE_MAX_COUNT"]
      
    
    for index in range(0,tcpipNetConfigNumMax): 
        tcpipDhcpServerInstance.append(tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_IDX"+str(index),None))
        tcpipDhcpServerInstance[index].setLabel("DHCP Server on Interface "+ str(index))
        if (index < tcpipDhcpServerMaxIntrfcNum.getValue()):
            tcpipDhcpServerInstance[index].setVisible(True)            
        else:
            tcpipDhcpServerInstance[index].setVisible(False) 
        tcpipDhcpServerInstance[index].setDefaultValue(False)     
            
        tcpipDhcpServerInstnDependency.append(tcpipDhcpServerInstance[index].getID())
        
        # DHCP Server interface index
        tcpipDhcpServerIfIdx.append(tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_INTERFACE_INDEX_IDX" + str(index), tcpipDhcpServerInstance[index]))
        tcpipDhcpServerIfIdx[index].setLabel("Interface Index for DHCP Server")
        tcpipDhcpServerIfIdx[index].setVisible(False)
        tcpipDhcpServerIfIdx[index].setDefaultValue(index)
        tcpipDhcpServerIfIdx[index].setReadOnly(True)

        # Maximum Number of Leases
        tcpipDhcpServerLeaseEntryMaxNum.append(tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_MAX_LEASE_NUM_IDX" + str(index), tcpipDhcpServerInstance[index]))
        tcpipDhcpServerLeaseEntryMaxNum[index].setLabel("Number of Leases")
        tcpipDhcpServerLeaseEntryMaxNum[index].setVisible(False)
        tcpipDhcpServerLeaseEntryMaxNum[index].setDefaultValue(32)
        tcpipDhcpServerLeaseEntryMaxNum[index].setDependencies(tcpipDhcpServerInstnMenuVisible, [tcpipDhcpServerInstance[index].getID()])

        # Default Lease Duration
        tcpipDhcpServerLeaseDurationDflt.append(tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_LEASEDURATION_DFLT_IDX" + str(index), tcpipDhcpServerInstance[index]))
        tcpipDhcpServerLeaseDurationDflt[index].setLabel("Default Lease Duration(in Sec)")
        tcpipDhcpServerLeaseDurationDflt[index].setVisible(False)
        tcpipDhcpServerLeaseDurationDflt[index].setDefaultValue(28800) # 8 Hours - 8 x 60 x 60
        tcpipDhcpServerLeaseDurationDflt[index].setDependencies(tcpipDhcpServerInstnMenuVisible, [tcpipDhcpServerInstance[index].getID()])
        
        # Server IP Address
        tcpipDhcpServerIpAddr.append(tcpipDhcpServerComponent.createStringSymbol("TCPIP_DHCPS_SERVER_IP_ADDRESS_IDX" + str(index), tcpipDhcpServerInstance[index]))
        tcpipDhcpServerIpAddr[index].setLabel("DHCPS Server IP Address")
        tcpipDhcpServerIpAddr[index].setVisible(False)
        tcpipDhcpServerIpAddr[index].setDefaultValue("192.168.1." + str(index + 1))
        tcpipDhcpServerIpAddr[index].setDependencies(tcpipDhcpServerInstnMenuVisible, [tcpipDhcpServerInstance[index].getID()])
        
        # Lease Start IP Address
        tcpipDhcpServerStartIPAddr.append(tcpipDhcpServerComponent.createStringSymbol("TCPIP_DHCPS_START_IP_ADDR_IDX" + str(index), tcpipDhcpServerInstance[index]))
        tcpipDhcpServerStartIPAddr[index].setLabel("Lease IP Address Starting from")
        tcpipDhcpServerStartIPAddr[index].setVisible(False)
        tcpipDhcpServerStartIPAddr[index].setDefaultValue("192.168.1.100")
        tcpipDhcpServerStartIPAddr[index].setDependencies(tcpipDhcpServerInstnMenuVisible, [tcpipDhcpServerInstance[index].getID()])
        
        # Prefix Length (CIDR notation) Network Mask
        tcpipDhcpServerLeadBitNum.append(tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_MASK_PREFIX_NUM_IDX" + str(index), tcpipDhcpServerInstance[index]))
        tcpipDhcpServerLeadBitNum[index].setLabel("Prefix Length (CIDR notation)")
        tcpipDhcpServerLeadBitNum[index].setVisible(False)
        tcpipDhcpServerLeadBitNum[index].setMin(0)
        tcpipDhcpServerLeadBitNum[index].setMax(32)
        tcpipDhcpServerLeadBitNum[index].setDefaultValue(24) 
        tcpipDhcpServerLeadBitNum[index].setDependencies(tcpipDhcpServerInstnMenuVisible, [tcpipDhcpServerInstance[index].getID()])
        
        # Client Options        
        tcpipDhcpServerClientOptMenu.append(tcpipDhcpServerComponent.createMenuSymbol("TCPIP_DHCPS_CLIENT_OPT_MENU_IDX" + str(index), tcpipDhcpServerInstance[index]))
        tcpipDhcpServerClientOptMenu[index].setLabel("Client Options")
        tcpipDhcpServerClientOptMenu[index].setVisible(False)
        tcpipDhcpServerClientOptMenu[index].setDependencies(tcpipDhcpServerInstnMenuVisible, [tcpipDhcpServerInstance[index].getID()])
        
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
        tcpipDhcpServerInstanceAdvMenu[index].setLabel("Interface "+ str(index) + " Advanced Setting")
        tcpipDhcpServerInstanceAdvMenu[index].setVisible(False)
        tcpipDhcpServerInstanceAdvMenu[index].setDependencies(tcpipDhcpServerInstnMenuVisible, [tcpipDhcpServerInstance[index].getID()])

        # Config Flags menu       
        tcpipDhcpServerConfigFlagMenu.append(tcpipDhcpServerComponent.createMenuSymbol("TCPIP_DHCPS_CONFIG_MENU_IDX" + str(index), tcpipDhcpServerInstanceAdvMenu[index]))
        tcpipDhcpServerConfigFlagMenu[index].setLabel("Configuration Flags")
        tcpipDhcpServerConfigFlagMenu[index].setVisible(True)
        
        # Config Flag - DHCP Server Disabled at Start
        tcpipDhcpServerStartDisable.append(tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_START_DISABLE_IDX"+str(index),tcpipDhcpServerConfigFlagMenu[index]))
        tcpipDhcpServerStartDisable[index].setLabel("Disabled at Start of Interface")
        tcpipDhcpServerStartDisable[index].setVisible(True)
        tcpipDhcpServerStartDisable[index].setDefaultValue(False)
        tcpipDhcpServerStartDisable[index].setDescription("DHCP Server Disabled at Start of Interface")
                
        # Config Flag - DHCP Server Delete Lease Info on Interface Restart
        tcpipDhcpServerDelLeaseInfo.append(tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_DELETE_LEASE_INFO_IDX"+str(index),tcpipDhcpServerConfigFlagMenu[index]))
        tcpipDhcpServerDelLeaseInfo[index].setLabel("Delete Lease Info on Restart")
        tcpipDhcpServerDelLeaseInfo[index].setVisible(True)
        tcpipDhcpServerDelLeaseInfo[index].setDefaultValue(False)  
        tcpipDhcpServerDelLeaseInfo[index].setDescription("DHCP Server Delete Lease Info on Interface Restart")
     
        # Config Flag - DHCP Server Disable Conflict Detection of newly allocated addresses
        tcpipDhcpServerCoflictDetectDisable.append(tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_CONFLICT_DETECT_DISABLE_IDX"+str(index),tcpipDhcpServerConfigFlagMenu[index]))
        tcpipDhcpServerCoflictDetectDisable[index].setLabel("Disable Conflict Detection")
        tcpipDhcpServerCoflictDetectDisable[index].setVisible(True)
        tcpipDhcpServerCoflictDetectDisable[index].setDefaultValue(False)  
        tcpipDhcpServerCoflictDetectDisable[index].setDescription("DHCP Server Disable Conflict Detection of newly allocated addresses")
                
        # Config Flag - DHCP Server Disable Lease Extension Request
        tcpipDhcpServerLeaseExtDisable.append(tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_LEASE_EXT_DISABLE_IDX"+str(index),tcpipDhcpServerConfigFlagMenu[index]))
        tcpipDhcpServerLeaseExtDisable[index].setLabel("Disable Lease Extension Request")
        tcpipDhcpServerLeaseExtDisable[index].setVisible(True)
        tcpipDhcpServerLeaseExtDisable[index].setDefaultValue(False)  
        tcpipDhcpServerLeaseExtDisable[index].setDescription("DHCP Server Disable Lease Extension Request from Client")
                
        # Config Flag - DHCP Server Keep Info for Offers Not Requested (multi-server environment)
        tcpipDhcpServerKeepInfoUnreq.append(tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_KEEP_INFO_UNREQ_IDX"+str(index),tcpipDhcpServerConfigFlagMenu[index]))
        tcpipDhcpServerKeepInfoUnreq[index].setLabel("Keep Info of Offers Not Requested")
        tcpipDhcpServerKeepInfoUnreq[index].setVisible(True)
        tcpipDhcpServerKeepInfoUnreq[index].setDefaultValue(False)  
        tcpipDhcpServerKeepInfoUnreq[index].setDescription("DHCP Server Keep Info of Offers Not Requested by Client (multi-server environment)")
                
        # Config Flag - DHCP Server Abort if ICMP Probe for Conflict Detection Fails
        tcpipDhcpServerProbeFailAbort.append(tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_PROB_FAIL_ABORT_IDX"+str(index),tcpipDhcpServerConfigFlagMenu[index]))
        tcpipDhcpServerProbeFailAbort[index].setLabel("ICMP Probe Failure Abort")
        tcpipDhcpServerProbeFailAbort[index].setVisible(True)
        tcpipDhcpServerProbeFailAbort[index].setDefaultValue(False)  
        tcpipDhcpServerProbeFailAbort[index].setDescription("DHCP Server Abort if ICMP Probe for Conflict Detection Fails")
        
        # Config Flag - DHCP Server Silent if No Client record
        tcpipDhcpServerKeepSilent.append(tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_KEEP_SILENT_IDX"+str(index),tcpipDhcpServerConfigFlagMenu[index]))
        tcpipDhcpServerKeepSilent[index].setLabel("Keep Silent if No Record")
        tcpipDhcpServerKeepSilent[index].setVisible(True)
        tcpipDhcpServerKeepSilent[index].setDefaultValue(False)  
        tcpipDhcpServerKeepSilent[index].setDescription("DHCP Server Keep Silent for a Client Request with No Record")
        
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
        tcpipDhcpServerLeaseDurationMax[index].setDescription("When 0, Default Lease Duration will be used")
       
        # Unrequested Offer Timeout
        tcpipDhcpServerUnreqTimeout.append(tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_UNREQ_TMO_IDX" + str(index), tcpipDhcpServerInstanceAdvMenu[index]))
        tcpipDhcpServerUnreqTimeout[index].setLabel("Unrequested Offer Timeout(in Sec)")
        tcpipDhcpServerUnreqTimeout[index].setVisible(True)
        tcpipDhcpServerUnreqTimeout[index].setDefaultValue(0)
        
        # T1 Renewal Time       
        tcpipDhcpServerT1RenewalTimeMenu.append(tcpipDhcpServerComponent.createMenuSymbol("TCPIP_DHCPS_T1_RENEWAL_MENU_IDX" + str(index), tcpipDhcpServerInstanceAdvMenu[index]))
        tcpipDhcpServerT1RenewalTimeMenu[index].setLabel("T1 Renewal Time Calculation")
        tcpipDhcpServerT1RenewalTimeMenu[index].setVisible(True)

        # T1 Renewal Time - Multiplication Factor
        tcpipDhcpServerT1RenewMultFactor.append(tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_T1RENEW_MULT_FACT_IDX" + str(index), tcpipDhcpServerT1RenewalTimeMenu[index]))
        tcpipDhcpServerT1RenewMultFactor[index].setLabel("Multiplication Factor")
        tcpipDhcpServerT1RenewMultFactor[index].setVisible(True)
        tcpipDhcpServerT1RenewMultFactor[index].setDefaultValue(1)
        tcpipDhcpServerT1RenewMultFactor[index].setMin(0)
        
        # T1 Renewal Time - Division Factor
        tcpipDhcpServerT1RenewDivFactor.append(tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_T1RENEW_DIV_FACT_IDX" + str(index), tcpipDhcpServerT1RenewalTimeMenu[index]))
        tcpipDhcpServerT1RenewDivFactor[index].setLabel("Division Factor")
        tcpipDhcpServerT1RenewDivFactor[index].setVisible(True)
        tcpipDhcpServerT1RenewDivFactor[index].setDefaultValue(2)
        tcpipDhcpServerT1RenewDivFactor[index].setMin(1)
        
        # T2 Rebind Time       
        tcpipDhcpServerT2RebindTimeMenu.append(tcpipDhcpServerComponent.createMenuSymbol("TCPIP_DHCPS_T2_REBIND_MENU_IDX" + str(index), tcpipDhcpServerInstanceAdvMenu[index]))
        tcpipDhcpServerT2RebindTimeMenu[index].setLabel("T2 Rebind Time Calculation")
        tcpipDhcpServerT2RebindTimeMenu[index].setVisible(True)

        # T2 Rebind Time  - Multiplication Factor
        tcpipDhcpServerT2RebindMultFactor.append(tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_T2REBIND_MULT_FACT_IDX" + str(index), tcpipDhcpServerT2RebindTimeMenu[index]))
        tcpipDhcpServerT2RebindMultFactor[index].setLabel("Multiplication Factor")
        tcpipDhcpServerT2RebindMultFactor[index].setVisible(True)
        tcpipDhcpServerT2RebindMultFactor[index].setDefaultValue(7)
        tcpipDhcpServerT2RebindMultFactor[index].setMin(0)
        
        # T2 Rebind Time  - Division Factor
        tcpipDhcpServerT2RebindDivFactor.append(tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_T2REBIND_DIV_FACT_IDX" + str(index), tcpipDhcpServerT2RebindTimeMenu[index]))
        tcpipDhcpServerT2RebindDivFactor[index].setLabel("Division Factor")
        tcpipDhcpServerT2RebindDivFactor[index].setVisible(True)
        tcpipDhcpServerT2RebindDivFactor[index].setDefaultValue(8)
        tcpipDhcpServerT2RebindDivFactor[index].setMin(1)
            
    ######################################################################################################################################    

    # DHCP Server Instance Count
    tcpipDhcpServerInstanceCount = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCPS_INTERFACE_COUNT", None)
    tcpipDhcpServerInstanceCount.setLabel("DHCP Server Interface Count")
    tcpipDhcpServerInstanceCount.setVisible(False)
    tcpipDhcpServerInstanceCount.setDefaultValue(0) 
    tcpipDhcpServerInstanceCount.setDependencies(tcpipDhcpServerInstnCount, tcpipDhcpServerInstnDependency)
    
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
    tcpipDhcpServerOptionT1T2Suppress = tcpipDhcpServerComponent.createBooleanSymbol("TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS", tcpipDhcpServerAdvSettings)
    tcpipDhcpServerOptionT1T2Suppress.setLabel("Suppress T1 Renewal and T2 Rebind")
    tcpipDhcpServerOptionT1T2Suppress.setVisible(True)
    tcpipDhcpServerOptionT1T2Suppress.setDescription("Suppress T1 Renewal and T2 Rebind")
    tcpipDhcpServerOptionT1T2Suppress.setDefaultValue(False)

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

    tcpipDhcpServerheapdependency = ["TCPIP_DHCPS_MAX_LEASES", "TCPIP_DHCPS_OPTION_ROUTER_VALUES", "TCPIP_DHCPS_OPTION_DNS_VALUES", "TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES", "TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES", "TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES", "TCPIP_DHCPS_MAX_EVENT_REGISTRATIONS", "TCPIP_DHCPS_INTERFACE_MAX_COUNT", "TCPIP_DHCPS_CLIENT_ID_MAX_SIZE", "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK", "TCPIP_DHCPS_INTERFACE_COUNT"]     
    
    # DHCP Server Heap Size
    tcpipDhcpServerHeapSize = tcpipDhcpServerComponent.createIntegerSymbol("TCPIP_DHCP_SERVER_V2_HEAP_SIZE", None)
    tcpipDhcpServerHeapSize.setLabel("DHCP Server Heap Size (bytes)")  
    tcpipDhcpServerHeapSize.setVisible(False)
    tcpipDhcpServerHeapSize.setDefaultValue(tcpipDhcpServerHeapCalc())
    tcpipDhcpServerHeapSize.setReadOnly(True)
    tcpipDhcpServerHeapSize.setDependencies(tcpipDhcpServerHeapUpdate, tcpipDhcpServerheapdependency)      
    
    #Add to system_config.h
    tcpipDhcpServerHeaderFtl = tcpipDhcpServerComponent.createFileSymbol(None, None)
    tcpipDhcpServerHeaderFtl.setSourcePath("tcpip/config/dhcp_server.h.ftl")
    tcpipDhcpServerHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipDhcpServerHeaderFtl.setMarkup(True)
    tcpipDhcpServerHeaderFtl.setType("STRING")

    # Add dhcp_server.c file
    tcpipDhcpServerSourceFile = tcpipDhcpServerComponent.createFileSymbol(None, None)
    tcpipDhcpServerSourceFile.setSourcePath("tcpip/src/dhcp_server.c")
    tcpipDhcpServerSourceFile.setOutputName("dhcp_server.c")
    tcpipDhcpServerSourceFile.setOverwrite(True)
    tcpipDhcpServerSourceFile.setDestPath("library/tcpip/src/")
    tcpipDhcpServerSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipDhcpServerSourceFile.setType("SOURCE")
    tcpipDhcpServerSourceFile.setEnabled(True)
    
#############################################################################################################

def tcpipDhcpServerHeapCalc(): 
    max_leases = ((Database.getSymbolValue("tcpipDhcpServer","TCPIP_DHCPS_MAX_LEASES") + 31)/32)*32 #round up to a multiple of 32
    dhcps_interface_dcpt_size = 78  + 4 * ( Database.getSymbolValue("tcpipDhcpServer","TCPIP_DHCPS_OPTION_ROUTER_VALUES") + \
                                            Database.getSymbolValue("tcpipDhcpServer","TCPIP_DHCPS_OPTION_DNS_VALUES") + \
                                            Database.getSymbolValue("tcpipDhcpServer","TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES") + \
                                            Database.getSymbolValue("tcpipDhcpServer","TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES") + \
                                            Database.getSymbolValue("tcpipDhcpServer","TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES") + \
                                            max_leases / 32)
                                            
    dhcps_dcpt_size = 24 + ((Database.getSymbolValue("tcpipDhcpServer","TCPIP_DHCPS_OPTION_DNS_VALUES")) * 12) + \
                           (Database.getSymbolValue("tcpipDhcpServer","TCPIP_DHCPS_OPTION_DNS_VALUES")) * (dhcps_interface_dcpt_size)
    
    leaseEntries = max_leases
    extraSize = 40 + leaseEntries * (76 + min(6, Database.getSymbolValue("tcpipDhcpServer","TCPIP_DHCPS_CLIENT_ID_MAX_SIZE"))) 
    instanceCount = Database.getSymbolValue("tcpipDhcpServer","TCPIP_DHCPS_INTERFACE_COUNT")
    instance_size = extraSize * instanceCount
    heap_size = dhcps_dcpt_size + instance_size
    return heap_size    
    
def tcpipDhcpServerHeapUpdate(symbol, event): 
    heap_size = tcpipDhcpServerHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])
    
        
def tcpipDhcpServerGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

def tcpipDhcpServerMaxInstCount(symbol, event):   
    global tcpipDhcpServerInstanceRangePrev 
    symbol.setValue(int(event["value"]))
    
    for index in range(event["value"]):
        tcpipDhcpServerInstance[index].setVisible(True)
    
    if tcpipDhcpServerInstanceRangePrev > event["value"]:
        for index in range(event["value"], tcpipDhcpServerInstanceRangePrev):
            tcpipDhcpServerInstance[index].setVisible(False)
            tcpipDhcpServerInstance[index].setValue(False)
            
    tcpipDhcpServerInstanceRangePrev = event["value"]

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


def tcpipDhcpServerInstnCount(symbol, event):
    maxInterfaces = Database.getSymbolValue("tcpipDhcpServer","TCPIP_DHCPS_INTERFACE_MAX_COUNT")
    instanceCount = 0
    for index in range(maxInterfaces):
        if tcpipDhcpServerInstance[index].getValue() == True:
            instanceCount += 1   
    symbol.setValue(instanceCount)

    
def tcpipDhcpServerInstnMenuVisible(symbol, event):
    symbol.setVisible(event["value"])

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
        
def finalizeComponent( macComponent ):
    tcpipDhcpServerInstance[0].setValue(True)

def destroyComponent(component):
    Database.setSymbolValue("tcpipDhcpServer", "TCPIP_STACK_USE_DHCP_SERVER_V2", False, 2)
