/*****************************************************************************

 Advanced Encryption Standard (AES) Include Header for 
   output feedback (OFB) mode

****************************************************************************/

/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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
*******************************************************************************/
#ifndef AES_OFB_H
#define AES_OFB_H

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

typedef struct
{
    /* The initialization vector must be loaded before the first call to the 
       AESOFBGenerateKeyStream() function.  After that point this variable should
       not be touched.  This buffer is used to hold intermediate data. */
	uint8_t __attribute__((aligned)) initial_vector[16];

	/* must be aligned to the processors required alignment 
	   must be a multiple of AES_BLOCK_SIZE number of bytes but smaller than 2^25 (33,554,432) bytes*/
	void* key_stream;

	/*
       pointer that points to the current position in the key stream.
     */
	void* key_stream_current_position;

	/* 
       must be a multiple of AES_BLOCK_SIZE number of bytes but smaller 
       than 2^25 (33,554,432) bytes
     */
	uint32_t key_stream_size;

    /*  This field indicates the number of bytes left remaining in the key stream.  This can be used to 
        determine if you need to generate more key stream before the next call to the encrypt or decrypt
        functions.  This variable should not be checked by the user during the middle of an execution
        of the encrypt or decrypt functions.  This variable can be checked between calls to these 
        functions. */
    uint32_t bytes_remaining_in_key_stream;

} AES_OFB_STATE_DATA, *P_AES_OFB_STATE_DATA;

// ****************************************************************************
// ****************************************************************************
// Section: AES Interface Routines
// ****************************************************************************
// ****************************************************************************

/******************************************************************************
  Function:
      void AESOFBEncrypt(   uint8_t * cipher_text,
                            uint8_t * plain_text,
                            uint32_t num_bytes, 
                            void * round_keys,
                            AES_OFB_STATE_DATA *p_ofb_state_data,
                            uint32_t options
                        );
    
  Summary:
    Encrypts a specified amount of data using output feedback (OFB) mode.
  Description:
    Encrypts a specified amount of data using output feedback (OFB) mode.
    This function will always return the same number of bytes out as the 
    bytes passed in.
  Conditions:
    * p_ofb_state_data must contain initialized data. 
    * num_bytes must be less than or equal to 
      p_ofb_state_data->bytes_remaining_in_key_stream. If not then the
      AESOFBGenerateKeyStream() will be called in this function
      to generate additional key stream material as many times that are
      required to generate the required key stream based on the input.
      AESOFBGenerateKeyStream() can be called independently if you wish to
      pregenerate the key data.
    * AES_OFB_MODE must be defined in AESConfig.h
  Input:
    cipher_text -  [out] pointer to the buffer where the encrypted data should
                   reside.  Should be num_bytes bytes long.
    plain_text -   [in] pointer to the data that needs to be encrypted.  Should 
                   be num_bytes bytes long.
    num_bytes - [in] the number of bytes in the plain_text and cipher_text
                   buffers
    round_keys  -   [in] pointer to the round key buffer that will be used
                   for this encryption cycle.
    p_ofb_state_data -   [in] pointer to an instance of the AES_OFB_STATE_DATA for this
                   stream.
    options -     [in] Set of options that are ORed together (e.g. \-
                  AES_STREAM_START | AES_STREAM_COMPLETE). Valid options
                  are\:
                  * AES_STREAM_START
                  * AES_STREAM_CONTINUE
                  * AES_STREAM_COMPLETE
  Return:
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
    AES_OFB_STATE_DATA current_stream;
    //key_stream could also be allocated memory instead of fixed memory
    uint8_t key_stream[AES_BLOCK_SIZE*4];
    
    //Create the round keys.  This only needs to be done once for each key.
    //  This example is here for completeness.
    AESCreateRoundKeys(     &round_keys,
                            AESKey128,
                            AES_KEY_TYPE_128_BIT
                       );

    //Before the first call of AESOFBGenerateKeyStream(), the
    //  p_ofb_state_data->initial_vector must be loaded with the initialization
    //  vector information for that specific stream of data.
    memcpy(current_stream.initial_vector,initialization_vector,16);

    //Before the first call of AESOFBGenerateKeyStream(), the 
    //  key_stream variable must be set.
    current_stream.key_stream = (void*)&key_stream;

    //Before the first call of AESOFBGenerateKeyStream(), the
    //  num_blocks variable must be set, AES_BLOCK_SIZE*4 in this example
    current_stream.num_blocks = sizeof(key_stream); 

    //Generate 4 blocks of key stream
    AESOFBGenerateKeyStream(    4,                  //num_blocks
                                &current_stream, 
                                AES_STREAM_START    //options
                           );

    //Encrypt 23 bytes (184 bits) of plain_text and place it in cipher_text.
    AESOFBEncrypt(  cipher_text, 
                    plain_text, 
                    23,                     //num_bytes
                    &current_stream         //current state data
                 );

    //At this point of time there are still 41 bytes of key stream left.  If
    //  plain_text is loaded with new data for the same stream, then we only 
    //  need to call the AESOFBEncrypt() function again pointing to the same 
    //  stream variable.
    AESOFBEncrypt(  cipher_text_part2, 
                    plain_text_part2, 
                    41,                 //num_bytes
                    &current_stream     //current state data
                 );
    </code>
                                                                                                             
  ****************************************************************************/
