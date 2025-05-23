/*********************************************************************
*
*  XML interface and MPFSConfigDetails.xml file creation.
*
**********************************************************************
* File Name:       xmlInterface.java
* Complier:        JAVA version "1.6.0_20 and onwards"
* IDE :            NetBeans
*/

/*
Copyright (C) 2012-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
*/


package MicrochipMPFS;


/**
 *
 * @author i00265
 */
import java.util.*;
import java.io.File;
//import java.io.FileNotFoundException;
//import java.io.FileOutputStream;
import java.io.IOException;
import javax.xml.parsers.*;
import javax.xml.transform.Transformer;
//import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
//import org.w3c.dom.Attr;
import org.w3c.dom.DOMException;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;


public class xmlInterface {
    private String errorMessage;
    //private Document document;
    private DocumentBuilderFactory docFactory;
    private DocumentBuilder docBuilder;
    private Document doc;
    private File xmlOutputFile;
    MainMPFS MPFS2;
    private NodeList currentNodeList;
    private int listCount;
    private int currentIteratorCount;
    private Node nodeIterator;
    private Node currentNode;
    private Vector<String> nodeValueList;
    private Vector<String> nodeNameList;
    private Vector<String> nodeAttrNameList;
    private Vector<String> nodeAttrValueList;

    

    xmlInterface(File XmlOuputFile,javax.swing.JFrame parent)
    {
        try
        {
            docFactory= DocumentBuilderFactory.newInstance();
            docFactory.setIgnoringComments(true);
            docFactory.setNamespaceAware(true);
            docFactory.setValidating(false);
            docBuilder = docFactory.newDocumentBuilder();
            doc = docBuilder.newDocument();
            xmlOutputFile = XmlOuputFile;
            MPFS2 = (MainMPFS)parent;
            nodeNameList = new Vector<String>();
            nodeValueList = new Vector<String>();
            nodeAttrNameList = new Vector<String>();
            nodeAttrValueList = new Vector<String>();
        }
        catch (ParserConfigurationException pce) {
		pce.printStackTrace();
        }
    }

