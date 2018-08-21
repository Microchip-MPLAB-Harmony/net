/*******************************************************************************
 *  Summary:Simple Network Management Protocol (SNMP) Version 3 Agent 
 *  
 *  Module for Microchip TCP/IP Stack
 *	 -Provides SNMPv3 API for doing stuff
 *	
 *	-Reference: RFCs 3410, 3411, 3412, 3413, 3414 

*******************************************************************************/

/*******************************************************************************
File Name:  snmpv3_usm.c
Copyright © 2012 released Microchip Technology Inc.  All rights
reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_SNMPV3_SERVER

#include "tcpip/src/tcpip_private.h"
#if defined(TCPIP_STACK_USE_SNMPV3_SERVER)
#include "tcpip/snmpv3.h"
#include "tcpip/src/snmp_private.h"
#include "tcpip/src/snmpv3_private.h"
#include "tcpip/src/crypto/aes.h"
#include "crypto/src/des3.h"
#include "crypto/crypto.h"

extern TCPIP_SNMP_DCPT gSnmpDcpt;
static uint8_t md5LocalizedAuthKey[16];
static uint8_t sha1LocalizedAuthKey[20];
static uint8_t hmacAuthKeyBuf[64];
static uint8_t authKey_iPad[64];
static uint8_t authKey_oPad[64];
static uint8_t HmacMd5Digest[16];
static uint8_t HmacSHADigest[20];
static uint8_t snmpV3AesEncryptInitVector[16+1];//128 Bit
static uint8_t snmpV3AesDecryptInitVector[16+1];//128 Bit


static void _SNMPv3_PswdToLocalizedAuthKeyMD5Hashing(uint8_t* pswdToLocalized, uint8_t pswdLen);
static void _SNMPv3_PswdToLocalizedAuthKeySHAHashing(uint8_t* pswdToLocalized, uint8_t pswdLen);
static void _SNMPv3_ComputeMd5HmacCode(uint8_t xx_bits,
                                       uint8_t* digestptr,
                                       uint8_t * indata,
                                       uint32_t dataLen,
                                       uint8_t* userExtendedLclzdKeyIpad,
                                       uint8_t* userExtendedLclzdKeyOpad);
static void _SNMPv3_ComputeShaHmacCode(uint8_t xx_bits,
                                       uint8_t* digestptr,
                                       uint8_t * indata,
                                       uint32_t dataLen,
                                       uint8_t* userExtendedLclzdKeyIpad,
                                       uint8_t* userExtendedLclzdKeyOpad);
static void _SNMPv3_AuthKeyZeroingToHmacBufLen64(uint8_t* authKey, uint8_t authKeyLen,    uint8_t hashType);
static uint8_t* _SNMPv3_ComputeHmacMD5Digest(uint8_t * inData, uint32_t dataLen,uint8_t* userExtendedLclzdKeyIpad,uint8_t* userExtendedLclzdKeyOpad);
static uint8_t* _SNMPv3_ComputeHmacShaDigest(uint8_t * inData, uint32_t dataLen,uint8_t* userExtendedLclzdKeyIpad,uint8_t* userExtendedLclzdKeyOpad);
void SNMPv3USMAuthPrivPswdLocalization(uint8_t userDBIndex);

/****************************************************************************
  Function:
	void SNMPv3UsmAesEncryptDecrptInitVector(uint8_t inOutPdu)
	
  Summary:
  	AES Encryption and decryption  init vector.(RFC 3826 )
	
  Description:
  	The IV is concatenated as : the 32-bit snmpEngineBoots is converted to the first 4 Octects and 
  	the snmpEngineTime converted to subsequent four bytes.
  	 		 		  	
  Precondition:
   	SNMPv3Init() and ProcessVariabels() are called.	

  Parameters:
  	inOutPdu - SNMP request PDU
  	
  Return Values:
	None

  Remarks:
	None 
***************************************************************************/
void SNMPv3UsmAesEncryptDecrptInitVector(uint8_t inOutPdu)
{
    uint8_t j;
    uint8_t* decryptPtr;
    uint8_t* prvParamPtr;

    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;

    if(inOutPdu== SNMP_REQUEST_PDU) //init vector for decryption
    {

        prvParamPtr=snmpv3EngnDcptMemoryStubPtr->snmpInMsgPrvParamStrng;
        decryptPtr= snmpV3AesDecryptInitVector;

        //RFC 3826 section 3.1.2.1 Page #7
        //snmpV3AesDecryptInitVector=AuthSnmpEngnBoots+AuthSnmpEngnTime+inMsgPrivParam;
        *decryptPtr++=(snmpv3EngnDcptMemoryStubPtr->AuthoritativeSnmpEngineBoots>>24)&0xFF;
        *decryptPtr++=(snmpv3EngnDcptMemoryStubPtr->AuthoritativeSnmpEngineBoots>>16)&0xFF;
        *decryptPtr++=(snmpv3EngnDcptMemoryStubPtr->AuthoritativeSnmpEngineBoots>>8)&0xFF;
        *decryptPtr++=snmpv3EngnDcptMemoryStubPtr->AuthoritativeSnmpEngineBoots & 0xFF;

        *decryptPtr++=(snmpv3EngnDcptMemoryStubPtr->AuthoritativeSnmpEngnTime>>24)&0xFF;
        *decryptPtr++=(snmpv3EngnDcptMemoryStubPtr->AuthoritativeSnmpEngnTime>>16)&0xFF;
        *decryptPtr++=(snmpv3EngnDcptMemoryStubPtr->AuthoritativeSnmpEngnTime>>8)&0xFF;
        *decryptPtr++=snmpv3EngnDcptMemoryStubPtr->AuthoritativeSnmpEngnTime&0xFF;

        j=0;
        while(1)
        {
            *decryptPtr++=*(prvParamPtr+j);
            if(j==7)
                break;
            j++;
        }
    }
    else if(inOutPdu == SNMP_RESPONSE_PDU) //init vector for encryption
    {

        prvParamPtr=snmpv3EngnDcptMemoryStubPtr->SnmpOutMsgPrvParmStrng;
        decryptPtr= snmpV3AesEncryptInitVector;

        *decryptPtr++=(snmpv3EngnDcptMemoryStubPtr->SnmpEngineBoots>>24)&0xFF;
        *decryptPtr++=(snmpv3EngnDcptMemoryStubPtr->SnmpEngineBoots>>16)&0xFF;
        *decryptPtr++=(snmpv3EngnDcptMemoryStubPtr->SnmpEngineBoots>>8)&0xFF;
        *decryptPtr++=(snmpv3EngnDcptMemoryStubPtr->SnmpEngineBoots)&0xFF;

        *decryptPtr++=(snmpv3EngnDcptMemoryStubPtr->SnmpEngineTime>>24)&0xFF;
        *decryptPtr++=(snmpv3EngnDcptMemoryStubPtr->SnmpEngineTime>>16)&0xFF;
        *decryptPtr++=(snmpv3EngnDcptMemoryStubPtr->SnmpEngineTime>>8)&0xFF;
        *decryptPtr++=snmpv3EngnDcptMemoryStubPtr->SnmpEngineTime&0xFF;

        j=0;
        while(1)
        {
            *decryptPtr++=*(prvParamPtr+j);
            if(j==7)
                break;
            j++;
        }
    }
}