AES_ERRORS AESOFBEncrypt(uint8_t * cipher_text, uint8_t * plain_text, uint32_t num_bytes, void* round_keys, AES_OFB_STATE_DATA *p_ofb_state_data, uint32_t options);

/******************************************************************************
  Function:
      void AESOFBDecrypt(   uint8_t * plain_text, 
                            uint8_t * cipher_text, 
                            uint32_t num_bytes, 
                            void * round_keys,
                            AES_OFB_STATE_DATA *p_ofb_state_data,
                            uint32_t options
                        );
    
  Summary:
    Decrypts a specified amount of data using output feedback (OFB) mode.
  Description:
    Decrypts a specified amount of data using output feedback (OFB) mode.
    This function will return the same number of bytes out as the bytes
    passed in.
  Conditions:
    * p_ofb_state_data must contain initialized data. 
    * num_bytes must be less than or equal to 
      p_ofb_state_data->bytes_remaining_in_key_stream. If not then the
      AESOFBGenerateKeyStream() will be called in this function
      to generate additional key stream material as many times that are
      required to generate the required key stream based on the input.
      AESOFBGenerateKeyStream() can be called independently if you wish to
      pregenerate the key data.
  Input:
    plain_text -   [out] pointer to the buffer where the decrypted data should
                   reside.  Should be num_bytes bytes long.
    cipher_text -  [in] pointer to the data that needs to be decrypted.  Should
                   be num_bytes bytes long.
    num_bytes - [in] the number of bytes in the cipher_text and plain_text
                   buffers
    round_keys  -   [in] pointer to the round key buffer that will be used
                   for this encryption cycle.
    p_ofb_state_data -   [in] pointer to an instance of the AES_OFB_STATE_DATA for this
                   stream.
    options -     [in] Set of options that are ORed together (e.g. \-
                  AES_STREAM_START | AES_STREAM_COMPLETE). Valid options
                  are\:
                  * AES_STREAM_START
                  * AES_STREAM_CONTINUE
                  * AES_STREAM_COMPLETE
    
  Return:
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
    AES_OFB_STATE_DATA current_stream;
    //key_stream could also be allocated memory instead of fixed memory
    uint8_t key_stream[AES_BLOCK_SIZE*4];
    
    //Create the round keys.  This only needs to be done once for each key.
    //  This example is here for completeness.
    AESCreateRoundKeys(     &round_keys,
                            AESKey128,
                            AES_KEY_TYPE_128_BIT
                       );

    //Before the first call of AESOFBGenerateKeyStream(), the
    //  p_ofb_state_data->initial_vector must be loaded with the initialization
    //  vector information for that specific stream of data.
    memcpy(current_stream.initial_vector,initialization_vector,16);

    //Before the first call of AESOFBGenerateKeyStream(), the 
    //  key_stream variable must be set.
    current_stream.key_stream = (void*)&key_stream;

    //Before the first call of AESOFBGenerateKeyStream(), the
    //  num_blocks variable must be set, AES_BLOCK_SIZE*4 in this example
    current_stream.num_blocks = sizeof(key_stream); 

    //Before the first call of AESOFBGenerateKeyStream() for a stream of data, the
    //  p_ofb_state_data->round_keys pointer must be set to point to the buffer
    //  that contains the round keys.
    current_stream.round_keys = &round_keys;

    //Generate 4 blocks of key stream
    AESOFBGenerateKeyStream(    4,                  //num_blocks
                                &current_stream, 
                                AES_STREAM_START    //options
                           );

    //Decrypt 23 bytes (184 bits) of cipher_text and place it in plain_text.
    AESOFBDecrypt(  plain_text, 
                    cipher_text, 
                    23,                     //num_bytes
                    &current_stream         //the current state data
                 );

    //At this point of time there are still 41 bytes of key stream left.  If
    //  plain_text is loaded with new data for the same stream, then we only 
    //  need to call the AESOFBDecrypt() function again pointing to the same 
    //  stream variable.
    AESOFBDecrypt(  plain_text_part2, 
                    cipher_text_part2, 
                    41,                 //num_bytes
                    &current_stream     //the current state data
                 );
    </code>
                                                                                                             
  ****************************************************************************/
