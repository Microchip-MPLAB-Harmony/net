/*****************************************************************************

 Advanced Encryption Standard (AES)

****************************************************************************
 FileName:      AES_CFB.c
 Dependencies:  AES.h
 Processor:		PIC24F, PIC24H, dsPIC30F, or dsPIC33F
 Compiler:		MPLAB C30 2.02 or later
 Linker:			MPLAB LINK30 2.02 or later
 Company:		Microchip Technology Incorporated

 Software License Agreement

 The software supplied herewith by Microchip Technology Incorporated
 (the �Company�) for its PICmicro� Microcontroller is intended and
 supplied to you, the Company�s customer, for use solely and
 exclusively on Microchip PICmicro Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 Microchip Technology Inc. (�Microchip�) licenses this software to 
 you solely for use with Microchip products.  The software is owned 
 by Microchip and is protected under applicable copyright laws.  
 All rights reserved.

 You may not export or re-export Software, technical data, direct 
 products thereof or any other items which would violate any applicable
 export control laws and regulations including, but not limited to, 
 those of the United States or United Kingdom.  You agree that it is
 your responsibility to obtain copies of and to familiarize yourself
 fully with these laws and regulations to avoid violation.

 SOFTWARE IS PROVIDED �AS IS.�  MICROCHIP EXPRESSLY DISCLAIM ANY 
 WARRANTY OF ANY KIND, WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT 
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE, OR NON-INFRINGEMENT. IN NO EVENT SHALL MICROCHIP
 BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES,
 LOST PROFITS OR LOST DATA, HARM TO YOUR EQUIPMENT, COST OF PROCUREMENT
 OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS BY THIRD PARTIES
 (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR 
 INDEMNITY OR CONTRIBUTION, OR OTHER SIMILAR COSTS. 

 Rev    Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 v1.0   Original release - released with AN1044

 v2.0   Updated to support 192-bit and 256-bit modes.  API change to be
        compatible with PIC32 implementation.  This increases RAM usage but
        decreases the decode time.
****************************************************************************/

#include "crypto.h"
//#include <AES.h>

#if defined(__C30__)
    /* The include for aes_pic24f.h must be after the prototypes for the AES
       functions since they will macro wrap them into other functions */
    #include <src/pic24f/local_aes_pic24f.h>
#endif

#if defined(__PIC32MX__)
    #include <src/pic32/aes_ecb_pic32.h>
#endif

// ****************************************************************************
// ****************************************************************************
// Section: Constants & Data Types
// ****************************************************************************
// ****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: AES Interface Routines
// *****************************************************************************
// *****************************************************************************


