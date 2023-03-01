"""
Copyright (C) 2019-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

global ORG_PATH
global webpageListFile
global count
global webPagePathParsing
global tcpipHttpNetWebPageDirMyPath
global createWebPageFileSymbol
global webpageGenSourceFile
global clearFileSymbols
global mytcpipHttpNetComponent
global symbolList


#Disable all the file symbols
def clearFileSymbols():
    for symbol in symbolList:
        symbol.setEnabled(False)

#Return File symbols from the Symbol list.
def createWebPageFileSymbol(count):
    return symbolList[count]
#webpage files generation
def webpageGenSourceFile(sourceFile, event):
    print("SourceFile: ")
    print(sourceFile)
    sourceFile.setEnabled(event["value"])
  
    
#Callback function which is called when there is a path configuration from MHC
def tcpipHttpNetWebServerMyPathVisible(sym, event):
    ORG_PATH = event["value"]
    webPagePathParsing(ORG_PATH)

tcpipHttpNetWebPageDirMyPath = tcpipHttpNetComponent.createStringSymbol("TCPIP_HTTP_NET_WEBPAGE_DIRECTORY_MYPATH", tcpipHttpNetCustTemplate)
tcpipHttpNetWebPageDirMyPath.setLabel("Configure Webpage directory path")
tcpipHttpNetWebPageDirMyPath.setVisible(False)
tcpipHttpNetWebPageDirMyPath.setDescription("Configure Webpage directory path")
tcpipHttpNetWebPageDirMyPath.setDefaultValue(Module.getPath() + "web_pages")
tcpipHttpNetWebPageDirMyPath.setDependencies(tcpipHttpNetWebServerMyPathVisible, ["TCPIP_HTTP_NET_WEBPAGE_DIRECTORY_PATH"])

def webPagePathParsing(path):
    import re
    import os
    import sys

    count = 0
    # Get the Root PATH 
    ORG_PATH = path
    clearFileSymbols()
    #split webpage curent web page path
    webPagePath = path.split("\\")
    #get the last directory from the given web page path
    lastDirectory = webPagePath[len(webPagePath)-1]
    #get the current root web page directory path
    webPageRootPath = ORG_PATH.split(lastDirectory)[0]
    print("webpageRootPath: ")
    print(webPageRootPath)
    
    for (root, dirs, fileNames) in os.walk(ORG_PATH):
        for fileName in fileNames:
            relativeFilePath = os.path.join(root,fileName)
            #file = file[file.find(ORG_PATH):]
            #Replace the module path from the Root path with empty string
            file = relativeFilePath.replace(webPageRootPath, "")
            sepWebpageDir = file[file.find(os.path.sep):]
            htmFile = sepWebpageDir.replace(os.path.sep, "",1)
            # Get the Webpage file symbol and each symbol is for the each file
            webpageListFile = createWebPageFileSymbol(count)
            # To allow the web pages outside the net repo
            webpageListFile.setRelative(False)
            #Set the source path
            webpageListFile.setSourcePath(relativeFilePath)
            webpageListFile.setOutputName(htmFile)
            fileList = file.split(os.path.sep)
            #set the destination path , the location where the webpage file will be copied
            destPath = ".."+os.path.sep+".."+os.path.sep+"web_pages"
            webpageListFile.setDestPath(destPath)
            fileList = fileList[0:len(fileList)-1]
            fileList[0] = "web_pages"
            folderPath = ""
            for fileStr in fileList:
                folderPath += fileStr+os.path.sep
            #set the project path , Webpage diretory will be added to the project   
            webpageListFile.setProjectPath(folderPath)
            webpageListFile.setType("SOURCE")
            webpageListFile.setMarkup(False)
            webpageListFile.setEnabled(True)
            print("SourceFile: " + htmFile)
            webpageListFile.setDependencies(webpageGenSourceFile, ["TCPIP_HTTP_NET_CUSTOM_TEMPLATE"])
            count += 1



#ORG_PATH = "../src/web_pages"
ORG_PATH = Module.getPath() + "web_pages"
tcpipHttpNetWebDirSymbol = tcpipHttpNetWebPageDirPath.getValue()
ORG_PATH = tcpipHttpNetWebDirSymbol
count = 0

mytcpipHttpNetComponent = tcpipHttpNetComponent
global symbolList
symbolList = []
fileCount = 0
global MAX_NUMBER_WEBPAGE_FILES
MAX_NUMBER_WEBPAGE_FILES = 100
del symbolList[:]

#Create MAX_NUMBER_WEBPAGE_FILES file symbols during the instantiation . 
#use of createFileSymbol() is not possible during the dynamic configuration of webPage path configuration
#So due to that we create a max number of webpage files during componet instatiation

for fileCount in range(MAX_NUMBER_WEBPAGE_FILES):
    webPageSymbolStr = "WEBPAGE_LIST_FILE"+str(fileCount)
    mySym = tcpipHttpNetComponent.createFileSymbol(webPageSymbolStr, None)
    mySym.setEnabled(False)
    symbolList.append(mySym)
    fileCount +=1
    

#default webapge path and diretory parsing
webPagePathParsing(ORG_PATH)
