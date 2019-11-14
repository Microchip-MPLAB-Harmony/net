/*****************************************************************************

 Advanced Encryption Standard (AES) Include Header for 
   electronic codebook (ECB) mode

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
#ifndef AES_ECB_H
#define AES_ECB_H

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

/* AES_ECB_STATE_DATA, *P_AES_ECB_STATE_DATA

  Summary:
    State information that must be maintained between blocks and initialization
    data.

  Description:
    The AES_ECB_STATE_DATA structure contains state information that needs to be
    retained between blocks of ECB data.  This structure is also used to pass
    some initialization data to the first block of a new stream.
*/
typedef struct
{
    /* Buffer for any data bytes that may have been remaining from the last
       call to the encrypt or decrypt function */
    /* DOM-IGNORE-BEGIN */
    /* This buffer is 16 bytes in length instead of 15 bytes in length
       because this provides a simple, word aligned buffer for the PIC32
       implementation to use */
    /* DOM-IGNORE-END */
    uint8_t __attribute__((aligned)) remaining_data[16];

    /* The number of valid bytes in the remaining_data buffer */
    uint8_t bytes_remaining;

} AES_ECB_STATE_DATA, *P_AES_ECB_STATE_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: AES Interface Routines
// *****************************************************************************
// *****************************************************************************


/*******************************************************************************
  Function:
    void AESECBEncrypt( uint8_t *cipher_text,
                        uint32_t *num_cipher_bytes,
                        uint8_t *plain_text,
                        uint32_t num_plain_bytes,
                        void * round_keys,
                        AES_ECB_STATE_DATA *p_ecb_state_data,
                        uint32_t options
                      )

  Summary:
    Encrypts a specified amount of data using Electronic Codebook (ECB) mode.
  Description:
    Encrypts a specified amount of data using Electronic Codebook (ECB) mode.

  Precondition:
      * Round keys must be create by AESCreateRoundKeys() before calling
        this routine.
      * p_cbc_state_data->round_keys must be set to point to a buffer
        containing the round keys for this session.  This buffer must
        maintain the round keys until this session is complete.  The round
        keys can be created using the AESCreateRoundKeys() function.

  Parameters:
    cipher_text     - [out] pointer to the buffer where the encrypted data should reside.  This buffer must be a multiple of 16 bytes even if the plain text passed in is not.  It must also be larger than or equal to the plain_text buffer.
    num_cipher_bytes - [out] the number of bytes written to the cipher_text buffer.  Note that this could be more or less than the num_plain_bytes due to buffered data or padding.
    plain_text      - [in]  pointer to the data that needs to be encrypted.
    num_plain_bytes - [in]  the number of bytes in the plain_text buffer
    round_keys  - [in] pointer to the round key buffer that will be used
                  for this encryption cycle.
    p_ecb_state_data      - [in]  pointer to an instance of the AES_ECB_STATE_DATA for this stream.  The first call of this function should have the p_ecb_state_data->round_keys pointing to the set of round keys created by the AESCreateRoundKeys() function.
    options         - [in]  Set of options that are ORed together (e.g. - AES_PAD_NONE | AES_STREAM_START).  Valid options are\: 
                            * AES_PAD_NONE
                            * AES_PAD_NULLS
                            * AES_PAD_8000
                            * AES_PAD_NUMBER
                            * AES_STREAM_START
                            * AES_STREAM_COMPLETE
                            * AES_STREAM_CONTINUE
                            * AES_CIPHER_TEXT_POINTER_ALIGNED
                            * AES_PLAIN_TEXT_POINTER_ALIGNED
  Returns:
    None

  Example:
    <code>

    static const uint8_t AESKey128[]              = { 0x95, 0xA8, 0xEE, 0x8E,
                                                    0x89, 0x97, 0x9B, 0x9E, 
                                                    0xFD, 0xCB, 0xC6, 0xEB, 
                                                    0x97, 0x97, 0x52, 0x8D
                                                  };

    const uint8_t AESDataBlock[] = { 0x4E, 0xC1, 0x37, 0xA4, 0x26, 0xDA, 0xBF, 0x8A, 0xA0, 0xBE, 0xB8, 0xBC, 0x0C, 0x2B, 0x89, 0xD6 };
    AES_ROUND_KEYS_128_BIT round_keys;
    uint8_t   cipher_text[AES_BLOCK_SIZE];
    AES_ECB_STATE_DATA current_stream;
    uint32_t num_bytes_encrypted;

    //Create the round keys.  This only needs to be done once for each key.
    //  This example is here for completeness.
    AESCreateRoundKeys(     &round_keys,
                            AESKey128,
                            AES_KEY_SIZE_128_BIT
                       );

    //Before the first call of AESECBEncrypt() for a stream of data, the
    //  round_keys pointer must be set to point to the buffer
    //  that contains the round keys.
    current_stream.round_keys = &round_keys;

    AESECBEncrypt(  cipher_text, 
                    &num_bytes_encrypted, 
                    AESDataBlock, 
                    sizeof(AESDataBlock), 
                    &round_keys,
                    &current_stream,
                    AES_STREAM_START | AES_STREAM_COMPLETE, 
                 );
    </code>

 *****************************************************************************/