/*******************************************************************************
  Function:  void AESCFBEncrypt(	UINT8 * cipher_text,
                                    UINT8 * plain_text, 
									UINT32 num, 
                                    void* round_keys,
									AES_CFB_STATE_DATA *p_cfb_state_data,
                                    UINT32 options
								);

  Summary:
    Encrypts a specified amount of data using cipher feedback (CFB) mode.

  Description:
    Encrypts a specified amount of data using cipher feedback (CFB) mode.

  Precondition:
    * p_cfb_state_data->intial_vector must be populated with the initialization vector before the first call of this function on a new data stream.
    * p_cfb_state_data->round_keys must be initialized to point to the round_keys before the first call of this function on a new data stream.
    * round_keys must be properly generated using AESCreateRoundKeys().
  Parameters:
    cipher_text      - [out] pointer to the buffer where the encrypted data sould reside.  It must also be the same length as the plain_text buffer.
    plain_text       - [in]  pointer to the data that needs to be encrypted.
    num              - [in]  the number of bits (CFB1) or bytes (CFB8/128) in the plain_text and cipher_text buffers
    round_keys       - [in]  pointer to the round key buffer that will be used
                       for this encryption cycle.
    p_cfb_state_data - [in]  pointer to an instance of the AES_CFB_STATE_DATA for this stream.  The first call of this function should have the p_cfb_state_data->initial_vector set to the initialization vector.
    options          - [in]  Set of options that are ORed together (e.g. - AES_CIPHER_TEXT_POINTER_ALIGNED | AES_STREAM_START).  Valid options are\: 
                            * AES_STREAM_START
                            * AES_STREAM_COMPLETE
                            * AES_STREAM_CONTINUE
                            * AES_CIPHER_TEXT_POINTER_ALIGNED
                            * AES_PLAIN_TEXT_POINTER_ALIGNED
                            * AES_USE_CFB1
                            * AES_USE_CFB8
                            * AES_USB_CFB128
  Returns:
    None

  Example:
    <code>
    //Example variables.  The key and initialization vector information
    //  should be customized for the target application.

    //This example comes from Appendix F.3.1 of the 
    //  "NIST Special Publication 800-38A: Recommendation for Block Cipher Modes 
    //  of Operation: Methods and Techniques" (sp800-38a.pdf)
    static unsigned char AESKey128[]      =       { 0x2b, 0x7e, 0x15, 0x16, 
                                                    0x28, 0xae, 0xd2, 0xa6, 
                                                    0xab, 0xf7, 0x15, 0x88, 
                                                    0x09, 0xcf, 0x4f, 0x3c
                                                  };

    static unsigned char initialization_vector[] = 
                                                  { 0x00, 0x01, 0x02, 0x03, 
                                                    0x04, 0x05, 0x06, 0x07, 
                                                    0x08, 0x09, 0x0a, 0x0b, 
                                                    0x0c, 0x0d, 0x0e, 0x0f
                                                  };

    static unsigned char plain_text[]     =       { 0b11010110, 0b10000011
                                                  };

    AES_ROUND_KEYS_128_BIT round_keys;
    AES_CFB_STATE_DATA current_stream;

    //We need a buffer to contain the resulting data.  
    //  This buffer can be created statically or dynamically and can be
    //  of any size as long as it is larger than or equal to AES_BLOCK_SIZE
    unsigned char cipher_text[sizeof(plain_text)];
    
    //Create the round keys.  This only needs to be done once for each key.
    //  This example is here for completeness.
    AESCreateRoundKeys (    &round_keys,
                            (unsigned char*)AESKey128,
                            AES_KEY_SIZE_128_BIT
                       );
    
    //Before the first call of AESCBCEncrypt() for a stream of data, the
    //  p_cbc_state_data->initial_vector must be loaded with the initialization
    //  vector information for that specific stream of data.
    memcpy(current_stream.initial_vector,initialization_vector,16);
    
    //Encrypt the data
    AESCFBEncrypt(  cipher_text,
                    (void*)plain_text,
                    sizeof(plain_text) * 8,     //num
                    &round_keys,
                    &current_stream,        //stream state data
                    AES_STREAM_START | AES_USE_CFB1        //options
                 );

    //Now let's check the results
    {
        static unsigned char expected_result[] = 
                                                      { 0b00010110, 0b11001101                        
                                                      };

        unsigned int i;

        for(i=0;i<sizeof(plain_text);i++)
        {
            if(cipher_text[i] != expected_result[i])
            {
                //Error: results don't match what was expected
                Nop();
                while(1){};
            }
        }

        Nop();
    }
    </code>

 *****************************************************************************/
void AESCFBEncrypt(uint8_t * cipher_text, uint8_t * plain_text, uint32_t num, void * round_keys, AES_CFB_STATE_DATA *p_cfb_state_data, uint32_t options)
{
    if((options & AES_USE_CFB1) == AES_USE_CFB1)
    {
        uint8_t i, plain_text_temp = 0;
        uint8_t __attribute__((aligned)) buffer[AES_BLOCK_SIZE];
        uint16_t j;

        j=0;

        while(num--)
        {
            if(j++==0)
            {
                plain_text_temp = *plain_text++;
                *cipher_text = 0;
            }

            AESEncryptBlock(p_cfb_state_data->initial_vector,round_keys,buffer);

            *cipher_text >>= 1;
    
            if((plain_text_temp & 0x01) != 0)
            {
                if((buffer[0] & 0x80) == 0)
                {
                    *cipher_text |= 0x80;
                }
            }
            else
            {
                if((buffer[0] & 0x80) != 0)
                {
                    *cipher_text |= 0x80;
                }
            }

            plain_text_temp >>= 1;

            for(i=0;i<(AES_BLOCK_SIZE - 1);i++)
            {
                p_cfb_state_data->initial_vector[i] <<= 1;
                if((p_cfb_state_data->initial_vector[i+1] & 0x80) == 0x80)
                {
                    p_cfb_state_data->initial_vector[i] |= 0x01;
                }
            }

            p_cfb_state_data->initial_vector[AES_BLOCK_SIZE - 1] <<=1;

            if((*cipher_text & (0x80)) == 0x80)
            {
                p_cfb_state_data->initial_vector[AES_BLOCK_SIZE - 1] |= 0x01;
            }

            if(j==8)
            {
                cipher_text++;
                j = 0;
            }
        }
        while(j++ != 8)
        {
            *cipher_text >>= 1;
        }
    }
    else if ((options & AES_USE_CFB8) == AES_USE_CFB8)
    {
        uint8_t i;
        uint8_t __attribute__((aligned)) buffer[AES_BLOCK_SIZE];

        while(num--)
        {
            AESEncryptBlock(p_cfb_state_data->initial_vector,round_keys,buffer);
    
            *cipher_text = *plain_text++ ^ buffer[0];

            for(i=0;i<(AES_BLOCK_SIZE - 1);i++)
            {
                p_cfb_state_data->initial_vector[i] = p_cfb_state_data->initial_vector[i+1];
            }

            p_cfb_state_data->initial_vector[AES_BLOCK_SIZE-1] = *cipher_text++;
        }
    }
    else
    {
        if((options & AES_STREAM_START) == AES_STREAM_START)
        {
            p_cfb_state_data->bytes_in_buffer = AES_BLOCK_SIZE;
        }
    
        while(num--)
        {
            if(p_cfb_state_data->bytes_in_buffer == AES_BLOCK_SIZE)
            {
                AESEncryptBlock(p_cfb_state_data->initial_vector,round_keys,p_cfb_state_data->initial_vector);
                p_cfb_state_data->bytes_in_buffer = 0;
            }
    
            *cipher_text = *plain_text++ ^ p_cfb_state_data->initial_vector[p_cfb_state_data->bytes_in_buffer];
            p_cfb_state_data->initial_vector[p_cfb_state_data->bytes_in_buffer++] = *cipher_text++;
        }
    }
}