    void defaultSettingXmlFileCreation()
    {
       // FileOutputStream fos;
        Transformer transformer;
        TransformerFactory transformerFactory;
      
        doc = docBuilder.newDocument();
        Element rootElement = doc.createElement("Settings");
        doc.appendChild(rootElement);

        Element SourceDirectory_new = doc.createElement("Setting");
        //SourceDirectory_new.appendChild(value);
        SourceDirectory_new.setAttribute("Name",
                (String)MPFS2.xmlAttrInfoMap.get(MainMPFS.eXmlNodeAttribute.SOURCE_DIRECTORY_PATH));
        SourceDirectory_new.setAttribute("Type", "String");
        //SourceDirectory_new.setAttribute("Scope", "User");
        Element value = doc.createElement("Value");
        //value.appendChild(doc.createTextNode("..//..//..//TCPIP//Demo App//WebPages2"));
        value.appendChild(doc.createTextNode(MPFS2.sourceDirectoryPath));
        SourceDirectory_new.appendChild(value);
        rootElement.appendChild(SourceDirectory_new);

        Element projectDirectory_new = doc.createElement("Setting");
        projectDirectory_new.setAttribute("Name",
                (String)MPFS2.xmlAttrInfoMap.get(MainMPFS.eXmlNodeAttribute.PROJECT_DIRECTORY_PATH));
        projectDirectory_new.setAttribute("Type", "String");
        value = doc.createElement("Value");
        value.appendChild(doc.createTextNode(MPFS2.projectDirectoryPath));
        projectDirectory_new.appendChild(value);
        rootElement.appendChild(projectDirectory_new);

        Element projectBinFile_new = doc.createElement("Setting");
        projectBinFile_new.setAttribute("Name",
                (String)MPFS2.xmlAttrInfoMap.get(MainMPFS.eXmlNodeAttribute.PROJECT_BIN_FILE_PATH));
        projectBinFile_new.setAttribute("Type", "String");
        value = doc.createElement("Value");
        value.appendChild(doc.createTextNode(MPFS2.projectBinFilePath));
        projectBinFile_new.appendChild(value);
        rootElement.appendChild(projectBinFile_new);


        Element radStartDirectory = doc.createElement("Setting");
        radStartDirectory.setAttribute("Name",
                (String)MPFS2.xmlAttrInfoMap.get(MainMPFS.eXmlNodeAttribute.START_DIRECTORY_RAD_BOTTON));
        radStartDirectory.setAttribute("Type","Boolean");
        value = doc.createElement("Value");
        value.appendChild(doc.createTextNode("True"));
        radStartDirectory.appendChild(value);
        rootElement.appendChild(radStartDirectory);

        Element generateImageName = doc.createElement("Setting");
        generateImageName.setAttribute("Name",
                (String)MPFS2.xmlAttrInfoMap.get(MainMPFS.eXmlNodeAttribute.IMAGE_NAME));
        generateImageName.setAttribute("Type","String");
        value = doc.createElement("Value");
        value.appendChild(doc.createTextNode("MPFSImg2"));
        generateImageName.appendChild(value);
        rootElement.appendChild(generateImageName);

        Element uploadImgAfterGenerate = doc.createElement("Setting");
        uploadImgAfterGenerate.setAttribute("Name",
                (String)MPFS2.xmlAttrInfoMap.get(MainMPFS.eXmlNodeAttribute.UPLOAD_BIN_IMAGE_CHKBOX));
        uploadImgAfterGenerate.setAttribute("Type","Boolean");
        value = doc.createElement("Value");
        value.appendChild(doc.createTextNode("True"));
        uploadImgAfterGenerate.appendChild(value);
        rootElement.appendChild(uploadImgAfterGenerate);

        Element outputImageFormat = doc.createElement("Setting");
        outputImageFormat.setAttribute("Name",
                (String)MPFS2.xmlAttrInfoMap.get(MainMPFS.eXmlNodeAttribute.OUTPUT_IMAGE_FORMAT_RAD_BOTTON));
        outputImageFormat.setAttribute("Type","Byte");
        value = doc.createElement("Value");
        value.appendChild(doc.createTextNode("0"));
        outputImageFormat.appendChild(value);
        rootElement.appendChild(outputImageFormat);

        Element uploadProtocol = doc.createElement("Setting");
        uploadProtocol.setAttribute("Name",
                (String)MPFS2.xmlAttrInfoMap.get(MainMPFS.eXmlNodeAttribute.UPLOAD_PROTOCOL));
        uploadProtocol.setAttribute("Type","Byte");
        value = doc.createElement("Value");
        value.appendChild(doc.createTextNode("1"));
        uploadProtocol.appendChild(value);
        rootElement.appendChild(uploadProtocol);

        Element dynamicFiles = doc.createElement("Setting");
        dynamicFiles.setAttribute("Name",
                (String)MPFS2.xmlAttrInfoMap.get(MainMPFS.eXmlNodeAttribute.DYNAMIC_FILES));
        dynamicFiles.setAttribute("Type","String");
        value = doc.createElement("Value");
        value.appendChild(doc.createTextNode("*.htm, *.html, *.cgi, *.xml"));
        dynamicFiles.appendChild(value);
        rootElement.appendChild(dynamicFiles);

        Element noCompressFiles = doc.createElement("Setting");
        noCompressFiles.setAttribute("Name",
                (String)MPFS2.xmlAttrInfoMap.get(MainMPFS.eXmlNodeAttribute.NO_COMPRESS_FILE_LIST));
        noCompressFiles.setAttribute("Type","String");
        value = doc.createElement("Value");
        value.appendChild(doc.createTextNode("snmp.bib,*.bin"));
        noCompressFiles.appendChild(value);
        rootElement.appendChild(noCompressFiles);

        Element uploadAddress = doc.createElement("Setting");
        uploadAddress.setAttribute("Name",
                (String)MPFS2.xmlAttrInfoMap.get(MainMPFS.eXmlNodeAttribute.UPLOAD_ADDRESS));
        uploadAddress.setAttribute("Type","String");
        value = doc.createElement("Value");
        value.appendChild(doc.createTextNode("MCHPBOARD"));
        uploadAddress.appendChild(value);
        rootElement.appendChild(uploadAddress);
        Element uploadPath = doc.createElement("Setting");
        uploadPath.setAttribute("Name",
                (String)MPFS2.xmlAttrInfoMap.get(MainMPFS.eXmlNodeAttribute.UPLOAD_IMAGE_PATH));
        uploadPath.setAttribute("Type","String");
        value = doc.createElement("Value");
        value.appendChild(doc.createTextNode("mpfsupload"));
        uploadPath.appendChild(value);
        rootElement.appendChild(uploadPath);
        Element uploadUserName = doc.createElement("Setting");
        uploadUserName.setAttribute("Name",
                (String)MPFS2.xmlAttrInfoMap.get(MainMPFS.eXmlNodeAttribute.UPLOAD_USER_NAME));
        uploadUserName.setAttribute("Type","String");
        value = doc.createElement("Value");
        value.appendChild(doc.createTextNode("admin"));
        uploadUserName.appendChild(value);
        rootElement.appendChild(uploadUserName);
        Element uploadUserPasswd = doc.createElement("Setting");
        uploadUserPasswd.setAttribute("Name",
                (String)MPFS2.xmlAttrInfoMap.get(MainMPFS.eXmlNodeAttribute.UPLOAD_PASSWORD));
        uploadUserPasswd.setAttribute("Type","String");
        value = doc.createElement("Value");
        value.appendChild(doc.createTextNode("microchip"));
        uploadUserPasswd.appendChild(value);
        rootElement.appendChild(uploadUserPasswd);
        
        Element unifiedStack = doc.createElement("Setting");
        unifiedStack.setAttribute("Name",
                (String)MPFS2.xmlAttrInfoMap.get(MainMPFS.eXmlNodeAttribute.UNIFIEDSTACK_USE));
        unifiedStack.setAttribute("Type","Boolean");        
        value = doc.createElement("Value");
        value.appendChild(doc.createTextNode("False"));
        unifiedStack.appendChild(value);
        rootElement.appendChild(unifiedStack);

        try
        {// File Saved
            transformerFactory = TransformerFactory.newInstance();
            transformer = transformerFactory.newTransformer();
            DOMSource source = new DOMSource(doc);
            StreamResult result = new StreamResult(xmlOutputFile);
            transformer.transform(source, result);
	} 
        catch (TransformerException tfe) 
        {
            tfe.printStackTrace();
        }
    }
// Node Name = " Setting" , Attr =  " DefaultSourceSetting ", Value = " Path "
    boolean modifyExistingXmlFile(String nodeName,String sAttr, String sValue)
    {
        Vector v_AttributeNames, v_AttributeValues;
        //FileOutputStream fos;
        Transformer transformer;
        TransformerFactory transformerFactory;
        
        if(xmlOutputFile.exists() == false)
            return false;
        try
        {
            doc = docBuilder.parse(xmlOutputFile);
        }
        catch (SAXException sae) 
        {
            sae.printStackTrace();
        }
        catch (IOException ioe) 
        {
            ioe.printStackTrace();
	}
        // get the Node element
        if(selectNodes(nodeName)!= true)
        {
            return false;
        }
        try
        {
            while(getNext() == true)
            {
                v_AttributeNames=getNodeAttrNameList();
                v_AttributeValues=getNodeAttrValueList();
                for(int iLoop=0;iLoop<v_AttributeNames.size();iLoop++){
                    if((v_AttributeNames.get(iLoop).toString().compareToIgnoreCase("Name")==0)
                            && (v_AttributeValues.get(iLoop).toString().compareToIgnoreCase(sAttr)==0))
                        if(setChildNode("Value", sValue) != true)
                        {
                            return false;
                        }
                        else
                        {
                            try
                            {
                            // write the content into xml file
                               // fos = new FileOutputStream(xmlOutputFile);
                                transformerFactory = TransformerFactory.newInstance();
                                transformer = transformerFactory.newTransformer();
                                DOMSource source = new DOMSource(doc);
                                StreamResult result = new StreamResult(xmlOutputFile);

                                // Output to console for testing
                                // StreamResult result = new StreamResult(System.out);

                                transformer.transform(source, result);

                                //System.out.println("File saved!");
                            }
                            catch (TransformerException tfe)
                            {
                                    tfe.printStackTrace();
                            }
                            
                            return true;
                        }
                    
                }
            }
        }catch(Exception e){
          //  JOptionPane.showMessageDialog(null, "processNodeInfo failed", "System Alert:", JOptionPane.INFORMATION_MESSAGE);
            return false;
        }  
       // Node rootSettings = doc.getFirstChild();
        return true;
    }