/****************************************************************************
  Function:
	void SNMPv3UsmDesEncryptDecrptInitVector(uint8_t inOutPdu)
	
  Summary:
  	DES Encryption and decryption  init vector.(RFC 3826 )
	
  Description:
  	The IV is concatenated as : the 32-bit snmpEngineBoots is converted to the first 4 Octects and 
  	the snmpEngineTime (used as the 32 bit interger for the 'Salt') converted to subsequent four bytes.
  	 		 		  	
  Precondition:
   	SNMPv3Init() and ProcessVariabels() are called.	

  Parameters:
  	inOutPdu - SNMP request PDU
  	
  Return Values:
	None

  Remarks:
	None 
***************************************************************************/
void SNMPv3UsmDesEncryptDecrptInitVector(uint8_t inOutPdu, uint8_t *preIV, uint8_t* snmpv3DesEncrptDcrptIV)
{
    uint8_t cntr;
    uint8_t* prvParamInoutPduSaltPtr;
    //uint8_t privParamSalt[TCPIP_SNMPV3_DES_CRYPTO_KEY_LEN];
	
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;

    if(inOutPdu== SNMP_REQUEST_PDU) //init vector for decryption
    {
        prvParamInoutPduSaltPtr=snmpv3EngnDcptMemoryStubPtr->snmpInMsgPrvParamStrng;

        //RFC 3414 section 8.1.1.1 Page #64
	//snmpv3DesEncrptDcrptIV=prvParamSaltPtr EX-OR preIV;

    	for(cntr=0;cntr<TCPIP_SNMPV3_DES_CRYPTO_KEY_LEN;cntr++)	
        {
           *(snmpv3DesEncrptDcrptIV + cntr) =(*(preIV + cntr) ^ *(prvParamInoutPduSaltPtr + cntr));            
        }
    }
    else if(inOutPdu == SNMP_RESPONSE_PDU) //init vector for encryption
    {
        //prvParamInoutPduSaltPtr=privParamSalt;
        prvParamInoutPduSaltPtr = snmpv3EngnDcptMemoryStubPtr->SnmpOutMsgPrvParmStrng;
		
        snmpv3EngnDcptMemoryStubPtr->SnmpOutMsgPrivParmLen=TCPIP_SNMPV3_DES_CRYPTO_KEY_LEN;

        for(cntr=0;cntr<TCPIP_SNMPV3_DES_CRYPTO_KEY_LEN;cntr++)
        {
            *(snmpv3DesEncrptDcrptIV+cntr) =(*(preIV+cntr) ^ *(prvParamInoutPduSaltPtr+cntr));
        }
    }
}

/****************************************************************************
  Function:
	void SNMPv3USMOutMsgPrivParam(void)
	
  Summary:
  	SNMP USM out message uses Privacy protocol (RFC 3826 )
	
  Description:
  	SNMPEngineTime is used to encrypt the outgoing message with a random value.
  	 		 		  	
  Precondition:
   	SNMPv3Init() and ProcessVariabels() are called.	

  Parameters:
  	None
  	
  Return Values:
	None

  Remarks:
	None 
***************************************************************************/
void SNMPv3USMOutMsgPrivParam(SNMPV3_PRIV_PROT_TYPE privType)
{
    uint8_t* prvParamPtr;
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;
	

	//SNMP ENgine Time is 32 Bit counter. 64 Bit counter (Extended Data type) to be implemented.
    TCPIP_SNMPv3_AuthEngineTimeGet();

    prvParamPtr=snmpv3EngnDcptMemoryStubPtr->SnmpOutMsgPrvParmStrng;
    
    if(privType == SNMPV3_AES_PRIV)
    {
	    *prvParamPtr++=0x00;
	    *prvParamPtr++=0x00;
	    *prvParamPtr++=0x00;
	    *prvParamPtr++=0x00;
	    //ORing is done to generate some random number from the snmp engine time SYS_RANDOM_PseudoGet() not used.
	    *prvParamPtr++=((snmpv3EngnDcptMemoryStubPtr->SnmpEngineTime>>24)&0xFF)^0xFF;
	    *prvParamPtr++=((snmpv3EngnDcptMemoryStubPtr->SnmpEngineTime>>16)&0xFF)^0xEE;
	    *prvParamPtr++=((snmpv3EngnDcptMemoryStubPtr->SnmpEngineTime>>8)&0xFF)^0xDD;
	    *prvParamPtr++=(snmpv3EngnDcptMemoryStubPtr->SnmpEngineTime&0xFF)^0xCC;
    }
    else if(privType == SNMPV3_DES_PRIV)
    {
        *prvParamPtr++=(snmpv3EngnDcptMemoryStubPtr->SnmpEngineBoots>>24)&0xFF;
        *prvParamPtr++=(snmpv3EngnDcptMemoryStubPtr->SnmpEngineBoots>>16)&0xFF;
        *prvParamPtr++=(snmpv3EngnDcptMemoryStubPtr->SnmpEngineBoots>>8)&0xFF;
        *prvParamPtr++=(snmpv3EngnDcptMemoryStubPtr->SnmpEngineBoots)&0xFF;

        *prvParamPtr++=(snmpv3EngnDcptMemoryStubPtr->SnmpEngineTime>>24)&0xFF;
        *prvParamPtr++=(snmpv3EngnDcptMemoryStubPtr->SnmpEngineTime>>16)&0xFF;
        *prvParamPtr++=(snmpv3EngnDcptMemoryStubPtr->SnmpEngineTime>>8)&0xFF;
        *prvParamPtr++=snmpv3EngnDcptMemoryStubPtr->SnmpEngineTime&0xFF;
        snmpv3EngnDcptMemoryStubPtr->SnmpOutMsgPrivParmLen = TCPIP_SNMPV3_DES_CRYPTO_KEY_LEN;
    }
    else
        return;
	
}

/****************************************************************************
  Function:
	void SNMPv3UsmOutMsgAuthParam(UINT8 hashType)
	
  Summary:
  	Both MD5 and SHA1 is used for the outgoing message authentication. 
	
  Description:
  	This routine prepares out message with HMAC-MD5 or HMAC-SHA1
  	authentication protocol.(RFC- 3414 - section 6)
  	 		 		  	
  Precondition:
   	SNMPv3Init() and ProcessVariabels() are called.	

  Parameters:
  	hashType  - authentication protocol type
  	
  Return Values:
	None

  Remarks:
	None 
***************************************************************************/

void SNMPv3UsmOutMsgAuthParam(uint8_t hashType)
{
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;
		

    if(hashType == SNMPV3_HMAC_MD5)
    {
        _SNMPv3_ComputeMd5HmacCode(96,snmpv3EngnDcptMemoryStubPtr->SnmpOutMsgAuthParaStrng,
                                (uint8_t*)snmpv3EngnDcptMemoryStubPtr->TrapMsgHeaderBuf.head,
                                        snmpv3EngnDcptMemoryStubPtr->TrapMsgHeaderBuf.length,
                            snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[0].userAuthLocalKeyHmacIpad,
                            snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[0].userAuthLocalKeyHmacOpad);
    }
    else if(hashType == SNMPV3_HMAC_SHA1)
    {
        _SNMPv3_ComputeShaHmacCode(96,snmpv3EngnDcptMemoryStubPtr->SnmpOutMsgAuthParaStrng,
                                (uint8_t*)snmpv3EngnDcptMemoryStubPtr->TrapMsgHeaderBuf.head,
                                    snmpv3EngnDcptMemoryStubPtr->TrapMsgHeaderBuf.length,
                                    snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[0].userAuthLocalKeyHmacIpad,
                                    snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[0].userAuthLocalKeyHmacOpad);
    }
}