/*******************************************************************************
  Function:  void AESCFBDecrypt(	UINT8 * plain_text,
									UINT8 * cipher_text, 
									UINT32 num, 
                                    void * round_keys,
									AES_CFB_STATE_DATA *p_cfb_state_data,
                                    UINT32 options
								);

  Summary:
    Decrypts a specified amount of data using cipher feedback (CFB) mode.

  Description:
    Decrypts a specified amount of data using cipher feedback (CFB) mode.

  Precondition:
    Round keys must be create by AESCreateRoundKeys before calling this routine.
    p_cfb_state_data->intial_vector must be populated with the initialization vector before the first call of this function on a new data stream.

  Parameters:
    plain_text       - [out] pointer to the buffer where the decrypted data sould reside.  This buffer must be a muliple of 16 bytes and must be equal to or larger than the cipher_text buffer.
    cipher_text      - [in]  pointer to the data that needs to be decrypted.
    num              - [in]  the number of bits (CFB1) or bytes (CFB8 or CFB128) in the buffer 
    round_keys       - [in]  pointer to the round key buffer that will be used
                             for this encryption cycle.
    p_cfb_state_data - [in]  pointer to an instance of the AES_CFB_STATE_DATA for this stream.  The first call of this function should have the p_cfb_state_data->initial_vector set to the initialization vector.
    options          - [in]  Set of options that are ORed together (e.g. - AES_PAD_NONE | AES_STREAM_START).  Valid options are\: 
                            * AES_STREAM_START
                            * AES_STREAM_COMPLETE
                            * AES_STREAM_CONTINUE
                            * AES_CIPHER_TEXT_POINTER_ALIGNED
                            * AES_PLAIN_TEXT_POINTER_ALIGNED
  Returns:
    None

  Example:
    <code>
    //Example variables.  The key and initialization vector information
    //  should be customized for the target application.

    //This example comes from Appendix F.3.2 of the 
    //  "NIST Special Publication 800-38A: Recommendation for Block Cipher Modes 
    //  of Operation: Methods and Techniques" (sp800-38a.pdf)
    static unsigned char AESKey128[]      =       { 0x2b, 0x7e, 0x15, 0x16, 
                                                    0x28, 0xae, 0xd2, 0xa6, 
                                                    0xab, 0xf7, 0x15, 0x88, 
                                                    0x09, 0xcf, 0x4f, 0x3c
                                                  };

    static unsigned char initialization_vector[] = 
                                                  { 0x00, 0x01, 0x02, 0x03, 
                                                    0x04, 0x05, 0x06, 0x07, 
                                                    0x08, 0x09, 0x0a, 0x0b, 
                                                    0x0c, 0x0d, 0x0e, 0x0f
                                                  };

    static unsigned char cipher_text[]    =       { 0b00010110, 0b11001101
                                                  };

    AES_ROUND_KEYS_128_BIT round_keys;
    AES_CFB_STATE_DATA current_stream;

    //We need a buffer to contain the resulting data.  
    //  This buffer can be created statically or dynamically and can be
    //  of any size as long as it is larger than or equal to AES_BLOCK_SIZE
    unsigned char plain_text[sizeof(cipher_text)];
    
    //Create the round keys.  This only needs to be done once for each key.
    //  This example is here for completeness.
    AESCreateRoundKeys (    &round_keys,
                            (unsigned char*)AESKey128,
                            AES_KEY_SIZE_128_BIT
                       );
    
    //Before the first call of AESCBCDecrypt() for a stream of data, the
    //  p_cbc_state_data->initial_vector must be loaded with the initialization
    //  vector information for that specific stream of data.
    memcpy(current_stream.initial_vector,initialization_vector,16);
    
    //Decrypt the data
    AESCFBDecrypt(  plain_text,
                    cipher_text,
                    sizeof(cipher_text) * 8,    //num
                    &round_keys,
                    &current_stream,        //stream state data
                    AES_STREAM_START | AES_USE_CFB1        //options
                 );

    //Now let's check the results
    {
        static unsigned char expected_result[] = 
                                                      { 0b11010110, 0b10000011
                                                      };

        unsigned int i;

        for(i=0;i<sizeof(cipher_text);i++)
        {
            if(plain_text[i] != expected_result[i])
            {
                //Error: results don't match what was expected
                Nop();
                while(1){};
            }
        }

        Nop();
    }
    </code>

 *****************************************************************************/