    public boolean setChildNode(String childNodeName, String childNodeValue)
    {
        try
        {
            NodeList nodeList;
            nodeList = currentNode.getChildNodes();
            for (int jLoop = 0;jLoop<nodeList.getLength();jLoop++)
            {
                if(nodeList.item(jLoop).getNodeType() == nodeList.item(jLoop).ELEMENT_NODE)
                {
                    if(nodeList.item(jLoop).getNodeName().compareToIgnoreCase(childNodeName)==0){
                        Node curChildNode=nodeList.item(jLoop);
                        curChildNode.setTextContent(childNodeValue);
                    }
                }
            }
        }
        catch(DOMException ex)
        {
            errorMessage = ex.getMessage();
            return false;
        }
        return true;  
     }
    public boolean getNext()
    {
        if(currentIteratorCount >= currentNodeList.getLength())
        {
            return false;
        }
        if(currentNodeList.item(currentIteratorCount).getNodeType() == currentNodeList.item(currentIteratorCount).ELEMENT_NODE)
        {
            nodeIterator = currentNodeList.item(currentIteratorCount);
            currentNode = nodeIterator;
            if(nodeIterator.hasChildNodes())
            {
                NodeList nodeList;

                nodeAttrNameList.clear();
                nodeAttrValueList.clear();
                nodeNameList.clear();
                nodeValueList.clear();

                nodeList = nodeIterator.getChildNodes();
                for (int j = 0;j<nodeList.getLength();j++)
                {
                    if(nodeList.item(j).getNodeType() == nodeList.item(j).ELEMENT_NODE)
                    {
                        nodeNameList.add(nodeList.item(j).getNodeName());
                        nodeValueList.add(nodeList.item(j).getTextContent());
                    }
                }
                //Attribute List
                if(nodeIterator.hasAttributes())
                {
                    NamedNodeMap nm = nodeIterator.getAttributes();
                    for (int j = 0;j<nm.getLength();j++)
                    {
                        if(nm.item(j).getNodeType() == nm.item(j).ATTRIBUTE_NODE)
                        {
                            nodeAttrNameList.add(nm.item(j).getNodeName());
                            nodeAttrValueList.add(nm.item(j).getTextContent());
                        }
                    }
                }
                nodeIterator = nodeIterator.getNextSibling();
            }
        }
        currentIteratorCount++;
        return true;
     }
    /**
      getNodeValueList: Returns the Node Value List
      */
     public Vector getNodeValueList()
     {
        return nodeValueList;
     }
/**
      getNodeValueList: Returns the Node Name List
      */
     public Vector getNodeNameList()
     {
        return nodeNameList;
     }
      /**
      getNodeValueList: Returns the Node Attribute Name List
      */
     public Vector getNodeAttrNameList()
     {
        return nodeAttrNameList;
     }