/****************************************************************************
  Function:
	void SNMPv3USMAuthPrivPswdLocalization(UINT8 userDBIndex)
	
  Summary:
  	Convert Auth and Priv  password to the localized Key using SNMPEngineID.
	
  Description:
  	This routine converts MD5 or SHA1 and AES privacy password key
  	to localized key using snmpSngineID(RFC- 3414 - section 6).
  	 		 		  	
  Precondition:
   	SNMPv3Init() and ProcessVariabels() are called.	

  Parameters:
  	userDBIndex  - authentication protocol type
  	
  Return Values:
	None

  Remarks:
	None 
***************************************************************************/
void SNMPv3USMAuthPrivPswdLocalization(uint8_t userDBIndex)
{
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;

    if(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userHashType== SNMPV3_HMAC_MD5)
    {
        _SNMPv3_PswdToLocalizedAuthKeyMD5Hashing(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userAuthPswd,
                                                                                    snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userAuthPswdLen);
        memcpy(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userAuthPswdLoclizdKey,md5LocalizedAuthKey,16);

        if((snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userPrivPswdLen != 0x00)) //&&
            //(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userPrivType == SNMPV3_AES_PRIV))
        {
            _SNMPv3_PswdToLocalizedAuthKeyMD5Hashing(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userPrivPswd,
                                                                                        snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userPrivPswdLen);

            memcpy(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userPrivPswdLoclizdKey,md5LocalizedAuthKey,16);
        }
    }
    else if(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userHashType == SNMPV3_HMAC_SHA1)
    {
        _SNMPv3_PswdToLocalizedAuthKeySHAHashing(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userAuthPswd,
                                                                                    snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userAuthPswdLen);
        memcpy(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userAuthPswdLoclizdKey,sha1LocalizedAuthKey,20);

        if((snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userPrivPswdLen != 0x00)) //&&
            //(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userPrivType == SNMPV3_AES_PRIV))
        {
            _SNMPv3_PswdToLocalizedAuthKeySHAHashing(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userPrivPswd,
                                                                                        snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userPrivPswdLen);

            memcpy(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userPrivPswdLoclizdKey,sha1LocalizedAuthKey,20);
        }
    }

    return;
}

/****************************************************************************
  Function:
    static void SNMPv3PswdToLocalizedAuthKeyMD5Hashing(UINT8* pswdToLocalized, UINT8 pswdLen)
	
  Summary:
    Convert MD5 Auth password to the localized Key using SNMPEngineID.
	
  Description:
    This routine converts HMAC-MD5 authentication password key
    to localized key using snmpSngineID(RFC- 3414).
  	 		 		  	
  Precondition:
    SNMPv3Init() and ProcessVariabels() are called.

  Parameters:
    pswdToLocalized -  password storage poniter
    pswdLen - password length.
  	
  Return Values:
    None

  Remarks:
    None
***************************************************************************/
static void _SNMPv3_PswdToLocalizedAuthKeyMD5Hashing(uint8_t* pswdToLocalized, uint8_t pswdLen)
{
    CRYPT_MD5_CTX  md5;
    uint8_t *compressionPtr, pswdBuf[64];
    uint32_t index = 0;
    uint32_t count = 0, i;
    uint8_t* pswdPtr;
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;


    pswdPtr=pswdToLocalized;

    CRYPT_MD5_Initialize(&md5);

    while (count < 1048576)
    {
        compressionPtr = pswdBuf;
        for (i = 0; i < 64; i++)
        {
            *compressionPtr++ = pswdPtr[index++ % pswdLen];
        }
        CRYPT_MD5_DataAdd(&md5, pswdBuf, 64);
        count+=64;
    }
    CRYPT_MD5_Finalize(&md5, md5LocalizedAuthKey);

    memcpy(pswdBuf, md5LocalizedAuthKey, 16 /*localizedAuthKey buf len*/);
    memcpy(pswdBuf+16, snmpv3EngnDcptMemoryStubPtr->SnmpEngineID, snmpv3EngnDcptMemoryStubPtr->SnmpEngnIDLength);
    memcpy(pswdBuf+16+snmpv3EngnDcptMemoryStubPtr->SnmpEngnIDLength, md5LocalizedAuthKey, 16 /*localizedAuthKey buf len*/);

    CRYPT_MD5_Initialize(&md5);
    
    CRYPT_MD5_DataAdd(&md5,pswdBuf,32+snmpv3EngnDcptMemoryStubPtr->SnmpEngnIDLength);

    CRYPT_MD5_Finalize(&md5, md5LocalizedAuthKey);
    
    count+=64;

    return;
}

/****************************************************************************
  Function:
	static void SNMPv3PswdToLocalizedAuthKeySHAHashing(UINT8* pswdToLocalized, UINT8 pswdLen)
	
  Summary:
  	Convert SHA Auth password to the localized Key using SNMPEngineID.
	
  Description:
  	This routine converts HMAC-SHA authentication password key
  	to localized key using snmpSngineID(RFC- 3414).
  	 		 		  	
  Precondition:
   	SNMPv3Init() and ProcessVariabels() are called.	

  Parameters:
  	pswdToLocalized -  password storage poniter 
  	pswdLen - password length.
  	
  Return Values:
	None

  Remarks:
	None 
***************************************************************************/
static void _SNMPv3_PswdToLocalizedAuthKeySHAHashing(uint8_t* pswdToLocalized, uint8_t pswdLen)
{
    CRYPT_SHA_CTX  sha1;
    uint8_t *compressionPtr, pswdBuf[72];
    uint32_t index = 0;
    uint32_t count = 0, i;
    uint8_t* pswdPtr;
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;

    pswdPtr=pswdToLocalized;

    CRYPT_SHA_Initialize(&sha1);
	
    while (count < 1048576)
    {
        compressionPtr = pswdBuf;
        for (i = 0; i < 64; i++)
        {
            *compressionPtr++ = pswdPtr[index++ % pswdLen];
        }
        CRYPT_SHA_DataAdd(&sha1, pswdBuf, 64);
        count+=64;
    }
    CRYPT_SHA_Finalize(&sha1, sha1LocalizedAuthKey);

    memcpy(pswdBuf, sha1LocalizedAuthKey, 20 /*SHA1 localizedAuthKey buf len*/);
    memcpy(pswdBuf+20, snmpv3EngnDcptMemoryStubPtr->SnmpEngineID, snmpv3EngnDcptMemoryStubPtr->SnmpEngnIDLength);
    memcpy(pswdBuf+20+snmpv3EngnDcptMemoryStubPtr->SnmpEngnIDLength, sha1LocalizedAuthKey, 20 /*SHA1 localizedAuthKey buf len*/);

    CRYPT_SHA_Initialize(&sha1);
    CRYPT_SHA_DataAdd(&sha1,pswdBuf,40+snmpv3EngnDcptMemoryStubPtr->SnmpEngnIDLength);

    CRYPT_SHA_Finalize(&sha1, sha1LocalizedAuthKey);
    count+=64;

    return;
}

/****************************************************************************
  Function:
    void SNMPv3ComputeHMACIpadOpadForAuthLoclzedKey(UINT8 userDBIndex)
	
  Summary:
    Compute HMAC inner and outer pad for authorization localized key.
	
  Description:
    This routine computes HMAC inner and outer pad strings for authorization localized key.
    RFC - 2104.
  	 		 		  	
  Precondition:
    SNMPv3Init() and ProcessVariabels() are called.

  Parameters:
    userDBIndex -  password storage poniter
  	  	
  Return Values:
    None

  Remarks:
    None
***************************************************************************/
void SNMPv3ComputeHMACIpadOpadForAuthLoclzedKey(uint8_t userDBIndex)
{
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;

    if(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userHashType==SNMPV3_HMAC_MD5)
    {
        _SNMPv3_AuthKeyZeroingToHmacBufLen64(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userAuthPswdLoclizdKey,
                                                            16,snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userHashType);
    }
    else if(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userHashType==SNMPV3_HMAC_SHA1)
    {
        _SNMPv3_AuthKeyZeroingToHmacBufLen64(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userAuthPswdLoclizdKey,
                                                            20,snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userHashType);
    }

    //Authorization key inner padding
    uint8_t i=0;
    for(i=0;i<64;i++)
    {
        authKey_iPad[i]=hmacAuthKeyBuf[i]^0x36;
    }

    //Authorization key outer padding
    for(i=0;i<64;i++)
    {
        authKey_oPad[i]=hmacAuthKeyBuf[i]^0x5c;
    }

    memcpy(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userAuthLocalKeyHmacIpad,authKey_iPad,64);
    memcpy(snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userDBIndex].userAuthLocalKeyHmacOpad,authKey_oPad,64);
}