void AESECBEncrypt( uint8_t *cipher_text, uint32_t *num_cipher_bytes, uint8_t *plain_text, uint32_t num_plain_bytes, void * round_keys, AES_ECB_STATE_DATA *p_ecb_state_data, uint32_t options);

/*******************************************************************************
  Function:
    void AESECBDecrypt( uint8_t *plain_text,
                        uint32_t *num_plain_bytes,
                        uint8_t *cipher_text,
                        uint32_t num_cipher_bytes,
                        void * round_keys,
                        AES_ECB_STATE_DATA *p_ecb_state_data,
                        uint32_t options
                      )

  Summary:
    Decrypts a specified amount of data using Electronic Codebook (ECB) mode.
  Description:
    Decrypts a specified amount of data using Electronic Codebook (ECB) mode.

  Precondition:
      * Round keys must be create by AESCreateRoundKeys() before calling
        this routine.
      * p_cbc_state_data->round_keys must be set to point to a buffer
        containing the round keys for this session.  This buffer must
        maintain the round keys until this session is complete.  The round
        keys can be created using the AESCreateRoundKeys() function.

  Parameters:
    plain_text      - [out] pointer to the buffer where the decrypted data should reside.  This buffer must be a multiple of 16 bytes even if the cipher text passed in is not.  It must also be larger than or equal to the cipher_text buffer.
    num_plain_bytes - [out] the number of bytes written to the cipher_text buffer.  Note that this could be more or less than the num_cipher_bytes due to buffered data or padding.
    cipher_text     - [in]  pointer to the data that needs to be decrypted.
    num_cipher_bytes - [in]  the number of bytes in the cipher_text buffer
    round_keys  - [in] pointer to the round key buffer that will be used
                  for this encryption cycle.
    p_ecb_state_data      - [in]  pointer to an instance of the AES_ECB_STATE_DATA for this stream.  The first call of this function should have the p_ecb_state_data->round_keys pointing to the set of round keys created by the AESCreateRoundKeys() function.
    options         - [in]  Set of options that are ORed together (e.g. - AES_STREAM_START | AES_CIPHER_TEXT_POINTER_ALIGNED).  Valid options are\: 
                            * AES_STREAM_START
                            * AES_STREAM_COMPLETE
                            * AES_STREAM_CONTINUE
                            * AES_CIPHER_TEXT_POINTER_ALIGNED
                            * AES_PLAIN_TEXT_POINTER_ALIGNED

  Returns:
    None

  Example:
    <code>

    static const uint8_t AESKey128[]              = { 0x95, 0xA8, 0xEE, 0x8E,
                                                    0x89, 0x97, 0x9B, 0x9E, 
                                                    0xFD, 0xCB, 0xC6, 0xEB, 
                                                    0x97, 0x97, 0x52, 0x8D
                                                  };

    AES_ROUND_KEYS_128_BIT round_keys;
    uint8_t   plain_text[AES_BLOCK_SIZE];
    uint32_t num_bytes_decrypted;
    
    //Create the round keys.  This only needs to be done once for each key.
    //  This example is here for completeness.
    AESCreateRoundKeys(     &round_keys,
                            AESKey128,
                            AES_KEY_SIZE_128_BIT
                       );

    //Before the first call of AESECBEncrypt() for a stream of data, the
    //  round_keys pointer must be set to point to the buffer
    //  that contains the round keys.
    current_stream.round_keys = &round_keys;

    AESECBDecrypt(  plain_text, 
                    &num_bytes_decrypted, 
                    cipher_text, 
                    sizeof(plain_text), 
                    &round_keys,
                    current_stream,
                    AES_STREAM_START | AES_CIPHER_TEXT_POINTER_ALIGNED | AES_PLAIN_TEXT_POINTER_ALIGNED
                 );
    </code>

 *****************************************************************************/
void AESECBDecrypt( uint8_t *plain_text, uint32_t *num_plain_bytes, uint8_t *cipher_text, uint32_t num_cipher_bytes, void * round_keys, AES_ECB_STATE_DATA *p_ecb_state_data, uint32_t options);

#endif //AES_ECB_H
