"""
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
interfaceNum = []
def instantiateComponent(drvPPPComponent):
    
    processor =  Variables.get("__PROCESSOR")  

    print("PPP MAC Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    

    # Use PPP MAC Driver? 
    drvPPPMac = drvPPPComponent.createBooleanSymbol("TCPIP_STACK_USE_PPP_INTERFACE", None)
    drvPPPMac.setHelp("mcc_h3_ppp_configurations")
    drvPPPMac.setLabel("Use PPP MAC Driver?")
    drvPPPMac.setVisible(False)
    drvPPPMac.setDescription("Use Serial PPP MAC Driver?")
    drvPPPMac.setDefaultValue(True) 

    tcpipPPPTraffic = drvPPPComponent.createComboSymbol("TCPIP_PPP_TRAFFIC",None,["Low", "Medium", "High"])
    tcpipPPPTraffic.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPTraffic.setVisible(True)
    tcpipPPPTraffic.setLabel("PPP Default Configuration for Network Traffic")
    tcpipPPPTraffic.setDefaultValue("Medium")
    
    # Number of Dedicated MAC RX Buffers
    tcpipPPPRxOwnBuffer = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_RX_DEDICATED_BUFFERS", tcpipPPPTraffic)
    tcpipPPPRxOwnBuffer.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPRxOwnBuffer.setLabel("Number of Dedicated PPP RX Buffers")
    tcpipPPPRxOwnBuffer.setVisible(True)
    tcpipPPPRxOwnBuffer.setDescription("Number of Dedicated PPP MAC RX Buffers")
    tcpipPPPRxOwnBuffer.setDefaultValue(4)
    tcpipPPPRxOwnBuffer.setDependencies(tcpipPPPRxOwnBufferCallBack, ["TCPIP_PPP_TRAFFIC"])

    # Number of Non_Dedicated MAC RX Buffers for MAC Initialization
    tcpipPPPRxDynBuffer = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_RX_INIT_BUFFERS", tcpipPPPTraffic)
    tcpipPPPRxDynBuffer.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPRxDynBuffer.setLabel("Number of Non_Dedicated PPP RX Buffers")
    tcpipPPPRxDynBuffer.setVisible(True)
    tcpipPPPRxDynBuffer.setDescription("Number of Non_Dedicated PPP MAC RX Buffers for MAC Initialization")
    tcpipPPPRxDynBuffer.setDefaultValue(0)
    tcpipPPPRxDynBuffer.setDependencies(tcpipPPPRxDynBufferCallBack, ["TCPIP_PPP_TRAFFIC"])

    # Minimum Threshold for the Buffer Replenish Process
    tcpipPPPRxBufferThres = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_RX_LOW_THRESHOLD", tcpipPPPTraffic)
    tcpipPPPRxBufferThres.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPRxBufferThres.setLabel("Minimum Threshold for the Buffer Replenish Process")
    tcpipPPPRxBufferThres.setVisible(True)
    tcpipPPPRxBufferThres.setDescription("Minimum Threshold for the Buffer Replenish Process")
    tcpipPPPRxBufferThres.setDefaultValue(1)
    tcpipPPPRxBufferThres.setDependencies(tcpipPPPRxBufferThresCallBack, ["TCPIP_PPP_TRAFFIC"])

    # Number of RX Buffers to Allocate when Below Threshold Condition is Detected
    tcpipPPPRxBufferReFill = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_RX_LOW_FILL", tcpipPPPTraffic)
    tcpipPPPRxBufferReFill.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPRxBufferReFill.setLabel("Rx Buffer Allocate Count during  Replenish Process")
    tcpipPPPRxBufferReFill.setVisible(True)
    tcpipPPPRxBufferReFill.setDescription("Number of RX Buffers to Allocate when Below Threshold Condition is Detected")
    tcpipPPPRxBufferReFill.setDefaultValue(2)
    tcpipPPPRxBufferReFill.setDependencies(tcpipPPPRxBufferReFillCallBack, ["TCPIP_PPP_TRAFFIC"])

    # PPP Config Flags
    tcpipPPPConfFlag = drvPPPComponent.createMenuSymbol(None, None) 
    tcpipPPPConfFlag.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPConfFlag.setLabel("PPP Configuration Flags")
    tcpipPPPConfFlag.setVisible(True)
    tcpipPPPConfFlag.setDescription("PPP Configuration Flags")
    
    # Start in NO_OPEN state
    tcpipPPPConfFlagNoOpen = drvPPPComponent.createBooleanSymbol("TCPIP_PPP_CONF_FLAG_NO_OPEN", tcpipPPPConfFlag)
    tcpipPPPConfFlagNoOpen.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPConfFlagNoOpen.setLabel("No Admin Open at Start Up")
    tcpipPPPConfFlagNoOpen.setVisible(True) 
    tcpipPPPConfFlagNoOpen.setDescription("Do NOT Issue an Admin Open at Start Up")
    tcpipPPPConfFlagNoOpen.setDefaultValue(False)
    
    # Start Passive
    tcpipPPPConfFlagPassive = drvPPPComponent.createBooleanSymbol("TCPIP_PPP_CONF_FLAG_PASSIVE", tcpipPPPConfFlag)
    tcpipPPPConfFlagPassive.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPConfFlagPassive.setLabel("Start Passive")
    tcpipPPPConfFlagPassive.setVisible(True)
    tcpipPPPConfFlagPassive.setDescription("Do Not Initiate a Connection: Start Passive")
    tcpipPPPConfFlagPassive.setDefaultValue(False)
    
    # Always Passive
    tcpipPPPConfFlagAllPassive = drvPPPComponent.createBooleanSymbol("TCPIP_PPP_CONF_FLAG_ALWAYS_PASSIVE", tcpipPPPConfFlag)
    tcpipPPPConfFlagAllPassive.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPConfFlagAllPassive.setLabel("Always Passive")
    tcpipPPPConfFlagAllPassive.setVisible(True)
    tcpipPPPConfFlagAllPassive.setDescription("Do Not Ever Initiate a Connection: Always Passive")
    tcpipPPPConfFlagAllPassive.setDefaultValue(False)
    
    # PPP LCP Config Flags
    tcpipLCPConfFlag = drvPPPComponent.createMenuSymbol(None, None) 
    tcpipLCPConfFlag.setHelp("mcc_h3_ppp_configurations")
    tcpipLCPConfFlag.setLabel("PPP LCP Configuration Flags")
    tcpipLCPConfFlag.setVisible(True)
    tcpipLCPConfFlag.setDescription("PPP LCP Configuration Flags")

    # Negotiate MRU
    tcpipLCPConfFlagMRU = drvPPPComponent.createBooleanSymbol("TCPIP_LCP_CONF_FLAG_MRU", tcpipLCPConfFlag)
    tcpipLCPConfFlagMRU.setHelp("mcc_h3_ppp_configurations")
    tcpipLCPConfFlagMRU.setLabel("Negotiate MRU")
    tcpipLCPConfFlagMRU.setVisible(True) 
    tcpipLCPConfFlagMRU.setDescription("Negotiate MRU with the PEER")
    tcpipLCPConfFlagMRU.setDefaultValue(True)

    # Negotiate Magic-Number
    tcpipLCPConfFlagMagic = drvPPPComponent.createBooleanSymbol("TCPIP_LCP_CONF_FLAG_MAGIC", tcpipLCPConfFlag)
    tcpipLCPConfFlagMagic.setHelp("mcc_h3_ppp_configurations")
    tcpipLCPConfFlagMagic.setLabel("Negotiate Magic-Number")
    tcpipLCPConfFlagMagic.setVisible(True) 
    tcpipLCPConfFlagMagic.setDescription("Negotiate Magic-Number with the PEER")
    tcpipLCPConfFlagMagic.setDefaultValue(True)

    # Negotiate ACCM
    tcpipLCPConfFlagAccm = drvPPPComponent.createBooleanSymbol("TCPIP_LCP_CONF_FLAG_ACCM", tcpipLCPConfFlag)
    tcpipLCPConfFlagAccm.setHelp("mcc_h3_ppp_configurations")
    tcpipLCPConfFlagAccm.setLabel("Negotiate ACCM")
    tcpipLCPConfFlagAccm.setVisible(True) 
    tcpipLCPConfFlagAccm.setDescription("Negotiate ACCM with the PEER")
    tcpipLCPConfFlagAccm.setDefaultValue(True)

    # PPP IPCP Config Flags
    tcpipIPCPConfFlag = drvPPPComponent.createMenuSymbol(None, None) 
    tcpipIPCPConfFlag.setHelp("mcc_h3_ppp_configurations")
    tcpipIPCPConfFlag.setLabel("PPP IPCP Configuration Flags")
    tcpipIPCPConfFlag.setVisible(True)
    tcpipIPCPConfFlag.setDescription("PPP IPCP Configuration Flags")

    # No IPv4 negotiation
    tcpipIPCPConfFlagNoIp4 = drvPPPComponent.createBooleanSymbol("TCPIP_IPCP_CONF_FLAG_NO_ADDRESS", tcpipIPCPConfFlag)
    tcpipIPCPConfFlagNoIp4.setHelp("mcc_h3_ppp_configurations")
    tcpipIPCPConfFlagNoIp4.setLabel("Don't Negotiate IP4 Address")
    tcpipIPCPConfFlagNoIp4.setVisible(True) 
    tcpipIPCPConfFlagNoIp4.setDescription("Don't Negotiate IP4 Address with the PEER")
    tcpipIPCPConfFlagNoIp4.setDefaultValue(False)

    # Local IPv4 address
    tcpipPPPLclIpAddr = drvPPPComponent.createStringSymbol("TCPIP_PPP_LOCAL_IPV4_ADDRESS", None)
    tcpipPPPLclIpAddr.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPLclIpAddr.setLabel("PPP Local IP Address")
    tcpipPPPLclIpAddr.setDescription("PPP Interface Local IP Address")
    tcpipPPPLclIpAddr.setVisible(True)
    tcpipPPPLclIpAddr.setDefaultValue("192.168.1.100")
    # tcpipPPPLclIpAddr.setDependencies - tcpipIPCPConfFlagNoIp4 == False

    # Peer IPv4 address
    tcpipPPPPeerIpAddr = drvPPPComponent.createStringSymbol("TCPIP_PPP_PEER_IPV4_ADDRESS", None)
    tcpipPPPPeerIpAddr.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPPeerIpAddr.setLabel("PPP Peer IP Address")
    tcpipPPPPeerIpAddr.setDescription("PPP Interface Peer IP Address")
    tcpipPPPPeerIpAddr.setVisible(True)
    tcpipPPPPeerIpAddr.setDefaultValue("192.168.1.101")
    # tcpipPPPPeerIpAddr.setDependencies - tcpipIPCPConfFlagNoIp4 == False

    # RX ACCM
    tcpipPPPRxAccm = drvPPPComponent.createHexSymbol("TCPIP_PPP_RX_ACCM", None)
    tcpipPPPRxAccm.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPRxAccm.setLabel("PPP RX Side ACCM")
    tcpipPPPRxAccm.setDescription("PPP RX Side ACCM for the PEER")
    tcpipPPPRxAccm.setVisible(True)
    tcpipPPPRxAccm.setDefaultValue(0x0)
    tcpipPPPRxAccm.setMin(0x0)
    tcpipPPPRxAccm.setMax(0xffffffff)

    # Advanced Settings
    tcpipPPPAdvSettings = drvPPPComponent.createMenuSymbol("TCPIP_PPP_ADV_SETTING", None)
    tcpipPPPAdvSettings.setLabel("Advanced Settings")
    tcpipPPPAdvSettings.setDescription("Advanced Settings")
    tcpipPPPAdvSettings.setVisible(True)
    
    # Restart Timeout
    tcpipPPPRestTmo = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_RESTART_TMO", tcpipPPPAdvSettings)
    tcpipPPPRestTmo.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPRestTmo.setLabel("Restart Timeout")
    tcpipPPPRestTmo.setVisible(True)
    tcpipPPPRestTmo.setDescription("Restart Timeout for Configure-Request and Terminate-Request. Seconds")
    tcpipPPPRestTmo.setDefaultValue(0)

    # Max-Terminate
    tcpipPPPMaxTerm = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_MAX_TERM", tcpipPPPAdvSettings)
    tcpipPPPMaxTerm.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPMaxTerm.setLabel("Max-Terminate Counter")
    tcpipPPPMaxTerm.setVisible(True)
    tcpipPPPMaxTerm.setDescription("Max-Terminate counter")
    tcpipPPPMaxTerm.setDefaultValue(0)

    # Max-Configure
    tcpipPPPMaxCfg = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_MAX_CONFIGURE", tcpipPPPAdvSettings)
    tcpipPPPMaxCfg.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPMaxCfg.setLabel("Max-Configure Counter")
    tcpipPPPMaxCfg.setVisible(True)
    tcpipPPPMaxCfg.setDescription("Max-Configure counter")
    tcpipPPPMaxCfg.setDefaultValue(0)

    # Max-Failure
    tcpipPPPMaxFail = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_MAX_FAILURE", tcpipPPPAdvSettings)
    tcpipPPPMaxFail.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPMaxFail.setLabel("Max-Failure Counter")
    tcpipPPPMaxFail.setVisible(True)
    tcpipPPPMaxFail.setDescription("Max-Failure counter")
    tcpipPPPMaxFail.setDefaultValue(0)

    # Maximum receive unit
    tcpipPPPMru = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_MAX_RECEIVE_UNIT", tcpipPPPAdvSettings)
    tcpipPPPMru.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPMru.setLabel("Maximum Receive Unit")
    tcpipPPPMru.setVisible(True)
    tcpipPPPMru.setDescription("Maximum Receive Unit: Size of a RX Buffer. Should Be >= 1500")
    tcpipPPPMru.setDefaultValue(0)

    # Echo Timeout
    tcpipPPPEchoTmo = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_ECHO_TMO", tcpipPPPAdvSettings)
    tcpipPPPEchoTmo.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPEchoTmo.setLabel("Echo Request Timeout")
    tcpipPPPEchoTmo.setVisible(True)
    tcpipPPPEchoTmo.setDescription("Echo Request Timeout: milliseconds")
    tcpipPPPEchoTmo.setDefaultValue(0)

    # Use Magic Number Function
    drvPPPMagicCallbackEnable = drvPPPComponent.createBooleanSymbol("DRV_PPP_USE_MAGIC_CALLBACK", tcpipPPPAdvSettings)
    drvPPPMagicCallbackEnable.setHelp("mcc_h3_ppp_configurations")
    drvPPPMagicCallbackEnable.setLabel("Use Magic Number Function")
    drvPPPMagicCallbackEnable.setVisible(True)
    drvPPPMagicCallbackEnable.setDescription("Use a Function to generate a Magic Number")
    drvPPPMagicCallbackEnable.setDefaultValue(False)
    
    # Magic Number Function
    drvPPPMagicCallback = drvPPPComponent.createStringSymbol("DRV_PPP_MAGIC_CALLBACK", drvPPPMagicCallbackEnable)
    drvPPPMagicCallback.setHelp("mcc_h3_ppp_configurations")
    drvPPPMagicCallback.setLabel("Magic-Number Function")
    drvPPPMagicCallback.setVisible(False)
    drvPPPMagicCallback.setDescription("Magic-Number Function")
    drvPPPMagicCallback.setDefaultValue("UserMagicF")
    drvPPPMagicCallback.setDependencies(drvPPPMenuVisibleSingle, ["DRV_PPP_USE_MAGIC_CALLBACK"])

    # HDLC object to use
    tcpipPPPHdlcObj = drvPPPComponent.createStringSymbol("DRV_PPP_HDLC_DRIVER_OBJECT", tcpipPPPAdvSettings)
    tcpipPPPHdlcObj.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPHdlcObj.setLabel("PPP HDLC Driver Object")
    tcpipPPPHdlcObj.setVisible(True)
    tcpipPPPHdlcObj.setDescription("PPP HDLC Driver Object to Use")
    tcpipPPPHdlcObj.setDefaultValue("DRV_HDLC_AsyncObject") 
    
    # HDLC processing buffer size
    tcpipHDLCProcSize = drvPPPComponent.createIntegerSymbol("TCPIP_HDLC_PROC_BUFFER_SIZE", tcpipPPPAdvSettings)
    tcpipHDLCProcSize.setHelp("mcc_h3_ppp_configurations")
    tcpipHDLCProcSize.setLabel("HDLC Processing Buffer Size")
    tcpipHDLCProcSize.setVisible(True)
    tcpipHDLCProcSize.setDescription("HDLC Processing Buffer Size")
    tcpipHDLCProcSize.setDefaultValue(0)

    # HDLC peek buffer size
    tcpipHDLCPeekSize = drvPPPComponent.createIntegerSymbol("TCPIP_HDLC_PEEK_BUFFER_SIZE", tcpipPPPAdvSettings)
    tcpipHDLCPeekSize.setHelp("mcc_h3_ppp_configurations")
    tcpipHDLCPeekSize.setLabel("HDLC Peek Buffer Size")
    tcpipHDLCPeekSize.setVisible(True)
    tcpipHDLCPeekSize.setDescription("HDLC Peek Buffer Size")
    tcpipHDLCPeekSize.setDefaultValue(0)

    # HDLC object index
    tcpipHDLCObjIx = drvPPPComponent.createIntegerSymbol("TCPIP_HDLC_OBJECT_INDEX", tcpipPPPAdvSettings)
    tcpipHDLCObjIx.setHelp("mcc_h3_ppp_configurations")
    tcpipHDLCObjIx.setLabel("HDLC Object Index")
    tcpipHDLCObjIx.setVisible(True)
    tcpipHDLCObjIx.setDescription("HDLC Object Index to Use")
    tcpipHDLCObjIx.setDefaultValue(0)

    # HDLC statistics
    tcpipHDLCEnableStat = drvPPPComponent.createBooleanSymbol("TCPIP_PPP_HDLC_ENABLE_STATISTICS", tcpipPPPAdvSettings)
    tcpipHDLCEnableStat.setHelp("mcc_h3_ppp_configurations")
    tcpipHDLCEnableStat.setLabel("Enable HDLC Statistics")
    tcpipHDLCEnableStat.setVisible(True) 
    tcpipHDLCEnableStat.setDescription("Enable HDLC Statistics")
    tcpipHDLCEnableStat.setDefaultValue(False)

    # PPP statistics
    tcpipPPPEnableStat = drvPPPComponent.createBooleanSymbol("TCPIP_PPP_ENABLE_STATISTICS", tcpipPPPAdvSettings)
    tcpipPPPEnableStat.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPEnableStat.setLabel("Enable PPP Statistics")
    tcpipPPPEnableStat.setVisible(True) 
    tcpipPPPEnableStat.setDescription("Enable PPP Statistics")
    tcpipPPPEnableStat.setDefaultValue(False)

    # PPP statistics registrations
    tcpipPPPStatRegistrations = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_MAX_REGISTRATIONS", tcpipPPPAdvSettings)
    tcpipPPPStatRegistrations.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPStatRegistrations.setLabel("Maximum PPP Statistics Registrations")
    tcpipPPPStatRegistrations.setVisible(True) 
    tcpipPPPStatRegistrations.setDescription("Maximum PPP Statistics Registrations")
    tcpipPPPStatRegistrations.setDefaultValue(1)

    # PPP/LCP Enable Echo requests
    tcpipPPPEnableEcho = drvPPPComponent.createBooleanSymbol("TCPIP_PPP_ECHO_ENABLE", tcpipPPPAdvSettings)
    tcpipPPPEnableEcho.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPEnableEcho.setLabel("Enable PPP Echo")
    tcpipPPPEnableEcho.setVisible(True) 
    tcpipPPPEnableEcho.setDescription("Enable PPP Echo")
    tcpipPPPEnableEcho.setDefaultValue(True)

    # PPP/LCP Max Simultaneous Echo requests
    tcpipPPPEchoRequests = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_MAX_ECHO_REQUESTS", tcpipPPPAdvSettings)
    tcpipPPPEchoRequests.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPEchoRequests.setLabel("Max Number of Simultaneous PPP Requests")
    tcpipPPPEchoRequests.setVisible(True) 
    tcpipPPPEchoRequests.setDescription("Max Number of Simultaneous PPP Requests")
    tcpipPPPEchoRequests.setDefaultValue(2)

    # PPP Notifications Enable
    tcpipPPPEnableNotification = drvPPPComponent.createBooleanSymbol("TCPIP_PPP_NOTIFICATION_ENABLE", tcpipPPPAdvSettings)
    tcpipPPPEnableNotification.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPEnableNotification.setLabel("Enable PPP Notifications")
    tcpipPPPEnableNotification.setVisible(True) 
    tcpipPPPEnableNotification.setDescription("Enable PPP Notifications")
    tcpipPPPEnableNotification.setDefaultValue(False)

    # PPP Max Event Registrations
    tcpipPPPEventRegistrations = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_MAX_EVENT_REGISTRATIONS", tcpipPPPAdvSettings)
    tcpipPPPEventRegistrations.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPEventRegistrations.setLabel("Max Number of PPP Event Registrations")
    tcpipPPPEventRegistrations.setVisible(True) 
    tcpipPPPEventRegistrations.setDescription("Max Number of PPP Event Registrations")
    tcpipPPPEventRegistrations.setDefaultValue(1)

    # HDLC Commands
    tcpipPPPHdlcCommands = drvPPPComponent.createBooleanSymbol("TCPIP_PPP_ENABLE_HDLC_COMMANDS", tcpipPPPAdvSettings)
    tcpipPPPHdlcCommands.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPHdlcCommands.setLabel("Enable HDLC Commands")
    tcpipPPPHdlcCommands.setVisible(True) 
    tcpipPPPHdlcCommands.setDescription("Enable HDLC Commands")
    tcpipPPPHdlcCommands.setDefaultValue(False)

    # PPP Commands
    tcpipPPPCommands = drvPPPComponent.createBooleanSymbol("TCPIP_PPP_ENABLE_COMMANDS", tcpipPPPAdvSettings)
    tcpipPPPCommands.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPCommands.setLabel("Enable PPP Commands")
    tcpipPPPCommands.setVisible(True) 
    tcpipPPPCommands.setDescription("Enable PPP Commands")
    tcpipPPPCommands.setDefaultValue(False)

    # PPP Echo Command Timeout
    tcpipEchoCommandTmo = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_ECHO_CMD_TIMEOUT", tcpipPPPAdvSettings)
    tcpipEchoCommandTmo.setHelp("mcc_h3_ppp_configurations")
    tcpipEchoCommandTmo.setLabel("PPP Echo Command Timeout")
    tcpipEchoCommandTmo.setVisible(True) 
    tcpipEchoCommandTmo.setDescription("PPP Echo Command Timeout")
    tcpipEchoCommandTmo.setDefaultValue(100)
    # tcpipEchoCommandTmo.setDependencies(TCPIP_PPP_ENABLE_COMMANDS)

    # PPP Echo Command Data Size
    tcpipEchoCommandSize = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_ECHO_CMD_SIZE", tcpipPPPAdvSettings)
    tcpipEchoCommandSize.setHelp("mcc_h3_ppp_configurations")
    tcpipEchoCommandSize.setLabel("PPP Echo Command Data Size")
    tcpipEchoCommandSize.setVisible(True) 
    tcpipEchoCommandSize.setDescription("PPP Echo Command Data Size")
    tcpipEchoCommandSize.setDefaultValue(20)
    # tcpipEchoCommandSize.setDependencies(TCPIP_PPP_ENABLE_COMMANDS)

    # PPP Echo Command Number of Requests
    tcpipEchoCommandRequests = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_ECHO_CMD_REQUESTS", tcpipPPPAdvSettings)
    tcpipEchoCommandRequests.setHelp("mcc_h3_ppp_configurations")
    tcpipEchoCommandRequests.setLabel("PPP Echo Command Requests")
    tcpipEchoCommandRequests.setVisible(True) 
    tcpipEchoCommandRequests.setDescription("PPP Echo Command Number of Requests")
    tcpipEchoCommandRequests.setDefaultValue(4)
    # tcpipEchoCommandRequests.setDependencies(TCPIP_PPP_ENABLE_COMMANDS)

    # PPP Echo Command Inter Request Delay
    tcpipEchoCommandRequestDelay = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_ECHO_CMD_REQUEST_DELAY", tcpipPPPAdvSettings)
    tcpipEchoCommandRequestDelay.setHelp("mcc_h3_ppp_configurations")
    tcpipEchoCommandRequestDelay.setLabel("PPP Echo Command Request Delay")
    tcpipEchoCommandRequestDelay.setVisible(True) 
    tcpipEchoCommandRequestDelay.setDescription("PPP Echo Command Inter Request Delay")
    tcpipEchoCommandRequestDelay.setDefaultValue(100)
    # tcpipEchoCommandRequestDelay.setDependencies(TCPIP_PPP_ENABLE_COMMANDS)

    # PPP Max Packet Segments
    tcpipPPPMaxSegments = drvPPPComponent.createIntegerSymbol("TCPIP_PPP_MAX_SEGMENTS", tcpipPPPAdvSettings)
    tcpipPPPMaxSegments.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPMaxSegments.setLabel("PPP Maximum Number of Segments in a Packet")
    tcpipPPPMaxSegments.setVisible(True) 
    tcpipPPPMaxSegments.setDescription("Maximum Number of Segments in a PPP Packet")
    tcpipPPPMaxSegments.setDefaultValue(4)

    # iperf PPP interface TX Queue Limit
    tcpipIperfPPPQueueLimit = drvPPPComponent.createIntegerSymbol("TCPIP_IPERF_PPP_TX_QUEUE_LIMIT", tcpipPPPAdvSettings)
    tcpipIperfPPPQueueLimit.setHelp("mcc_h3_ppp_configurations")
    tcpipIperfPPPQueueLimit.setLabel("PPP Interface Iperf Queue Limit")
    tcpipIperfPPPQueueLimit.setVisible(True) 
    tcpipIperfPPPQueueLimit.setDescription("PPP Interface Iperf TX Queue Limit")
    tcpipIperfPPPQueueLimit.setDefaultValue(5)
    # tcpipIperfPPPQueueLimit.setDependencies(iperf is enabled)

    drvPPPUartType = drvPPPComponent.createStringSymbol("DRV_PPP_UART_TYPE", tcpipPPPAdvSettings)
    drvPPPUartType.setHelp("mcc_h3_ppp_configurations")
    drvPPPUartType.setLabel("External UART Connected to PPP MAC")
    drvPPPUartType.setVisible(True)
    drvPPPUartType.setDescription("UART Connected to PPP")
    drvPPPUartType.setDefaultValue("Not Connected")
    drvPPPUartType.setReadOnly(True)

    tcpipPPPMacHeapDependency = [ "TCPIP_PPP_MAX_RECEIVE_UNIT", "TCPIP_PPP_RX_DEDICATED_BUFFERS", 
                                 "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK" ]    
        
    # PPP MAC Heap Size
    tcpipPPPMacHeapSize = drvPPPComponent.createIntegerSymbol("DRV_PPP_MAC_HEAP_SIZE", None)
    tcpipPPPMacHeapSize.setHelp("mcc_h3_ppp_configurations")
    tcpipPPPMacHeapSize.setLabel("PPP MAC Heap Size (bytes)")     
    tcpipPPPMacHeapSize.setVisible(False)
    tcpipPPPMacHeapSize.setDefaultValue(tcpipPPPMacHeapCalc())
    tcpipPPPMacHeapSize.setReadOnly(True)
    tcpipPPPMacHeapSize.setDependencies(tcpipPPPMacHeapUpdate, tcpipPPPMacHeapDependency)        
        

    # Add to definitions.h
    tcpipPPPSystemDefFile = drvPPPComponent.createFileSymbol("DRV_PPP_H_FILE", None)
    tcpipPPPSystemDefFile.setType("STRING")
    tcpipPPPSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    tcpipPPPSystemDefFile.setSourcePath("driver/ppp/templates/system/system_definitions.h.ftl")
    tcpipPPPSystemDefFile.setMarkup(True)  
    
    # Add forward declaration to initialization.c
    tcpipPPPInitDataSourceFtl = drvPPPComponent.createFileSymbol(None, None)
    tcpipPPPInitDataSourceFtl.setType("STRING")
    tcpipPPPInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    tcpipPPPInitDataSourceFtl.setSourcePath("driver/ppp/templates/system/system_driver_initialize.c.ftl")
    tcpipPPPInitDataSourceFtl.setMarkup(True)
    
    # Add to initialization.c
    tcpipPPPSysInitDataSourceFtl = drvPPPComponent.createFileSymbol(None, None)
    tcpipPPPSysInitDataSourceFtl.setType("STRING")
    tcpipPPPSysInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_LIBRARY_INITIALIZATION_DATA")
    tcpipPPPSysInitDataSourceFtl.setSourcePath("driver/ppp/templates/system/system_data_initialize.c.ftl")
    tcpipPPPSysInitDataSourceFtl.setMarkup(True)
    
    # Add to system_config.h
    tcpipPPPHeaderFtl = drvPPPComponent.createFileSymbol(None, None)
    tcpipPPPHeaderFtl.setSourcePath("driver/ppp/config/drv_ppp.h.ftl")
    tcpipPPPHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipPPPHeaderFtl.setMarkup(True)
    tcpipPPPHeaderFtl.setType("STRING")
    
    # file "$HARMONY_VERSION_PATH/framework/driver/ppp/drv_ppp.h" to        "$PROJECT_HEADER_FILES/framework/driver/ppp/drv_ppp.h"
    # Add drv_ppp.h file to project
    tcpipPPPHeaderFile = drvPPPComponent.createFileSymbol(None, None)
    tcpipPPPHeaderFile.setSourcePath("driver/ppp/drv_ppp.h")
    tcpipPPPHeaderFile.setOutputName("drv_ppp.h")
    tcpipPPPHeaderFile.setDestPath("driver/ppp/")
    tcpipPPPHeaderFile.setProjectPath("config/" + configName + "/driver/ppp/")
    tcpipPPPHeaderFile.setType("HEADER")
    tcpipPPPHeaderFile.setOverwrite(True)
    
    # file "$HARMONY_VERSION_PATH/framework/driver/ppp/drv_hdlc_obj.h" to                     "$PROJECT_HEADER_FILES/framework/driver/ppp/drv_hdlc_obj.h"
    # Add drv_hdlc_obj.h file to project
    tcpipHDLCHeaderFile = drvPPPComponent.createFileSymbol(None, None)
    tcpipHDLCHeaderFile.setSourcePath("driver/ppp/drv_hdlc_obj.h")
    tcpipHDLCHeaderFile.setOutputName("drv_hdlc_obj.h")
    tcpipHDLCHeaderFile.setDestPath("driver/ppp/")
    tcpipHDLCHeaderFile.setProjectPath("config/" + configName + "/driver/ppp/")
    tcpipHDLCHeaderFile.setType("HEADER")
    tcpipHDLCHeaderFile.setOverwrite(True)
    
    # file "$HARMONY_VERSION_PATH/framework/driver/ppp/drv_ppp_mac.h" to           "$PROJECT_HEADER_FILES/framework/driver/ppp/drv_ppp_mac.h"
    # Add drv_ppp_mac.h file to project
    tcpipPPPMacHeaderFile = drvPPPComponent.createFileSymbol(None, None)
    tcpipPPPMacHeaderFile.setSourcePath("driver/ppp/drv_ppp_mac.h")
    tcpipPPPMacHeaderFile.setOutputName("drv_ppp_mac.h")
    tcpipPPPMacHeaderFile.setDestPath("driver/ppp/")
    tcpipPPPMacHeaderFile.setProjectPath("config/" + configName + "/driver/ppp/")
    tcpipPPPMacHeaderFile.setType("HEADER")
    tcpipPPPMacHeaderFile.setOverwrite(True)
    
    # file "$HARMONY_VERSION_PATH/framework/driver/ppp/ppp_lib.X.a" to         "$PROJECT_SOURCE_FILES/framework/driver/ppp/drv_ppp_mac.c"
    # Add ppp_lib.X.a library
    pppLibraryFile = drvPPPComponent.createLibrarySymbol("PPP_LIB", None)
    if "PIC32MZ" in processor:
        pppLibraryFile.setSourcePath("driver/ppp/ppp_lib_pic32mz.X.a")
        pppLibraryFile.setOutputName("ppp_lib_pic32mz.X.a")
    elif "SAME5" in processor:
        pppLibraryFile.setSourcePath("driver/ppp/ppp_lib_same54.X.a")
        pppLibraryFile.setOutputName("ppp_lib_same54.X.a")
    else:
        # should not happen
        return

    pppLibraryFile.setDestPath("/driver/ppp/lib/")
    pppLibraryFile.setEnabled(True)

def tcpipPPPRxOwnBufferCallBack(symbol, event):
    print event["value"]
    if (event["value"] == "Low"):     
        symbol.setValue(2)
    elif(event["value"] == "Medium"):   
        symbol.setValue(4)
    else:
        symbol.setValue(8)

def tcpipPPPRxDynBufferCallBack(symbol, event):
    print event["value"]
    if (event["value"] == "Low"):     
        symbol.setValue(0)
    elif(event["value"] == "Medium"):   
        symbol.setValue(0)
    else:
        symbol.setValue(2)

def tcpipPPPRxBufferThresCallBack(symbol, event):
    print event["value"]
    if (event["value"] == "Low"):     
        symbol.setValue(1)
    elif(event["value"] == "Medium"):   
        symbol.setValue(1)
    else:
        symbol.setValue(2)

def tcpipPPPRxBufferReFillCallBack(symbol, event):
    print event["value"]
    if (event["value"] == "Low"):     
        symbol.setValue(1)
    elif(event["value"] == "Medium"):   
        symbol.setValue(2)
    else:
        symbol.setValue(3)


def drvPPPMenuVisibleSingle(symbol, event):
    if (event["value"] == True):
        print("PPP Menu Visible.")       
        symbol.setVisible(True)
    else:
        print("PPP Menu Invisible.")
        symbol.setVisible(False)


def tcpipPPPMacHeapCalc(): 
    rxBuffSize = Database.getSymbolValue("PPP_MAC","TCPIP_PPP_MAX_RECEIVE_UNIT")
    nRxDedicatedBuffers = Database.getSymbolValue("PPP_MAC","TCPIP_PPP_RX_DEDICATED_BUFFERS")
    
    heap_size = rxBuffSize * nRxDedicatedBuffers
    return heap_size    
    
def tcpipPPPMacHeapUpdate(symbol, event): 
    heap_size = tcpipPPPMacHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
        symbol.setVisible(event["value"])

# Set symbols of other components
def setVal(component, symbol, value):
    triggerDict = {"Component":component,"Id":symbol, "Value":value}
    if(Database.sendMessage(component, "SET_SYMBOL", triggerDict) == None):
        print "Set Symbol Failure" + component + ":" + symbol + ":" + str(value)
        return False
    else:
        return True

# Handle messages from other components
def handleMessage(messageID, args):
    retDict= {}
    if (messageID == "SET_SYMBOL"):
        print "handleMessage: Set Symbol"
        retDict= {"Return": "Success"}
        Database.setSymbolValue(args["Component"], args["Id"], args["Value"])
    else:
        retDict= {"Return": "UnImplemented Command"}
    return retDict
    
def onAttachmentConnected(source, target):
    if (source["id"] == "PPP_Uart_Dependency"): 
        Database.setSymbolValue("PPP_MAC", "DRV_PPP_UART_TYPE", target["component"].getDisplayName(),2)
        uartComponent = "drvPPPUart" + target['component'].getDisplayName().capitalize()
        setVal(uartComponent, "DRV_PPP_UART_NAME", "PPP")
    elif (target["id"] == "NETCONFIG_MAC_Dependency"):
        interface_number = int(target["component"].getID().strip("tcpipNetConfig_"))
        interfaceNum.append(interface_number)
        setVal("tcpipStack", "TCPIP_STACK_INT_MAC_IDX" + str(interface_number), True)
        
def onAttachmentDisconnected(source, target):
    if (source["id"] == "PPP_Uart_Dependency"): 
        Database.clearSymbolValue("PPP_MAC", "DRV_PPP_UART_TYPE")
        Database.setSymbolValue("PPP_MAC", "DRV_PPP_UART_TYPE", "Not Connected")
        uartComponent = "drvPPPUart" + target['component'].getDisplayName().capitalize()
        setVal(uartComponent, "DRV_PPP_UART_NAME", "")
    elif (target["id"] == "NETCONFIG_MAC_Dependency"):
        interface_number = int(target["component"].getID().strip("tcpipNetConfig_"))
        interfaceNum.remove(interface_number)
        setVal("tcpipStack", "TCPIP_STACK_INT_MAC_IDX" + str(interface_number), False)
        setVal("tcpipStack", "TCPIP_STACK_MII_MODE_IDX" + str(interface_number), "")    


def destroyComponent(drvPPPComponent):
    Database.setSymbolValue("PPP_MAC", "TCPIP_STACK_USE_PPP_INTERFACE", False, 2)    
