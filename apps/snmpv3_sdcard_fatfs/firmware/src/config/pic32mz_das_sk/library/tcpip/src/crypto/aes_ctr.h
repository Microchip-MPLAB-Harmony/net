/*****************************************************************************

 Advanced Encryption Standard (AES) Include Header for 
   counter (CTR) mode

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
#ifndef AES_CTR_H
#define AES_CTR_H

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

/* AES_CTR_STATE_DATA, *P_AES_CTR_STATE_DATA

  Summary:
    State information that must be maintained between blocks and initialization
    data.

  Description:
    The AES_CTR_STATE_DATA structure contains state information that needs to be
    retained between blocks of CTR data.  This structure is also used to pass
    some initialization data to the first block of a new stream.
*/
typedef struct
{
    /* Buffer containing the initial NONCE and counter for the CTR mode
       encryption.  This buffer should be initialized before the 
       first call of either the CTR encrypt or decrypt function */
	uint8_t __attribute__((aligned)) nonce_plus_counter[16];

    /* Buffer containing the current counter value.  This memory
       should not be touched by the user. */
	uint8_t __attribute__((aligned)) counter[16];

	/*
       must be aligned to the processors required alignment 
	   must be a multiple of AES_BLOCK_SIZE number of bytes but smaller 
       than 2^25 (33,554,432) bytes
     */
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

} AES_CTR_STATE_DATA, *P_AES_CTR_STATE_DATA;

// ****************************************************************************
// ****************************************************************************
// Section: AES Interface Routines
// ****************************************************************************
// ****************************************************************************

/******************************************************************************
  Function:
      AES_ERRORS AESCTREncrypt(   uint8_t * cipher_text,
                            uint8_t * plain_text,
                            uint32_t num_bytes,
                            void* round_keys,
                            AES_CTR_STATE_DATA *p_ctr_state_data,
                            uint32_t options
                        );
    
  Summary:
    Encrypts a specified amount of data using counter (CTR) mode.
  Description:
    Encrypts a specified amount of data using counter (CTR) mode.
  Conditions:
    * p_ctr_state_data must contain initialized data.
    * num_bytes must be less than or equal to 
      p_ofb_state_data->bytes_remaining_in_key_stream. If not then the
      AESCTRGenerateKeyStream() will be called in this function
      to generate additional key stream material as many times that are
      required to generate the required key stream based on the input.
      AESCTRGenerateKeyStream() can be called independently if you wish to
      pregenerate the key data.
    * round_keys must be properly generated using AESCreateRoundKeys().
  Input:
    cipher_text -  [out] pointer to the buffer where the encrypted data should
                   reside.  Must be num_bytes bytes long.
    plain_text  -  [in] pointer to the data that needs to be encrypted.  Must be
                   num_bytes bytes long.
    num_bytes   -  [in] the number of bytes in the plain_text and cipher_text
                   buffers
    round_keys  -   [in] pointer to the round key buffer that will be used
                   for this encryption cycle.
    p_ctr_state_data -   [in] pointer to an instance of the AES_CTR_STATE_DATA for this
                   stream.
    options -     [in] Set of options that are ORed together (e.g. \-
                  OPTION_1 | OPTION_2). Valid options are\:
                  * AES_STREAM_START
                  * AES_STREAM_CONTINUE
                  * AES_CTR_32BIT
                  * AES_CTR_64BIT
                  * AES_CTR_128BIT

                  If AES_STREAM_START is not selected then AES_STREAM_CONTINUE
                  is assumed.
  Return:
    Returns a member of the AES_ERRORS enumeration.
    * AES_NONE - No errors creating the requested key stream
    * AES_KEY_STREAM_GEN_OUT_OF_SPACE - There was not enough room remaining
      in the p_ctr_state_data->key_stream buffer to fit the key data requested by
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
    static const uint8_t initial_data[]           = { //Nonce
                                                    0x00, 0x01, 0x02, 0x03, 
                                                    0x04, 0x05, 0x06, 0x07,
                                                    //Counter
                                                    0x00, 0x00, 0x00, 0x00, 
                                                    0x00, 0x00, 0x00, 0x00
                                                  };
    AES_ROUND_KEYS_128_BIT round_keys;
    AES_CTR_STATE_DATA current_stream;
    //key_stream could also be allocated memory instead of fixed memory
    uint8_t key_stream[AES_BLOCK_SIZE*4];
    
    //Create the round keys.  This only needs to be done once for each key.
    //  This example is here for completeness.
    AESCreateRoundKeys(     &round_keys,
                            AESKey128,
                            AES_KEY_TYPE_128_BIT
                       );

    //Before the first call of AESCTRGenerateKeyStream(), the
    //  p_ctr_state_data->nonce_plus_counter must be loaded with the nonce
    //  and initial count value for that specific stream of data.
    memcpy(current_stream.nonce_plus_counter,initial_data,16);

    //Before the first call of AESCTRGenerateKeyStream(), the 
    //  key_stream variable must be set.
    current_stream.key_stream = (void*)&key_stream;

    //Before the first call of AESCTRGenerateKeyStream(), the
    //  key_stream_size variable must be set, AES_BLOCK_SIZE*4 in this example
    current_stream.key_stream_size = sizeof(key_stream); 

    //Generate 4 blocks of key stream
    AESCTRGenerateKeyStream(    4,                  //num_blocks
                                &round_keys,
                                &current_stream,
                                AES_STREAM_START    //options
                           );

    //Encrypt 23 bytes of plain_text and place it in cipher_text.
    AESCTREncrypt(  cipher_text, 
                    plain_text, 
                    23,                 //num_bytes
                    &round_keys,
                    &current_stream,    //the current state data
                    AES_STREAM_CONTINUE
                 );

    //At this point of time there are still 41 bytes of key stream left.  If
    //  plain_text is loaded with new data for the same stream, then we only 
    //  need to call the AESCTREncrypt() function again pointing to the same 
    //  stream variable.
    AESCTREncrypt(  cipher_text_part2, 
                    plain_text_part2, 
                    41,                 //num_bytes
                    &round_keys,
                    &current_stream,    //the current state data
                    AES_STREAM_CONTINUE //We use continue here just in case there
                                        //  isn't enough key material, then the code
                                        //  will generate new material from the same
                                        //  stream.
                 );
    </code>
                                                                               
  ******************************************************************************/
