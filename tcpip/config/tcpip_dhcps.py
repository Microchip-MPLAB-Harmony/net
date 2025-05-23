"""
Copyright (C) 2019-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
"""

################## DHCP Server Instances ##############################
tcpipDhcpsInstancesNumPrev = 1
tcpipDhcpsInstance = []
tcpipDhcpsAddrRangeStart = []
tcpipDhcpsIpAddr = []
tcpipDhcpsNetMaskAddr = []
tcpipDhcpsGatewayAddr = []
tcpipDhcpsPrimDnsAddr = []
tcpipDhcpsSecDnsAddr = []
tcpipDhcpsIfIdx = []
tcpipDhcpsPoolEn = []
tcpipNetConfigNumMax = 2  # hard coded value
############################################################################
def instantiateComponent(tcpipDhcpsComponent):
    print(tcpipNetConfigNumMax)
    print("TCPIP DHCP Server Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    tcpipDhcpsICMPClientEnable()
    # Enable DHCP Server
    tcpipDhcps = tcpipDhcpsComponent.createBooleanSymbol("TCPIP_STACK_USE_DHCP_SERVER", None)
    tcpipDhcps.setHelp("mcc_h3_dhcps_configurations")
    tcpipDhcps.setLabel("DHCP Server")
    tcpipDhcps.setVisible(False)
    tcpipDhcps.setDescription("Enable DHCP Server")
    tcpipDhcps.setDefaultValue(True)
    

    # Maximum Number of Entries in the Lease Table
    tcpipDhcpsLeaseEntryMaxNum = tcpipDhcpsComponent.createIntegerSymbol("TCPIP_DHCPS_LEASE_ENTRIES_DEFAULT", None)
    tcpipDhcpsLeaseEntryMaxNum.setHelp("mcc_h3_dhcps_configurations")
    tcpipDhcpsLeaseEntryMaxNum.setLabel("Maximum Number of Entries in the Lease Table")
    tcpipDhcpsLeaseEntryMaxNum.setVisible(True)
    tcpipDhcpsLeaseEntryMaxNum.setDescription("Maximum Number of Entries in the Lease Table")
    tcpipDhcpsLeaseEntryMaxNum.setDefaultValue(15)
######################################################################################################################################

    # Number of DHCP Server Driver Instances
    tcpipDhcpsInstancesNum = tcpipDhcpsComponent.createIntegerSymbol("TCPIP_DHCP_SERVER_INSTANCES_NUMBER", None)
    tcpipDhcpsInstancesNum.setHelp("mcc_h3_dhcps_configurations")
    tcpipDhcpsInstancesNum.setLabel("Number of DHCP Server Instances")
    tcpipDhcpsInstancesNum.setMax(tcpipNetConfigNumMax)
    tcpipDhcpsInstancesNum.setMin(1)
    tcpipDhcpsInstancesNum.setVisible(True)
    tcpipDhcpsInstancesNum.setDescription("Number of DHCP Server Instances")
    tcpipDhcpsInstancesNum.setDefaultValue(1)
    tcpipDhcpsInstancesNum.setDependencies(tcpipDhcpsInstnNumVisible, ["TCPIP_STACK_USE_DHCP_SERVER","tcpipNetConfig.TCPIP_STACK_NETWORK_CONFIG_NUMBER"])

    for index in range(0,tcpipNetConfigNumMax): 
        tcpipDhcpsInstance.append(tcpipDhcpsComponent.createBooleanSymbol("TCPIP_DHCP_SERVER_IDX"+str(index),tcpipDhcpsInstancesNum))
        tcpipDhcpsInstance[index].setLabel("DHCP Server Instance "+ str(index))
        if (index < tcpipDhcpsInstancesNum.getValue()):
            tcpipDhcpsInstance[index].setVisible(True)
            tcpipDhcpsInstance[index].setDefaultValue(True)
        else:
            tcpipDhcpsInstance[index].setDefaultValue(False)
            tcpipDhcpsInstance[index].setVisible(False) 
        tcpipDhcpsInstance[index].setDependencies(tcpipDhcpsEnableInstance, ["TCPIP_DHCP_SERVER_INSTANCES_NUMBER", "tcpipNetConfig.TCPIP_STACK_NETWORK_CONFIG_NUMBER", "TCPIP_STACK_USE_DHCP_SERVER"])      
    
        # DHCP Server interface index
        tcpipDhcpsIfIdx.append(tcpipDhcpsComponent.createIntegerSymbol("TCPIP_DHCP_SERVER_INTERFACE_INDEX_IDX" + str(index), tcpipDhcpsInstance[index]))
        tcpipDhcpsIfIdx[index].setHelp("mcc_h3_dhcps_configurations")
        tcpipDhcpsIfIdx[index].setLabel("Interface Index for DHCP Server")
        tcpipDhcpsIfIdx[index].setVisible(True)
        tcpipDhcpsIfIdx[index].setDefaultValue(index)
        tcpipDhcpsIfIdx[index].setDependencies(tcpipDhcpsInstnIfIdxMenu, [tcpipDhcpsInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"]) 
        
        # DHCP Server Address Range Start
        tcpipDhcpsAddrRangeStart.append(tcpipDhcpsComponent.createStringSymbol("TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START_IDX" + str(index), tcpipDhcpsInstance[index]))
        tcpipDhcpsAddrRangeStart[index].setHelp("mcc_h3_dhcps_configurations")
        tcpipDhcpsAddrRangeStart[index].setLabel("DHCPS Address Range Start")
        tcpipDhcpsAddrRangeStart[index].setVisible(True)
        tcpipDhcpsAddrRangeStart[index].setDefaultValue("192.168.1.100")
        tcpipDhcpsAddrRangeStart[index].setDependencies(tcpipDhcpsInstnAddrRangeMenu, [tcpipDhcpsInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])
        
        # DHCP Server IP Address
        tcpipDhcpsIpAddr.append(tcpipDhcpsComponent.createStringSymbol("TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS_IDX" + str(index), tcpipDhcpsInstance[index]))
        tcpipDhcpsIpAddr[index].setHelp("mcc_h3_dhcps_configurations")
        tcpipDhcpsIpAddr[index].setLabel("DHCPS Server IP Address")
        tcpipDhcpsIpAddr[index].setVisible(True)
        tcpipDhcpsIpAddr[index].setDefaultValue("192.168.1.1")
        tcpipDhcpsIpAddr[index].setDependencies(tcpipDhcpsInstnIpAddrMenu, [tcpipDhcpsInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])
    
        # DHCP Server Net Mask Address
        tcpipDhcpsNetMaskAddr.append(tcpipDhcpsComponent.createStringSymbol("TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS_IDX" + str(index), tcpipDhcpsInstance[index]))
        tcpipDhcpsNetMaskAddr[index].setHelp("mcc_h3_dhcps_configurations")
        tcpipDhcpsNetMaskAddr[index].setLabel("DHCPS Netmask")
        tcpipDhcpsNetMaskAddr[index].setVisible(True)
        tcpipDhcpsNetMaskAddr[index].setDefaultValue("255.255.255.0")
        tcpipDhcpsNetMaskAddr[index].setDependencies(tcpipDhcpsInstnNetMaskAddrMenu, [tcpipDhcpsInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])
    
        # DHCP Server Default Gateway Address
        tcpipDhcpsGatewayAddr.append(tcpipDhcpsComponent.createStringSymbol("TCPIP_DHCPS_DEFAULT_SERVER_GATEWAY_ADDRESS_IDX" + str(index), tcpipDhcpsInstance[index]))
        tcpipDhcpsGatewayAddr[index].setHelp("mcc_h3_dhcps_configurations")
        tcpipDhcpsGatewayAddr[index].setLabel("Default Gateway")
        tcpipDhcpsGatewayAddr[index].setVisible(True)
        tcpipDhcpsGatewayAddr[index].setDefaultValue("192.168.1.1")
        tcpipDhcpsGatewayAddr[index].setDependencies(tcpipDhcpsInstnGatewayAddrMenu, [tcpipDhcpsInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"]) 
    
        # DHCP Server Primary DNS Server Address
        tcpipDhcpsPrimDnsAddr.append(tcpipDhcpsComponent.createStringSymbol("TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS_IDX" + str(index), tcpipDhcpsInstance[index]))
        tcpipDhcpsPrimDnsAddr[index].setHelp("mcc_h3_dhcps_configurations")
        tcpipDhcpsPrimDnsAddr[index].setLabel("Primary DNS Server Address")
        tcpipDhcpsPrimDnsAddr[index].setVisible(True)
        tcpipDhcpsPrimDnsAddr[index].setDefaultValue("192.168.1.1")
        tcpipDhcpsPrimDnsAddr[index].setDependencies(tcpipDhcpsInstnPrimDnsAddrMenu, [tcpipDhcpsInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"]) 
    
        # DHCP Server Secondary DNS Server Address
        tcpipDhcpsSecDnsAddr.append(tcpipDhcpsComponent.createStringSymbol("TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS_IDX" + str(index), tcpipDhcpsInstance[index]))
        tcpipDhcpsSecDnsAddr[index].setHelp("mcc_h3_dhcps_configurations")
        tcpipDhcpsSecDnsAddr[index].setLabel("Secondary DNS Server Address")
        tcpipDhcpsSecDnsAddr[index].setVisible(True)
        tcpipDhcpsSecDnsAddr[index].setDefaultValue("192.168.1.1")
        tcpipDhcpsSecDnsAddr[index].setDependencies(tcpipDhcpsInstnSecDnsAddrMenu, [tcpipDhcpsInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])   
    
        # DHCP Server Pool enabled
        tcpipDhcpsPoolEn.append(tcpipDhcpsComponent.createBooleanSymbol("TCPIP_DHCP_SERVER_POOL_ENABLED_IDX" + str(index), tcpipDhcpsInstance[index]))
        tcpipDhcpsPoolEn[index].setHelp("mcc_h3_dhcps_configurations")
        tcpipDhcpsPoolEn[index].setLabel("Pool Enabled")
        tcpipDhcpsPoolEn[index].setVisible(True)
        tcpipDhcpsPoolEn[index].setDefaultValue(True)
        tcpipDhcpsPoolEn[index].setDependencies(tcpipDhcpsInstnPoolEnMenu, [tcpipDhcpsInstance[index].getID(),"TCPIP_STACK_USE_DHCP_SERVER"])   
    
    ######################################################################################################################################    
    # Advanced Settings
    tcpipDhcpsAdvSettings = tcpipDhcpsComponent.createMenuSymbol("TCPIP_DHCPS_ADV_SETTING", None)
    tcpipDhcpsAdvSettings.setLabel("Advanced Settings")
    tcpipDhcpsAdvSettings.setDescription("Advanced Settings")
    tcpipDhcpsAdvSettings.setVisible(True)

    # DHCP Server Update Rate in msec
    tcpipDhcpsTskProcessRate = tcpipDhcpsComponent.createIntegerSymbol("TCPIP_DHCPS_TASK_PROCESS_RATE", tcpipDhcpsAdvSettings)
    tcpipDhcpsTskProcessRate.setHelp("mcc_h3_dhcps_configurations")
    tcpipDhcpsTskProcessRate.setLabel("DHCP Server Task Rate (in msec)")
    tcpipDhcpsTskProcessRate.setVisible(True)
    tcpipDhcpsTskProcessRate.setDescription("DHCP Server Update Rate in msec")
    tcpipDhcpsTskProcessRate.setDefaultValue(200)

    # Time-out for a Solved Entry in the Cache in Seconds.
    tcpipDhcpsLeaseSolvedEntryTimeout = tcpipDhcpsComponent.createIntegerSymbol("TCPIP_DHCPS_LEASE_SOLVED_ENTRY_TMO", tcpipDhcpsAdvSettings)
    tcpipDhcpsLeaseSolvedEntryTimeout.setHelp("mcc_h3_dhcps_configurations")
    tcpipDhcpsLeaseSolvedEntryTimeout.setLabel("Timeout for a Solved Entry in the Cache (in sec)")
    tcpipDhcpsLeaseSolvedEntryTimeout.setVisible(True)
    tcpipDhcpsLeaseSolvedEntryTimeout.setDescription("Time-out for a Solved Entry in the Cache in Seconds.")
    tcpipDhcpsLeaseSolvedEntryTimeout.setDefaultValue(1200)

    # Time-out for a Solved Entry in the Cache in Seconds.
    tcpipDhcpsLeaseUnsolvedEntryTimeout = tcpipDhcpsComponent.createIntegerSymbol("TCPIP_DHCPS_LEASE_REMOVED_BEFORE_ACK", tcpipDhcpsAdvSettings)
    tcpipDhcpsLeaseUnsolvedEntryTimeout.setHelp("mcc_h3_dhcps_configurations")
    tcpipDhcpsLeaseUnsolvedEntryTimeout.setLabel("Timeout for an Unsolved Entry (in sec)")
    tcpipDhcpsLeaseUnsolvedEntryTimeout.setVisible(True)
    tcpipDhcpsLeaseUnsolvedEntryTimeout.setDescription("Time-out for an Unsolved Entry in Seconds")
    tcpipDhcpsLeaseUnsolvedEntryTimeout.setDefaultValue(5)

    # Delete Old Entries
    tcpipDhcpsOldEntryDelete = tcpipDhcpsComponent.createBooleanSymbol("TCPIP_DHCP_SERVER_DELETE_OLD_ENTRIES", tcpipDhcpsAdvSettings)
    tcpipDhcpsOldEntryDelete.setHelp("mcc_h3_dhcps_configurations")
    tcpipDhcpsOldEntryDelete.setLabel("Delete Old Entries")
    tcpipDhcpsOldEntryDelete.setVisible(True)
    tcpipDhcpsOldEntryDelete.setDescription("Delete Old Entries")
    tcpipDhcpsOldEntryDelete.setDefaultValue(True)    

    tcpipDhcpsheapdependency = ["TCPIP_DHCP_SERVER_INSTANCES_NUMBER", "TCPIP_DHCPS_LEASE_ENTRIES_DEFAULT", "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]    
        
    # DHCP Server Heap Size
    tcpipDhcpsHeapSize = tcpipDhcpsComponent.createIntegerSymbol("TCPIP_DHCP_SERVER_HEAP_SIZE", None)
    tcpipDhcpsHeapSize.setLabel("DHCP Server Heap Size (bytes)")  
    tcpipDhcpsHeapSize.setVisible(False)
    tcpipDhcpsHeapSize.setDefaultValue(tcpipDhcpsHeapCalc())
    tcpipDhcpsHeapSize.setReadOnly(True)
    tcpipDhcpsHeapSize.setDependencies(tcpipDhcpsHeapUpdate, tcpipDhcpsheapdependency)      
    
    #Add to system_config.h
    tcpipDhcpsHeaderFtl = tcpipDhcpsComponent.createFileSymbol(None, None)
    tcpipDhcpsHeaderFtl.setSourcePath("tcpip/config/dhcps.h.ftl")
    tcpipDhcpsHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipDhcpsHeaderFtl.setMarkup(True)
    tcpipDhcpsHeaderFtl.setType("STRING")

    # Add dhcps.c file
    tcpipDhcpSourceFile = tcpipDhcpsComponent.createFileSymbol(None, None)
    tcpipDhcpSourceFile.setSourcePath("tcpip/src/dhcps.c")
    tcpipDhcpSourceFile.setOutputName("dhcps.c")
    tcpipDhcpSourceFile.setOverwrite(True)
    tcpipDhcpSourceFile.setDestPath("library/tcpip/src/")
    tcpipDhcpSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipDhcpSourceFile.setType("SOURCE")
    tcpipDhcpSourceFile.setEnabled(True)
    tcpipDhcpSourceFile.setDependencies(tcpipDhcpsGenSourceFile, ["TCPIP_STACK_USE_DHCP_SERVER"])
    
#############################################################################################################

def tcpipDhcpsHeapCalc(): 
    nDHCPInterfaces = Database.getSymbolValue("tcpipDhcps","TCPIP_DHCP_SERVER_INSTANCES_NUMBER")
    leaseEntries = Database.getSymbolValue("tcpipDhcps","TCPIP_DHCPS_LEASE_ENTRIES_DEFAULT")
    heap_size = (nDHCPInterfaces * 32) + (40 + (leaseEntries * 28))
    return heap_size    
    
def tcpipDhcpsHeapUpdate(symbol, event): 
    heap_size = tcpipDhcpsHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])
    
def tcpipDhcpsEnableInstance(tcpipNetDependentSymbol, event):
    global tcpipDhcpsInstancesNumPrev
    print("Start tcpipDhcpsEnableInstance")
    
    if(event["id"] == "TCPIP_STACK_USE_DHCP_SERVER" ):
        print("USE DHCP Server")
        tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
        print("DHCP Server Enabled: " + str(tcpipDhcpsEnable) )
        tcpipDhcpsIndex = int(tcpipNetDependentSymbol.getID().strip("TCPIP_DHCP_SERVER_IDX"))
        print("DHCP Server Index: " + str(tcpipDhcpsIndex) )
        print(tcpipDhcpsInstancesNumPrev)
        if(tcpipDhcpsEnable == True):
            if(tcpipDhcpsIndex < tcpipDhcpsInstancesNumPrev ):
                tcpipNetDependentSymbol.setVisible(True)
        else:
            tcpipNetDependentSymbol.setVisible(False)
    
    else:   
        #tcpipNetDependentSymbol.setVisible(True)
        if(event["id"] == "TCPIP_STACK_NETWORK_CONFIG_NUMBER" ):
            tcpipDhcpsNetConfigNum = int(Database.getSymbolValue("tcpipNetConfig","TCPIP_STACK_NETWORK_CONFIG_NUMBER"))
            print("Net Config Num:" + str(tcpipDhcpsNetConfigNum))
            tcpipNetDependentSymbol.getComponent().getSymbolByID("TCPIP_DHCP_SERVER_INSTANCES_NUMBER").setMax(tcpipDhcpsNetConfigNum)
        else:           
            print(tcpipNetDependentSymbol.getID())
            tcpipDhcpsInstanceNumberValue = event["value"]
            print("Max No:" + str(tcpipNetDependentSymbol.getComponent().getSymbolByID("TCPIP_DHCP_SERVER_INSTANCES_NUMBER").getMax()))
            print(tcpipDhcpsInstanceNumberValue)
            print(tcpipDhcpsInstancesNumPrev)
            if(tcpipDhcpsInstanceNumberValue > tcpipDhcpsInstancesNumPrev ):
                tcpipDhcpsInstance[tcpipDhcpsInstancesNumPrev].setVisible(True)
                tcpipDhcpsInstance[tcpipDhcpsInstancesNumPrev].setValue(True, 1)
                print("Set TRUE"+ str(tcpipDhcpsInstancesNumPrev))
                tcpipDhcpsInstancesNumPrev = tcpipDhcpsInstancesNumPrev + 1
                #Add more network configurations
            else:
                if(tcpipDhcpsInstanceNumberValue < tcpipDhcpsInstancesNumPrev ):
                    #Reduce network configurations
                    tcpipDhcpsInstancesNumPrev = tcpipDhcpsInstancesNumPrev - 1
                    tcpipDhcpsInstance[tcpipDhcpsInstancesNumPrev].setVisible(False)
                    tcpipDhcpsInstance[tcpipDhcpsInstancesNumPrev].setValue(False, 1)
                    print("Set FALSE"+ str(tcpipDhcpsInstancesNumPrev))
                    
                #else:
                    #Do Nothing
    print("END tcpipDhcpsEnableInstance")
    
def tcpipDhcpsInstnNumVisible(symbol, event):   
    if(event["id"] == "TCPIP_STACK_NETWORK_CONFIG_NUMBER" ):
        eventValue = event["value"]
        symbol.setValue(int(eventValue),1)
    else:
        if (event["value"] == True):
            tcpipDhcpsNetConfigNum = int(Database.getSymbolValue("tcpipNetConfig","TCPIP_STACK_NETWORK_CONFIG_NUMBER"))
            symbol.getComponent().getSymbolByID("TCPIP_DHCP_SERVER_INSTANCES_NUMBER").setMax(tcpipDhcpsNetConfigNum)        
            symbol.setVisible(True)
        else:       
            symbol.setVisible(False)

##############################################################################################################

# make DHCP Server option visible
def tcpipDhcpsMenuVisible(tcpipDependentSymbol, tcpipIPSymbol):
    tcpipIPv4 = Database.getSymbolValue("tcpipIPv4","TCPIP_STACK_USE_IPV4")
    tcpipUdp = Database.getSymbolValue("tcpipUdp","TCPIP_USE_UDP")

    if(tcpipIPv4 and tcpipUdp):
        tcpipDependentSymbol.setVisible(True)
    else:
        tcpipDependentSymbol.setVisible(False)

def tcpipDhcpsICMPClientEnable():
    tcpipIcmpComp = Database.getComponentByID("tcpipIcmp")        
    if(tcpipIcmpComp != None):
        tcpipICMPClientSymbol = tcpipIcmpComp.getSymbolByID("TCPIP_STACK_USE_ICMP_CLIENT")
        tcpipICMPClientSymbol.setValue(True)
    
    
  
def tcpipDhcpsMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("DHCP Server Menu Visible.")      
        symbol.setVisible(True)
    else:
        print("DHCP Server Menu Invisible.")
        symbol.setVisible(False)    

def tcpipDhcpsInstnAddrRangeMenu(symbol, event):
    print("DHCP Server Instance Start.")
    tcpipDhcpsInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_IP_ADDRESS_RANGE_START_IDX"))
    print(tcpipDhcpsInstnIndex)
    tcpipDhcpsInstnEnable = tcpipDhcpsInstance[tcpipDhcpsInstnIndex].getValue()
    tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
    print(tcpipDhcpsEnable)
    
    if(tcpipDhcpsInstnEnable and tcpipDhcpsEnable):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def tcpipDhcpsInstnIpAddrMenu(symbol, event):
    print("DHCP Server Instance Start.")
    tcpipDhcpsInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_SERVER_IP_ADDRESS_IDX"))
    print(tcpipDhcpsInstnIndex)
    tcpipDhcpsInstnEnable = tcpipDhcpsInstance[tcpipDhcpsInstnIndex].getValue()
    tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
    print(tcpipDhcpsEnable)
    
    if(tcpipDhcpsInstnEnable and tcpipDhcpsEnable):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def tcpipDhcpsInstnNetMaskAddrMenu(symbol, event):
    print("DHCP Server Instance Start.")
    tcpipDhcpsInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_SERVER_NETMASK_ADDRESS_IDX"))
    print(tcpipDhcpsInstnIndex)
    tcpipDhcpsInstnEnable = tcpipDhcpsInstance[tcpipDhcpsInstnIndex].getValue()
    tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
    print(tcpipDhcpsEnable)

    if(tcpipDhcpsInstnEnable and tcpipDhcpsEnable):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)
    
def tcpipDhcpsInstnGatewayAddrMenu(symbol, event):
    print("DHCP Server Instance Start.")
    tcpipDhcpsInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_SERVER_GATEWAY_ADDRESS_IDX"))
    print(tcpipDhcpsInstnIndex)
    tcpipDhcpsInstnEnable = tcpipDhcpsInstance[tcpipDhcpsInstnIndex].getValue()
    tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
    print(tcpipDhcpsEnable)

    if(tcpipDhcpsInstnEnable and tcpipDhcpsEnable):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)    

def tcpipDhcpsInstnPrimDnsAddrMenu(symbol, event):
    print("DHCP Server Instance Start.")
    tcpipDhcpsInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_SERVER_PRIMARY_DNS_ADDRESS_IDX"))
    print(tcpipDhcpsInstnIndex)
    tcpipDhcpsInstnEnable = tcpipDhcpsInstance[tcpipDhcpsInstnIndex].getValue()
    tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
    print(tcpipDhcpsEnable)

    if(tcpipDhcpsInstnEnable and tcpipDhcpsEnable):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)    

def tcpipDhcpsInstnSecDnsAddrMenu(symbol, event):
    print("DHCP Server Instance Start.")
    tcpipDhcpsInstnIndex = int(symbol.getID().strip("TCPIP_DHCPS_DEFAULT_SERVER_SECONDARY_DNS_ADDRESS_IDX"))
    print(tcpipDhcpsInstnIndex)
    tcpipDhcpsInstnEnable = tcpipDhcpsInstance[tcpipDhcpsInstnIndex].getValue()
    tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
    print(tcpipDhcpsEnable)

    if(tcpipDhcpsInstnEnable and tcpipDhcpsEnable):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)    

def tcpipDhcpsInstnIfIdxMenu(symbol, event):
    print("DHCP Server Instance Start.")
    tcpipDhcpsInstnIndex = int(symbol.getID().strip("TCPIP_DHCP_SERVER_INTERFACE_INDEX_IDX"))
    print(tcpipDhcpsInstnIndex)
    tcpipDhcpsInstnEnable = tcpipDhcpsInstance[tcpipDhcpsInstnIndex].getValue()
    tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
    print(tcpipDhcpsEnable)

    if(tcpipDhcpsInstnEnable and tcpipDhcpsEnable):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)    

def tcpipDhcpsInstnPoolEnMenu(symbol, event):
    print("DHCP Server Instance Start.")
    tcpipDhcpsInstnIndex = int(symbol.getID().strip("TCPIP_DHCP_SERVER_POOL_ENABLED_IDX"))
    print(tcpipDhcpsInstnIndex)
    tcpipDhcpsInstnEnable = tcpipDhcpsInstance[tcpipDhcpsInstnIndex].getValue()
    tcpipDhcpsEnable = Database.getSymbolValue("tcpipDhcps","TCPIP_STACK_USE_DHCP_SERVER")
    print(tcpipDhcpsEnable)

    if(tcpipDhcpsInstnEnable and tcpipDhcpsEnable):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)        
def tcpipDhcpsGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

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
    Database.setSymbolValue("tcpipDhcps", "TCPIP_STACK_USE_DHCP_SERVER", False, 2)
