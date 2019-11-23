/*********************************************************************
*
*  MPFSImg2.bin,MPFSImg2.c,MPFSImg2.s,FileRcrd.bin,DynRcrd.bin
*  generation.
*
**********************************************************************
* FileName:        MPFS2Lib.java
* Complier:        JAVA version "1.6.0_20 and onwards"
* IDE :            NetBeans
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


package MicrochipMPFS;
import java.util.*;
import java.io.*;
import java.nio.file.Files;
import java.util.zip.*;
import java.text.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;


public class MPFS2Lib //: MPFS2Writer
{
    MainMPFS  mainMpfs;
    public String LocalPath;
    public String LocalFile;
    public String SourcePath;
    private Collection<String> dynamicTypes;
    private Collection<String> nonGZipTypes;
    private Collection<String> nonGzipSSIFiles;
    private DynVar dynVarParser;
    public List<String> log;
    public List<MPFSFileRecord> files;
    protected boolean indexUpdated;
    List<File> ListFiles = null;
    public FileOutputStream file_output;
    public DataOutputStream data_out;
    public int MPFS2_FLAG_ISZIPPED = 0x0001;
    public int MPFS2_FLAG_HASINDEX = 0x0002;
    //public static long ImageLength=0;
    public static String ASCIILine;
    public static String emptyStr;
    public String generatedImageName;
    public List<File> deeperList = new ArrayList<File>();
    public String DirPathString;
    boolean ifUnifiedStackIsUsed  = false;
    //public Collection dynTypes;
    public MPFS2Lib(boolean ifUnifiedStackIsenabled)
    {
       ifUnifiedStackIsUsed = ifUnifiedStackIsenabled ;  
    }
    public enum MPFSOutputFormat
    {
        BIN,
        C18,
        ASM30,
        C32,
        MDD,
    }
    
     // The parameters accepted by the if command 
   
    //public String  SSIInclude = "<!-- *#include *(virtual|file)=[\'\"]([^\'\"]+)[\'\"] *-->";
    public String  SSIWithThreeArgument = "<!-- *#(include|echo|exec|set|config|fileSize|if|elif|else|endif) *(virtual|file|var|cgi|cmd|timefmt|sizefmt|errmsg)=[\'\"]([^\'\"]+)[\'\"] *(value)=[\'\"]([^\'\"]+)[\'\"] *-->";
    public String  SSIWithTwoArgument = "<!-- *#(include|echo|exec|set|config|fileSize|if|elif|else|endif) *(virtual|file|var|cgi|cmd|timefmt|sizefmt|errmsg)=[\'\"]([^\'\"]+)[\'\"] *-->";
    private Pattern parser_three_args = Pattern.compile(SSIWithThreeArgument);
    private Pattern parser_two_args = Pattern.compile(SSIWithTwoArgument);
    
    public void SSI_CommandParse(String strLine)
    {
        int count=0;
        String fileName="";
        Matcher matches_three_args = parser_three_args.matcher(strLine);
        Matcher matches_two_args = parser_two_args.matcher(strLine);
        if(matches_three_args.find())
        {
            fileName = matches_three_args.group(count);
            SSICommandProcessing(fileName);
        }
        if(matches_two_args.find())
        {
            fileName = matches_two_args.group(count);
            SSICommandProcessing(fileName);
        }
     }
    /**
  * Recursively walk a directory tree and return a List of all
  * Files found; the List is sorted using File.compareTo().
  *
  * @param aStartingDir is a valid directory, which can be read.
  */
  public List<File> getFileListing( File aStartingDir
                                          ) throws FileNotFoundException {
    DirPathString = aStartingDir.getPath();
    if(deeperList != null)
    {
        deeperList.clear();
    }
    List<File> result = getFileListingNoSort(aStartingDir);

    //Collections.sort(result);
    //Collections.reverse(deeperList);
    
    return result;
  }

  // PRIVATE //
    public  List<File> getFileListingNoSort(File aStartingDir
                                                ) throws FileNotFoundException {
    List<File> result = new ArrayList<File>();
    File[] filesAndDirs = aStartingDir.listFiles();
    List<File> filesDirs = Arrays.asList(filesAndDirs);
    //List<File> deeperList = new ArrayList<File>();
    //result.add(aStartingDir);
    for(File file : filesDirs) {
        if(file.isHidden())
        {
            continue;
        }
        if ( ! file.isFile() ) {
        //must be a directory
        //recursive call!       
        deeperList = getFileListingNoSort(file);        
        //result.addAll(deeperList);
      }
      else
      {
        result.add(file); //always add, even if directory
      }
    }
//    if(DirPathString.contains(aStartingDir.getPath())==true)
//        Collections.sort(deeperList);
    result.addAll(deeperList);
    return result;
  }