AES_ERRORS AESCTREncrypt(uint8_t * cipher_text, uint8_t * plain_text, uint32_t num_bytes, void* round_keys, AES_CTR_STATE_DATA *p_ctr_state_data, uint32_t options);

/******************************************************************************
  Function:
      AES_ERRORS AESCTRDecrypt (    uint8_t * plain_text,
                                    uint8_t * cipher_text,
                                    uint32_t num_bytes,
                                    void * round_keys,
                                    AES_CTR_STATE_DATA *p_ctr_state_data,
                                    uint32_t options
                                );
    
  Summary:
    Decrypts a specified amount of data using counter (CTR) mode.
  Description:
    Decrypts a specified amount of data using counter (CTR) mode.
  Conditions:
    * p_ctr_state_data must contain initialized data.
    * num_bytes must be less than or equal to 
      p_ofb_state_data->bytes_remaining_in_key_stream. If not then the
      AESCTRGenerateKeyStream() will be called in this function
      to generate additional key stream material as many times that are
      required to generate the required key stream based on the input.
      AESCTRGenerateKeyStream() can be called independently if you wish to
      pregenerate the key data.
    * round_keys must be properly generated using AESCreateRoundKeys().
  Input:
    plain_text  -  [out] pointer to the buffer where the decrypted data should
                   reside.  Must be num_bytes bytes long.
    cipher_text -  [in] pointer to the data that needs to be decrypted.  Must be
                   num_bytes bytes long
    num_bytes   -  [in] the number of bytes in the cipher_text and plain_text
                   buffers
    round_keys  -   [in] pointer to the round key buffer that will be used
                   for this encryption cycle.
    p_ctr_state_data -   [in] pointer to an instance of the AES_CTR_STATE_DATA for this
                   stream.
    options -     [in] Set of options that are ORed together (e.g. \-
                  OPTION_1 | OPTION_2). Valid options are\:
                  * AES_STREAM_START
                  * AES_STREAM_CONTINUE
                  * AES_CTR_32BIT
                  * AES_CTR_64BIT
                  * AES_CTR_128BIT

                  If AES_STREAM_START is not selected then AES_STREAM_CONTINUE
                  is assumed.
  Return:
    Returns a member of the AES_ERRORS enumeration.
    * AES_NONE - No errors creating the requested key stream
    * AES_KEY_STREAM_GEN_OUT_OF_SPACE - There was not enough room remaining
      in the p_ctr_state_data->key_stream buffer to fit the key data requested by
      the num_blocks parameter.
  Example:
    <code>
    //Decrypt 1 block of cipher_text and place it in plain_text.
    AESCTRDecrypt(  plain_text, 
                    cipher_text, 
                    AES_BLOCK_SIZE,             //num_bytes
                    &round_keys,
                    &current_stream,
                    AES_STREAM_START            //options
                 );
    </code>

    The following example shows a complete decryption cycle starting from
    the round key generation.
    <code>
    //Example variables.  The key and initialization vector information
    //  should be customized for the target application.
    static const uint8_t AESKey128[]              = { 0x95, 0xA8, 0xEE, 0x8E,
                                                    0x89, 0x97, 0x9B, 0x9E, 
                                                    0xFD, 0xCB, 0xC6, 0xEB, 
                                                    0x97, 0x97, 0x52, 0x8D
                                                  };
    static const uint8_t initial_data[]           = { //Nonce
                                                    0x00, 0x01, 0x02, 0x03, 
                                                    0x04, 0x05, 0x06, 0x07,
                                                    //Counter
                                                    0x00, 0x00, 0x00, 0x00, 
                                                    0x00, 0x00, 0x00, 0x00
                                                  };
    AES_ROUND_KEYS_128_BIT round_keys;
    AES_CTR_STATE_DATA current_stream;
    //key_stream could also be allocated memory instead of fixed memory
    uint8_t key_stream[AES_BLOCK_SIZE*4];
    
    //Create the round keys.  This only needs to be done once for each key.
    //  This example is here for completeness.
    AESCreateRoundKeys(     &round_keys,
                            AESKey128,
                            AES_KEY_TYPE_128_BIT
                       );

    //Before the first call of AESCTRGenerateKeyStream(), the
    //  p_ctr_state_data->nonce_plus_counter must be loaded with the nonce
    //  and initial count value for that specific stream of data.
    memcpy(current_stream.nonce_plus_counter,initial_data,16);

    //Before the first call of AESCTRGenerateKeyStream(), the 
    //  key_stream variable must be set.
    current_stream.key_stream = (void*)&key_stream;

    //Before the first call of AESCTRGenerateKeyStream(), the
    //  key_stream_size variable must be set, AES_BLOCK_SIZE*4 in this example
    current_stream.key_stream_size = sizeof(key_stream); 

    //Generate 4 blocks of key stream
    AESCTRGenerateKeyStream(    4,                  //num_blocks
                                &current_stream, 
                                &round_keys,
                                AES_STREAM_START    //options
                           );

    //Decrypt 23 bytes of cipher_text and place it in plain_text.
    AESCTRDecrypt(  plain_text, 
                    cipher_text, 
                    23,                 //num_bytes
                    &round_keys,
                    &current_stream,    //the current stream data
                    AES_STREAM_CONTINUE
                 );

    //At this point of time there are still 41 bytes of key stream left.  If
    //  plain_text is loaded with new data for the same stream, then we only 
    //  need to call the AESCTREncrypt() function again pointing to the same 
    //  stream variable.
    AESCTRDecrypt(  plain_text_part2, 
                    cipher_text_part2, 
                    41,                 //num_bytes
                    &round_keys,
                    &current_stream,    //the current stream data
                    AES_STREAM_CONTINUE
                 );
    </code>

    If only a particular block of cipher stream needs to be decrypted, this 
    can be done by creating a key stream for the specified block and decrypting
    that block only.  This method can be seen in the following example:
    <code>
    //Before the first call of AESCTRGenerateKeyStream(), the
    //  p_ctr_state_data->nonce_plus_counter must be loaded with the nonce
    //  and initial count value for that specific stream of data.
    memcpy(current_stream.nonce_plus_counter,initial_data,16);

    //Increment the nonce to the fourth block.  This will allow us to access
    //  block 4 of the cipher stream.  If the nonce used is a concatenation
    //  with the counter, it may be possible to just set the counter value to
    //  the desired block instead of incrementing the entire nonce;

    //Add 4 to the last block of the nonce+counter variable since in this 
    //  example the nonce is just concatenated with the nonce.
    current_stream.nonce_plus_counter[0] += 4;

    //Before the first call of AESCTRGenerateKeyStream(), the 
    //  key_stream variable must be set.  Don't need to do this again if
    //  it was already done.
    current_stream.key_stream = (void*)&key_stream;

    //Before the first call of AESCTRGenerateKeyStream(), the
    //  key_stream_size variable must be set, AES_BLOCK_SIZE*4 in this example
    //  Don't need to do this again if it was already done.
    current_stream.key_stream_size = sizeof(key_stream); 

    //Before the first call of AESCTRGenerateKeyStream() for a stream of data, the
    //  p_ctr_state_data->round_keys pointer must be set to point to the buffer
    //  that contains the round keys.
    current_stream.round_keys = &round_keys;

    //Generate 1 blocks of key stream starting at block 4
    AESCTRGenerateKeyStream(    1,                  //num_blocks
                                &round_keys,
                                &current_stream, 
                                AES_STREAM_START    //options
                           );

    //Decrypt 1 block of cipher_text and place it in plain_text.
    AESCTRDecrypt(  plain_text, 
                    cipher_text, 
                    AES_BLOCK_SIZE,             //num_bytes
                    &round_keys,
                    &current_stream,
                    AES_STREAM_CONTINUE
                 );
    </code>
                                                                               
  ******************************************************************************/
