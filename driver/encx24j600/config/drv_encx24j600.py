"""
Copyright (C) 2020-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
def instantiateComponent(drvExtMacEncx24j600Component, index):
    configName = Variables.get("__CONFIGURATION_NAME")      
    
    drvEncx24j600InstnIndex = drvExtMacEncx24j600Component.createIntegerSymbol("INDEX", None)
    drvEncx24j600InstnIndex.setHelp("mcc_h3_enc600_configurations")
    drvEncx24j600InstnIndex.setLabel("ENCX24J600 Instance Number")
    drvEncx24j600InstnIndex.setVisible(True)
    drvEncx24j600InstnIndex.setDefaultValue(index)
    drvEncx24j600InstnIndex.setReadOnly(True)
    
    drvEncx24j600InstnClientNum = drvExtMacEncx24j600Component.createIntegerSymbol("DRV_ENCX24J600_CLIENT_INSTANCES_IDX" + str(index),None)
    drvEncx24j600InstnClientNum.setHelp("mcc_h3_enc600_configurations")
    drvEncx24j600InstnClientNum.setLabel("Number of Client Instances")
    drvEncx24j600InstnClientNum.setVisible(True)
    drvEncx24j600InstnClientNum.setDefaultValue(1)
    drvEncx24j600InstnClientNum.setDescription("Number of Client Instances")
    
    drvEncx24j600InstnTxDescNum = drvExtMacEncx24j600Component.createIntegerSymbol("DRV_ENCX24J600_MAC_TX_DESCRIPTORS_IDX" + str(index),None)
    drvEncx24j600InstnTxDescNum.setHelp("mcc_h3_enc600_configurations")
    drvEncx24j600InstnTxDescNum.setVisible(True)
    drvEncx24j600InstnTxDescNum.setLabel("Number of the TX Descriptors")
    drvEncx24j600InstnTxDescNum.setDefaultValue(3)
    drvEncx24j600InstnTxDescNum.setDescription("Number of the TX Descriptors")
    
    drvEncx24j600InstnRxDescNum = drvExtMacEncx24j600Component.createIntegerSymbol("DRV_ENCX24J600_MAC_RX_DESCRIPTORS_IDX" + str(index),None)
    drvEncx24j600InstnRxDescNum.setHelp("mcc_h3_enc600_configurations")
    drvEncx24j600InstnRxDescNum.setVisible(True)
    drvEncx24j600InstnRxDescNum.setLabel("Number of the RX Descriptors")
    drvEncx24j600InstnRxDescNum.setDefaultValue(3)
    drvEncx24j600InstnRxDescNum.setDescription("Number of the RX Descriptors")
    
    drvEncx24j600InstnRxDescBuffSize = drvExtMacEncx24j600Component.createIntegerSymbol("DRV_ENCX24J600_MAX_RX_BUFFER_IDX" + str(index),None)
    drvEncx24j600InstnRxDescBuffSize.setHelp("mcc_h3_enc600_configurations")
    drvEncx24j600InstnRxDescBuffSize.setVisible(True)
    drvEncx24j600InstnRxDescBuffSize.setLabel("RX Descriptor Buffer Size")
    drvEncx24j600InstnRxDescBuffSize.setDefaultValue(1536)
    drvEncx24j600InstnRxDescBuffSize.setDescription("RX Descriptor Buffer Size")    
    
    drvEncx24j600InstnRxBuffSize = drvExtMacEncx24j600Component.createIntegerSymbol("DRV_ENCX24J600_RX_BUFFER_SIZE_IDX" + str(index),None)
    drvEncx24j600InstnRxBuffSize.setHelp("mcc_h3_enc600_configurations")
    drvEncx24j600InstnRxBuffSize.setVisible(True)
    drvEncx24j600InstnRxBuffSize.setLabel("RX Buffer Size (should be even)")
    drvEncx24j600InstnRxBuffSize.setDefaultValue(16384)
    drvEncx24j600InstnRxBuffSize.setDescription("RX Buffer Size")    
            
    drvEncx24j600SpiChipSelect = drvExtMacEncx24j600Component.createKeyValueSetSymbol("DRV_ENCX24J600_SPI_CS_IDX" + str(index),None)
    drvEncx24j600SpiChipSelect.setHelp("mcc_h3_enc600_configurations")
    drvEncx24j600SpiChipSelect.setLabel("SPI Chip Select Pin")
    drvEncx24j600SpiChipSelect.setOutputMode("Key")
    drvEncx24j600SpiChipSelect.setDisplayMode("Key")
    drvEncx24j600SpiChipSelect.setVisible( True )

    # Send message to core to get available pins
    chipSelectavailablePinDictionary = {}
    chipSelectavailablePinDictionary = Database.sendMessage("core", "PIN_LIST", chipSelectavailablePinDictionary)
    
    drvEncx24j600SpiChipSelect.addKey("SYS_PORT_PIN_NONE", "0", "No Pin Selected")
    for pad in sort_alphanumeric(chipSelectavailablePinDictionary.values()):
        key = "SYS_PORT_PIN_" + pad
        value = list(chipSelectavailablePinDictionary.keys())[list(chipSelectavailablePinDictionary.values()).index(pad)]
        description = pad
        drvEncx24j600SpiChipSelect.addKey(key, value, description)
        
    if "PIC32MX" in Variables.get("__PROCESSOR"):
        drvEncx24j600SpiChipSelect.setSelectedKey("SYS_PORT_PIN_RD9")
    elif "PIC32MZ" in Variables.get("__PROCESSOR"):
        drvEncx24j600SpiChipSelect.setSelectedKey("SYS_PORT_PIN_RE9")
    else:
        drvEncx24j600SpiChipSelect.setSelectedKey("SYS_PORT_PIN_NONE")    
        
    drvEncx24j600SpiComment = drvExtMacEncx24j600Component.createCommentSymbol("DRV_ENCX24J600_SPI_COMMENT_IDX" + str(index),drvEncx24j600SpiChipSelect)
    drvEncx24j600SpiComment.setLabel("***Configure the Chip-Select pin as GPIO Output in Pin Manager***")
    drvEncx24j600SpiComment.setVisible( True )
        
    # Advanced Settings
    drvEncx24j600AdvSettings = drvExtMacEncx24j600Component.createMenuSymbol("TCPIP_ETHMAC_ADV_SETTING", None)
    drvEncx24j600AdvSettings.setLabel("Advanced Settings")
    drvEncx24j600AdvSettings.setDescription("Advanced Settings")
    drvEncx24j600AdvSettings.setVisible(True)
    
    drvEncx24j600InstnMaxFrameSize = drvExtMacEncx24j600Component.createIntegerSymbol("DRV_ENCX24J600_MAX_FRAME_SIZE_IDX" + str(index),drvEncx24j600AdvSettings)
    drvEncx24j600InstnMaxFrameSize.setHelp("mcc_h3_enc600_configurations")
    drvEncx24j600InstnMaxFrameSize.setVisible(True)
    drvEncx24j600InstnMaxFrameSize.setLabel("Maximum Frame Length")
    drvEncx24j600InstnMaxFrameSize.setDefaultValue(1536)
    drvEncx24j600InstnMaxFrameSize.setDescription("Maximum Frame Length") 

    drvEncx24j600InstnAutoFlowCtrl = drvExtMacEncx24j600Component.createBooleanSymbol("DRV_ENCX24J600_MAX_USE_AUTOMATIC_FLOW_CONTROL_IDX" + str(index),drvEncx24j600AdvSettings)
    drvEncx24j600InstnAutoFlowCtrl.setHelp("mcc_h3_enc600_configurations")
    drvEncx24j600InstnAutoFlowCtrl.setLabel("Enable Automatic Flow Control")
    drvEncx24j600InstnAutoFlowCtrl.setVisible(True)
    drvEncx24j600InstnAutoFlowCtrl.setDefaultValue(True)
    drvEncx24j600InstnAutoFlowCtrl.setDescription("Use Automatic Flow Control")    

    drvEncx24j600InstnEthType = drvExtMacEncx24j600Component.createKeyValueSetSymbol("DRV_ENCX24J600_ETHERNET_TYPE_IDX"+str(index),drvEncx24j600AdvSettings)
    drvEncx24j600InstnEthType.setHelp("mcc_h3_enc600_configurations")
    drvEncx24j600InstnEthType.setVisible(True)
    drvEncx24j600InstnEthType.setLabel("Ethernet Type")
    drvEncx24j600InstnEthType.addKey("Auto Negotiate", "0", "Auto Negotiate")
    drvEncx24j600InstnEthType.addKey("10base-T", "1", "10base-T")
    drvEncx24j600InstnEthType.addKey("100base-T", "2", "100base-T")
    drvEncx24j600InstnEthType.setDisplayMode("Key")
    drvEncx24j600InstnEthType.setOutputMode("Key")
    drvEncx24j600InstnEthType.setDefaultValue(0)

    drvEncx24j600InstnDuplexMode = drvExtMacEncx24j600Component.createKeyValueSetSymbol("DRV_ENCX24J600_DUPLEX_MODE_IDX"+str(index),drvEncx24j600AdvSettings)
    drvEncx24j600InstnDuplexMode.setHelp("mcc_h3_enc600_configurations")
    drvEncx24j600InstnDuplexMode.setVisible(True)
    drvEncx24j600InstnDuplexMode.setLabel("Duplex mode")
    drvEncx24j600InstnDuplexMode.addKey("Auto Negotiate", "0", "Auto Negotiate")
    drvEncx24j600InstnDuplexMode.addKey("Half Duplex", "1", "Half Duplex")
    drvEncx24j600InstnDuplexMode.addKey("Full Duplex", "2", "Full Duplex")
    drvEncx24j600InstnDuplexMode.setDisplayMode("Key")
    drvEncx24j600InstnDuplexMode.setOutputMode("Key")
    drvEncx24j600InstnDuplexMode.setDefaultValue(0)
    drvEncx24j600InstnDuplexMode.setDependencies(drvEncx24j600InstnDuplexModeSelect, [drvEncx24j600InstnEthType.getID()])    

def drvEncx24j600InstnDuplexModeSelect(symbol, event):

    print symbol.getID()
    print event["value"]
    symbol.setValue(event["value"])

def sort_alphanumeric(l):
    import re
    convert = lambda text: int(text) if text.isdigit() else text.lower()
    alphanum_key = lambda key: [ convert(c) for c in re.split('([0-9]+)', key) ]
    return sorted(l, key = alphanum_key)
    
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

def onAttachmentConnected(source, target):
    if (target["id"] == "NETCONFIG_MAC_Dependency"):
        interface_number = int(target["component"].getID().strip("tcpipNetConfig_"))
        interfaceNum.append(interface_number)
        setVal("tcpipStack", "TCPIP_STACK_INT_MAC_IDX" + str(interface_number), False)
        setVal("tcpipStack", "TCPIP_STACK_OTH_INT_IDX" + str(interface_number), "SPI") 

def onAttachmentDisconnected(source, target):
    if (target["id"] == "NETCONFIG_MAC_Dependency"):
        interface_number = int(target["component"].getID().strip("tcpipNetConfig_"))
        interfaceNum.remove(interface_number)
        setVal("tcpipStack", "TCPIP_STACK_OTH_INT_IDX" + str(interface_number), "")   

def finalizeComponent( macComponent ):
    print "Finalize Instance Component " + macComponent.getID()
    tcpipAutoConfigDataLinkGroup = Database.findGroup("DATA LINK LAYER")
    tcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    if (tcpipAutoConfigStackGroup != None) and (tcpipAutoConfigDataLinkGroup != None):  
        print "Exporting Attachment " + macComponent.getID()
        tcpipAutoConfigDataLinkGroup.setAttachmentVisible(macComponent.getID(), "libdrvExtMacEncx24j600")
        tcpipAutoConfigDataLinkGroup.setAttachmentVisible(macComponent.getID(),"ENCX24J600_SPI")
        tcpipAutoConfigStackGroup.setAttachmentVisible("DATA LINK LAYER", macComponent.getID() + ":ENCX24J600_SPI")