     public Vector getNodeAttrValueList()
     {
        return nodeAttrValueList;
     }
// This method will validate the XML file for a perticular node and collect all the node value and attribute.
     // And update the global variable.
    public boolean validateSettingsNodeInfo(String nodeName)
    {
        Vector v_AttributeNames, v_AttributeValues;
        Vector v_NodeName,v_NodeValue;
        String sName,sValue;
        String sType;
        try
        {
            doc = docBuilder.parse(xmlOutputFile);
        }
        catch (SAXException sae)
        {
            sae.printStackTrace();
        }
        catch (IOException ioe)
        {
            ioe.printStackTrace();
	}
        try
        {
            sName=new String("");
            sValue=new String("");
            sType = new String("");
            if(selectNodes(nodeName) != true)
            {
                return false;
            }
            while(getNext()==true)
            {
                v_AttributeNames  = getNodeAttrNameList();
                v_AttributeValues = getNodeAttrValueList();
                v_NodeName = getNodeNameList();
                v_NodeValue = getNodeValueList();

                // check the number of attributes
                //if(v_AttributeNames.size() > 3)
                  //  return false;
                for(int iLoop=0;iLoop<v_AttributeNames.size();iLoop++){
                    if(v_AttributeNames.get(iLoop).toString().compareToIgnoreCase("Name")==0){
                        sValue=v_AttributeValues.get(iLoop).toString();
                    }
                   
                    // check First attribute of the Node  tag "Setting" 
                    if(sValue.equals(MPFS2.XMLNodeSettingAttribute[currentIteratorCount-1]) != true)
                    {
                        return false;
                    }

                    if(v_AttributeNames.get(iLoop).toString().compareToIgnoreCase("Type")==0){
                        sType = v_AttributeValues.get(iLoop).toString();
                    
                        if((sType.equals("String") != true) && (sType.equals("Byte") != true)&&
                            (sType.equals("Boolean") != true))
                        {
                            return false;
                        }
                    }
                }
            }
            return true;
        }catch(Exception e){
            //JOptionPane.showMessageDialog(null, "processNodeInfo failed", "System Alert:", JOptionPane.INFORMATION_MESSAGE);
            return false;
        }
       // return true;
    }