/*
 * File matching
 */
    private boolean FileMatches(String fileName, Collection<String> endings)
    {
        for(String end : endings)
        {
            if(fileName.endsWith(end))
            {
                return true;
            }
        }
        return false;
    }

    public void MPFS2Builder(String localPath, String localFile)//,String sourcePath)
    {
        if(!localPath.endsWith("\\"))
        { localPath += File.separator;}

        this.LocalPath = localPath;
        this.LocalFile = localFile;
        //this.SourcePath = sourcePath;
        this.dynamicTypes = new ArrayList<String>();
        this.nonGZipTypes = new ArrayList<String>();
        this.nonGzipSSIFiles = new ArrayList<String>();
        this.log = new ArrayList<String>();
        this.files = new LinkedList<MPFSFileRecord>();        
        this.dynVarParser = new MicrochipMPFS.DynVar(localPath,ifUnifiedStackIsUsed);
        this.indexUpdated = false;

    }
    public String GeneratedImageFileName()
    {
        return this.generatedImageName;
    }
    /// <summary>
    /// Sets a comma-separated list of types to be considered dynamic
    /// </summary>
    public void DynamicTypes(String dynFiles)
    {
        String[] str = dynFiles.split(",");
        this.dynamicTypes.clear();
        for(String s:str)
        {
            String s_trimmed = s.replace('*',' ').trim();
            if(s_trimmed.length() > 0)
            {
                this.dynamicTypes.add(s_trimmed);
            }
        }
    }
    
    public void SSICommandProcessing(String ssiCommandFile)
    {
        //String[] str = ssiCommandFile.split("\\s+|#|=|\"");
        String[] str = ssiCommandFile.split("\\s+");
        String s = str[0].replace("<!--#"," ").trim();
        
        
        // command : include
        if((s.compareTo("include") == 0) || (s.compareTo("fsize") == 0))
        {
            String[] var_fileName=str[1].split("=");
            if((var_fileName[0].compareTo("file") == 0) || (var_fileName[0].compareTo("vertual") == 0))
            {                
                String s_trimmed = var_fileName[1].replace('"',' ').trim();
                              
                if(nonGzipSSIFiles.contains(s_trimmed) != true)
                {
                    nonGzipSSIFiles.add(s_trimmed);
                }
            }
        }
        else if(s.compareTo("exec") == 0)
        {
            //System.out.println("command Name Name is: "+s);
        }
        else if(s.compareTo("echo") == 0)
        {
            //System.out.println("command Name Name is: "+s);
        }
        else if(s.compareTo("config") == 0)
        {
            //System.out.println("command Name Name is: "+s);
        }
        else if(s.compareTo("config") == 0)
        {
            //System.out.println("command Name Name is: "+s);
        }      
    }

    /// <summary>
    /// Sets a comma-separated list of types not to be compressed
    /// </summary>
    public void NonGZipTypes(String nonGZipFiles)
    {
        String[] str = nonGZipFiles.split(",");
        this.nonGZipTypes.clear();
        for(String s:str)
        {
            String s_trimmed = s.replace('*',' ').trim();
            if(s_trimmed.length() > 0)
            {
                this.nonGZipTypes.add(s_trimmed);
            }
        }
    }

    /// <summary>
    /// Adds a file to the MPFS image
    /// </summary>
    /// <param name="localName">Local file name to read with absolute pathpath</param>
    /// <param name="imageName">Name to use in image file</param>
    public boolean AddFile(String localName, String imageName,File file)
    {
        //String strLine;
        int   byteRead;
        int count=0;
        StringBuilder strLine=new StringBuilder();
        char[] buf ;
        byte[] fileData;
        //File file = new File(localName);
        // Skip if can't be opened
        if (!file.exists())
        {
            log.add("\r\nERROR: Could not read " + localName);
            return false;
        }
        int fileSizeLength = (int)file.length();
        if(fileSizeLength < 10000000)
        {
            fileData = new byte[fileSizeLength];
            buf = new char[fileSizeLength];
        }
        else
        {
            fileData = new byte[10000000];
            buf = new char[10000000];
        }

        // Set up the new file record
        MPFSFileRecord newFile = new MPFSFileRecord();
        newFile.SetFileName(imageName);//FileName = imageName;
        newFile.SetFiledate(file.lastModified());

        // Read the data in
        try{
        FileInputStream inputFile = new FileInputStream(localName);
        DataInputStream in = new DataInputStream(inputFile);
        BufferedReader br = new BufferedReader(new InputStreamReader(in));
        
        int byteCnt=0;
      
        while((byteRead = br.read(buf))!= -1)
        {
            if(byteRead == 0) {break;}
            byteCnt++;
            strLine.append(buf);
        }
        br.close();
        inputFile = new FileInputStream(localName);
        byteRead = 0;
        byteCnt = 0;
        ByteArrayOutputStream tempout = new ByteArrayOutputStream();
        while((byteRead = inputFile.read(fileData))!= -1)
        {
           if(byteRead == 0) {break;}
           byteCnt += byteRead;
           tempout.write(fileData);
        }
        newFile.data = tempout.toByteArray();
        newFile.fileSizeLen = byteCnt; //tempout.size();

        inputFile.close();
        // Parse the file if necessary
        MPFSFileRecord idxFile = null;
        if (this.FileMatches(localName, this.dynamicTypes))
        {
            idxFile = dynVarParser.Parse(newFile,strLine);
        }
        

        // GZip the file if possible
        int gzipRatio = 0;
        if (idxFile == null && !this.FileMatches(localName, this.nonGZipTypes))
        {
            boolean ssiFileStatus = false;
            for(String ssiFile:nonGzipSSIFiles)
            {
                if(imageName.contains(ssiFile))
                {
                    // No compression is allowed for SSI files.
                    ssiFileStatus = true;
                    break;                    
                }
            }
            if(!ssiFileStatus)
            {

                inputFile = new FileInputStream(localName);
               /*ZipOutputStream zipout = new ZipOutputStream(new FileOutputStream("output.zip"));
                zipout.putNextEntry(new ZipEntry("output.zip"));*/
                GZIPOutputStream zipout  =  new GZIPOutputStream(new
                                            FileOutputStream("output.gzip"));
                if(fileSizeLength < 10000000)
                {
                    fileData = new byte[fileSizeLength];               
                }
                else
                {
                    fileData = new byte[10000000];             
                }
                while((byteRead = inputFile.read(fileData)) != -1)
                {
                    if(byteRead == 0) {break;}
                    zipout.write(fileData,0,byteRead);
                }
                zipout.finish();
    //            byteRead = inputFile.read();
    //            while (byteRead != -1){
    //            zipout.write(byteRead);
    //            byteRead = inputFile.read();
    //            }
                //zipout.closeEntry();
                zipout.close();
                File file_zip = new File("output.gzip");
    //            if(file_zip.isFile())
    //                System.out.println("original:"+file.length()+
    //                        "   zip file length" +file_zip.getName()+file_zip.length()) ;
                FileInputStream inputZipFile = new FileInputStream("output.gzip");

                // Only use zipped copy if it's smaller
                if ((file_zip.length() < newFile.fileSizeLen) && (file_zip.length() > 0))
                {
                    ByteArrayInputStream tempNewFileDataArray =
                            new ByteArrayInputStream(newFile.data);
                    gzipRatio = (int)(100 - (100 * file_zip.length() / newFile.fileSizeLen));
                    if(file_zip.length() < 10000000)
                    {
                        fileData = new byte[(int)file_zip.length()];
                    }
                    else
                    {
                        fileData = new byte[10000000];
                    }
                    //newFile.data.clear();
                    tempNewFileDataArray.reset();
                    tempout = new ByteArrayOutputStream();
                    byteCnt = 0;
                    while((byteRead=inputZipFile.read(fileData))!= -1)
                    {
                        if(byteRead == 0) {break;}
                        byteCnt += byteRead;
                        tempout.write(fileData);
                    }
                    newFile.data = tempout.toByteArray();
                    newFile.fileSizeLen = byteCnt; //tempout.size();
                    newFile.isZipped = true;
                }
                inputZipFile.close();
                file_zip.delete();
                inputFile.close();
            }
        }

        // Add the file and return
        if (idxFile == null)
        {
            log.add("    " + imageName + ": " + newFile.fileSizeLen + " bytes" +
                ((gzipRatio > 0) ? " (gzipped by " + gzipRatio + "%)" : ""));
            files.add(newFile);
        }
        else
        {
            log.add("    " + imageName + ": " + newFile.fileSizeLen + " bytes, " + (idxFile.fileSizeLen / 8) + " vars");
            newFile.hasIndex = true;
            files.add(newFile);            
            if(!ifUnifiedStackIsUsed)
            {
                files.add(idxFile);
            }            
        }
        }catch(IOException e){
         System.out.println ("IO exception = " + e );
         //log.add("ERROR: " + e.getMessage());
        }
        return true;
    }
    
    public boolean addSSIFilestoList(String localName)
    {
        try{
            FileInputStream inputFile_ssi = new FileInputStream(localName);        
            DataInputStream in_ssi = new DataInputStream(inputFile_ssi);
            BufferedReader br_ssi = new BufferedReader(new InputStreamReader(in_ssi));
        
            String ssiString="";
      
            while((ssiString = br_ssi.readLine())!= null)
            {
                SSI_CommandParse(ssiString);
            }
        }catch(IOException e){
         System.out.println ("IO exception = " + e );
         //log.add("ERROR: " + e.getMessage());
        }
        return true;
    }

    public boolean AddDirectory(String dataPath)
    {
       String imagePath="",tempImagePath="";
       String parentPath = "";
      // String rootFilePath;
        
        File dir = new File(dataPath);
        if(dir.exists() == false)
        {
           log.add("\r\nERROR: Directory " + dataPath + " does not exist.");
           return false;
        }
        try{
            if(ListFiles != null)
            {
                if(!ListFiles.isEmpty())
                {
                    ListFiles.clear();
                }
            }
            if(files != null)
            {
                if(!files.isEmpty())
                {
                    files.clear();
                }
            }
            if(log != null)
            {
                if(!log.isEmpty())
                {
                    log.clear();
                }
            }
            ListFiles = getFileListing(dir);
        }catch(IOException e)
        {
            log.add("\r\nERROR: Directory " + dataPath + " does not exist.");
            return false;
        }

        // Make sure directory is not the project directory
        if (this.LocalPath.contains(dataPath))
        {
            log.add("<html>\r\nERROR: The project directory is located <br>in the source " +
                    "directory.  The generator cannot run if <br>the image is to be placed " +
                    "in the source directory. <br> Please select the base MPLAB project " +
                    "<br>directory before continuing.</html>");
            return false;
        }
        //rootFilePath = ListFiles.get(0).getPath();
        //SSi file adding to the list
        for(File ssiFile : ListFiles)
        {
            if(ssiFile.isHidden())
            {
               continue;
            }
            if(!ssiFile.isDirectory())
            {                
                addSSIFilestoList(ssiFile.getPath());                
            }
        }

        for (File f : ListFiles)
        {
           if(f.isHidden())
           {
              continue;
           }
           if (f.isDirectory())
           {
/*TODO - Following code can be reused for a directory processing         
              AddDirectory(f.getPath());
              log.add(f.getPath() + " :");
               if(dataPath.contains(f.getPath()) == false)
               {
                  if((tempImagePath.length() > 0) &&(f.getPath().contains(tempImagePath.split("/")[0])))
                    tempImagePath = tempImagePath+"/" + f.getName();
                  else
                    tempImagePath = f.getName();

                 imagePath = tempImagePath;
                 imagePath = imagePath+"/";
               }
               */
           }
           else
           {
               if(f.getParent().compareTo(parentPath) != 0)
               {
                    log.add(f.getParent() + " :");
               }
               tempImagePath = f.getPath();
               
               if( dataPath.contains(File.separator) == false)
               {
                   if(File.separator.contains("/") == false)
                   {
                        dataPath = dataPath.replace("/",File.separator);
                   }
                   else
                   {
                        dataPath = dataPath.replace("\\",File.separator);
                   }
               }
               
               imagePath = tempImagePath.replace(dataPath,"").replace(File.separator,"/");
               
               imagePath = imagePath.substring(1);
               
               AddFile(f.getPath(),imagePath,f);
               parentPath = f.getParent();
           }
        }
    
        return true;
    }

    /// <summary>
    /// Generates an image in the specified format
    /// </summary>
    /// <param name="format">One of the MPFSOutputFormat constants indicating the format</param>
    /// <returns>true if generation was successful, false otherwise</returns>
    public boolean Generate(MPFSOutputFormat format)
    {
        File temp_file;
        // Start with nothing as the output name
        generatedImageName = null;

        // Make sure all paths exist
        if (!new File(LocalPath).exists())
        {
            log.add("ERROR: Output directory \"" + LocalPath + "\" does not exist!");
            return false;
        }

        // Make sure we have some files
        if (files.size() == 0)
        {
            return false;
        }

        // Write any index files that have changed
        //indexUpdated = dynVarParser.WriteIndices();
        dynVarParser.WriteIndices();
        // Determine address of each file and string
        int numFiles = (int)files.size();
        int lenHeader = 8;
        int lenHashes = 2 * numFiles;
        int lenFAT = 22 * numFiles;
        int baseAddr = lenHeader + lenHashes + lenFAT;
        int counter=0;
        int loopCntr=0;
        int numFileRecrds = 0;

        for(MPFSFileRecord file : files)
        {
            file.locStr = baseAddr;
            baseAddr += (int)file.GetFileName().length()+1;//FileName.Length + 1;
        }
        for (MPFSFileRecord file : files)
        {
            file.locData = baseAddr;
            baseAddr += (int)file.fileSizeLen;
        }

        // Set up the writer
        try
        {
            MPFS2Writer w;
            switch (format)
            {
                case C18:
                case C32:
                    w = new MPFS2C18Writer(LocalPath + LocalFile);
                    WriteImage(w);
                    break;                
                default:
                    w = new MPFS2BINWriter(LocalPath + LocalFile);
                    WriteImage(w);
                    break;
            }
       }catch (Exception e){
       log.add("ERROR: " + e.getMessage());
       return false;
       }
       return true;
    }

    private void WriteImage(MPFS2Writer x)
    {
        //byte[] hashByte= new byte[2];
        long timeVal=(long)621355968000000000L;
        long epoch = 0L;
        MPFS2Writer w;
        w = x;
        w.Write("MPFS");
        w.Write((byte)0x02);       
        w.Write((byte)0x01);
       
        w.Write((short)(files.size()));

        for(MPFSFileRecord file : files)
        {
           //byte[0]
           w.Write((byte)(file.nameHash));
           w.Write((byte)(file.nameHash>>8));
        }

        int flags;
        for(MPFSFileRecord file : files)
        {
            w.Write(file.locStr);
            w.Write(file.locData);
            w.Write(file.fileSizeLen);
            epoch = file.fileDate/1000;
            w.Write((int)(epoch));
            w.Write((int)0);
            flags = 0;
            if (file.hasIndex)
            {
                flags |= MPFS2_FLAG_HASINDEX;
            }
            if (file.isZipped)
            {
                flags |= MPFS2_FLAG_ISZIPPED;
            }
            w.Write((short)(flags));
            timeVal=(long)621355968000000000L;
        }
        for(MPFSFileRecord file : files)
        {
            w.Write(file.GetFileName());
            w.Write((byte)(0x00));
        }

        for(MPFSFileRecord file : files)
        {
            for(int i=0;i<file.fileSizeLen;i++)
            {
                w.Write(file.data[i]);
            }
        }
        w.Close();
        generatedImageName = w.imageName;

        log.add("GENERATED MPFS2 IMAGE: " + w.ImageLength + " bytes");
    }

  abstract class MPFS2Writer
  {
    public int ImageLength = 0;
    public String imageName;

    public abstract void Write(byte data);
    public abstract void Close();

    public void Write(byte[] data)
    {
        for(byte b : data)
        {
            Write(b);
        }
    }

    public void Write(String data)
    {
        for(byte b : data.getBytes())
        {
            Write(b);
        }
    }

    public void Write(short data)
    {
        Write((byte)(data));
        Write((byte)(data >> 8));
    }

    public void Write(int data)
    {
        Write((byte)(data));
        Write((byte)(data >> 8));
        Write((byte)(data >> 16));
        Write((byte)(data >> 24));
    }
    public void Write(long data)
    {
        Write((byte)(data));
        Write((byte)(data >> 8));
        Write((byte)(data >> 16));
        Write((byte)(data >> 24));
        Write((byte)(data >> 32));
        Write((byte)(data >> 40));
        Write((byte)(data >> 48));
        Write((byte)(data >> 56));
       
    }
  }

  public class MPFS2C18Writer extends MPFS2Writer {
   
    public FileOutputStream file_output;
    public DataOutputStream data_out;
    public BufferedWriter fout;
    public MPFS2C18Writer(String filename)
    {
        Date date = new Date();
        SimpleDateFormat sdf = new SimpleDateFormat("E MMM dd yyyy hh:mm:ss");
        if (!filename.endsWith(".c"))//.equalsIgnoreCase(".C")))
        {
            filename += ".c";
        }
       try{
        // Create an output stream to the file.
        file_output = new FileOutputStream (filename);
        // Wrap the FileOutputStream with a DataOutputStream
        data_out = new DataOutputStream (file_output);
        fout = new BufferedWriter(new OutputStreamWriter(data_out));
        
        imageName = filename;
        if(ifUnifiedStackIsUsed)
        {
            fout.write(
                "/***************************************************************\r\n" +
                "* MPFS image source file \r\n" +
                "* \r\n"+
                "* Company:\r\n" +
                "*   Microchip Technology Inc. \r\n"+
                "* \r\n" +
                "* File Name:\r\n" +
                "*   mpfs_net_img.c\r\n" +
                "* \r\n" +
                "* Description: \r\n" +
                "*   Defines an MPFS2 image to be stored in program memory.\r\n" +
                "* Processor:   PIC32\r\n" +
                "* Compiler:    Microchip C32,XC32\r\n" +
                "* \r\n" +
                "* NOT FOR HAND MODIFICATION\r\n" +
                "* This file is automatically generated by the MPFS2 Utility\r\n" +
                "* ALL MODIFICATIONS WILL BE OVERWRITTEN BY THE MPFS2 GENERATOR\r\n" +
                "* Generated " + sdf.format(date) + " " + "\r\n" +
                "* \r\n" +
                "***************************************************************/\r\n\r\n"+
                "/*****************************************************************************\r\n" +        
                "Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries\r\n\r\n" +
                "Microchip Technology Inc. and its subsidiaries.\r\n" +
                "\r\n" +
                "Subject to your compliance with these terms, you may use Microchip software\r\n" +
                "and any derivatives exclusively with Microchip products. It is your \r\n" +
                "responsibility to comply with third party license terms applicable to your\r\n" +
                "use of third party software (including open source software) that may\r\n" +
                "accompany Microchip software.\r\n" +
                "\r\n" +
                "THIS SOFTWARE IS SUPPLIED BY MICROCHIP \"AS IS\". NO WARRANTIES, WHETHER\r\n" +
                "EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED\r\n" +
                "WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR\r\n" +
                "PURPOSE.\r\n\r\n" +
                "IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,\r\n" +
                "INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND\r\n" +
                "WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS\r\n" +
                "BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE\r\n" +
                "FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN\r\n" +
                "ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,\r\n" +
                "THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.\r\n" +
                "\r\n"+
                " *****************************************************************************/\r\n\r\n" +
                "#include <stdint.h>\r\n" +
                "#include \"configuration.h\"\r\n" +
                "#include \"definitions.h\"\r\n" +
                "\r\n" +
                "/**************************************\r\n" +
                " * MPFS2 Image Data\r\n" +
                " **************************************/ \r\n\r\n" +
                "#if defined( __ICCARM__ )   // IAR compiler build \r\n"+
                "    #define DRV_MEMORY_DEVICE_MEDIA_SIZE         1024UL \r\n"+
                "    const uint8_t NVM_MEDIA_DATA[ DRV_MEMORY_DEVICE_MEDIA_SIZE * 1024 ] = {\r\n"+
                "#else\r\n"+
                "    const uint8_t __attribute__((space(prog),address(DRV_MEMORY_DEVICE_START_ADDRESS))) __attribute__((keep))\r\n"+
                "    NVM_MEDIA_DATA[ DRV_MEMORY_DEVICE_MEDIA_SIZE * 1024 ] = {\r\n"+
                "#endif"        
            );
        }        
        }catch (IOException e){}
    }

    public void Write(byte data)
    {
        char ASCIIdata;

        String tempStr;
        int temp=0;
        try{
        ASCIIdata = '.';
        /* cannot be displayed because it would open the possibility of */ 
        /*occuring in the sequence, which would mess up our block comment*/
        if (data >= 32 && data <= 126 && data != '*')   
        {
            ASCIIdata = (char)data;
        }
        if(ifUnifiedStackIsUsed && (ImageLength>0))
        {
            fout.write(",");
        }
        else
        {
            if (ImageLength % 1024 == 0)
            {
                ASCIILine = " " + ASCIILine;
            }
            else
            {
                fout.write(",");
            }
        }
        if (ImageLength % 16 == 0)
        {
            if (ImageLength != 0)
            {
                fout.write(ASCIILine + " */");
            }
            ASCIILine = " /* ";
        }        
        //System.out.println ("tempstr: " + tempStr );
        if(!ifUnifiedStackIsUsed)
        {
            temp = (int)(ImageLength / 1024);
            tempStr = String.format("%06x", temp);
            if (ImageLength % 1024 == 0)
            {
                fout.write("\r\n#define DATACHUNK"+tempStr);
            }
        }
        if ((ImageLength % 16 == 0) && (ImageLength != 0))
        {
            fout.write(" \\\r\n\t");
        }
        else if(ImageLength == 0)
        {
            fout.write("\r\n\t");
        }
        ASCIILine += Character.toString(ASCIIdata);
        tempStr = String.format("%02x", data);

        fout.write("0x" + tempStr);
        // System.out.println ("0x: " + tempStr );
        ImageLength++;
        }catch (IOException e) {
            System.out.println ("IO exception = " + e );
            log.add("ERROR: " + e.getMessage());
        }
    }
    public void Close()
    {
       String tempStr = " ";
       int lineLen=0;

       try{
       if (ImageLength % 16 != 0)
       {
           lineLen = (16-(ImageLength % 16))*5+1;
           String  formatString = "%"+lineLen+"c";
           emptyStr = " ";
           //tempStr = String.format(formatString,' ');
           //tempStr = tempStr+String.format("%20s",ASCIILine);
           tempStr = padLeft(emptyStr,lineLen);
           tempStr = tempStr+padRight(ASCIILine,20);
           tempStr = tempStr+" */";
           //tempStr = String.format("%20s",ASCIILine+" */");
           fout.write(tempStr);
       }

       fout.write("\r\n\r\n\r\n");
       if(ifUnifiedStackIsUsed)
       {
           if (ImageLength != 0)
           {
                fout.write("};\r\n\r\n");
                fout.write("/**************************************\r\n" +
                " * MPFS2 C linkable symbols\r\n" +
                " **************************************/\r\n" +
                "// The entire data array is defined as a single variable to \r\n" +
                "// ensure that the linker does not reorder the data chunks in Flash when compiler \r\n" +
                "// optimizations are turned on.\r\n" );

                 fout.write("struct MEDIA_STORAGE_PARTITION_INFO\r\n");
                 fout.write("{\r\n");
                 fout.write("\tconst char*     partName;           // name that identifies the partition\r\n");
                 fout.write("\t\t\t\t\t\t\t\t\t\t// valid names: \"mpfs2\", \"tcpip_storage\";\r\n");
                 fout.write("\tunsigned long   partStartOffset;    // the absolute starting offset on that media \r\n");
                 fout.write("\tunsigned long   partSize;           // size of the partition, in bytes\r\n");
                 fout.write("} MPFS_IMAGE_PARTITION_INFO = \r\n");
                 fout.write("{\r\n");
                 fout.write("\t\"mpfs2\",\r\n");
                 fout.write("\t(unsigned long)NVM_MEDIA_DATA,\r\n");
                 fout.write("\tsizeof(NVM_MEDIA_DATA),\r\n");
                 fout.write("};\r\n\r\n");
           }
           fout.write(
            "/**************************************************************\r\n" +
            " * End of MPFS\r\n" +
            " **************************************************************/\r\n"
             
            );
       }
       
       
       fout.flush();
       fout.close();
       } catch (IOException e) {
            System.out.println ("IO exception = " + e );
            log.add("ERROR: " + e.getMessage());
       }
    }
  }

  public class MPFS2BINWriter extends MPFS2Writer
  {
    public FileOutputStream file_output;
    public DataOutputStream data_out;
    public BufferedWriter fout;
    public ByteArrayOutputStream binOut = new ByteArrayOutputStream();
    public MPFS2BINWriter(String filename)
    {
        //Date date = new Date();
        //SimpleDateFormat sdf = new SimpleDateFormat("E MMM dd yyyy hh:mm:ss");
        if (!filename.endsWith(".bin"))//.equalsIgnoreCase(".C")))
        {
            filename += ".bin";
        }
        try{
        // Create an output stream to the file.
        file_output = new FileOutputStream (filename);
       
        }catch (IOException e) {
            System.out.println ("IO exception = " + e );
            log.add("ERROR: " + e.getMessage());
        }
        imageName = filename;
    }

    public void Write(byte data)
    {
        //try{
        ImageLength++;
        //file_output.write(data);
        binOut.write(data);
//        }catch (IOException e) {
//            System.out.println ("IO exception = " + e );
//            log.add("ERROR: " + e.getMessage());
//        }
    }

    public void Close()
    {/*
     * before closing the file descriptor ,
     * write byteArray collection to the file descriptor
     * this has been done to increase the speed to generate a .bin file.
     * This has been done only for the BIN file.
     */
        try{
            byte[] temp = binOut.toByteArray();
            file_output.write(temp,0, ImageLength);
            file_output.flush();
            file_output.close();
        }
        catch (IOException e) {
            System.out.println ("IO exception = " + e );
            log.add("ERROR: " + e.getMessage());
        }
    }
  }
 