/****************************************************************************
  Function:
    uint8_t SNMPv3AuthenticateRxedPduForDataIntegrity(void)
	
  Summary:
    Authenticate an incoming SNMPV3 USM PDU using MD5 or SHA
	
  Description:
    This routine authenticates SNMPV3 incoming report PDU message and also for different
    type of GET requests with both MD5 and SHA protocol.If the received PDU username is
    similar to "initial", then there should be report PDU.
    RFC - 3414.
  	 		 		  	
  Precondition:
    SNMPv3Init() and ProcessVariabels() are called.

  Parameters:
    rxDataPtr -  incoming PDU
  	  	
  Return Values:
    SNMPV3_MSG_AUTH_PASS  - Authentication success
    SNMPV3_MSG_AUTH_FAIL - Authentication failure

  Remarks:
    None
***************************************************************************/
uint8_t SNMPv3AuthenticateRxedPduForDataIntegrity(void)
{	
    uint8_t reportMsgName[7]="initial";//response is "report" 0xa8 msg
    uint8_t* secNamePtr;
    uint8_t i;
    uint16_t authParamOffset;
    uint8_t hashTYpe;
    static CRYPT_MD5_CTX md5;
    static CRYPT_SHA_CTX  sha;
    uint32_t dataLen=0;
    uint32_t   tempRxOffset=0;

    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;
					
    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;

    secNamePtr= snmpv3EngnDcptMemoryStubPtr->SecurtyPrimtvesOfIncmngPdu.securityName;
    hashTYpe=snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx].userHashType;

    //Check if the received packet is expecting "report" as response.
    if(!strncmp((const char *)secNamePtr,
                            (const char *)reportMsgName,
                            (snmpv3EngnDcptMemoryStubPtr->SecurtyPrimtvesOfIncmngPdu.securityNameLength)))
    {
        return false; //If "report" is expected, Return.
    }

    tempRxOffset = TCPIP_SNMP_GetRXOffset();
    authParamOffset=snmpv3EngnDcptMemoryStubPtr->InPduWholeMsgBuf.msgAuthParamOffsetInWholeMsg;
    TCPIP_SNMP_SetRXOffset(authParamOffset);
    for(i=0;i<snmpv3EngnDcptMemoryStubPtr->SnmpInMsgAuthParamLen /*Should be 12 Bytes*/;i++)
    {
         //RFC3414 Section 6.3.2 Page#56 Step3
        TCPIP_SNMP_CopyOfDataToINUDPBuff(&gSnmpDcpt.udpGetBufferData,0x00);        
    }
        
    dataLen = gSnmpDcpt.udpGetBufferData.endPtr - gSnmpDcpt.udpGetBufferData.head;

    if(hashTYpe == SNMPV3_HMAC_MD5)
    {
        CRYPT_MD5_Initialize(&md5);
        CRYPT_MD5_DataAdd(&md5,snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx].userAuthLocalKeyHmacIpad, (uint16_t)0x40);
        CRYPT_MD5_DataAdd(&md5, gSnmpDcpt.udpGetBufferData.head,dataLen);
        CRYPT_MD5_Finalize(&md5, HmacMd5Digest);
        
        CRYPT_MD5_Initialize(&md5);
        CRYPT_MD5_DataAdd(&md5, snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx].userAuthLocalKeyHmacOpad, (uint16_t)0x40);
        CRYPT_MD5_DataAdd(&md5, HmacMd5Digest,16);
        CRYPT_MD5_Finalize(&md5, HmacMd5Digest);
    }
    else if(hashTYpe == SNMPV3_HMAC_SHA1)
    {
        CRYPT_SHA_Initialize(&sha);
        CRYPT_SHA_DataAdd(&sha,snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx].userAuthLocalKeyHmacIpad, (uint16_t)0x40);
        CRYPT_SHA_DataAdd(&sha, gSnmpDcpt.udpGetBufferData.head,dataLen);
        CRYPT_SHA_Finalize(&sha, HmacSHADigest);

        CRYPT_SHA_Initialize(&sha);
        CRYPT_SHA_DataAdd(&sha, snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx].userAuthLocalKeyHmacOpad, (uint16_t)0x40);
        CRYPT_SHA_DataAdd(&sha,HmacSHADigest,20);
        CRYPT_SHA_Finalize(&sha, HmacSHADigest);
            //return true;
    }
    else
        return SNMPV3_MSG_AUTH_FAIL ;

    if(hashTYpe == SNMPV3_HMAC_MD5)
    {
        i=strncmp((const char *)&snmpv3EngnDcptMemoryStubPtr->SnmpInMsgAuthParmStrng,(const char *)&HmacMd5Digest,12);
    }
    else if(hashTYpe == SNMPV3_HMAC_SHA1)
    {
        i=strncmp((const char *)&snmpv3EngnDcptMemoryStubPtr->SnmpInMsgAuthParmStrng,(const char *)&HmacSHADigest,12);
    }
    if(i!=0)
        return SNMPV3_MSG_AUTH_FAIL;


    //Authparam validated on WholeMsg. Write back the auth param string to received buffer
    authParamOffset=snmpv3EngnDcptMemoryStubPtr->InPduWholeMsgBuf.msgAuthParamOffsetInWholeMsg;
    TCPIP_SNMP_SetRXOffset(authParamOffset);
    for(i=0;i<snmpv3EngnDcptMemoryStubPtr->SnmpInMsgAuthParamLen /*Should be 12 Bytes*/;i++)
    {
        TCPIP_SNMP_CopyOfDataToINUDPBuff(&gSnmpDcpt.udpGetBufferData,snmpv3EngnDcptMemoryStubPtr->SnmpInMsgAuthParmStrng[i]);
    }

    TCPIP_SNMP_SetRXOffset(tempRxOffset);
    return SNMPV3_MSG_AUTH_PASS;	

}
/****************************************************************************
  Function:
    uint8_t SNMPv3AuthenticateTxPduForDataIntegrity(SNMPV3_RESPONSE_WHOLEMSG* txDataPtr)
	
  Summary:
    Authenticate to an outgoing SNMPV3 USM PDU using MD5 or SHA
	
  Description:
    This routine authenticates SNMPV3 outgoing report PDU message and also for GET
    Response PDU for whole message.
    RFC - 3414.
  	 		 		  	
  Precondition:
    SNMPv3Init() and ProcessVariabels() are called.

  Parameters:
    txDataPtr -  outgoing PDU
  	  	
  Return Values:
    SNMPV3_MSG_AUTH_PASS  - Authentication success
    SNMPV3_MSG_AUTH_FAIL - Authentication failure

  Remarks:
    None
***************************************************************************/
uint8_t SNMPv3AuthenticateTxPduForDataIntegrity(SNMPV3_RESPONSE_WHOLEMSG* txDataPtr)
{	
    uint8_t* secNamePtr = 0;
    uint8_t i;
    static CRYPT_MD5_CTX md5;
    static CRYPT_SHA_CTX  sha;
    uint8_t* tempPtr;
    uint8_t hashTYpe;

    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;
				
    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;

    hashTYpe=snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx].userHashType;

    if(hashTYpe == SNMPV3_HMAC_MD5)
    {
        CRYPT_MD5_Initialize(&md5);
        CRYPT_MD5_DataAdd(&md5,snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx].userAuthLocalKeyHmacIpad, (uint16_t)0x40);
        CRYPT_MD5_DataAdd(&md5,txDataPtr->wholeMsgHead, txDataPtr->wholeMsgLen);
        CRYPT_MD5_Finalize(&md5, HmacMd5Digest);

        CRYPT_MD5_Initialize(&md5);
        CRYPT_MD5_DataAdd(&md5,snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx].userAuthLocalKeyHmacOpad, (uint16_t)0x40);
        CRYPT_MD5_DataAdd(&md5, HmacMd5Digest,16);
        CRYPT_MD5_Finalize(&md5, HmacMd5Digest);
    }
    else if(hashTYpe == SNMPV3_HMAC_SHA1)
    {
        CRYPT_SHA_Initialize(&sha);
        CRYPT_SHA_DataAdd(&sha,snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx].userAuthLocalKeyHmacIpad, (uint16_t)0x40);
        CRYPT_SHA_DataAdd(&sha, txDataPtr->wholeMsgHead, txDataPtr->wholeMsgLen);
        CRYPT_SHA_Finalize(&sha, HmacSHADigest);

        CRYPT_SHA_Initialize(&sha);
        CRYPT_SHA_DataAdd(&sha,snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx].userAuthLocalKeyHmacOpad, (uint16_t)0x40);
        CRYPT_SHA_DataAdd(&sha, HmacSHADigest,20);
        CRYPT_SHA_Finalize(&sha, HmacSHADigest);
    }
    else
        return SNMPV3_MSG_AUTH_FAIL ;

    //Authparam validated on WholeMsg. Write back the auth param string to received buffer
    tempPtr=snmpv3EngnDcptMemoryStubPtr->SnmpOutMsgAuthParaStrng;
    if(hashTYpe == SNMPV3_HMAC_MD5)
        secNamePtr=HmacMd5Digest;
    else if(hashTYpe == SNMPV3_HMAC_SHA1)
        secNamePtr=HmacSHADigest;


    i=0;
    for(i=0;i < 12/*SnmpOutMsgAuthParmLen Should be 12 Bytes*/;i++)
    {
        tempPtr[i]=secNamePtr[i];
    }
    return SNMPV3_MSG_AUTH_PASS;
}

