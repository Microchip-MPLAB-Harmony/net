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
def instantiateComponent(drvExtMacEnc28j60Component, index):
    configName = Variables.get("__CONFIGURATION_NAME")      
    
    drvEnc28j60InstnIndex = drvExtMacEnc28j60Component.createIntegerSymbol("INDEX", None)
    drvEnc28j60InstnIndex.setHelp("mcc_h3_enc60_configurations")
    drvEnc28j60InstnIndex.setLabel("ENC28J60 Instance Number")
    drvEnc28j60InstnIndex.setVisible(True)
    drvEnc28j60InstnIndex.setDefaultValue(index)
    drvEnc28j60InstnIndex.setReadOnly(True)
    
    drvEnc28j60InstnClientNum = drvExtMacEnc28j60Component.createIntegerSymbol("DRV_ENC28J60_CLIENT_INSTANCES_IDX" + str(index),None)
    drvEnc28j60InstnClientNum.setHelp("mcc_h3_enc60_configurations")
    drvEnc28j60InstnClientNum.setLabel("Number of Client Instances")
    drvEnc28j60InstnClientNum.setVisible(True)
    drvEnc28j60InstnClientNum.setDefaultValue(1)
    drvEnc28j60InstnClientNum.setDescription("Number of Client Instances")
    
    drvEnc28j60InstnTxDescNum = drvExtMacEnc28j60Component.createIntegerSymbol("DRV_ENC28J60_MAC_TX_DESCRIPTORS_IDX" + str(index),None)
    drvEnc28j60InstnTxDescNum.setHelp("mcc_h3_enc60_configurations")
    drvEnc28j60InstnTxDescNum.setVisible(True)
    drvEnc28j60InstnTxDescNum.setLabel("Number of the TX Descriptors")
    drvEnc28j60InstnTxDescNum.setDefaultValue(2)
    drvEnc28j60InstnTxDescNum.setDescription("Number of the TX Descriptors")
    
    drvEnc28j60InstnRxDescNum = drvExtMacEnc28j60Component.createIntegerSymbol("DRV_ENC28J60_MAC_RX_DESCRIPTORS_IDX" + str(index),None)
    drvEnc28j60InstnRxDescNum.setHelp("mcc_h3_enc60_configurations")
    drvEnc28j60InstnRxDescNum.setVisible(True)
    drvEnc28j60InstnRxDescNum.setLabel("Number of the RX Descriptors")
    drvEnc28j60InstnRxDescNum.setDefaultValue(2)
    drvEnc28j60InstnRxDescNum.setDescription("Number of the RX Descriptors")
    
    drvEnc28j60InstnRxDescBuffSize = drvExtMacEnc28j60Component.createIntegerSymbol("DRV_ENC28J60_MAX_RX_BUFFER_IDX" + str(index),None)
    drvEnc28j60InstnRxDescBuffSize.setHelp("mcc_h3_enc60_configurations")
    drvEnc28j60InstnRxDescBuffSize.setVisible(True)
    drvEnc28j60InstnRxDescBuffSize.setLabel("RX Descriptor Buffer Size")
    drvEnc28j60InstnRxDescBuffSize.setDefaultValue(1536)
    drvEnc28j60InstnRxDescBuffSize.setDescription("RX Descriptor Buffer Size")    
    
    drvEnc28j60InstnRxBuffSize = drvExtMacEnc28j60Component.createIntegerSymbol("DRV_ENC28J60_RX_BUFFER_SIZE_IDX" + str(index),None)
    drvEnc28j60InstnRxBuffSize.setHelp("mcc_h3_enc60_configurations")
    drvEnc28j60InstnRxBuffSize.setVisible(True)
    drvEnc28j60InstnRxBuffSize.setLabel("RX Buffer Size (should be even)")
    drvEnc28j60InstnRxBuffSize.setDefaultValue(16384)
    drvEnc28j60InstnRxBuffSize.setDescription("RX Buffer Size")    
            
    drvEnc28j60SpiChipSelect = drvExtMacEnc28j60Component.createKeyValueSetSymbol("DRV_ENC28J60_SPI_CS_IDX" + str(index),None)
    drvEnc28j60SpiChipSelect.setHelp("mcc_h3_enc60_configurations")
    drvEnc28j60SpiChipSelect.setLabel("SPI Chip Select Pin")
    drvEnc28j60SpiChipSelect.setOutputMode("Key")
    drvEnc28j60SpiChipSelect.setDisplayMode("Key")
    drvEnc28j60SpiChipSelect.setVisible( True )

    # Send message to core to get available pins
    chipSelectavailablePinDictionary = {}
    chipSelectavailablePinDictionary = Database.sendMessage("core", "PIN_LIST", chipSelectavailablePinDictionary)
    
    drvEnc28j60SpiChipSelect.addKey("SYS_PORT_PIN_NONE", "0", "No Pin Selected")
    for pad in sort_alphanumeric(chipSelectavailablePinDictionary.values()):
        key = "SYS_PORT_PIN_" + pad
        value = list(chipSelectavailablePinDictionary.keys())[list(chipSelectavailablePinDictionary.values()).index(pad)]
        description = pad
        drvEnc28j60SpiChipSelect.addKey(key, value, description)
        
    if "PIC32MX" in Variables.get("__PROCESSOR"):
        drvEnc28j60SpiChipSelect.setSelectedKey("SYS_PORT_PIN_RD9")
    elif "PIC32MZ" in Variables.get("__PROCESSOR"):
        drvEnc28j60SpiChipSelect.setSelectedKey("SYS_PORT_PIN_RE9")
    else:
        drvEnc28j60SpiChipSelect.setSelectedKey("SYS_PORT_PIN_NONE")
        
    drvEnc28j60SpiComment = drvExtMacEnc28j60Component.createCommentSymbol("DRV_ENC28J60_SPI_COMMENT_IDX" + str(index),drvEnc28j60SpiChipSelect)
    drvEnc28j60SpiComment.setLabel("***Configure the Chip-Select pin as GPIO Output in Pin Manager***")
    drvEnc28j60SpiComment.setVisible( True )
        
    # Advanced Settings
    drvEnc28j60AdvSettings = drvExtMacEnc28j60Component.createMenuSymbol("TCPIP_ETHMAC_ADV_SETTING", None)
    drvEnc28j60AdvSettings.setLabel("Advanced Settings")
    drvEnc28j60AdvSettings.setDescription("Advanced Settings")
    drvEnc28j60AdvSettings.setVisible(True)    
        
    drvEnc28j60InstnMaxFrameSize = drvExtMacEnc28j60Component.createIntegerSymbol("DRV_ENC28J60_MAX_FRAME_SIZE_IDX" + str(index),drvEnc28j60AdvSettings)
    drvEnc28j60InstnMaxFrameSize.setHelp("mcc_h3_enc60_configurations")
    drvEnc28j60InstnMaxFrameSize.setVisible(True)
    drvEnc28j60InstnMaxFrameSize.setLabel("Maximum Frame Length")
    drvEnc28j60InstnMaxFrameSize.setDefaultValue(1536)
    drvEnc28j60InstnMaxFrameSize.setDescription("Maximum Frame Length") 

    drvEnc28j60InstnAutoFlowCtrl = drvExtMacEnc28j60Component.createBooleanSymbol("DRV_ENC28J60_MAX_USE_AUTOMATIC_FLOW_CONTROL_IDX" + str(index),drvEnc28j60AdvSettings)
    drvEnc28j60InstnAutoFlowCtrl.setHelp("mcc_h3_enc60_configurations")
    drvEnc28j60InstnAutoFlowCtrl.setLabel("Enable Automatic Flow Control")
    drvEnc28j60InstnAutoFlowCtrl.setVisible(True)
    drvEnc28j60InstnAutoFlowCtrl.setDefaultValue(True)
    drvEnc28j60InstnAutoFlowCtrl.setDescription("Use Automatic Flow Control")  

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
        tcpipAutoConfigDataLinkGroup.setAttachmentVisible(macComponent.getID(), "libdrvExtMacEnc28j60")
        tcpipAutoConfigDataLinkGroup.setAttachmentVisible(macComponent.getID(),"ENC28J60_SPI")
        tcpipAutoConfigStackGroup.setAttachmentVisible("DATA LINK LAYER", macComponent.getID() + ":ENC28J60_SPI")