public int getnumberOfDynamicRecordFiles()
{
    int numFileRecrds = 0;
    for(MPFSFileRecord file : files)
    {
        if(file.dynVarCntr >0)
        {
            numFileRecrds++;
        }        
    }
    return numFileRecrds;
}

		//public bool MDDWriter(String localPath)

public class MPFSClassicBuilder //extends MPFSBuilder
{
    public int ReserveBlock;
    Date date = new Date();
    SimpleDateFormat sdf = new SimpleDateFormat("E MMM dd yyyy hh:mm:ss");
    private FileOutputStream file_output;
    private DataOutputStream data_out;
    private BufferedWriter fout;
    /// <summary>
    /// Creates a new MPFS Classic image builder
    /// </summary>
    /// <param name="localPath">The directory to save the image in</param>
    /// <param name="localFile">The output file name for the image</param>
    public MPFSClassicBuilder(String localPath, String localFile)
    {
        if (!localPath.endsWith("\\"))
        {
            localPath += File.separator;
        }
        LocalPath = localPath;
        LocalFile = localFile;
        ReserveBlock = 0;
        log = new ArrayList<String>();
        files = new LinkedList<MPFSFileRecord>();
        indexUpdated = false;
    }

    byte MPFS_DATA = 0x00;
    byte MPFS_DELETED = 0x01;
    byte MPFS_DLE = 0x03;
    byte MPFS_ETX = 0x04;

