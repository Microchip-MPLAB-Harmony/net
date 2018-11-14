
/*********************************************************************
*
*  TCPIPDiscoverer UI with a UDP socket( 30303) to transmit IPV4 broadcast
*  over the network.
*
**********************************************************************
* FileName:        TcpIpDiscoverTool.java
* Complier:        JAVA version "1.6.0_20 and onwards"
* IDE :            Netbean
* Software License Agreement
*
* Copyright (C) 2012 Microchip Technology Inc.  All rights reserved.
*
* Microchip licenses to you the right to use, modify, copy, and
* distribute the Software when used with a Microchip microcontroller or
* digital signal controller product which is integrated into Licensee's product.
*
* You should refer to the license agreement accompanying this
* Software for additional information regarding your rights and
* obligations.
*
* THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
* WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
* LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
* MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
* CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
* PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
* BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
* THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
* SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
* (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE
**********************************************************************/
package TCPIP_Discoverer;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.*;
import java.awt.*;
import javax.swing.tree.DefaultTreeModel;
import java.net.*;
import java.util.concurrent.locks.*;
import java.io.*;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Enumeration;
import javax.swing.tree.TreePath;


public class TcpIpDiscoverTool extends javax.swing.JFrame {
    // private	javax.swing.JPanel		topPanel;
     private	JTree		tree;
     private	JScrollPane scrollPane;
     private DatagramSocket socket;
     private DatagramPacket packet;
     private Thread recvThread=null;
    //        private Thread manageTree=null;
     private String mchipUrl;
     TCPIPAboutBox aboutBox;
     String discvStr = new String("Discovery: Who is out there?\0\n");
     String received = " ";
     long threadSleep = 500;
     ReentrantLock lock = new ReentrantLock();
     ImageIcon icon = new ImageIcon(getClass().getResource
             ("/Resource/mchpIcon.png"));
     // Tree variables
     DefaultMutableTreeNode root;
     DefaultMutableTreeNode harmonyNode,mlaNode;
     DefaultTreeModel model;
     short gSegFlag = 0;
     int clickCnt=0;

     static String[] OUI_Microchip_byte= 
     {          
         "00-04-A3",  
         "00-1E-C0",
         "D8-80-39",
         "54-10-EC",
         "FC-C2-3D", // Atmel
         "00-04-25", // Atmel OUI number
     };

     public enum ANNOUNCE_SOCKET_BCAST_TYPE
     {
         ANNOUNCE_UDP_BCAST_NONE,                    // no broadcast
         ANNOUNCE_UDP_BCAST_NETWORK_LIMITED,         // network limited broadcast
         ANNOUNCE_UDP_BCAST_NETWORK_DIRECTED,        // network directed broadcast

     }
     