/****************************************************************************
  Function:
    uint8_t SNMPv3AESDecryptRxedScopedPdu(void)
	
  Summary:
    Incoming SNMPv3 scoped PDU decryption using AES decryption protocol.
	
  Description:
    This routine decrypts SNMPV3 incoming PDU using AES protocol , but before this
    encrypted data length is verified.If the length of the encrypted OCTECT-STRING
    is not multiple of 8, then dryption will be halted.
    RFC - 3414. ( section 8)
  	 		 		  	
  Precondition:
    SNMPv3Init() and ProcessVariabels() are called.

  Parameters:
    None
  	  	
  Return Values:
    SNMPV3_MSG_PRIV_FAIL - Failure
    SNMPV3_MSG_PRIV_PASS - Success

  Remarks:
    None
***************************************************************************/
uint8_t SNMPv3AESDecryptRxedScopedPdu(void)
{
    uint8_t* cryptoKey;
    uint8_t* initVector;
    uint8_t* snmpv3_cipher_text;
    uint16_t cipherTextLen;
    uint8_t decrypted_text[TCPIP_SNMP_MAX_MSG_SIZE];
    uint16_t temp;
	
    AES_ROUND_KEYS_128_BIT round_keys;
    AES_CFB_STATE_DATA current_stream;

    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);
    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;

    cryptoKey=snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx].userPrivPswdLoclizdKey;
    initVector=snmpV3AesDecryptInitVector;
    temp=snmpv3EngnDcptMemoryStubPtr->InPduWholeMsgBuf.scopedPduOffset;
    snmpv3_cipher_text = gSnmpDcpt.udpGetBufferData.head+temp;
    cipherTextLen= snmpv3EngnDcptMemoryStubPtr->InPduWholeMsgBuf.scopedPduStructLen;
    memset(decrypted_text,0,sizeof(decrypted_text));
    

    AESCreateRoundKeys(&round_keys,cryptoKey,AES_KEY_SIZE_128_BIT);
    memcpy(current_stream.initial_vector,initVector,16);
    AESCFBDecrypt(decrypted_text,snmpv3_cipher_text, cipherTextLen,
                                    &round_keys, &current_stream,
                                    AES_STREAM_START | AES_USE_CFB128);
	
    //Copy decrypted text to already allocated WholeMsg dynamic memory Buffer.
    memcpy(snmpv3_cipher_text,decrypted_text,cipherTextLen);

    return SNMPV3_MSG_PRIV_PASS;
}

/****************************************************************************
  Function:
    uint8_t SNMPv3AESEncryptResponseScopedPdu(SNMPV3_RESPONSE_WHOLEMSG* plain_text)
	
  Summary:
    outGoing SNMPv3 scoped PDU Encryption using AES encryption protocol.
	
  Description:
    This routine encrypts SNMPV3 outgoing PDU using AES protocol to maintain the data
    confidentiality. The data is encrypted in Cipher Block Chaining mode. The length of the
    encrypted data should be multiple of 8 and it is not then then data is padded in the end if necessary.
    RFC - 3414. ( section 8)
  	 		 		  	
  Precondition:
    SNMPv3Init() and ProcessVariabels() are called.

  Parameters:
    plain_text -  whole PDU message
  	  	
  Return Values:
    SNMPV3_MSG_PRIV_FAIL - Failure
    SNMPV3_MSG_PRIV_PASS - Success

  Remarks:
    None
***************************************************************************/
uint8_t SNMPv3AESEncryptResponseScopedPdu(SNMPV3_RESPONSE_WHOLEMSG* plain_text)
{	
    uint8_t* cryptoKey;
    uint8_t* initVector;
    uint8_t* plainText;
    uint16_t plaintextLen;
    uint8_t  encrypted_text[TCPIP_SNMP_MAX_MSG_SIZE];
    AES_ROUND_KEYS_128_BIT round_keys;
    AES_CFB_STATE_DATA current_stream;

    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;

    //This is a secured request. Compute the AES Encryption IV
    SNMPv3UsmAesEncryptDecrptInitVector(SNMP_RESPONSE_PDU);

    plaintextLen= (plain_text->scopedPduStructLen);
    cryptoKey=snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx].userPrivPswdLoclizdKey;
    initVector=snmpV3AesEncryptInitVector;
    plainText=(plain_text->scopedPduOffset);

    memset(encrypted_text,0,sizeof(encrypted_text));

    AESCreateRoundKeys (&round_keys,cryptoKey,AES_KEY_SIZE_128_BIT);

    memcpy(current_stream.initial_vector,initVector,16);

    AESCFBEncrypt(encrypted_text,plainText, plaintextLen,
    &round_keys, &current_stream,
    AES_STREAM_START | AES_USE_CFB128 );

    //Copy decrypted text to already allocated WholeMsg dynamic memory Buffer.
    memcpy(plainText,encrypted_text,plaintextLen);

    return SNMPV3_MSG_PRIV_PASS;
}