void AESCFBDecrypt(uint8_t * plain_text, uint8_t * cipher_text, uint32_t num, void * round_keys, AES_CFB_STATE_DATA *p_cfb_state_data, uint32_t options)
{
    if((options & AES_USE_CFB1) == AES_USE_CFB1)
    {
        uint8_t i,cipher_text_temp = 0;
        uint8_t __attribute__((aligned)) buffer[AES_BLOCK_SIZE];

        uint16_t j;

        j=0;

        while(num--)
        {
            if(j++==0)
            {
                *plain_text = 0;
                cipher_text_temp = *cipher_text++;
            }

            AESEncryptBlock(p_cfb_state_data->initial_vector,round_keys,buffer);

            *plain_text >>= 1;
    
            if((cipher_text_temp & 0x01) != 0)
            {
                if((buffer[0] & 0x80) == 0)
                {
                    *plain_text |= 0x80;
                }
            }
            else
            {
                if((buffer[0] & 0x80) != 0)
                {
                    *plain_text |= 0x80;
                }
            }

            for(i=0;i<(AES_BLOCK_SIZE - 1);i++)
            {
                p_cfb_state_data->initial_vector[i] <<= 1;
                if((p_cfb_state_data->initial_vector[i+1] & 0x80) == 0x80)
                {
                    p_cfb_state_data->initial_vector[i] |= 0x01;
                }
            }

            p_cfb_state_data->initial_vector[AES_BLOCK_SIZE-1] <<=1;
            if((cipher_text_temp & (0x01)) == 0x01)
            {
                p_cfb_state_data->initial_vector[AES_BLOCK_SIZE-1] |= 0x01;
            }

            cipher_text_temp >>= 1;

            if(j==8)
            {
                plain_text++;
                j = 0;
            }
        }
        while(j++ != 8)
        {
            *plain_text >>= 1;
        }
    }
    else if ((options & AES_USE_CFB8) == AES_USE_CFB8)
    {
        uint8_t i;
        uint8_t __attribute__((aligned)) buffer[AES_BLOCK_SIZE];

        while(num--)
        {
            AESEncryptBlock(p_cfb_state_data->initial_vector,round_keys,buffer);
    
            *plain_text++ = *cipher_text ^ buffer[0];

            for(i=0;i<(AES_BLOCK_SIZE - 1);i++)
            {
                p_cfb_state_data->initial_vector[i] = p_cfb_state_data->initial_vector[i+1];
            }

            p_cfb_state_data->initial_vector[AES_BLOCK_SIZE-1] = *cipher_text++;
        }
    }
    else
    {
        if((options & AES_STREAM_START) == AES_STREAM_START)
        {
            p_cfb_state_data->bytes_in_buffer = AES_BLOCK_SIZE;
        }
    
        while(num--)
        {
            if(p_cfb_state_data->bytes_in_buffer == AES_BLOCK_SIZE)
            {
                AESEncryptBlock(p_cfb_state_data->initial_vector,round_keys,p_cfb_state_data->initial_vector);
                p_cfb_state_data->bytes_in_buffer = 0;
            }
    
            *plain_text++ = *cipher_text ^ p_cfb_state_data->initial_vector[p_cfb_state_data->bytes_in_buffer];
            p_cfb_state_data->initial_vector[p_cfb_state_data->bytes_in_buffer++] = *cipher_text++;
        }
    }
}