    String MPFS_C_HEADER =
        "/***************************************************************\r\n" +
        " * MPFSImg.c\r\n" +
        " * Defines an MPFS2 image to be stored in program memory.\r\n" +
        " *\r\n" +
        " * NOT FOR HAND MODIFICATION\r\n" +
        " * This file is automatically generated by the MPFS2 Utility\r\n" +
        " * ALL MODIFICATIONS WILL BE OVERWRITTEN BY THE MPFS2 GENERATOR\r\n" +
        " * Generated " + sdf.format(date) + "\r\n" +
        " ***************************************************************/\r\n" +
        "\r\n" +
        "#define __MPFSIMG_C\r\n" +
        "\r\n" +
        "#include \"TCPIP Stack/TCPIP.h\"\r\n" +
        "\r\n" +
        "#if defined(STACK_USE_MPFS) && !defined(MPFS_USE_EEPROM) && !defined(MPFS_USE_SPI_FLASH)\r\n" +
        "\r\n";
    String MPFS_C_FOOTER =
        "/**************************************************************\r\n" +
        " * End of MPFS\r\n" +
        " **************************************************************/\r\n" +
        "#endif // #if defined(STACK_USE_MPFS) && !defined(MPFS_USE_EEPROM) && !defined(MPFS_USE_SPI_FLASH)" +
        "\r\n\r\n";
    String MPFS_ASM_HEADER =
        ";**************************************************************\r\n" +
        "; MPFSImg.s\r\n" +
        "; Defines an MPFS2 image to be stored in program memory.\r\n" +
        "; Defined in ASM30 assembly for optimal storage size.\r\n" +
        ";\r\n" +
        "; NOT FOR HAND MODIFICATION\r\n" +
        "; This file is automatically generated by the MPFS2 Utility\r\n" +
        "; ALL MODIFICATIONS WILL BE OVERWRITTEN BY THE MPFS2 GENERATOR\r\n" +
        "; Generated " + sdf.format(date) + "\r\n" +
        ";**************************************************************\r\n\r\n" +
        ".equ VALID_ID,0\r\n" +
        ".ifdecl __dsPIC30F\r\n" +
        "    .include \"p30fxxxx.inc\"\r\n" +
        ".endif\r\n" +
        ".ifdecl __dsPIC33F\r\n" +
        "    .include \"p33fxxxx.inc\"\r\n" +
        ".endif\r\n" +
        ".ifdecl __PIC24H\r\n" +
        "    .include \"p24hxxxx.inc\"\r\n" +
        ".endif\r\n" +
        ".ifdecl __PIC24F\r\n" +
        "    .include \"p24fxxxx.inc\"\r\n" +
        ".endif\r\n" +
        ".if VALID_ID <> 1\r\n" +
        "    .error \"Processor ID not specified in generic include files.  New ASM30 assembler needs to be downloaded?\"\r\n" +
        ".endif\r\n" +
        "	 .text\r\n" +
        "	 .section	MPFSData,code\r\n\r\n" +
        "	 goto END_OF_MPFS	; Prevent accidental execution of constant data.\r\n" +
        "	 .global BEGIN_MPFS\r\n" +
        "BEGIN_MPFS:\r\n";
    String MPFS_ASM_FOOTER = "\r\n\r\n; End of Generated Image\r\n";