    // This method will validate the XML file for a perticular node and collect all the node value and attribute.
     // And update the global variable.
    public boolean loadSettingsNodeInfo(String nodeName)
    {
        Vector v_AttributeNames, v_AttributeValues;
        Vector v_NodeName,v_NodeValue;
        String sName,sValue;
        String sType;
        int nIteration = 0;
        boolean nodeValue = true;
        int bNodeValue=1;
        int iLoop = 0;
        try
        {
            doc = docBuilder.parse(xmlOutputFile);
        }
        catch (SAXException sae)
        {
            sae.printStackTrace();
        }
        catch (IOException ioe)
        {
            ioe.printStackTrace();
	}
        try
        {
            sName=new String("");
            sValue=new String("");
            sType = new String("");
            if(selectNodes(nodeName) != true)
                return false;
            while(getNext()==true)
            {
                v_AttributeNames  = getNodeAttrNameList();
                v_AttributeValues = getNodeAttrValueList();
                v_NodeName = getNodeNameList();
                v_NodeValue = getNodeValueList();
                // Attribute name
                sName = MPFS2.XMLNodeSettingAttribute[nIteration];
                sType = v_AttributeValues.get(iLoop+1).toString();
                sValue = v_NodeValue.get(iLoop).toString();
                switch(nIteration)
                {
                    case 0 : //SOURCE_DIRECTORY_PATH
                        if(sType.contains("String"))
                        {
                            MPFS2.sourceDirectoryPath = v_NodeValue.get(iLoop).toString();
                        }
                        else
                        {
                            return false;
                        }
                        break;
                    case 1: //PROJECT_DIRECTORY_PATH
                        if(sType.contains("String"))
                        {
                            MPFS2.projectDirectoryPath = v_NodeValue.get(iLoop).toString();
                        }
                        else
                        {
                            return false;
                        }
                        break;
                    case 2:  //PROJECT_BIN_FILE_PATH
                        if(sType.contains("String"))
                        {
                            MPFS2.projectBinFilePath = v_NodeValue.get(iLoop).toString();
                        }
                        else
                        {
                            return false;
                        }
                        break;
                    case 3:  //START_DIRECTORY_RAD_BOTTON
                        if(sType.contains("Boolean"))
                        {
                            if(sValue.contains("True"))
                            {
                                nodeValue = true;
                            }
                            else
                            {
                                nodeValue = false;
                            }
                            MPFS2.setConfiguration(nodeValue, sValue, bNodeValue, nIteration);//.setConfiguration(nodeValue,"",0,3);
                        }
                        break;
                    case 4:  // IMAGE_NAME
                        if(sType.contains("String"))
                        {
                            MPFS2.setConfiguration(nodeValue, sValue, bNodeValue, nIteration);
                        }
                        else
                        {
                            return false;
                        }
                        break;
                    case 5:  // UPLOAD_BIN_IMAGE_CHKBOX
                        if(sType.contains("Boolean"))
                        {
                            if(sValue.contains("True"))
                            {
                                nodeValue = true;
                            }
                            else
                            {
                                nodeValue = false;
                            }
                            MPFS2.setConfiguration(nodeValue, sValue, bNodeValue, nIteration);
                        }
                        break;
                    case 6:  // OUTPUT_IMAGE_FORMAT_RAD_BOTTON
                        if(sType.contains("Byte"))
                        {
                            bNodeValue = Integer.parseInt(sValue);
                            if(bNodeValue < 0 && bNodeValue > 3)
                            {
                                return false;
                            }
                            MPFS2.setConfiguration(nodeValue, sValue, bNodeValue, nIteration);
                        }
                        break;
                    case 7:  // UPLOAD_PROTOCOL
                       if(sType.contains("Byte"))
                        {
                            bNodeValue = Integer.parseInt(sValue);
                            if(bNodeValue < 1 && bNodeValue > 2)
                            {
                                return false;
                            }
                            MPFS2.setConfiguration(nodeValue, sValue, bNodeValue, nIteration);
                        }
                        break;
                    case 8:  // DYNAMIC_FILES
                    case 9: // NO_COMPRESS_FILE_LIST
                    case 10: // UPLOAD_ADDRESS
                    case 11: // UPLOAD_IMAGE_PATH
                    case 12: // UPLOAD_USER_NAME
                    case 13: // UPLOAD_PASSWORD
                        if(sType.contains("String"))
                        {
                            MPFS2.setConfiguration(nodeValue, sValue, bNodeValue, nIteration);
                        }
                        else
                        {
                            return false;
                        }
                        break;
                    case 14: //UNIFIEDSTACK_USE
                        if(sType.contains("Boolean"))
                        {
                            if(sValue.contains("True"))
                            {
                                nodeValue = true;
                            }
                            else
                            {
                                nodeValue = false;
                            }
                            MPFS2.setConfiguration(nodeValue, sValue, bNodeValue, nIteration);
                        }
                        break;
                    default :
                        return false;
                }
                nIteration = nIteration+1;               
            }
            return true;
        }catch(Exception e){
            //JOptionPane.showMessageDialog(null, "processNodeInfo failed", "System Alert:", JOptionPane.INFORMATION_MESSAGE);
            return false;
        }
       // return true;
    }
     /**
      selectNodes: User wants to select a node before each operation.This function will
       * load the Node Names,Node Values,Attribute Names,Attribute Values in the Vector
       * initialized using the initialize function.Before each opertion on a node user
       * should call this function.
      */
    // nodeName  == "Setting"
     public boolean selectNodes(String nodeName)
     {
        //nodeNameList.clear();
        //nodeValueList.clear();
        //nodeAttrNameList.clear();
        //nodeAttrValueList.clear();
        currentNodeList = doc.getElementsByTagName(nodeName);
        //currentNode = currentNodeList.item(0);
        listCount = currentNodeList.getLength();
        try
        {
            currentIteratorCount = 0;
        }
        catch(DOMException ex)
        {
            errorMessage = ex.getMessage();
        }
        if(listCount>0)
            return true;
        else
            return false;
    }

}