     ANNOUNCE_SOCKET_BCAST_TYPE annce_bcast_type;
    /** Creates new form TcpIpDiscoverTool */
    public TcpIpDiscoverTool() {
        initComponents();
        aboutBox = new TCPIPAboutBox(this,true);
        jPanel1.setSize(800, 400);
        this.setIconImage(icon.getImage());
        UIManager.put("Button.defaultButtonFollowsFocus", Boolean.TRUE);
        String aboutStr = "<html>"+"Date  "+" - Nov,12 2018"+"<br>";
        String version = "Version"+" - TCPIP Discoverer 3.0";
        lebelAbout.setText(aboutStr+version+"</html>");
                
        // tree initialization
        root = new DefaultMutableTreeNode("Microchip Devices");
        model = new DefaultTreeModel(root);
        // Create a new tree control
        tree = new JTree(root);
        scrollPane = new JScrollPane();
        scrollPane.getViewport().add( tree );
        jPanel1.setLayout( new BorderLayout() );
 
        //basicTreeExample();
        udpAction();
        jPanel1.add( scrollPane, BorderLayout.CENTER );

        // mouse event for web browser
        tree.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                treeMouseClicked(evt);
            }
        });
        this.pack();       
    }
    /*
     * this method is used to get the microchip TCPIP web browser,
     * when someone will do a double click to the MAC address node of the tree.
     */
    void  treeMouseClicked(java.awt.event.MouseEvent evt)
    {
        //DefaultTreeCellRenderer renderer = ( DefaultTreeCellRenderer ) tree.getCellRenderer();
        clickCnt = clickCnt + evt.getClickCount();
        if(clickCnt == 2)
        {
            DefaultMutableTreeNode parentNode = null, ipNode =null;
            String ipv4Str,MacAddress;

            TreePath parentPath = tree.getSelectionPath();
            if(parentPath == null)
                return;
            // expect MAC address Node
            parentNode = (DefaultMutableTreeNode)
                         (parentPath.getLastPathComponent());

            MacAddress = parentNode.toString();
            if(MacAddress.startsWith("MAC-Address") == false)
                return;
           // renderer.setToolTipText(" Double Click  ");
            if(parentNode != null)
            {
                ipNode = findNode("IP Addresses", parentNode);
                if(ipNode != null)
                {
                    DefaultMutableTreeNode ipAddress = (DefaultMutableTreeNode)ipNode.getChildAt(0);
                    ipv4Str = ipAddress.toString();
                    mchipUrl = (String)"http://"+ipv4Str;
                     try{
                        URL myUrl = new URL(mchipUrl);
                        Desktop.getDesktop().browse(myUrl.toURI());
                    }catch (Exception ex) {}
                }
            }

        }
        clickCnt = 0;
    }

    /* Receive thread */
    public void receiveUDPPacket()
    {
        try
        {
            byte[] buf = new byte[1500-28];
            //String tempStr = Byte.toString(tempBufPak);
            for(int i=0;i<buf.length;i++)
                buf[i]=' ';
            packet = new DatagramPacket(buf, buf.length);

            //ignore first packet which is a loopback one;
            socket.receive(packet);

            received = new String(packet.getData());
            for(;;)
            {
                for(int i=0;i<buf.length;i++)
                buf[i]=' ';
                packet = new DatagramPacket(buf, buf.length);

                try{
                socket.receive(packet);
                }
                catch(SocketTimeoutException  e){
                 // System.out.println("lengthcatch"+packet.getLength());
                }
                // * */
                if(new String(packet.getData()).trim().length() == 0)
                    continue;
                if(packet.getLength() < 35)
                {
                    continue;
                }

               // convert byte array to string.
              //  received = new String(packet.getData());

                lock.lock();
                try {
                    addDiscoveryEntry(packet);
                    received = " ";
                    model.reload();
                    jPanel1.revalidate();
                    SwingUtilities.invokeLater(new Runnable() {

                        public void run() {
                            tree.updateUI();
                            for (int i=0; i < tree.getRowCount(); i++) {
                                tree.expandRow(i);
                            }
                        }
                    });
                } finally {
                    lock.unlock();
                }
            }
        }
        catch (IOException e)
        {
            close_action();
        }
    }
     // create dtatgram socket and send the broadcast packet to all the listeners
    public void udpAction()
    {
        try
        {
            recvThread = new Thread()
            {
                public void run()
                {
                    receiveUDPPacket();
                }
            };

            socket = new DatagramSocket(30303);
            socket.setBroadcast(true);
            ArrayList<NetworkInterface> interfaces = Collections.list(NetworkInterface.getNetworkInterfaces());
            int mask = 0;
            InetAddress ipAddr2;
            byte[] result = new byte[4];
            byte[] netmask = new byte[4];
            byte[] ipAddrs;
            if(annce_bcast_type == ANNOUNCE_SOCKET_BCAST_TYPE.ANNOUNCE_UDP_BCAST_NETWORK_DIRECTED)
            {
                for(NetworkInterface iface : interfaces) 
                {
                    // check if the interface is Up or down
                    if(iface.isUp())
                    {
                        // Interface addresses of the network interface
//                        System.out.println(iface.getDisplayName());
                        for (InterfaceAddress addr : iface.getInterfaceAddresses()) 
                        {
                            InetAddress i = addr.getAddress();
                            if(i instanceof Inet4Address && !i.isLoopbackAddress())
                            {
                                ipAddrs = i.getAddress();
                                if(addr.getNetworkPrefixLength() != -1)
                                {
                                    mask = 0xffffffff << (32 - addr.getNetworkPrefixLength());
                                }
                                else
                                {
                                    // net mask will be 255.255.255.255 if getNetworkPrefixLength()
                                    // returns -1. This is seen for Wifi interface.
                                   // mask = 0xFFFFFFFF;
                                    continue;
                                }
                                netmask[0] = (byte) (mask >> 24);
                                netmask[1] = (byte) (mask >> 16);
                                netmask[2] = (byte) (mask >> 8);
                                netmask[3] = (byte) (mask /*>> 0*/);

                                result[0] = (byte) (ipAddrs[0] | (~netmask[0]));
                                result[1] = (byte) (ipAddrs[1] | (~netmask[1]));
                                result[2] = (byte) (ipAddrs[2] | (~netmask[2]));
                                result[3] = (byte) (ipAddrs[3] | (~netmask[3]));
                                ipAddr2 = InetAddress.getByAddress(result);

//                                System.out.println("interface address: "+ addr.getAddress());
//                                System.out.println("broadcast address: "+ addr.getBroadcast().getHostName());
//                                System.out.println("broadcast string address: "+ addr.getBroadcast());
//                                System.out.println("prefixlen: "+ addr.getNetworkPrefixLength());
//                                System.out.println(ipAddr2.getHostAddress());
                                packet = new DatagramPacket(discvStr.getBytes(),discvStr.length(),
                                ipAddr2, 30303);
                                socket.send(packet);
                            }
                        }
                        
                    }
                }
            }
            else
            {
//                for(NetworkInterface iface : interfaces) 
//                {
//                    // check if the interface is Up or down
//                    if(iface.isUp())
//                    {
//                        // Interface addresses of the network interface
//                        System.out.println(iface.getDisplayName());
//                        for (InterfaceAddress addr : iface.getInterfaceAddresses()) 
//                        {
//                            InetAddress i = addr.getAddress();                        
//                            if(i instanceof Inet4Address && !i.isLoopbackAddress())
//                            {
//                                InetAddress bcast_address = addr.getBroadcast();
//                                System.out.println("interface address: "+ addr.getAddress());
//                                System.out.println("broadcast address: "+ addr.getBroadcast().getHostName());
//                                System.out.println("broadcast string address: "+ addr.getBroadcast());
//                                packet = new DatagramPacket(discvStr.getBytes(),discvStr.length(),bcast_address, 30303);
//                                socket.send(packet);
//                            }
//                        }
//                    }
//                }
                InetAddress bcast_address = InetAddress.getByName("255.255.255.255");
                packet = new DatagramPacket(discvStr.getBytes(),discvStr.length(),bcast_address, 30303);
                socket.send(packet);
            }
            recvThread.start();           
        }
        catch(IOException e)
        {
            String optionStr = "<html>"+"Unable to transmit discovery message. <br> Check network " +
                    "connectivity and ensure that no other instances of this " +
                    "program are running.";

            JOptionPane.showMessageDialog(null,optionStr,
                            "Error", JOptionPane.ERROR_MESSAGE);
            close_action();
            System.exit(0);
        }
    }

    public void close_action()
    {
        if(socket != null)
        {
            if(socket.isConnected() == true)
            {
                socket.close();
            }
        }
        if(recvThread != null)
        {
            if(recvThread.isAlive() == true)
            {
                recvThread = null;
            }
        }

    }

    /*
     *  This method is used to collect the received byte[] and parse
     *  whole packet before inserting into jTree.
     */
 public void addDiscoveryEntry(DatagramPacket receivedPacket)
 {
    byte[] paktBuf = receivedPacket.getData();
    int packetLen = receivedPacket.getLength();
    int dataCnt=0;
    String macAddress = "MAC-Address - ";
    String macType = "MAC-Type";
    String hostName = "Hostname";
    String ipAddr = "IP Addresses";
    String multicastAddr = "Multicast Listeners";
    String defaultRouter = "Default Router";
    String defaultGateway = "Default Gateway";
    String harmonyNodeName = "Harmony";
    String message = "User Data";
    String msgBuf;
    byte[] temp ;
    int    i = 0;
    DefaultMutableTreeNode macNode=null,multicastListenerNode=null,ipNode=null,messageNode=null;
    DefaultMutableTreeNode macTypeNode=null,netBiosNode=null;
    String ipv6AddressStr;
    byte[] multicastListener = 
        {(byte)0xFF,(byte)0x02,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,
         (byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x01,(byte)0xFF};

    for(dataCnt=0;dataCnt<packetLen; )
    {
        switch(paktBuf[dataCnt])
        {
            case 0x01: // truncated
            // add a message to the tree
                insertNode("Truncated",macNode,macNode.getChildCount());
                return;
              //  break;
            case 0x02: // MAC address
                temp =  new byte[6];
                dataCnt++;
                for (i = 0; i < 6; i++)
                {
                    temp[i] = paktBuf[dataCnt++];
                }

                if(temp.length == 0)
                    break;
                // populate Harmony devices node
                harmonyNode = findNode(harmonyNodeName,root);
                if(harmonyNode == null)
                {
                    insertNode(harmonyNodeName,root,root.getChildCount());
                    harmonyNode = findNode(harmonyNodeName,root);
                }
                // MAC address string construction
                StringBuilder sb = new StringBuilder(22);
                for (byte b : temp) {
                    if (sb.length() > 0)
                        sb.append(':');
                    sb.append(String.format("%02x", b));
                }
                macAddress = macAddress.concat(sb.toString());
                macNode = findNode(macAddress,harmonyNode);
                if(macNode == null)
                {
                    //removeNodeFromRoot(macNode);
                    insertNode(macAddress,harmonyNode,harmonyNode.getChildCount());
                    macNode = findNode(macAddress,harmonyNode);
                }
                break;
            case 0x03: // MAC Type
                if((macNode == null) || (harmonyNode == null))
                {// shift filed terminator ( 0x0D, 0x0A)
                   break;
                }
                dataCnt++; // skip first byte for module information
                //sb = new StringBuilder(22);
                temp =  new byte[16];
                i = 0;
                while((paktBuf[dataCnt] != 0x0d) && (paktBuf[dataCnt+1] != 0x0a))
                {
                    temp[i++] = paktBuf[dataCnt++];
                }

                temp[i] = '\n';
                String byteToStr = new String(temp);
                byteToStr = byteToStr.substring(0,i);
                macTypeNode = findNode(macType,macNode);
                if(macTypeNode == null)
                {
                    insertNode(macType,macNode,macNode.getChildCount());
                    macTypeNode = findNode(macType,macNode);
                }
                //insert Mactype string to the Mactype path
                if(findNode(byteToStr,macTypeNode)== null)
                {
                    insertNode(byteToStr,macTypeNode,macTypeNode.getChildCount());
                }               
                break;
            case 0x04: // Host Name
                if((macNode == null) || (harmonyNode == null))
                {// shift filed terminator ( 0x0D, 0x0A)
                   break;
                }
                dataCnt++;
                //sb = new StringBuilder(22);
                temp =  new byte[16]; i = 0;
                while((paktBuf[dataCnt] != 0x0d) && (paktBuf[dataCnt+1] != 0x0a))
                {
                    temp[i++] = paktBuf[dataCnt++];
                }
                byteToStr = new String(temp);
                byteToStr = byteToStr.substring(0,i);
                netBiosNode =  findNode(hostName,macNode);
                //hostName = hostName.concat(byteToStr);
                 // insert Hostname after comparing "Hostname" string
                if(netBiosNode == null)
                {
                    insertNode(hostName,macNode,macNode.getChildCount());
                    netBiosNode =  findNode(hostName,macNode);
                }
                //insert real Hostname string to the Hostname path
                if(findNode(byteToStr,netBiosNode) == null)
                {
                    insertNode(byteToStr,netBiosNode,netBiosNode.getChildCount());
                }
                break;
            case 0x05: // IPv4 Address
                if((macNode == null) || (harmonyNode == null))
                {
                    break;
                }
                temp =  new byte[4];
                dataCnt++;
                i = 0;
                while(i < 4)
                {
                    temp[i++] = paktBuf[dataCnt++];
                }

                if(temp.length == 0)
                    break;
                // MAC address string construction
                sb = new StringBuilder(22);
                for (byte b : temp) {
                    if (sb.length() > 0)
                        sb.append('.');
                    sb.append(String.format("%d",(short) (b & 0xFF)));
                }
                ipNode = findNode(ipAddr,macNode);
                if(ipNode == null)
                {
                    insertNode(ipAddr,macNode,macNode.getChildCount());
                    ipNode = findNode(ipAddr,macNode);
                }
                if(findNode(sb.toString(),ipNode) == null)
                {
                    insertNode(sb.toString(),ipNode,ipNode.getChildCount());
                }
                break;
            case 0x06: // IPv6U address
                if((macNode == null) || (harmonyNode == null))
                {
                    break;
                }
                temp =  new byte[16];
                dataCnt++;
                i = 0;
                while(i < 16)
                {
                    temp[i++] = paktBuf[dataCnt++];
                }

                if(temp.length == 0)
                    break;
                // MAC address string construction
                ipv6AddressStr = ipv6NumericToTextFormat(temp);
                ipNode = findNode(ipAddr,macNode);
                if(ipNode == null)
                {
                    insertNode(ipAddr,macNode,macNode.getChildCount());
                    ipNode = findNode(ipAddr,macNode);
                }
                if(findNode(ipv6AddressStr,ipNode) == null)
                {
                    insertNode(ipv6AddressStr,ipNode,ipNode.getChildCount());
                    for(i=0;i<multicastListener.length;i++)
                    {
                        temp[i] = multicastListener[i];
                    }
                    // byte13,byte 14 and byte 15  are considered as the prebious one
                    ipv6AddressStr = ipv6NumericToTextFormat(temp);
                    multicastListenerNode = findNode(multicastAddr,macNode);
                    if(multicastListenerNode == null)
                    {
                        insertNode(multicastAddr,macNode,macNode.getChildCount());
                        multicastListenerNode = findNode(multicastAddr,macNode);
                    }
                    if(findNode(ipv6AddressStr,multicastListenerNode) == null)
                    {
                        insertNode(ipv6AddressStr,multicastListenerNode,multicastListenerNode.getChildCount());
                    }
                }
                break;
            case 0x07 : // IPv6M Address
                if((macNode == null) || (harmonyNode == null))
                {
                    break;
                }
                temp =  new byte[16];
                dataCnt++;
                i = 0;
                while(i < 16)
                {
                    temp[i++] = paktBuf[dataCnt++];
                }

                if(temp.length == 0)
                    break;
                // MAC address string construction
                ipv6AddressStr = ipv6NumericToTextFormat(temp);
                multicastListenerNode = findNode(multicastAddr,macNode);
                if(multicastListenerNode == null)
                {
                    insertNode(multicastAddr,macNode,macNode.getChildCount());
                    multicastListenerNode = findNode(multicastAddr,macNode);
                }
                if(findNode(ipv6AddressStr,multicastListenerNode) == null)
                {
                    insertNode(ipv6AddressStr,multicastListenerNode,multicastListenerNode.getChildCount());
                }
                break;
            case 0x08: // IPv6Deafult Router address
                if((macNode == null) || (harmonyNode == null))
                {
                    break;
                }
                temp =  new byte[16];
                dataCnt++;
                i = 0;
                while(i < 16)
                {
                    temp[i++] = paktBuf[dataCnt++];
                }

                if(temp.length == 0)
                    break;
                // MAC address string construction
                ipv6AddressStr = ipv6NumericToTextFormat(temp);
                ipNode = findNode(defaultRouter,macNode);
                if(ipNode == null)
                {
                    insertNode(defaultRouter,macNode,macNode.getChildCount());
                    ipNode = findNode(defaultRouter,macNode);
                }
                if(findNode(ipv6AddressStr,ipNode) == null)
                {
                    insertNode(ipv6AddressStr,ipNode,ipNode.getChildCount());
                    for(i=0;i<multicastListener.length;i++)
                    {
                        temp[i] = multicastListener[i];
                    }
                    // byte13,byte 14 and byte 15  are considered as the prebious one
                    ipv6AddressStr = ipv6NumericToTextFormat(temp);
                    multicastListenerNode = findNode(multicastAddr,macNode);
                    if(multicastListenerNode == null)
                    {
                        insertNode(multicastAddr,macNode,macNode.getChildCount());
                        multicastListenerNode = findNode(multicastAddr,macNode);
                    }
                    if(findNode(ipv6AddressStr,multicastListenerNode) == null)
                        insertNode(ipv6AddressStr,multicastListenerNode,multicastListenerNode.getChildCount());
                }
                break;
             case 0x09: // IPv6Deafult Gateway address
                if((macNode == null) || (harmonyNode == null))
                {
                    break;
                }
                temp =  new byte[16];
                dataCnt++;
                i = 0;
                while(i < 16)
                {
                    temp[i++] = paktBuf[dataCnt++];
                }

                if(temp.length == 0)
                    break;
                // MAC address string construction
                ipv6AddressStr = ipv6NumericToTextFormat(temp);
                ipNode = findNode(defaultGateway,macNode);
                if(ipNode == null)
                {
                    insertNode(defaultGateway,macNode,macNode.getChildCount());
                    ipNode = findNode(defaultGateway,macNode);
                }
                if(findNode(ipv6AddressStr,ipNode) == null)
                {
                    insertNode(ipv6AddressStr,ipNode,ipNode.getChildCount());                   
                }
                break;
                 
             default:
                 addEntryForMLAAnnoucePacket(receivedPacket);
                 return;
        }
        if(paktBuf[dataCnt] == 0x0d)
            dataCnt++;
        if(paktBuf[dataCnt] == 0x0a)
            dataCnt++;
        // this is for the " user data " section and it looks for both 0xd and 0xa
        if((paktBuf[dataCnt] == 0x0d) && (paktBuf[dataCnt+1] == 0x0a))
        {
            messageNode = findNode(message,macNode);
            if(messageNode == null)
            {
                insertNode(message,macNode,macNode.getChildCount());
                messageNode = findNode(message,macNode);
            }
            // update the data count.
            dataCnt = dataCnt+2;
            temp =  new byte[100];
            i = 0;
            // collect all the packet bytes into the temp buffer
            while(dataCnt < packetLen)
            {
                temp[i++] = paktBuf[dataCnt++];
                // break from the loop if the number of characters are more than 100 bytes.
                if(i==100)
                {
                    break;
                }
            }
            //msgBuf = temp.toString();
            try {
                    msgBuf = new String(temp,"UTF-8"); // Best way to decode using "UTF-8"
		    //System.out.println(msgBuf);
                    if(findNode(msgBuf,messageNode) == null)
                    {
                        insertNode(msgBuf,messageNode,messageNode.getChildCount());
                    }
            } catch (UnsupportedEncodingException e){} 
        }
    }
 }
 
 public boolean addEntryForMLAAnnoucePacket(DatagramPacket receivedPacket)
 {
    String packetString = new String(receivedPacket.getData());
    String[] packetBuf = packetString.split("\r\n");
    InetAddress ipAddres = receivedPacket.getAddress();
    String ipAddrStr = ipAddres.toString();
    DefaultMutableTreeNode macNode=null,ipNode=null,messageNode=null;
    String mlaNodeName = "Microchip Legacy Library";
    String macAddress = "MAC-Address - ";
    String hostName = "Hostname - ";
    String ipAddr = "IP Addresses";
    String message = "Message";
    int i;
    if(!ipAddrStr.isEmpty())
    {
        ipAddrStr = ipAddrStr.replace("/","");
    }
    else
    {
        return false;
    }
    if(packetBuf[0].isEmpty())
    {
        return false;
    }    

    hostName = hostName.concat(packetBuf[0]);
    // Mac address
    if(packetBuf[1].isEmpty())
    {
        return false;
    }
    // validate the MAC address with Microchip OUI number
    // and it should match to be part of the root tree
    for(i=0;i<OUI_Microchip_byte.length;i++)
    {
        if(packetBuf[1].contains(OUI_Microchip_byte[i]))        
        {            
            break;
        }
    }
    if(i>OUI_Microchip_byte.length)
    {
        return false;
    }
    // Populate MLA devices node
    mlaNode = findNode(mlaNodeName,root);
    if(mlaNode == null)
    {
        insertNode(mlaNodeName,mlaNode,root.getChildCount());
        mlaNode = findNode(mlaNodeName,root);
    }
    macAddress = macAddress.concat(packetBuf[1]);                
    macNode = findNode(macAddress,mlaNode);
    if(macNode != null)
    {
       removeNodeFromRoot(macNode);
    }
    
    insertNode(macAddress,mlaNode,mlaNode.getChildCount());
    macNode = findNode(macAddress,mlaNode);
     
   
     // insert Hostname after comparing "Hostname - " string
    if(findNode(hostName,macNode)== null)
    {
        insertNode(hostName,macNode,macNode.getChildCount());        
    }
    
    ipNode = findNode(ipAddr,macNode);
    if(ipNode == null)
    {
        insertNode(ipAddr,macNode,macNode.getChildCount());
        ipNode = findNode(ipAddr,macNode);
    }
    
    if(findNode(ipAddrStr,ipNode) == null)
        insertNode(ipAddrStr,ipNode,ipNode.getChildCount());
    
    if((packetBuf[2].length() != 0)&&(packetBuf[2].contains("DHCP")))
    {
        messageNode = findNode(message,macNode);
        if(messageNode == null)
        {
            insertNode(message,macNode,macNode.getChildCount());
            messageNode = findNode(message,macNode);
        }
        if(findNode(packetBuf[2],messageNode) == null)
            insertNode(packetBuf[2],messageNode,messageNode.getChildCount());
    }
    return true;
 }

/*
 * Convert IPv6 binary address into presentation (printable) format.
 *
 * @param src a byte array representing the IPv6 numeric address
 * @return a String representing an IPv6 address in
 *         textual representation format
 * 
 */
    static String ipv6NumericToTextFormat(byte[] src)
    {
        StringBuffer sb = new StringBuffer(39);
        for (int i = 0; i < (16 / 2); i++) {
            sb.append(Integer.toHexString(((src[i<<1]<<8) & 0xff00)
                          | (src[(i<<1)+1] & 0xff)));
            if (i < (16 / 2) -1 ) {
               sb.append(":");
            }
        }
        return sb.toString();
    }
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        btnDiscover = new javax.swing.JButton();
        btnExit = new javax.swing.JButton();
        jPanel1 = new javax.swing.JPanel();
        lebelAbout = new javax.swing.JLabel();
        jCheckBox1 = new javax.swing.JCheckBox();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setTitle("Microchip TCPIP Discoverer");
        setResizable(false);

        btnDiscover.setText("Discover Devices");
        btnDiscover.setCursor(new java.awt.Cursor(java.awt.Cursor.DEFAULT_CURSOR));
        btnDiscover.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnDiscoverActionPerformed(evt);
            }
        });

        btnExit.setText("Exit");
        btnExit.setMaximumSize(new java.awt.Dimension(115, 23));
        btnExit.setMinimumSize(new java.awt.Dimension(115, 23));
        btnExit.setPreferredSize(new java.awt.Dimension(115, 23));
        btnExit.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnExitActionPerformed(evt);
            }
        });

        jPanel1.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        jPanel1.setMinimumSize(new java.awt.Dimension(600, 800));
        jPanel1.setPreferredSize(new java.awt.Dimension(651, 800));

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 648, Short.MAX_VALUE)
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 0, Short.MAX_VALUE)
        );

        lebelAbout.setBackground(new java.awt.Color(153, 153, 153));
        lebelAbout.setFont(new java.awt.Font("Microsoft Sans Serif", 1, 11)); // NOI18N
        lebelAbout.setForeground(new java.awt.Color(0, 0, 204));
        lebelAbout.setText("<html> date <br> version </html>");
        lebelAbout.setBorder(javax.swing.BorderFactory.createEmptyBorder(1, 1, 1, 1));
        lebelAbout.setCursor(new java.awt.Cursor(java.awt.Cursor.HAND_CURSOR));
        lebelAbout.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                lebelAboutMouseClicked(evt);
            }
        });

        jCheckBox1.setText("Network Direct Broadcast");
        jCheckBox1.setToolTipText("<html>On for Direct Broadcast (e.g: x.x.x.255)<br> Off for Limited broadcast (e.g: 255.255.255.255)");
        jCheckBox1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jCheckBox1ActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGap(64, 64, 64)
                .addComponent(btnDiscover, javax.swing.GroupLayout.PREFERRED_SIZE, 158, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addComponent(jCheckBox1)
                .addGap(72, 72, 72)
                .addComponent(btnExit, javax.swing.GroupLayout.PREFERRED_SIZE, 150, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(46, 46, 46))
            .addGroup(layout.createSequentialGroup()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, 652, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(layout.createSequentialGroup()
                        .addGap(403, 403, 403)
                        .addComponent(lebelAbout, javax.swing.GroupLayout.PREFERRED_SIZE, 236, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGap(20, 20, 20)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(btnDiscover, javax.swing.GroupLayout.PREFERRED_SIZE, 28, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(btnExit, javax.swing.GroupLayout.PREFERRED_SIZE, 28, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jCheckBox1))
                .addGap(18, 18, 18)
                .addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, 336, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(lebelAbout, javax.swing.GroupLayout.PREFERRED_SIZE, 37, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void btnDiscoverActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnDiscoverActionPerformed
        // TODO add your handling code here:
    try {
                if(socket.isBound() != true) {
                socket = new DatagramSocket(30303);
                System.out.println("EthDiscoverActionPerformed:after socket");
            }
            
            removeAllNode();
           // model.reload();
            SwingUtilities.invokeLater(new Runnable() {
                        public void run() {
                            tree.updateUI();
                        }}
                );
            
            // NetworkInterface implements a static method that returns all the interfaces on the PC,
            // which we add on a list in order to iterate over them.
            ArrayList<NetworkInterface> interfaces = Collections.list(NetworkInterface.getNetworkInterfaces());
            int mask = 0;
            InetAddress ipAddr2;
            byte[] result = new byte[4];
            byte[] netmask = new byte[4];
            byte[] ipAddrs;
            if(annce_bcast_type == ANNOUNCE_SOCKET_BCAST_TYPE.ANNOUNCE_UDP_BCAST_NETWORK_DIRECTED)
            {
                for(NetworkInterface iface : interfaces) 
                {
                    // check if the interface is Up or down
                    if(iface.isUp())
                    {
                        // Interface addresses of the network interface
                        
                        for (InterfaceAddress addr : iface.getInterfaceAddresses()) 
                        {
                            InetAddress i = addr.getAddress();
//                            System.out.println(iface.getDisplayName()+ "  prefixlen:" + addr.getNetworkPrefixLength());
//                            System.out.println("netmask:  " + addr.getBroadcast());
//                            System.out.println("address:  " + addr.getAddress());
                             if(i instanceof Inet4Address && !i.isLoopbackAddress())
                            {
                                ipAddrs = i.getAddress();
                                if(addr.getNetworkPrefixLength() != -1)
                                {
                                    mask = 0xffffffff << (32 - addr.getNetworkPrefixLength());
                                }
                                else
                                {
                                    // net mask will be 255.255.255.255 if getNetworkPrefixLength()
                                    // returns -1. This is seen for Wifi interface.
                                   // mask = 0xFFFFFFFF;
                                    continue;
                                }
                                netmask[0] = (byte) (mask >> 24);
                                netmask[1] = (byte) (mask >> 16);
                                netmask[2] = (byte) (mask >> 8);
                                netmask[3] = (byte) (mask /*>> 0*/);

                                result[0] = (byte) (ipAddrs[0] | (~netmask[0]));
                                result[1] = (byte) (ipAddrs[1] | (~netmask[1]));
                                result[2] = (byte) (ipAddrs[2] | (~netmask[2]));
                                result[3] = (byte) (ipAddrs[3] | (~netmask[3]));
                                ipAddr2 = InetAddress.getByAddress(result);

//                                System.out.println("interface address: "+ addr.getAddress());
//                                System.out.println("broadcast address: "+ addr.getBroadcast().getHostName());
//                                System.out.println("broadcast string address: "+ addr.getBroadcast());
//                                System.out.println("prefixlen: "+ addr.getNetworkPrefixLength());
//                                System.out.println(ipAddr2.getHostAddress());
                                packet = new DatagramPacket(discvStr.getBytes(),discvStr.length(),
                                ipAddr2, 30303);
                                socket.send(packet);
                            }
                        }
                        
                    }
                }
            }
            else
            {
                InetAddress bcast_address = InetAddress.getByName("255.255.255.255");
                packet = new DatagramPacket(discvStr.getBytes(),discvStr.length(),bcast_address, 30303);
                socket.send(packet);
            }
            //jTable1.addMouseListener(null);
        }catch(IOException soEx) {
            int res1;
            String optionStr = "<html>"+"Unable to transmit discovery message."+
                    " <br> Check network " +
                    "connectivity and ensure that no other instances of this " +
                    "program are running.";

            JOptionPane.showMessageDialog(null,optionStr,
                    "Error", JOptionPane.ERROR_MESSAGE);
            close_action();
            System.exit(0);
        }
}//GEN-LAST:event_btnDiscoverActionPerformed

    private void btnExitActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnExitActionPerformed
        //close_action();
        System.exit(0);   
}//GEN-LAST:event_btnExitActionPerformed

    private void lebelAboutMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_lebelAboutMouseClicked
        aboutBox.setVisible(true);
}//GEN-LAST:event_lebelAboutMouseClicked

    private void jCheckBox1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jCheckBox1ActionPerformed
        // TODO add your handling code here:
        if(jCheckBox1.isSelected())
        {
            annce_bcast_type = ANNOUNCE_SOCKET_BCAST_TYPE.ANNOUNCE_UDP_BCAST_NETWORK_DIRECTED;
        }
        else
        {
            annce_bcast_type = ANNOUNCE_SOCKET_BCAST_TYPE.ANNOUNCE_UDP_BCAST_NETWORK_LIMITED;
        }
    }//GEN-LAST:event_jCheckBox1ActionPerformed

    


/*
    public void addDiscoveryEntry(String receivePacket)
    {
        String splitString = "<|>";
        DefaultMutableTreeNode macNode,typeNode,childNode,ipNode;
        String temp;
        String[] payLoadtags = {"MACADDR", "MACTYPE","HOSTNAME", "IPV4","IPV6U","IPV6M","TRUNCATED"};

        String macAddress = "MAC-Address - ";
        String macType = "MAC-Type - ";
        String hostName = "Hostname - ";
        String ipAddr = "IP Addresses";
        String multicastAddr = "Multicast Addresses";
        //byte segFlag = 0;
        int treeNodeNum = 0;
        String[] s = receivePacket.split(splitString);
        int tagCnt=0, elementCnt=0;
        for(tagCnt=0,elementCnt=0;elementCnt < s.length;tag)

       
// s[0] = mac address
//s[1] = Mac Type
//s[2] = host name
//s[3] =  ipv4 address
//s[4] onwards IPV6 afddress starts
// it can be a unicast or Multicast address
        //topPanel.setLayout( new BorderLayout() );

       // add Mac address
        // s[0] = Mac Address
        //segFlag = (byte)s[0].charAt(0);
        //temp = s[0].substring(1);
        //macAddress = macAddress.concat(s[0]); // after concat "MAC-Address - 00:04:A3:12:26:B6"
        // if segFlag > 0 , that is packet is a fragmented one. there will be more than one announce packet
        // for the same MAC address.
       //if(segFlag == (byte) '$')
        macAddress = macAddress.concat(s[0]);
        if(gSegFlag >  0x80)
        {

            macNode = findNode(macAddress,root);
            if(macNode == null)
                return;
            for(treeNodeNum=1;treeNodeNum<s.length;treeNodeNum++)
            {
                // if any string length is zero , continue for the next one.
                if(s[treeNodeNum].length()== 0)
                    continue;
                // insert MAc-Type after comparing "MAC-Type - "  string
                if(findNode(macType,macNode)== null)
                {
                    macType = macType.concat(s[treeNodeNum]);
                    insertNode(macType,macNode,macNode.getChildCount());
                    break;
                }
                // insert Hostname after comparing "Hostname - " string
                if(findNode(hostName,macNode)== null)
                {
                    hostName = hostName.concat(s[treeNodeNum]);
                    insertNode(hostName,macNode,macNode.getChildCount());
                    break;
                }

                 // insert IPv4 address
                ipNode = findNode(ipAddr,macNode);
                if(ipNode == null)
                {
                    insertNode(ipAddr,macNode,macNode.getChildCount());
                    ipNode = findNode(ipAddr,macNode);
                    insertNode(s[treeNodeNum],ipNode,ipNode.getChildCount());
                    break;
                }
 
                if(s[treeNodeNum].startsWith("FF"))
                {
                    ipNode = findNode(multicastAddr,macNode);
                    // avoid inserting Multicast node every time using this following if condition
                    if(ipNode == null)
                    {
                        insertNode(multicastAddr,macNode,macNode.getChildCount());
                        // again search for multicast node to insert multicast IPV6 address
                        // this is only required if the previous ipNode value is null
                        ipNode = findNode(multicastAddr,macNode);
                    }
                    if(findNode(s[treeNodeNum],ipNode) == null)
                            insertNode(s[treeNodeNum], ipNode,ipNode.getChildCount());
                   
                }
                else
                {
                    // this one for Unicast IPv6 address
                    ipNode = findNode(ipAddr,macNode);
                    if(ipNode == null)
                    {
                        insertNode(ipAddr,macNode,macNode.getChildCount());
                        ipNode = findNode(ipAddr,macNode);
                    }
                    if(findNode(s[treeNodeNum],ipNode) == null)
                        insertNode(s[treeNodeNum], ipNode,ipNode.getChildCount());
                }
            }

        }
        else
        {
            macNode = findNode(macAddress,root);
            if(macNode == null)
            {
                insertNode(macAddress,macNode,root.getChildCount());
                macNode = findNode(macAddress,root);
            }
            else
                return;
            for(treeNodeNum=1;treeNodeNum<s.length;treeNodeNum++)
            {
                 // if any string length is zero , continue for the next one.
                if(s[treeNodeNum].length()== 0)
                    continue;
                switch(treeNodeNum)
                {
                    case 1: // MAC-Type
                        // insert MAC Type
                        macType = macType.concat(s[1]);
                        insertNode(macType,macNode,macNode.getChildCount());
                        break;
                    case 2:
                       // insert Hostname
                        hostName = hostName.concat(s[2]);
                        insertNode(hostName,macNode,macNode.getChildCount());
                        break;
                    case 3:
                        // insert IP address
                        insertNode(ipAddr,macNode,macNode.getChildCount());
                        ipNode = findNode(ipAddr,macNode);
                        insertNode(s[3],ipNode,ipNode.getChildCount());
                        break;
                }
                if(treeNodeNum > 3)
                {
//                    if(s[treeNodeNum].length()<= 2)
//                        continue;
                    if(s[treeNodeNum].startsWith("FF"))
                    {
                        ipNode = findNode(multicastAddr,macNode);
                        // avoid inserting Multicast node every time using this following if condition
                        if(ipNode == null)
                        {
                            insertNode(multicastAddr,macNode,macNode.getChildCount());
                            // again search for multicast node to insert multicast IPV6 address
                            // this is only required if the previous ipNode value is null
                            ipNode = findNode(multicastAddr,macNode);
                        }
                        insertNode(s[treeNodeNum], ipNode,ipNode.getChildCount());
                    }
                    else
                    {
                        // this one for Unicast IPv6 address
                        ipNode = findNode(ipAddr,macNode);
                        insertNode(s[treeNodeNum], ipNode,ipNode.getChildCount());
                    }
                }
            }
        }
        
    }

 *
 */

/*
 * find the node from the jTree. This method is used to searched with respect
 * to the compleate string value or part of the nodevalue (part of the node value should be
 * start value)
 * @ Param  - s  = node value to be searched which is a string
 *          node =  to which node value need to be inserted
 *  @ Return DefaultMutableTreeNode if found else null
 */
 private DefaultMutableTreeNode findNode(String s, DefaultMutableTreeNode node)
 {
    //DefaultMutableTreeNode root =
      //  (DefaultMutableTreeNode)tree.getModel().getRoot();
    Enumeration e = node.breadthFirstEnumeration();
    while(e.hasMoreElements()) {
        node = (DefaultMutableTreeNode)e.nextElement();
        if(node.toString().equals(s)) {
            return node;
        }
        else if(node.toString().startsWith(s))
        {
            return node;
        }
    }
    return null;
 }
/*
 * Create and insert new node to the jTree
 * @ Param  - s  = node value as a string format
 *          node =  to which node value need to be inserted
 *          pos  = at what position
 *  @ Return void
 */
 private void insertNode(String s, DefaultMutableTreeNode node,int pos) {

        DefaultMutableTreeNode child = new DefaultMutableTreeNode(s);
        if(node == null) // parent node
        {
            node = new DefaultMutableTreeNode(s);
            model.insertNodeInto(node, root, root.getChildCount()); // add the first child to this node         
        }
        else
        {
            model.insertNodeInto(child, node, pos);
        }
    }
/*
 * remove all node from jTree
 * @ Param  - null
 *  @ Return void
 */
   private void removeAllNode()
   {
       int i=0;
       DefaultMutableTreeNode node;
       //DefaultMutableTreeNode root =
         //   (DefaultMutableTreeNode)tree.getModel().getRoot();
      // Enumeration e = root.children();//.breadthFirstEnumeration();
       while(root.isLeaf() == false)
       {
           node =(DefaultMutableTreeNode) root.getFirstChild();
           model.removeNodeFromParent(node);
       }
       root.removeAllChildren();
       model.reload();
   }
/*
 * remove only one node from jTree
 * @ Param  - node =  to which node value need to be removed
 *
 *  @ Return void
 */
   private void removeNodeFromRoot(DefaultMutableTreeNode node)
   {
       model.removeNodeFromParent(node);
   }


/*
public void basicTreeExample()
{
        // Set the frame characteristics
        setTitle( "TCPIP Discoverer" );
        //setSize( 600, 600 );
        //jTree1.removeAll();
        setBackground( Color.gray );
        DefaultMutableTreeNode macNode,typeNode,childNode,ipNode;
        // Create a panel to hold all other components
        //topPanel = new JPanel();
        //jPanel1.setLayout( new BorderLayout() );
        //getContentPane().add( topPanel );
        //root = new DefaultMutableTreeNode("Devices");
        //DefaultTreeModel model = new DefaultTreeModel(root);
        macNode = new DefaultMutableTreeNode("MAC-Address - 00:04:A3:12:26:B6");
        //root.add(macNode);
        model.insertNodeInto(macNode, root,root.getChildCount());

        typeNode = new DefaultMutableTreeNode("MAC-Type - MRF24WB0M");
        macNode.add(typeNode);
        childNode = new DefaultMutableTreeNode("Hostname - MCHPBOARD");
        typeNode.add(childNode);
        ipNode = new DefaultMutableTreeNode("192.168.1.104");
        childNode.add(ipNode);
        ipNode = new DefaultMutableTreeNode("FE80:0000:0000:0000:0204:A3FF:FE11:2233");
        childNode.add(ipNode);
        ipNode = new DefaultMutableTreeNode("2001:0DB9:0000:0000:0204:A3FF:FE11:2233");
        childNode.add(ipNode);
        ipNode = new DefaultMutableTreeNode("FF02:0000:0000:0000:0000:0000:0000:0001");
        childNode.add(ipNode);
        ipNode = new DefaultMutableTreeNode("FF05:0000:0000:0000:0000:0000:0000:0001");
        childNode.add(ipNode);
//                typeNode = new DefaultMutableTreeNode("MAC-Type - PIC32INT");
//                macNode.add(typeNode);
//                childNode = new DefaultMutableTreeNode("Hostname - WSM");
//                typeNode.add(childNode);
//                ipNode = new DefaultMutableTreeNode("192.168.1.105");
//                childNode.add(ipNode);
        macNode = new DefaultMutableTreeNode("MAC-Address - 00:04:A3:12:34:56");
        root.add(macNode);
        typeNode = new DefaultMutableTreeNode("MAC-Type - PIC32INT");
        macNode.add(typeNode);
        childNode = new DefaultMutableTreeNode("Hostname - WSM");
        typeNode.add(childNode);
        ipNode = new DefaultMutableTreeNode("192.168.1.105");
        childNode.add(ipNode);

        macNode = new DefaultMutableTreeNode("MAC-Address - 00:04:A3:67:89:AB");
        root.add(macNode);
        typeNode = new DefaultMutableTreeNode("MAC-Type - PIC32INT");
        macNode.add(typeNode);
        childNode = new DefaultMutableTreeNode("Hostname - MCHPBOARD");
        typeNode.add(childNode);
        ipNode = new DefaultMutableTreeNode("192.168.1.106");
        childNode.add(ipNode);
        ipNode = new DefaultMutableTreeNode("FE80:0000:0000:0000:0204:A3FF:FE11:2233");
        childNode.add(ipNode);

        // Create a new tree control
        //tree = new JTree(model);

//        // Add the listbox to a scrolling pane
        scrollPane = new JScrollPane();
        scrollPane.getViewport().add( tree );
       // jPanel1.add( scrollPane, BorderLayout.CENTER );
}

*
 * 
 */
    /**
    * @param args the command line arguments
    */
    public static void main(String args[]) {
       //java.awt.EventQueue.invokeLater(new Runnable() {
        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                try{
                //UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName());
                UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
                // UIManager.setLookAndFeel(new com.sun.java.swing.plaf.windows.WindowsLookAndFeel());
                }catch (Exception e) {}
                new TcpIpDiscoverTool().setVisible(true);

            }
        });
    }

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btnDiscover;
    private javax.swing.JButton btnExit;
    private javax.swing.JCheckBox jCheckBox1;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JLabel lebelAbout;
    // End of variables declaration//GEN-END:variables

}

