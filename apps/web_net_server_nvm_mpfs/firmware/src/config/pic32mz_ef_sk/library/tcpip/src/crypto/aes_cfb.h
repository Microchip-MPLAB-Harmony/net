/*****************************************************************************

 Advanced Encryption Standard (AES) Include Header for 
   cipher feeback (CFB) mode

****************************************************************************/

/*****************************************************************************
 Copyright (C) 2016-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

#ifndef AES_CFB_H
#define AES_CFB_H

// ****************************************************************************
// ****************************************************************************
// Section: Includes
// ****************************************************************************
// ****************************************************************************

// ****************************************************************************
// ****************************************************************************
// Section: Constants & Data Types
// ****************************************************************************
// ****************************************************************************

/* AES_CFB_STATE_DATA, *P_AES_CFB_STATE_DATA

  Summary:
    State information that must be maintained between blocks and initialization
    data.

  Description:
    The AES_CFB_STATE_DATA structure contains state information that needs to be
    retained between blocks of CFB data.  This structure is also used to pass
    some initialization data to the first block of a new stream.
*/
typedef struct
{
    /* The initialization vector must be loaded before the first call to the 
       encrypt or decrypt functions.  After that point this variable should
       not be touched.  This buffer is used to hold intermediate data. */
    uint8_t __attribute__((aligned)) initial_vector[16];

    /* The number of bytes left in the key stream before a new block encryption
       needs to occur */
    uint8_t bytes_in_buffer;

} AES_CFB_STATE_DATA, *P_AES_CFB_STATE_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: AES Interface Routines
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:  void AESCFBEncrypt(	uint8_t * cipher_text,
                                    uint8_t * plain_text, 
									uint32_t num_bytes, 
                                    void* round_keys,
									AES_CFB_STATE_DATA *p_cfb_state_data,
                                    uint32_t options
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
    num_bytes        - [in]  the number of bytes in the plain_text and cipher_text buffers
    round_keys       - [in]  pointer to the round key buffer that will be used
                       for this encryption cycle.
    p_cfb_state_data - [in]  pointer to an instance of the AES_CFB_STATE_DATA for this stream.  The first call of this function should have the p_cfb_state_data->initial_vector set to the initialization vector.
    options          - [in]  Set of options that are ORed together (e.g. - AES_CIPHER_TEXT_POINTER_ALIGNED | AES_STREAM_START).  Valid options are\: 
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
    static const uint8_t AESKey128[]              = { 0x95, 0xA8, 0xEE, 0x8E,
                                                    0x89, 0x97, 0x9B, 0x9E, 
                                                    0xFD, 0xCB, 0xC6, 0xEB, 
                                                    0x97, 0x97, 0x52, 0x8D
                                                  };
    static const uint8_t initialization_vector[]  = { 0x00, 0x01, 0x02, 0x03, 
                                                    0x04, 0x05, 0x06, 0x07, 
                                                    0x08, 0x09, 0x0A, 0x0B, 
                                                    0x0C, 0x0D, 0x0E, 0x0F
                                                  };
    AES_ROUND_KEYS_128_BIT round_keys;
    AES_CFB_STATE_DATA current_stream;
    
    //Create the round keys.  This only needs to be done once for each key.
    //  This example is here for completeness.
    AESCreateRoundKeys(     &round_keys,
                            AESKey128,
                            AES_KEY_SIZE_128_BIT
                       );
    
    //Before the first call of AESCFBEncrypt() for a stream of data, the
    //  p_cfb_state_data->initial_vector must be loaded with the initialization
    //  vector information for that specific stream of data.
    memcpy(current_stream.initial_vector,initialization_vector,16);

    //Encrypt 128 bytes of of the current stream.  The num_bytes field must
    //  be a multiple of 16 unless the options field specifies that it is the
    //  end of the stream.
    AESCFBEncrypt(  cipher_text,
                    plain_text,
                    128,                    //num_bytes
                    &round_keys,
                    &current_stream,
                    AES_STREAM_START,       //options
                 );
    
    //At some point later we need to encrypt the rest of the stream.  The
    //  entire stream could have been encrypted in the first call if it was all
    //  available at the time.  Otherwise it can be done in sections, like this
    //  example.
    
    //Encrypt the remaining data of this stream.
    AESCFBEncrypt(  cipher_text_part2,
                    plain_text_part2,
                    53,                                     //num_bytes
                    &round_keys,
                    &current_stream,
                    AES_PAD_NULLS | AES_STREAM_COMPLETE     //options
                 );
    </code>

 *****************************************************************************/
void AESCFBEncrypt(uint8_t * cipher_text, uint8_t * plain_text, uint32_t num_bytes, void * round_keys, AES_CFB_STATE_DATA *p_cfb_state_data, uint32_t options);

/*******************************************************************************
  Function:  void AESCFBDecrypt(	uint8_t * plain_text,
									uint8_t * cipher_text, 
									uint32_t num_bytes, 
                                    void * round_keys,
									AES_CFB_STATE_DATA *p_cfb_state_data,
                                    uint32_t options
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
    num_bytes        - [in]  the number of bytes in the cipher_text buffer
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
    static const uint8_t AESKey128[]              = { 0x95, 0xA8, 0xEE, 0x8E,
                                                    0x89, 0x97, 0x9B, 0x9E, 
                                                    0xFD, 0xCB, 0xC6, 0xEB, 
                                                    0x97, 0x97, 0x52, 0x8D
                                                  };
    static const uint8_t initialization_vector[]  = { 0x00, 0x01, 0x02, 0x03, 
                                                    0x04, 0x05, 0x06, 0x07, 
                                                    0x08, 0x09, 0x0A, 0x0B, 
                                                    0x0C, 0x0D, 0x0E, 0x0F
                                                  };
    AES_ROUND_KEYS_128_BIT round_keys;
    AES_CFB_STATE_DATA current_stream;
    
    //Create the round keys.  This only needs to be done once for each key.
    //  This example is here for completeness.
    AESCreateRoundKeys(     &round_keys,
                            AESKey128,
                            AES_KEY_TYPE_128_BIT
                       );
    
    //Before the first call of AESCFBDecrypt() for a stream of data, the
    //  p_cfb_state_data->initial_vector must be loaded with the initialization
    //  vector information for that specific stream of data.
    memcpy(current_stream.initial_vector,initialization_vector,16);
    
    //Decrypt 128 bytes of of the current stream.  The num_bytes field must
    //  be a multiple of 16 unless the options field specifies that it is the
    //  end of the stream.
    AESCFBDecrypt(  plain_text,
                    cipher_text,
                    128,                    //num_bytes
                    &round_keys,
                    &current_stream,
                    AES_STREAM_START,       //options
                 );
    
    //At some point later we need to decrypt the rest of the stream.  The
    //  entire stream could have been decrypted in the first call if it was all
    //  available at the time.  Otherwise it can be done in sections, like this
    //  example.
    
    //Decrypt the remaining data of this stream.
    AESCFBDecrypt(  plain_text_part2,
                    cipher_text_part2,
                    53,                     //num_bytes
                    &round_keys,
                    &current_stream,
                    AES_STREAM_COMPLETE     //options
                 );
    </code>

 *****************************************************************************/
void AESCFBDecrypt(uint8_t * plain_text, uint8_t * cipher_text, uint32_t num_bytes, void * round_keys, AES_CFB_STATE_DATA *p_cfb_state_data, uint32_t options);

#endif //AES_CFB_H