/****************************************************************************
  Function:
    uint8_t SNMPv3DESDecryptRxedScopedPdu(void)
	
  Summary:
    Incoming SNMPv3 scoped PDU decryption using DES decryption protocol.
	
  Description:
    This routine decrypts SNMPV3 incoming PDU using DES protocol , but before this
    encrypted data length is verified.If the length of the encrypted OCTECT-STRING
    is not multiple of 8, then dryption will be halted.
    RFC - 3414. ( section 8)
  	 		 		  	
  Precondition:
    SNMPv3Init() and ProcessVariabels() are called.

  Parameters:
    None
  	  	
  Return Values:
    SNMPV3_MSG_PRIV_FAIL - Failure
    SNMPV3_MSG_PRIV_PASS - Success

  Remarks:
    None
***************************************************************************/
uint8_t SNMPv3DESDecryptRxedScopedPdu(void)
{
#ifdef TCPIP_SNMPV3_SUPPORT_DES
    uint8_t cryptoKey[8];
    uint8_t* privLclzdKeyPtr;
    uint8_t preIV[TCPIP_SNMPV3_DES_CRYPTO_KEY_LEN];
    uint8_t initVector[TCPIP_SNMPV3_DES_CRYPTO_KEY_LEN];
    uint8_t* snmpv3_cipher_text;
    uint16_t cipherTextLen;
    uint8_t decrypted_text[TCPIP_SNMP_MAX_MSG_SIZE];
    uint16_t temp;
    Des dec;
	

    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);
    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;


    cipherTextLen= snmpv3EngnDcptMemoryStubPtr->InPduWholeMsgBuf.scopedPduStructLen;

    if(! cipherTextLen % TCPIP_SNMPV3_DES_CRYPTO_BLOCK_SIZE)  //DES Blocks are in multiples of 8 Bytes
    {
        return SNMPV3_MSG_PRIV_FAIL;
    }

    /*DES Key is the first 8 octets of the localized key (private privacy key) created from privacy password. Refer RFC 3414, Page#64, Section 8.1.1.1 */

    privLclzdKeyPtr=snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx].userPrivPswdLoclizdKey;

    memcpy(cryptoKey, privLclzdKeyPtr,TCPIP_SNMPV3_DES_CRYPTO_KEY_LEN);

    /*DES IV (Initialization Vector) Calculation*/

    memcpy(preIV, privLclzdKeyPtr+8 /* preIV = The last 8 octets of the 16 octet private privacy key */,TCPIP_SNMPV3_DES_CRYPTO_KEY_LEN);

    SNMPv3UsmDesEncryptDecrptInitVector(SNMP_REQUEST_PDU, preIV, initVector);

    /*Setup the pointers to Decrypt incoming data */

    temp=snmpv3EngnDcptMemoryStubPtr->InPduWholeMsgBuf.scopedPduOffset;
    snmpv3_cipher_text = gSnmpDcpt.udpGetBufferData.head+temp;
    memset(decrypted_text,0,sizeof(decrypted_text));
    

    /*DES Decrypt */
    wc_Des_SetKey(&dec, cryptoKey, initVector, DES_DECRYPTION);
    wc_Des_CbcDecrypt(&dec, decrypted_text, snmpv3_cipher_text, cipherTextLen);

    	
    //Copy decrypted text to already allocated WholeMsg dynamic memory Buffer.
    memcpy(snmpv3_cipher_text,decrypted_text,cipherTextLen);
#endif
    return SNMPV3_MSG_PRIV_PASS;
}


/****************************************************************************
  Function:
    uint8_t SNMPv3DESEncryptResponseScopedPdu(SNMPV3_RESPONSE_WHOLEMSG* plain_text)
	
  Summary:
    outGoing SNMPv3 scoped PDU Encryption using DES encryption protocol.
	
  Description:
    This routine encrypts SNMPV3 outgoing PDU using DES protocol to maintain the data
    confidentiality. The data is encrypted in Cipher Block Chaining mode. The length of the
    encrypted data should be multiple of 8 and it is not then then data is padded in the end 
    if necessary.   RFC - 3414. ( section 8)
  	 		 		  	
  Precondition:
    SNMPv3Init() and ProcessVariabels() are called.

  Parameters:
    plain_text -  whole PDU message
  	  	
  Return Values:
    SNMPV3_MSG_PRIV_FAIL - Failure
    SNMPV3_MSG_PRIV_PASS - Success

  Remarks:
    None
***************************************************************************/
uint8_t SNMPv3DESEncryptResponseScopedPdu(SNMPV3_RESPONSE_WHOLEMSG* plain_text)
{	
#ifdef TCPIP_SNMPV3_SUPPORT_DES    
    uint8_t* plainText;
    uint16_t plaintextLen;
    uint8_t  encrypted_text[TCPIP_SNMP_MAX_MSG_SIZE];
    uint8_t extraMemReqd;
    uint8_t cryptoKey[8];
    uint8_t* privLclzdKeyPtr;
    uint8_t preIV[TCPIP_SNMPV3_DES_CRYPTO_KEY_LEN];
    uint8_t initVector[TCPIP_SNMPV3_DES_CRYPTO_KEY_LEN];
    Des enc;

    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;


    plaintextLen= (plain_text->scopedPduStructLen);
/*DES Key is the first 8 octets of the localized key (private privacy key) created from privacy password. Refer RFC 3414, Page#64, Section 8.1.1.1 */

    privLclzdKeyPtr=snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx].userPrivPswdLoclizdKey;

    memcpy(cryptoKey, privLclzdKeyPtr,TCPIP_SNMPV3_DES_CRYPTO_KEY_LEN);

    /*DES IV (Initialization Vector) Calculation*/

    memcpy(preIV, privLclzdKeyPtr+8 /* preIV = The last 8 octets of the 16 octet private privacy key */,TCPIP_SNMPV3_DES_CRYPTO_KEY_LEN);

    SNMPv3UsmDesEncryptDecrptInitVector(SNMP_RESPONSE_PDU, preIV, initVector);

    plainText=(plain_text->scopedPduOffset);

    extraMemReqd=(TCPIP_SNMPV3_DES_CRYPTO_BLOCK_SIZE-(plaintextLen%TCPIP_SNMPV3_DES_CRYPTO_BLOCK_SIZE)); //DES Blocks are in multiples of 8bytes
    memset(encrypted_text,0,sizeof(encrypted_text));
    plain_text->scopedPduStructLen += extraMemReqd;
    plaintextLen= (plain_text->scopedPduStructLen);

    /*DES Encrypt */
    wc_Des_SetKey(&enc, cryptoKey, initVector, DES_ENCRYPTION);
    wc_Des_CbcEncrypt(&enc, encrypted_text, plainText, plaintextLen);

     //Copy encrypted text to already allocated WholeMsg dynamic memory Buffer.
    memcpy(plainText,encrypted_text,plaintextLen);
#endif
    return SNMPV3_MSG_PRIV_PASS;
}

/****************************************************************************
  Function:
    BOOL SNMPv3ValidateSnmpEngnId(void)
	
  Summary:
    Validate engine ID.
	
  Description:
    This routine validates Engine ID.
  	 		 		  	
  Precondition:
    SNMPv3Init() and ProcessVariabels() are called.

  Parameters:
    None
  	  	
  Return Values:
    false - Failure
    true  - Success

  Remarks:
    None
***************************************************************************/
bool SNMPv3ValidateSnmpEngnId(void)
{
    uint8_t* inEngnIdPtr=NULL;
    uint8_t temp;
    uint8_t reportMsgName[7]="initial";//respose is "report" 0xa8 msg
    uint8_t* secNamePtr=NULL;
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;

    secNamePtr= snmpv3EngnDcptMemoryStubPtr->SecurtyPrimtvesOfIncmngPdu.securityName;

    if(snmpv3EngnDcptMemoryStubPtr->SecurtyPrimtvesOfIncmngPdu.securityNameLength == 0)
                    return true; //If "report" is expected, Retrun.

    //Check if the received packet is expecting "report" as response.
    if(!strncmp((const char *)secNamePtr,
                            (const char *)reportMsgName,
                            (snmpv3EngnDcptMemoryStubPtr->SecurtyPrimtvesOfIncmngPdu.securityNameLength)))
    {
        return true; //If "report" is expected, Return.
    }
    else
    {
        inEngnIdPtr=snmpv3EngnDcptMemoryStubPtr->SecurtyPrimtvesOfIncmngPdu.securityEngineID;

        temp=strncmp((const char *)inEngnIdPtr,
                                (const char *)snmpv3EngnDcptMemoryStubPtr->SnmpEngineID,
                                (snmpv3EngnDcptMemoryStubPtr->SecurtyPrimtvesOfIncmngPdu.securityEngineIDLen));
        if(temp!=0)
        {
            return false; //If "report" is expected, Retrun.
        }
        else
        {
            return true;
        }
    }
}

