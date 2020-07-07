"""*****************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
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


def instantiateComponent(drvExtMacEnc28j60Component, index):
    configName = Variables.get("__CONFIGURATION_NAME")      
    
    drvEnc28j60InstnIndex = drvExtMacEnc28j60Component.createIntegerSymbol("INDEX", None)
    drvEnc28j60InstnIndex.setLabel("ENC28J60 Instance Number")
    drvEnc28j60InstnIndex.setVisible(True)
    drvEnc28j60InstnIndex.setDefaultValue(index)
    drvEnc28j60InstnIndex.setReadOnly(True)
    
    drvEnc28j60InstnClientNum = drvExtMacEnc28j60Component.createIntegerSymbol("DRV_ENC28J60_CLIENT_INSTANCES_IDX" + str(index),None)
    drvEnc28j60InstnClientNum.setLabel("Number of Client Instances")
    drvEnc28j60InstnClientNum.setVisible(True)
    drvEnc28j60InstnClientNum.setDefaultValue(1)
    drvEnc28j60InstnClientNum.setDescription("Number of Client Instances")
    
    drvEnc28j60InstnTxDescNum = drvExtMacEnc28j60Component.createIntegerSymbol("DRV_ENC28J60_MAC_TX_DESCRIPTORS_IDX" + str(index),None)
    drvEnc28j60InstnTxDescNum.setVisible(True)
    drvEnc28j60InstnTxDescNum.setLabel("Number of the TX Descriptors")
    drvEnc28j60InstnTxDescNum.setDefaultValue(2)
    drvEnc28j60InstnTxDescNum.setDescription("Number of the TX Descriptors")
    
    drvEnc28j60InstnRxDescNum = drvExtMacEnc28j60Component.createIntegerSymbol("DRV_ENC28J60_MAC_RX_DESCRIPTORS_IDX" + str(index),None)
    drvEnc28j60InstnRxDescNum.setVisible(True)
    drvEnc28j60InstnRxDescNum.setLabel("Number of the RX Descriptors")
    drvEnc28j60InstnRxDescNum.setDefaultValue(2)
    drvEnc28j60InstnRxDescNum.setDescription("Number of the RX Descriptors")
    
    drvEnc28j60InstnRxDescBuffSize = drvExtMacEnc28j60Component.createIntegerSymbol("DRV_ENC28J60_MAX_RX_BUFFER_IDX" + str(index),None)
    drvEnc28j60InstnRxDescBuffSize.setVisible(True)
    drvEnc28j60InstnRxDescBuffSize.setLabel("RX Descriptor Buffer Size")
    drvEnc28j60InstnRxDescBuffSize.setDefaultValue(1536)
    drvEnc28j60InstnRxDescBuffSize.setDescription("RX Descriptor Buffer Size")    
    
    drvEnc28j60InstnRxBuffSize = drvExtMacEnc28j60Component.createIntegerSymbol("DRV_ENC28J60_RX_BUFFER_SIZE_IDX" + str(index),None)
    drvEnc28j60InstnRxBuffSize.setVisible(True)
    drvEnc28j60InstnRxBuffSize.setLabel("RX Buffer Size (should be even)")
    drvEnc28j60InstnRxBuffSize.setDefaultValue(16384)
    drvEnc28j60InstnRxBuffSize.setDescription("RX Buffer Size")    
    
    drvEnc28j60SpiChipSelect = drvExtMacEnc28j60Component.createStringSymbol("DRV_ENC28J60_SPI_CS_IDX" + str(index),None)
    drvEnc28j60SpiChipSelect.setLabel("SPI Chip Select")
    if "PIC32MX" in Variables.get("__PROCESSOR"):
        drvEnc28j60SpiChipSelect.setDefaultValue("SYS_PORT_PIN_RD9")
    elif "PIC32MZ" in Variables.get("__PROCESSOR"):
        drvEnc28j60SpiChipSelect.setDefaultValue("SYS_PORT_PIN_RE9")
        
    drvEnc28j60SpiChipSelect.setVisible( True )
    drvEnc28j60SpiChipSelect.setDescription("Chip Select Pin for SPI interface")     
        
    drvEnc28j60SpiComment = drvExtMacEnc28j60Component.createCommentSymbol("DRV_ENC28J60_SPI_COMMENT_IDX" + str(index),drvEnc28j60SpiChipSelect)
    drvEnc28j60SpiComment.setLabel("***Configure the Chip-Select pin as GPIO Output in Pin Manager***")
    drvEnc28j60SpiComment.setVisible( True )
        
    # Advanced Settings
    drvEnc28j60AdvSettings = drvExtMacEnc28j60Component.createMenuSymbol("TCPIP_ETHMAC_ADV_SETTING", None)
    drvEnc28j60AdvSettings.setLabel("Advanced Settings")
    drvEnc28j60AdvSettings.setDescription("Advanced Settings")
    drvEnc28j60AdvSettings.setVisible(True)    
        
    drvEnc28j60InstnMaxFrameSize = drvExtMacEnc28j60Component.createIntegerSymbol("DRV_ENC28J60_MAX_FRAME_SIZE_IDX" + str(index),drvEnc28j60AdvSettings)
    drvEnc28j60InstnMaxFrameSize.setVisible(True)
    drvEnc28j60InstnMaxFrameSize.setLabel("Maximum Frame Length")
    drvEnc28j60InstnMaxFrameSize.setDefaultValue(1536)
    drvEnc28j60InstnMaxFrameSize.setDescription("Maximum Frame Length") 

    drvEnc28j60InstnAutoFlowCtrl = drvExtMacEnc28j60Component.createBooleanSymbol("DRV_ENC28J60_MAX_USE_AUTOMATIC_FLOW_CONTROL_IDX" + str(index),drvEnc28j60AdvSettings)
    drvEnc28j60InstnAutoFlowCtrl.setLabel("Enable Automatic Flow Control")
    drvEnc28j60InstnAutoFlowCtrl.setVisible(True)
    drvEnc28j60InstnAutoFlowCtrl.setDefaultValue(True)
    drvEnc28j60InstnAutoFlowCtrl.setDescription("Use Automatic Flow Control")  

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