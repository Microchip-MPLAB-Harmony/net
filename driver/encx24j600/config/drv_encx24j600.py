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


def instantiateComponent(drvExtMacEncx24j600Component, index):
    configName = Variables.get("__CONFIGURATION_NAME")      
    
    drvEncx24j600InstnIndex = drvExtMacEncx24j600Component.createIntegerSymbol("INDEX", None)
    drvEncx24j600InstnIndex.setLabel("ENCX24J600 Instance Number")
    drvEncx24j600InstnIndex.setVisible(True)
    drvEncx24j600InstnIndex.setDefaultValue(index)
    drvEncx24j600InstnIndex.setReadOnly(True)
    
    drvEncx24j600InstnClientNum = drvExtMacEncx24j600Component.createIntegerSymbol("DRV_ENCX24J600_CLIENT_INSTANCES_IDX" + str(index),None)
    drvEncx24j600InstnClientNum.setLabel("Number of Client Instances")
    drvEncx24j600InstnClientNum.setVisible(True)
    drvEncx24j600InstnClientNum.setDefaultValue(1)
    drvEncx24j600InstnClientNum.setDescription("Number of Client Instances")
    
    drvEncx24j600InstnTxDescNum = drvExtMacEncx24j600Component.createIntegerSymbol("DRV_ENCX24J600_MAC_TX_DESCRIPTORS_IDX" + str(index),None)
    drvEncx24j600InstnTxDescNum.setVisible(True)
    drvEncx24j600InstnTxDescNum.setLabel("Number of the TX Descriptors")
    drvEncx24j600InstnTxDescNum.setDefaultValue(3)
    drvEncx24j600InstnTxDescNum.setDescription("Number of the TX Descriptors")
    
    drvEncx24j600InstnRxDescNum = drvExtMacEncx24j600Component.createIntegerSymbol("DRV_ENCX24J600_MAC_RX_DESCRIPTORS_IDX" + str(index),None)
    drvEncx24j600InstnRxDescNum.setVisible(True)
    drvEncx24j600InstnRxDescNum.setLabel("Number of the RX Descriptors")
    drvEncx24j600InstnRxDescNum.setDefaultValue(3)
    drvEncx24j600InstnRxDescNum.setDescription("Number of the RX Descriptors")
    
    drvEncx24j600InstnRxDescBuffSize = drvExtMacEncx24j600Component.createIntegerSymbol("DRV_ENCX24J600_MAX_RX_BUFFER_IDX" + str(index),None)
    drvEncx24j600InstnRxDescBuffSize.setVisible(True)
    drvEncx24j600InstnRxDescBuffSize.setLabel("RX Descriptor Buffer Size")
    drvEncx24j600InstnRxDescBuffSize.setDefaultValue(1536)
    drvEncx24j600InstnRxDescBuffSize.setDescription("RX Descriptor Buffer Size")    
    
    drvEncx24j600InstnRxBuffSize = drvExtMacEncx24j600Component.createIntegerSymbol("DRV_ENCX24J600_RX_BUFFER_SIZE_IDX" + str(index),None)
    drvEncx24j600InstnRxBuffSize.setVisible(True)
    drvEncx24j600InstnRxBuffSize.setLabel("RX Buffer Size (should be even)")
    drvEncx24j600InstnRxBuffSize.setDefaultValue(16384)
    drvEncx24j600InstnRxBuffSize.setDescription("RX Buffer Size")    
    
    drvEncx24j600InstnMaxFrameSize = drvExtMacEncx24j600Component.createIntegerSymbol("DRV_ENCX24J600_MAX_FRAME_SIZE_IDX" + str(index),None)
    drvEncx24j600InstnMaxFrameSize.setVisible(True)
    drvEncx24j600InstnMaxFrameSize.setLabel("Maximum Frame Length")
    drvEncx24j600InstnMaxFrameSize.setDefaultValue(1536)
    drvEncx24j600InstnMaxFrameSize.setDescription("Maximum Frame Length") 

    drvEncx24j600InstnAutoFlowCtrl = drvExtMacEncx24j600Component.createBooleanSymbol("DRV_ENCX24J600_MAX_USE_AUTOMATIC_FLOW_CONTROL_IDX" + str(index),None)
    drvEncx24j600InstnAutoFlowCtrl.setLabel("Enable Automatic Flow Control")
    drvEncx24j600InstnAutoFlowCtrl.setVisible(True)
    drvEncx24j600InstnAutoFlowCtrl.setDefaultValue(True)
    drvEncx24j600InstnAutoFlowCtrl.setDescription("Use Automatic Flow Control")    

    drvEncx24j600InstnEthType = drvExtMacEncx24j600Component.createKeyValueSetSymbol("DRV_ENCX24J600_ETHERNET_TYPE_IDX"+str(index),None)
    drvEncx24j600InstnEthType.setVisible(True)
    drvEncx24j600InstnEthType.setLabel("Ethernet Type")
    drvEncx24j600InstnEthType.addKey("Auto Negotiate", "0", "Auto Negotiate")
    drvEncx24j600InstnEthType.addKey("10base-T", "1", "10base-T")
    drvEncx24j600InstnEthType.addKey("100base-T", "2", "100base-T")
    drvEncx24j600InstnEthType.setDisplayMode("Key")
    drvEncx24j600InstnEthType.setOutputMode("Key")
    drvEncx24j600InstnEthType.setDefaultValue(0)

    drvEncx24j600InstnDuplexMode = drvExtMacEncx24j600Component.createKeyValueSetSymbol("DRV_ENCX24J600_DUPLEX_MODE_IDX"+str(index),None)
    drvEncx24j600InstnDuplexMode.setVisible(True)
    drvEncx24j600InstnDuplexMode.setLabel("Duplex mode")
    drvEncx24j600InstnDuplexMode.addKey("Auto Negotiate", "0", "Auto Negotiate")
    drvEncx24j600InstnDuplexMode.addKey("Half Duplex", "1", "Half Duplex")
    drvEncx24j600InstnDuplexMode.addKey("Full Duplex", "2", "Full Duplex")
    drvEncx24j600InstnDuplexMode.setDisplayMode("Key")
    drvEncx24j600InstnDuplexMode.setOutputMode("Key")
    drvEncx24j600InstnDuplexMode.setDefaultValue(0)
    drvEncx24j600InstnDuplexMode.setDependencies(drvEncx24j600InstnDuplexModeSelect, [drvEncx24j600InstnEthType.getID()])

    drvEncx24j600SpiChipSelect = drvExtMacEncx24j600Component.createStringSymbol("DRV_ENCX24J600_SPI_CS_IDX" + str(index),None)
    drvEncx24j600SpiChipSelect.setLabel("SPI Chip Select")
    drvEncx24j600SpiChipSelect.setDefaultValue("SYS_PORT_PIN_RD9")
    drvEncx24j600SpiChipSelect.setVisible( True )
    drvEncx24j600SpiChipSelect.setDescription("Chip Select Pin for SPI interface")     
        
    drvEncx24j600SpiComment = drvExtMacEncx24j600Component.createCommentSymbol("DRV_ENCX24J600_SPI_COMMENT_IDX" + str(index),drvEncx24j600SpiChipSelect)
    drvEncx24j600SpiComment.setLabel("***Configure the pin as GPIO Output using Pin Manager***")
    drvEncx24j600SpiComment.setVisible( True )

def drvEncx24j600InstnDuplexModeSelect(symbol, event):
    
    print "************niyas"
    print symbol.getID()
    print event["value"]
    # if (event["value"] == 1):
    symbol.setValue(event["value"])
    # else if(event["value"] == 1):
        # symbol.setVisible(False)