    /// <summary>
    /// Adds a file to the MPFS image
    /// </summary>
    /// <param name="localName">Local file name to read</param>
    /// <param name="imageName">Name to use in image file</param>
    public boolean AddFile(String localName, String imageName)
    {
        // Skip if can't be opened
        if (!new File(localName).exists())
        {
            log.add("\r\nERROR: Could not read " + localName);
            return false;
        }

        // Set up the new file record
        MPFSFileRecord newFile = new MPFSFileRecord();
        newFile.SetFileName(imageName);

        // Read the data in, escaping as it is read
        byte b;
        int byteRead;

        try{
        ByteArrayOutputStream tempout = new ByteArrayOutputStream();
        FileInputStream inputFile = new FileInputStream(localName);

        while((byteRead=inputFile.read()) != -1){
            if (byteRead == MPFS_DLE || byteRead == MPFS_ETX)
            {
                tempout.write((byte)MPFS_DLE);
            }
            tempout.write((byte)byteRead);
        }
        newFile.data = tempout.toByteArray();
        newFile.fileSizeLen =  tempout.size();
        inputFile.close();
        // Add the file and return
        log.add("    " + imageName + ": " + newFile.fileSizeLen + " bytes");
        files.add(newFile);
        }catch (Exception e)
        {
            log.add("\r\nERROR: " + e.getMessage());
            return false;
        }

        return true;
    }