AES_ERRORS AESOFBDecrypt(uint8_t * plain_text, uint8_t * cipher_text, uint32_t num_bytes, void * round_keys, AES_OFB_STATE_DATA *p_ofb_state_data, uint32_t options);
#define AESOFBDecrypt AESOFBEncrypt

/******************************************************************************
  Function:
        AES_ERRORS AESOFBGenerateKeyStream(   uint32_t num_blocks,
                                              void * round_keys,
                                              AES_OFB_STATE_DATA *p_ofb_state_data,
                                              uint32_t options
                                          );
    
  Summary:
    Request to generate more key stream data.
  Description:
    Request to generate more key stream data.
  Conditions:
      * Round keys must be create by AESCreateRoundKeys() before calling
        this routine.
      * p_ofb_state_data->key_stream, p_ofb_state_data->num_blocks, and
        p_ofb_state_data->initial_vector should all be initialized before
        calling this function.
  Input:
    num_blocks -  [in] The number of blocks of key stream that should be
                  created. p_ofb_state_data->key_stream should have enough space
                  remaining to handle this request.
    p_ofb_state_data -  [in] pointer to an instance of the AES_OFB_DATA for this
                  stream. The first call of this function should have the
                  p_ofb_state_data->initial_vector set to the initialization
                  vector.
    round_keys  - [in] pointer to the round key buffer that will be used
                  for this encryption cycle.
    options -     [in] Set of options that are ORed together (e.g. \-
                  AES_STREAM_START | AES_STREAM_COMPLETE). Valid options
                  are\:
                  * AES_STREAM_START
                  * AES_STREAM_CONTINUE
                  * AES_STREAM_COMPLETE
  Return:
    * AES_NONE - No errors creating the requested key stream
    * AES_KEY_STREAM_GEN_OUT_OF_SPACE - There was not enough room remaining
      in the p_ofb_state_data->key_stream buffer to fit the key data requested by
      the num_blocks parameter.
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
    AES_OFB_DATA current_stream;
    //key_stream could also be allocated memory instead of fixed memory
    uint8_t key_stream[AES_BLOCK_SIZE*4];
    
    //Create the round keys.  This only needs to be done once for each key.
    //  This example is here for completeness.
    AESCreateRoundKeys(     &round_keys,
                            AESKey128,
                            AES_KEY_TYPE_128_BIT
                       );

    //Before the first call of AESOFBGenerateKeyStream(), the
    //  p_ofb_state_data->initial_vector must be loaded with the initialization
    //  vector information for that specific stream of data.
    memcpy(current_stream.initial_vector,initialization_vector,16);

    //Before the first call of AESOFBGenerateKeyStream(), the 
    //  key_stream variable must be set.
    current_stream.key_stream = (void*)&key_stream;

    //Before the first call of AESOFBGenerateKeyStream(), the
    //  num_blocks variable must be set, AES_BLOCK_SIZE*4 in this example
    current_stream.num_blocks = sizeof(key_stream); 

    //Generate 4 blocks of key stream
    AESOFBGenerateKeyStream(    4,                  //num_blocks
                                &round_keys,
                                &current_stream, 
                                AES_STREAM_START    //options
                           );
    </code>
                                                                                                                                                      
  ****************************************************************************/
AES_ERRORS AESOFBGenerateKeyStream(uint32_t num_blocks, void * round_keys, AES_OFB_STATE_DATA *p_ofb_state_data, uint32_t options);

#endif //AES_OFB_H
