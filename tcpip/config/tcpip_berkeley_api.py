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

    
def instantiateComponent(tcpipBerkeleyApiComponent):
    print("TCPIP Berkeley Api Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    # Enable Berkeley API
    tcpipBerkeleyApi = tcpipBerkeleyApiComponent.createBooleanSymbol("TCPIP_STACK_USE_BERKELEY_API", None)
    tcpipBerkeleyApi.setHelp("mcc_h3_bsd_configurations")
    tcpipBerkeleyApi.setLabel("Berkeley API")
    tcpipBerkeleyApi.setVisible(False)
    tcpipBerkeleyApi.setDescription("Enable Berkeley API")
    tcpipBerkeleyApi.setDefaultValue(True)

    # Maximum Number of Simultaneous Sockets Supported
    tcpipBerkeleyBsdSktsMaxNum = tcpipBerkeleyApiComponent.createIntegerSymbol("TCPIP_BSD_MAX_BSD_SOCKETS", None)
    tcpipBerkeleyBsdSktsMaxNum.setHelp("mcc_h3_bsd_configurations")
    tcpipBerkeleyBsdSktsMaxNum.setLabel("Maximum Number of Simultaneous Sockets")
    tcpipBerkeleyBsdSktsMaxNum.setVisible(True)
    tcpipBerkeleyBsdSktsMaxNum.setDescription("Maximum Number of Simultaneous Sockets Supported")
    tcpipBerkeleyBsdSktsMaxNum.setDefaultValue(4)

    tcpipBerkeleyheapdependency = ["TCPIP_BSD_MAX_BSD_SOCKETS", "tcpipStack.TCPIP_STACK_HEAP_CALC_MASK"]    
        
    # Berkeley Heap Size
    tcpipBerkeleyHeapSize = tcpipBerkeleyApiComponent.createIntegerSymbol("TCPIP_BSD_HEAP_SIZE", None)
    tcpipBerkeleyHeapSize.setLabel("BSD Heap Size (bytes)") 
    tcpipBerkeleyHeapSize.setVisible(False)
    tcpipBerkeleyHeapSize.setDefaultValue(tcpipBerkeleyHeapCalc())
    tcpipBerkeleyHeapSize.setReadOnly(True)
    tcpipBerkeleyHeapSize.setDependencies(tcpipBerkeleyHeapSizeHeapUpdate, tcpipBerkeleyheapdependency) 

    # Add berkeley_api.c file
    tcpipBerkeleyApiSourceFile = tcpipBerkeleyApiComponent.createFileSymbol(None, None)
    tcpipBerkeleyApiSourceFile.setSourcePath("tcpip/src/berkeley_api.c")
    tcpipBerkeleyApiSourceFile.setOutputName("berkeley_api.c")
    tcpipBerkeleyApiSourceFile.setOverwrite(True)
    tcpipBerkeleyApiSourceFile.setDestPath("library/tcpip/src/")
    tcpipBerkeleyApiSourceFile.setProjectPath("config/" + configName + "/library/tcpip/src/")
    tcpipBerkeleyApiSourceFile.setType("SOURCE")
    tcpipBerkeleyApiSourceFile.setEnabled(True)

    tcpipBerkeleyApiHeaderFtl = tcpipBerkeleyApiComponent.createFileSymbol(None, None)
    tcpipBerkeleyApiHeaderFtl.setSourcePath("tcpip/config/berkeley_api.h.ftl")
    tcpipBerkeleyApiHeaderFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    tcpipBerkeleyApiHeaderFtl.setMarkup(True)
    tcpipBerkeleyApiHeaderFtl.setType("STRING") 
    
def tcpipBerkeleyApiMenuVisible(symbol, event):
    if (event["value"] == True):
        print("BerkeleyApi Menu Visible.")      
        symbol.setVisible(True)
    else:
        print("BerkeleyApi Menu Invisible.")
        symbol.setVisible(False)    
        
def tcpipBerkeleyApiGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])
    

def tcpipBerkeleyHeapCalc(): 
    maxNumSkt = Database.getSymbolValue("tcpipBerkeleyApi","TCPIP_BSD_MAX_BSD_SOCKETS")
    heap_size = maxNumSkt * 64
    return heap_size    
    
def tcpipBerkeleyHeapSizeHeapUpdate(symbol, event): 
    heap_size = tcpipBerkeleyHeapCalc()
    symbol.setValue(heap_size)
    if(event["id"] == "TCPIP_STACK_HEAP_CALC_MASK"):
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
    
def destroyComponent(component):
    Database.setSymbolValue("tcpipBerkeleyApi", "TCPIP_STACK_USE_BERKELEY_API", False, 2)