    /// <summary>
    /// Adds a directory to the MPFS image.  All non-hidden files will be included.
    /// </summary>
    /// <param name="dataPath">The local directory to search</param>
    /// <param name="imagePath">Ignored for MPFS Classic</param>
    /// <returns></returns>
    public boolean AddDirectory(String dataPath, String imagePath)
    {
        File dir = new File(dataPath);
        // Make sure directory exists
        if (!dir.exists())
        {
            log.add("\r\nERROR: Directory " + dataPath + " does not exist.");
            return false;
        }

        // Make sure directory is not the project directory
        if (LocalPath.contains(dataPath))
        {
            log.add("\r\nERROR: The project directory is located in the source " +
                    "directory.\r\n The generator cannot run if the image is" +
                    " \r\nto be placed " +
                    "in the source directory." +
                    "\r\n  Please select the base MPLAB project " +
                    "\r\ndirectory before continuing.");
            return false;
        }

        // Load directory members
        //DirectoryInfo dir = new DirectoryInfo(dataPath);
        File[] filelist = dir.listFiles();

        log.add(dataPath + " :");
        System.out.print("\r\ndataPath: " + dataPath);
        for(File f : filelist)
        {
            if(f.isHidden()||f.isDirectory())
            {
                continue;
            }
            else if(f.isFile())
            {
                
                System.out.print("\r\nimagePath: " + imagePath);
                System.out.print("\r\npath and File Name: " + imagePath+f.getName());
                this.AddFile(f.getPath(), imagePath+f.getName());
            }
        }

        return true;
    }

