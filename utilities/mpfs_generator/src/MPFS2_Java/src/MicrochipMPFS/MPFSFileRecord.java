/*********************************************************************
*
*  Keeps all the details of a file and all files in a list.
*
**********************************************************************
* File Name:       MPFSFileRecord.java
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

import java.util.*;

public class MPFSFileRecord
{
    private String fileName;
    public int nameHash;
    public long fileDate;
    public byte[] data;
    //public Vector<Byte> data;
    public int locStr;
    public int locData;
    public boolean hasIndex;
    public boolean isIndex;
    public boolean isZipped;
    public int dynVarCntr=0; /* Number of Dynamic Variables in the file */
    public Vector<Byte> dynVarOffsetAndIndexID = new Vector<Byte>(8, 8); /* Location of dynamic var and its ID */
    public int fileRecordOffset; /* Byte location in the Record file where this file record/information is written from */
    public int fileRecordLength; /* Total length/number of bytes in this file record */
    public int fileSizeLen;
    /// <summary>
    /// Sets up a new MPFSFileRecord
    /// </summary>
    public MPFSFileRecord()
    {
        locStr = 0;
        locData = 0;
        hasIndex = false;
        isIndex = false;
        isZipped = false;
        dynVarCntr = 0;
        //data = new Vector<Byte>(0);
        //Calendar cl = Calendar.getInstance();
        //cl.getTimeInMillis();
        fileSizeLen = 0;
    }
    public String GetFileName()
    {
        return this.fileName;
    }
    public void SetFileName(String value)
    {
        this.fileName = value;
        if(value.compareTo("") == 0)
            this.nameHash = 0xffff;
        else
        {
            this.nameHash = 0;
            for(byte b : value.getBytes())
            {
                nameHash += b;
                nameHash <<= 1;
            }
        }
        //System.out.println ("Value : " + value + "nameHash  " + String.format("%x", nameHash));
    }
    public void SetFiledate(long value)
    {
        this.fileDate = value;
    }
}

class FileRecord implements Comparable<FileRecord>
{
    public short nameHash;
    public int fileRecordOffset;
    public int dynVarCntr = 0;

    public FileRecord(short nameHash, int fileRecordOffset,int dynVarCntr)
    {
       this.nameHash = nameHash;
       this.fileRecordOffset = fileRecordOffset;
       this.dynVarCntr = dynVarCntr;
    }
    public int compareTo(FileRecord o) {
        int firstNameHash = this.nameHash & 0xffff;
        int secondNameHash = o.nameHash & 0xffff;
        return firstNameHash - secondNameHash;
    }
}