AES_ERRORS AESCTRDecrypt(uint8_t * cipher_text, uint8_t * plain_text, uint32_t num_bytes, void* round_keys, AES_CTR_STATE_DATA *p_ctr_state_data, uint32_t options);
#define AESCTRDecrypt AESCTREncrypt

/******************************************************************************
  Function:
      AES_ERRORS AESCTRGenerateKeyStream( uint32_t num_blocks,
                                          void * round_keys,
                                          AES_CTR_STATE_DATA *p_ctr_state_data,
                                          uint32_t options
                                        );
    
  Summary:
    Request to generate more key stream data.
  Description:
    Request to generate more key stream data.
  Conditions:
      * Round keys must be create by AESCreateRoundKeys() before calling
        this routine.
      * p_ctr_state_data->key_stream, p_ctr_state_data->key_stream_size, and
        p_ctr_state_data->nonce_plus_counter should all be initialized before
        calling this function.
      * round_keys must be properly generated using AESCreateRoundKeys().
  Input:
    num_blocks -  [in] The number of blocks of key stream that should be
                  created. p_ctr_state_data->key_stream should have enough space
                  remaining to handle this request.
    round_keys  - [in] pointer to the round key buffer that will be used
                  for this encryption cycle.
    p_ctr_state_data -  [in] pointer to an instance of the AES_CTR_STATE_DATA for this
                  stream. The first call of this function should have the
                  p_ctr_state_data->nonce_plus_counter set. This value will be
                  incremented for each block request.
    options -     [in] Set of options that are ORed together (e.g. \-
                  OPTION_1 | OPTION_2). Valid options are\:
                  * AES_STREAM_START
                  * AES_STREAM_CONTINUE
                  * AES_CTR_32BIT
                  * AES_CTR_64BIT
                  * AES_CTR_128BIT

                  If AES_STREAM_START is not selected then AES_STREAM_CONTINUE
                  is assumed.
  Return:
    Returns a member of the AES_ERRORS enumeration.
    * AES_NONE - No errors creating the requested key stream
    * AES_KEY_STREAM_GEN_OUT_OF_SPACE - There was not enough room remaining
      in the p_ctr_state_data->key_stream buffer to fit the key data requested by
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

    static const uint8_t initial_data[]           = { //Nonce
                                                    0x00, 0x01, 0x02, 0x03, 
                                                    0x04, 0x05, 0x06, 0x07,
                                                    //Counter
                                                    0x00, 0x00, 0x00, 0x00, 
                                                    0x00, 0x00, 0x00, 0x00
                                                  };
    AES_ROUND_KEYS_128_BIT round_keys;
    AES_CTR_STATE_DATA current_stream;
    //key_stream could also be allocated memory instead of fixed memory
    uint8_t key_stream[AES_BLOCK_SIZE*4];
    
    //Create the round keys.  This only needs to be done once for each key.
    //  This example is here for completeness.
    AESCreateRoundKeys(     &round_keys,
                            AESKey128,
                            AES_KEY_TYPE_128_BIT
                       );

    //Before the first call of AESCTRGenerateKeyStream(), the
    //  p_ctr_state_data->nonce_plus_counter must be loaded with the nonce
    //  and initial count value for that specific stream of data.
    memcpy(current_stream.nonce_plus_counter,initial_data,16);

    //Before the first call of AESCTRGenerateKeyStream(), the 
    //  key_stream variable must be set.
    current_stream.key_stream = (void*)&key_stream;

    //Before the first call of AESCTRGenerateKeyStream(), the
    //  key_stream_size variable must be set, AES_BLOCK_SIZE*4 in this example
    current_stream.key_stream_size = sizeof(key_stream); 

    //Generate 4 blocks of key stream 
    AESCTRGenerateKeyStream(    4,                                 //num_blocks
                                &round_keys,
                                &current_stream, 
                                AES_STREAM_START   //options
                           );

    //Use up the 4 blocks of data we just created
    AESCTREncrypt(  cipher_text, 
                    plain_text, 
                    AES_BLOCK_SIZE * 4, //num_bytes
                    &round_keys,
                    &current_stream,    //the current state data
                    AES_STREAM_CONTINUE
                 );

    //Generate 4 more blocks of key stream on the same stream
    AESCTRGenerateKeyStream(    4,                                 //num_blocks
                                &round_keys,
                                &current_stream, 
                                AES_STREAM_CONTINUE                //options
                           );
    </code>
                                                                                                                                                      
  ****************************************************************************/
AES_ERRORS AESCTRGenerateKeyStream(uint32_t num_blocks, void * round_keys, AES_CTR_STATE_DATA *p_ctr_state_data, uint32_t options);

#endif //AES_CTR_H