    public boolean Generate(MPFSOutputFormat format)
    {
        // Start with nothing
        generatedImageName = null;
        File dir = new File(LocalPath);
        // Make sure all paths exist
        if (!dir.exists())
        {
            log.add("\r\nERROR: Output directory \"" + LocalPath + "\" does not exist!");
            return false;
        }

        // Make sure we have some files
        if(files.size() == 0)
        {
            return false;
        }

        // Generate based on format
        try
        {
            switch (format)
            {
                case BIN:
                    return GenerateBIN(LocalPath + LocalFile);
                case C18:
                case C32:
                    return GenerateC(LocalPath + LocalFile);
                case ASM30:
                    return GenerateASM(LocalPath + LocalFile);
                default:
                    log.add("\r\nERROR: Invalid output format was specified.");
                    return false;
            }
        }
        catch (Exception e)
        {
            log.add("\r\nERROR: " + e.getMessage());
            return false;
        }

    }

    private boolean GenerateBIN(String filename)
    {
       ByteArrayOutputStream file_output = new ByteArrayOutputStream();
        // Open the file
        if (!filename.endsWith(".bin"))
        {
            filename += ".bin";
        }
        try{
        FileOutputStream classicbinOut = new FileOutputStream (filename);
      
        // Write the FAT data
        int baseAddr = ReserveBlock + 17 * ((int)files.size() + 1);
        for(MPFSFileRecord file : files)
        {
            file_output.write(MPFS_DATA);
            file_output.write((byte)(baseAddr));
            file_output.write((byte)(baseAddr >> 8));
            file_output.write((byte)(baseAddr >> 16));
            file_output.write((byte)(baseAddr >> 24));
            file_output.write(NormalizeFileName(file.GetFileName()).getBytes());
            //baseAddr += (int)file.data.size() + 5;
            baseAddr += (int)file.fileSizeLen+5;
        }
        file_output.write(MPFS_ETX);
        file_output.write(0xff);
        file_output.write(0xff);
        file_output.write(0xff);
        file_output.write(0xff);
        file_output.write("END OF FAT  ".getBytes());

        // Write the files
        for(MPFSFileRecord file : files)
        {
             for(int i=0;i<file.fileSizeLen;i++)
             {
                file_output.write(file.data[i]);
             }
            file_output.write(MPFS_ETX);
            file_output.write(0xff);
            file_output.write(0xff);
            file_output.write(0xff);
            file_output.write(0xff);
        }
        int fileSize = file_output.size();
        classicbinOut.write(file_output.toByteArray(),0,fileSize);
        // Flush the output and store the file name
        classicbinOut.flush();
        classicbinOut.close();
        }catch (Exception e)
        {
            log.add("\r\nERROR: " + e.getMessage());
            return false;
        }
        generatedImageName = filename;
        return true;
    }