/****************************************************************************
  Function:
	BOOL SNMPv3ValidateUserSecurityName(void)
	
  Summary:
  	Validate SNMPV3 user name or security name.
	
  Description:
  	This routine validates user name.
  	 		 		  	
  Precondition:
   	SNMPv3Init() and ProcessVariabels() are called.	

  Parameters:
  	None
  	  	
  Return Values:
	false - Failure
	true - Success

  Remarks:
	None 
***************************************************************************/

bool SNMPv3ValidateUserSecurityName(void)
{
    uint8_t* inSecNamePtr;
    uint8_t tempLen,i,temp;
    uint8_t reportMsgName[7]="initial";//respose is "report" 0xa8 msg
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;

    tempLen=snmpv3EngnDcptMemoryStubPtr->SecurtyPrimtvesOfIncmngPdu.securityNameLength;
    inSecNamePtr=snmpv3EngnDcptMemoryStubPtr->SecurtyPrimtvesOfIncmngPdu.securityName;

    //Check if the received packet is expecting "report" as response.
    if(!strncmp((const char *)inSecNamePtr,
                                    (const char *)reportMsgName,
                                    tempLen))
    {
        snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx=0;
        return true; //If "report" is expected, Retrun.
    }
    else
    {
        for(i=0;i<TCPIP_SNMPV3_USM_MAX_USER;i++)
        {
            temp=strncmp((const char *)snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[i].userName,
                                      (const char *)inSecNamePtr,tempLen);

            if(temp==0)
            {
                snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx=i;
                return true;
            }
        }
        snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx=0;
    }
    return false;
}

/****************************************************************************
  Function:
    uint8_t SNMPv3GetSecurityLevel(uint8_t userIndex)
	
  Summary:
    Get Security level from authentication and Privacy type.
	
  Description:
    This routine uses authenticationa dn privacy type to find out the exact
    security enum type.
  	 		 		  	
  Precondition:
    SNMPv3Init() and ProcessVariabels() are called.

  Parameters:
    None
  	  	
  Return Values:
    NO_REPORT_PRIVACY_AND_AUTH_PROVIDED - Only Privacy and Authentication
    NO_REPORT_NO_PRIVACY_BUT_AUTH_PROVIDED -  Only Auth , no  privacy and no report
    NO_REPORT_NO_PRIVACY_NO_AUTH - No report, no Privacy and no Authentication
	
  Remarks:
    None
***************************************************************************/

uint8_t SNMPv3GetSecurityLevel(uint8_t userIndex)
{
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;


    if(((snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userHashType == SNMPV3_HMAC_MD5) ||
            (snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userHashType == SNMPV3_HMAC_SHA1))
    && ((snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userPrivType == SNMPV3_AES_PRIV) ||
    (snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userPrivType == SNMPV3_DES_PRIV)))
    {
            return NO_REPORT_PRIVACY_AND_AUTH_PROVIDED;
    }
    else if(((snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userHashType == SNMPV3_HMAC_MD5) ||
            (snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userHashType == SNMPV3_HMAC_SHA1))
            && (snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[userIndex].userPrivType == SNMPV3_NO_PRIV))
    {
            return NO_REPORT_NO_PRIVACY_BUT_AUTH_PROVIDED;
    }
    else
    {
            return NO_REPORT_NO_PRIVACY_NO_AUTH;
    }

}

/****************************************************************************
  Function:
    BOOL SNMPv3ValidateSecNameAndSecLevel(void)
	
  Summary:
    Vaidate security name with Security level.
	
  Description:
    This routine validates secuirity name and secuirty level with SNMP global data base
    for an incoming PDU.
  	 		 		  	
  Precondition:
    SNMPv3Init() and ProcessVariabels() are called.

  Parameters:
    None
  	  	
  Return Values:
    true  - success
    false - failure
  Remarks:
    None
***************************************************************************/
bool SNMPv3ValidateSecNameAndSecLevel(void)
{
    uint8_t* inSecNamePtr=NULL;
    uint8_t reportMsgName[7]="initial";//respose is "report" 0xa8 msg
    uint8_t  tempLen=0,i=0,temp=0;
    uint8_t  inSecurityLevel=0;
    SNMPV3_PROCESSING_MEM_INFO_PTRS snmpv3PktProcessingMemPntr;
    SNMPV3_STACK_DCPT_STUB * snmpv3EngnDcptMemoryStubPtr=0;

    TCPIP_SNMPV3_PacketProcStubPtrsGet(&snmpv3PktProcessingMemPntr);

    snmpv3EngnDcptMemoryStubPtr=snmpv3PktProcessingMemPntr.snmpv3StkProcessingDynMemStubPtr;
	
    tempLen=snmpv3EngnDcptMemoryStubPtr->SecurtyPrimtvesOfIncmngPdu.securityNameLength;
    if(tempLen == 0x0u) // report is expected
    {
        snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx=0;
        return true;
    }

    inSecNamePtr=snmpv3EngnDcptMemoryStubPtr->SecurtyPrimtvesOfIncmngPdu.securityName;
    inSecurityLevel = snmpv3EngnDcptMemoryStubPtr->SecurtyPrimtvesOfIncmngPdu.securityLevel;


    if(!strncmp((const char *)inSecNamePtr,
                                    (const char *)reportMsgName,
                                    tempLen))
    {
        snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx=0;
        return true; //If "report" is expected, Retrun.
    }
    else
    {
        for(i=0;i<TCPIP_SNMPV3_USM_MAX_USER;i++)
        {
            if(tempLen != snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[i].userNameLength)
                    continue;
            temp=strncmp((const char *)snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[i].userName,
                                      (const char *)inSecNamePtr,snmpv3EngnDcptMemoryStubPtr->UserInfoDataBase[i].userNameLength);

            if((temp==0) && (SNMPv3GetSecurityLevel(i) == (inSecurityLevel&0x03)))
            {
                snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx=i;
                return true;
            }
        }
        snmpv3EngnDcptMemoryStubPtr->UserInfoDataBaseIndx=0;
    }
    return false;
}

/****************************************************************************
  Function:
    static void SNMPv3ComputeMd5HmacCode(UINT8 xx_bits,UINT8* digestptr,
						  UINT8 * indata, UINT32 dataLen,
						  UINT8* userExtendedLclzdKeyIpad,
						  UINT8* userExtendedLclzdKeyOpad)
	
  Summary:
    Compute HMAC - MD5 authentication code
	
  Description:
    This routine supports data origin authentication and data integrity MD5 authentication .
    Both iPAD and OPAD is used to calculate the authencate digest string.
    RFC - 3414 ( section 6)
  	 		 		  	
  Precondition:
    SNMPv3Init() and ProcessVariabels() are called.

  Parameters:
    xx_bits - 96 bit
    digestptr - output string
    indata   - input data
    dataLen - input data length
    userExtendedLclzdKeyIpad - IPAD
    userExtendedLclzdKeyOpad - OPAD
  	  	
  Return Values:
    None
  Remarks:
    None
***************************************************************************/
static void _SNMPv3_ComputeMd5HmacCode(uint8_t xx_bits,uint8_t* digestptr,
					  uint8_t * indata, uint32_t dataLen,
					  uint8_t* userExtendedLclzdKeyIpad,
					  uint8_t* userExtendedLclzdKeyOpad)
{
    uint8_t* hmacMd5DigestPtr;
    uint8_t i;
    uint8_t* dataPtr;
    dataPtr=indata;

	
    hmacMd5DigestPtr=_SNMPv3_ComputeHmacMD5Digest(dataPtr, dataLen,userExtendedLclzdKeyOpad,userExtendedLclzdKeyOpad);
	
    for(i=0;i<(xx_bits/8);i++)
    {
        digestptr[i]=*(hmacMd5DigestPtr+i);
    }
}