    private boolean GenerateC(String filename)
    {
        String tempStr;
        // Open the file
        if (!filename.endsWith(".c"))
        {
            filename += ".c";
        }
        try{
        file_output = new FileOutputStream (filename);
        // Wrap the FileOutputStream with a DataOutputStream
        data_out = new DataOutputStream (file_output);
        fout = new BufferedWriter(new OutputStreamWriter(data_out));

        fout.write(MPFS_C_HEADER);

        // Write the files
        int fileIndex = 0;
        for(MPFSFileRecord file : files)
        {
            fout.write("\r\n/*******************************\r\n * Begin ");
            fout.write(NormalizeFileName(file.GetFileName()));
            fout.write("\r\n ******************************/\r\nstatic ROM unsigned char MPFS_");
            tempStr = String.format("%04x",fileIndex++)+"[] = \r\n{";
            fout.write(tempStr);

            for (int i = 0; i < file.fileSizeLen; i++)
            {
                if (i % 12 == 0)
                {
                    fout.write("\r\n\t");
                }
                tempStr = String.format("0x%02x",file.data[i])+",";
                fout.write(tempStr);
            }

            fout.write("\r\n\t0x04,0xff,0xff,0xff\r\n};\r\n");
        }

        // Write the FAT
        fileIndex = 0;
        fout.write(
            "/**************************************************\r\n" +
            " * Start of MPFS FAT\r\n" +
            " **************************************************/\r\n" +
            "typedef struct\r\n" +
            "{\r\n" +
            "    unsigned char Flags;\r\n" +
            "    ROM unsigned char* Address;\r\n" +
            "    unsigned char Name[12];\r\n" +
            "} FAT_TABLE_ENTRY;\r\n" +
            "\r\n" +
            "ROM FAT_TABLE_ENTRY MPFS_Start[] = \r\n" +
            "{"
        );
        for(MPFSFileRecord file : files)
        {
            tempStr = "\r\n\t{ 0x00, MPFS_"+String.format("%04x",fileIndex++);
            fout.write(tempStr);
            for(byte b : NormalizeFileName(file.GetFileName()).getBytes())
            {
                fout.write(", '" + (char)b + "'");
            }
            fout.write(" },");
        }
        fout.write(
            "\r\n\t{ 0x04, (ROM unsigned char*)0xffffff, 'E', 'N', 'D', ' ', 'O', 'F', ' ', 'F', 'A', 'T', ' ', ' ' },\r\n" +
            "};\r\n" +
            "/**************************************************\r\n" +
            " * End of MPFS FAT\r\n" +
            " **************************************************/\r\n\r\n"
        );

        fout.write(MPFS_C_FOOTER);

        // Flush the output and store the file name
        fout.flush();
        fout.close();
        }catch (Exception e)
        {
            log.add("\r\nERROR: " + e.getMessage());
            return false;
        }
        generatedImageName = filename;
        return true;
    }

    private boolean GenerateASM(String filename)
    {
        String tempStr;
        // Open the file
        if (!filename.endsWith(".s"))
        {
            filename += ".s";
        }
        try{
        file_output = new FileOutputStream (filename);
        // Wrap the FileOutputStream with a DataOutputStream
        data_out = new DataOutputStream (file_output);
        fout = new BufferedWriter(new OutputStreamWriter(data_out));

        fout.write(MPFS_ASM_HEADER);

        // Write the files
        int fileIndex = 0;
        for(MPFSFileRecord file : files)
        {
            fout.write("\r\n;*******************************\r\n;  Begin ");
            fout.write(NormalizeFileName(file.GetFileName()));
            fout.write("\r\n;*******************************\r\n" +
                "\tgoto\tEND_OF_MPFS_");
            tempStr = String.format("%04x",fileIndex);
            fout.write(tempStr);
            fout.write("\t\t; Prevent accidental execution of constant data\r\n\t.global _MPFS_");
            fout.write(tempStr);
            fout.write("\r\n_MPFS_");
            fout.write(tempStr);
            fout.write(":");

            for (int i = 0; i < file.fileSizeLen; i++)
            {
                if (i % 12 == 0)
                {
                    fout.write("\r\n\t.pbyte\t");
                }
                tempStr = String.format("0x%02x",file.data[i]);
                fout.write(tempStr);
                if (i % 12 != 11 && i != file.fileSizeLen-1)
                {
                    fout.write(",");
                }
            }

            fout.write("\r\n\t.pbyte\t0x04,0xff,0xff,0xff,0xff\r\nEND_OF_MPFS_");
            tempStr = String.format("%04x",fileIndex++);
            fout.write(tempStr);
            fout.write(":\r\n");
        }

        // Write the FAT
        fileIndex = 0;
        fout.write(
            ";*************************************************\r\n" +
            ";  Start of MPFS FAT\r\n" +
            ";*************************************************\r\n" +
            "\t.section\t.const,psv\r\n" +
            "\t.global _MPFS_Start\r\n" +
            "_MPFS_Start:"
        );
        for(MPFSFileRecord file : files)
        {
            fout.write("\r\n\t.byte\t0x00,0x00\r\n\t.long\tpaddr(_MPFS_");
            tempStr = String.format("%04x",fileIndex++);
            fout.write(tempStr);
            fout.write(")\r\n\t.byte\t");
            int i = 0;
            for(byte b : NormalizeFileName(file.GetFileName()).getBytes())
            {
                fout.write("'" + (char)b + "'");
                if (++i < 12)
                {
                    fout.write(",");
                }
            }
        }
        fout.write(
            "\r\n\t.byte\t0x04,0x00\r\n\t.long\t0xffffffff" +
            "\r\n\t.byte\t'E','N','D',' ','O','F',' ','F','A','T',' ',' '\r\n\r\n" +
            "\t.section MPFSEnd,code\r\n" +
            "END_OF_MPFS:\r\n"
        );

        fout.write(MPFS_ASM_FOOTER);

        // Flush the output and store the file name
        fout.flush();
        fout.close();
        }catch (Exception e)
        {
            log.add("\r\nERROR: " + e.getMessage());
            return false;
        }
        generatedImageName = filename;
        return true;
    }

    private String NormalizeFileName(String name)
    {
        if (name.length() > 12)
        {
            name = name.substring(0, 12);
        }
        return padRight(name,12).toUpperCase();
    }
}
public static String padRight(String s, int n) {
     return String.format("%1$-" + n + "s", s);
}

public static String padLeft(String s, int n) {
    return String.format("%1$" + n + "s", s);
}
}