/****************************************************************************
  Function:
    static void SNMPv3ComputeShaHmacCode(UINT8 xx_bits,UINT8* digestptr,
						  UINT8 * indata, UINT32 dataLen,
						  UINT8* userExtendedLclzdKeyIpad,
						  UINT8* userExtendedLclzdKeyOpad)
	
  Summary:
    Compute HMAC - SHA authentication code
	
  Description:
    This routine supports data origin authentication and data integrity SHA authentication .
    Both iPAD and OPAD is used to calculate the authencate digest string.
    RFC - 3414 ( section 6)
  	 		 		  	
  Precondition:
    SNMPv3Init() and ProcessVariabels() are called.

  Parameters:
    xx_bits - 96 bit
    digestptr - output string
    indata   - input data
    dataLen - input data length
    userExtendedLclzdKeyIpad - IPAD
    userExtendedLclzdKeyOpad - OPAD
  	  	
  Return Values:
    None
  Remarks:
    None
***************************************************************************/

static void _SNMPv3_ComputeShaHmacCode(uint8_t xx_bits,uint8_t* digestptr,
					uint8_t * indata, uint32_t dataLen,
					uint8_t* userExtendedLclzdKeyIpad,
					uint8_t* userExtendedLclzdKeyOpad)
{
    uint8_t* hmacSHADigestPtr;
    uint8_t i;
    uint8_t* dataptr;
    dataptr=indata;


    hmacSHADigestPtr=_SNMPv3_ComputeHmacShaDigest(dataptr, dataLen,userExtendedLclzdKeyOpad,userExtendedLclzdKeyOpad);

    for(i=0;i<(xx_bits/8);i++)
    {
        digestptr[i]=*(hmacSHADigestPtr+i);
    }
}

/****************************************************************************
  Function:
    static void SNMPv3AuthKeyZeroingToHmacBufLen64(UINT8* authKey, UINT8 authKeyLen,  UINT8 hashType)
	
  Summary:
    Pad zero to the authentication key localized buffer.
	
  Description:
    this routine will pad the (64-authKeyLen) number of zeros to the end of auth key localized buffer.
  	 		 		  	
  Precondition:
    SNMPv3Init() and ProcessVariabels() are called.

  Parameters:
    authkey - authentication key buffer
    authKeylen  - authentication key length
    hashType - authentication type
  	  	
  Return Values:
    None
  Remarks:
    None
***************************************************************************/
static void _SNMPv3_AuthKeyZeroingToHmacBufLen64(uint8_t* authKey, uint8_t authKeyLen,  uint8_t hashType)
{
    uint8_t* tempAuthKeyptr;
    uint8_t i;
	
    tempAuthKeyptr = authKey;

    if(authKeyLen > 64)
    {
        if(hashType == SNMPV3_HMAC_MD5)
        {
            //Hash MD5 AuthKey;
            //Zero pad the Auth key;
        }
        else if(hashType == SNMPV3_HMAC_SHA1)
        {
            //Hash SHA AuthKey;
            //Zero pad the Auth key;
        }
    }
    else
    {
        //ZeroPad Auth Key
        memcpy((void*) &hmacAuthKeyBuf, (const void *)tempAuthKeyptr, authKeyLen);
        for(i=authKeyLen;i<64;i++)
        {
            hmacAuthKeyBuf[i]=0x00;
        }
    }
}

/****************************************************************************
  Function:
    static unit_8* SNMPv3ComputeHmacMD5Digest(UINT8 * inData, UINT32 dataLen,
                                             UINT8* userExtendedLclzdKeyIpad,
                                             UINT8* userExtendedLclzdKeyOpad)
  Summary:
    Compute HMAC - MD5 authentication code
	
  Description:
    This routine supports data origin authentication and data integrity MD5 authentication .
    Both iPAD and OPAD is used to calculate the authencate digest string.
    RFC - 3414 ( section 6)
  	 		 		  	
  Precondition:
    SNMPv3Init() and ProcessVariabels() are called.

  Parameters:
    digestptr - output string
    indata   - input data
    dataLen - input data length
    userExtendedLclzdKeyIpad - IPAD
    userExtendedLclzdKeyOpad - OPAD
  	  	
  Return Values:
    UINT8 *  - HMAC MD5 digest string
  Remarks:
    None
***************************************************************************/
static uint8_t* _SNMPv3_ComputeHmacMD5Digest(uint8_t * inData,
                                             uint32_t dataLen,
                                             uint8_t* userExtendedLclzdKeyIpad,
                                             uint8_t* userExtendedLclzdKeyOpad)
{
    static CRYPT_MD5_CTX md5;
    uint8_t * data2Hmac;

    data2Hmac=inData;

    CRYPT_MD5_Initialize(&md5);
    CRYPT_MD5_DataAdd(&md5,userExtendedLclzdKeyIpad, (uint16_t)0x40);
    CRYPT_MD5_DataAdd(&md5, data2Hmac, (uint16_t)dataLen);
    CRYPT_MD5_Finalize(&md5, HmacMd5Digest);

	
    CRYPT_MD5_Initialize(&md5);
    CRYPT_MD5_DataAdd(&md5,userExtendedLclzdKeyOpad, (uint16_t)0x40);
    CRYPT_MD5_DataAdd(&md5,HmacMd5Digest,16);
    CRYPT_MD5_Finalize(&md5,HmacMd5Digest);

    return HmacMd5Digest;
}

/****************************************************************************
  Function:
    UINT8* SNMPv3ComputeHmacShaDigest(UINT8 * inData, UINT32 dataLen,
                                     UINT8* userExtendedLclzdKeyIpad,
                                     UINT8* userExtendedLclzdKeyOpad)
	
  Summary:
    Compute HMAC - SHA authentication code
	
  Description:
    This routine supports data origin authentication and data integrity SHA authentication .
    Both iPAD and OPAD is used to calculate the authencate digest string.
    RFC - 3414 ( section 6)
  	 		 		  	
  Precondition:
    SNMPv3Init() and ProcessVariabels() are called.

  Parameters:
    digestptr - output string
    indata   - input data
    dataLen - input data length
    userExtendedLclzdKeyIpad - IPAD
    userExtendedLclzdKeyOpad - OPAD
  	  	
  Return Values:
    UINT8 *  - HMAC SHA digest string
  Remarks:
    None
***************************************************************************/
static uint8_t* _SNMPv3_ComputeHmacShaDigest(uint8_t * inData,
                                             uint32_t dataLen,
                                             uint8_t* userExtendedLclzdKeyIpad,
                                             uint8_t* userExtendedLclzdKeyOpad)
{	
    static CRYPT_SHA_CTX  sha1;
    uint8_t * data2Hmac;

    data2Hmac=inData;

	CRYPT_SHA_Initialize(&sha1);
    CRYPT_SHA_DataAdd(&sha1,authKey_iPad, (uint16_t)0x40);
    CRYPT_SHA_DataAdd(&sha1,data2Hmac, (uint16_t)dataLen);
    CRYPT_SHA_Finalize(&sha1, HmacSHADigest);    
   
    CRYPT_SHA_Initialize(&sha1);
    CRYPT_SHA_DataAdd(&sha1,authKey_oPad, (uint16_t)0x40);
    CRYPT_SHA_DataAdd(&sha1,HmacSHADigest,20);
    CRYPT_SHA_Finalize(&sha1,HmacSHADigest);

    return HmacSHADigest;
	
}

#endif // #if defined(TCPIP_STACK_USE_SNMPV3_SERVER)
