/*******************************************************************************
 Simple Network Management Protocol (SNMP) v3 API Header File

  Company:
    Microchip Technology Inc.
    
  File Name:
    snmpv3.h

  Summary:
    Simple Network Management Protocol Version3(SNMPv3) API header file.
    
  Description:
    SNMPv3 provides a secure access to the devices using a combination of 
    authentication and encryption of packets over the network. Comparing with 
    SNMPv1/v2, SNMPv3 ensures that packets can be collected securely from 
    SNMP devices.
    Microchip SNMPv3 Agent authentication is implemented with MD5 and SHA1 
    protocols  and encryption is implemented with AES protocol.
    These credentials and other user information are stored in the global array. 
    The user of the SNMPv3 stack can decide on the number of user names in the
    User data base to be stored with the Server. According to the SNMPv3 
    recommendation, SNMPv3 server should not be configured with the 
    authentication and privacy passwords. Instead could be configured with the 
    respective localized keys of the password. Microchip SNMPv3 agent is 
    provided with the password information in the database for the 
    'Getting Started' and for understanding purpose only. It is recommended 
    that the SNMPv3 stack should be modified to restrict access to the password 
    OIDs declared in the user data base.
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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








//DOM-IGNORE-END

#ifndef _SNMPV3_H_
#define _SNMPV3_H_

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: SNMPV3 Agent Data Types and Constants
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* 
  Summary:
    SNMPV3 Authentication and Privacy password length 

  Description:
    SNMPV3 authentication and privacy password string length should be less than
    this value.
    
  Remarks:
    None.
*/
#define TCPIP_SNMPV3_PRIVAUTH_PASSWORD_LEN      12

// *****************************************************************************
/*
  Enumeration:
    TCPIP_SNMPV3_USERDATABASECONFIG_TYPE

  Summary:
    Different Configuration parameters of SNMPv3 operation 

  Description:
    These configuration types are used by the SNMP user while doing configuration 
    SNMPv3 parameters. It includes SNMpv3 user name , authentication and encryption
    configuration parameters.
    
  Remarks:
    None.
*/
typedef enum
{
    SNMPV3_USERNAME_CONFIG_TYPE=0,  // Snmpv3 user name configuration
    SNMPV3_AUTHPASSWD_CONFIG_TYPE,  // Authentication configuration type
    SNMPV3_PRIVPASSWD_CONFIG_TYPE,  // Encryption and Decryption password configuration
    SNMPV3_AUTHPASSWDLOCALIZEDKEY_CONFIG_TYPE, // Authorization localized key configuration type
    SNMPV3_PRIVPASSWWDLOCALIZEDKEY_CONFIG_TYPE, // Encryption and Decryption password localized key configuration type
    SNMPV3_HASHTYPE_CONFIG_TYPE,            // SNMPv3 hash algorithm type
    SNMPV3_PRIVTYPE_CONFIG_TYPE,            // SNMPv3 privacy configuration type
    SNMPV3_TARGET_SECURITY_NAME_TYPE,       // SNMPv3 target address user name . This is for Trap communication
    SNMPV3_TARGET_SECURITY_LEVEL_TYPE,      // SNMPv3 target security type
    SNMPV3_TARGET_SECURITY_MODEL_TYPE,      // SNMPv3 target security model type
    SNMPV3_TARGET_MP_MODEL_TYPE,            // SNMPv3 target security message processing model type
    SNMPV3_ENGINE_ID_TYPE,                  // Identifier that uniquely and unambiguously identifies the local SNMPv3 engine
    SNMPV3_ENGINE_BOOT_TYPE,                // Number of times the local SNMPv3 engine has rebooted or reinitialized since the engine ID was last changed
    SNMPV3_ENGINE_TIME_TYPE,                // Number of seconds since the local SNMPv3 engine was last rebooted or reinitialized
    SNMPV3_ENGINE_MAX_MSG_TYPE,             // SNMPv3 Engine Maximum message size the sender can accommodate
}TCPIP_SNMPV3_USERDATABASECONFIG_TYPE;

// *****************************************************************************
/*
  Enumeration:
    STD_BASED_SNMP_MESSAGE_PROCESSING_MODEL

  Summary:
    Different SNMP Message processing model 

  Description:
    SNMP Message processing model is responsible for processing an SNMP version 
    specific message and for coordinating the interaction with security sub system.
    SNMP message processing subsystem is part of an SNMP engine which interacts with 
    the Dispatcher to handle the version specific SNMP messages.
    
    A Message Processing Model describes the version-specific procedures for 
    extracting data from messages, generating messages, calling upon a securityModel
    to apply its security services to messages.
    
  Remarks:
    None.
*/
typedef enum
{
    /* SNMP version 1 Message processing model */
    SNMPV1_MSG_PROCESSING_MODEL=0X00,
    /* SNMP version 2 Message processing model with community  as security feature */
    SNMPV2C_MSG_PROCESSING_MODEL=0X01,
    /* SNMP version 2 Message processing model */
    SNMPV2U_V2_MSG_PROCESSING_MODEL=0X02,
    /* SNMP version 3 Message processing model with authentication and
    encryption and decryption */
    SNMPV3_MSG_PROCESSING_MODEL=0X03
    /* Values between 0 to 255, inclusive, are reserved for standards-track
       Message processing Models and are managed by IANA.*/
}STD_BASED_SNMP_MESSAGE_PROCESSING_MODEL;

// *****************************************************************************
/*
  Enumeration:
    STD_BASED_SNMP_SECURITY_MODEL

  Summary:
    Different Security services for SNMPv3 messages.  

  Description:
    SNMP Security subsystem is applied to the transmission and reception of messages and 
    to the processing of the contents of messages.
    - The zero value does not identify any particular security model.
    - Values between 1 and 255, inclusive, are reserved for standards-track Security Models and are
    managed by the Internet Assigned Numbers Authority (IANA).
    - Values greater than 255 are allocated to enterprise-specific Security Models.  An
    enterprise specific securityModel value is defined to be:
    enterpriseID * 256 + security model within enterprise
    
  Remarks:
    None.
*/
typedef enum
{
    /* Security Model Reserved for ANY */
    ANY_SECUTIRY_MODEL=0x00,
    /* Security Model reserved fro SNMP version 1 */
    SNMPV1_SECURITY_MODEL=0X01,
    /* Community Security Model reserved for SNMP version 2 */
    SNMPV2C_SECURITY_MODEL=0X02,    
    /* User based security model reserved for SNMP version 3 */
    SNMPV3_USM_SECURITY_MODEL=0X03
    /* Values between 1 to 255, inclusive, are reserved for standards-track
         Security Models  and are managed by IANA.*/
}STD_BASED_SNMP_SECURITY_MODEL;

// *****************************************************************************
/*
  Enumeration:
    STD_BASED_SNMPV3_SECURITY_LEVEL

  Summary:
    Different Security Level for SNMPv3 messages.  

  Description:
    A Level of Security at which SNMPv3 messages can be sent or with which operations
    are being processed.                
    
  Remarks:
    None.
*/
typedef enum
{
    /* without authentication and without privacy */
    NO_AUTH_NO_PRIV=1,
    /* with authentication but without privacy */
    AUTH_NO_PRIV,
    /* with authentication but with privacy */
    AUTH_PRIV
}STD_BASED_SNMPV3_SECURITY_LEVEL;

// *****************************************************************************
/*
  Enumeration:
    SNMPV3_PRIV_PROT_TYPE

  Summary:
    Different type of encryption and decryption for SNMPv3.  

  Description:
    These below privacy types are supported by the SNMPv3 USM model for data confidentiality.
    SNMPv3 agent supports both AES-CFB and DES-CBC encryption and decryption algorithm.
    For DES-CBC privacy protocol, SNMPv3 agent will use Harmony Crypto Library.
    For AES-CFB privacy protocol, SNMPv3 agent will use Legacy TCP/IP Crypto Library
    (For AES, include -aes_pic32mx.a to the project.)        
    
  Remarks:
    128-bit, 192-bit and 256-bit AES are supported.
*/
typedef enum
{
    /* Data Encryption Standard (DES-CBC) encryption and decryption protocol */ 
    SNMPV3_DES_PRIV=0x0,
    /* Advanced Encryption Standard (AES-CFB) encryption and decryption protocol */ 
    SNMPV3_AES_PRIV,
    /* No encryption or decryption protocol is supported */
    SNMPV3_NO_PRIV
}SNMPV3_PRIV_PROT_TYPE;

// *****************************************************************************
/*
  Enumeration:
    SNMPV3_HMAC_HASH_TYPE

  Summary:
    Different type of authentication for SNMPv3.  

  Description:
    The following authentication types are supported by the SNMPv3 USM model for 
    data confidentiality.
    SNMPv3 agent supports both MD5 and SHA1 protocol for authentication. 
    
  Remarks:
    None.
*/
typedef enum
{
    /* HMAC MD5 authentication protocol */
    SNMPV3_HMAC_MD5 = 0u,       // MD5 is being calculated
    /* HMAC SHA1 authentication protocol*/
    SNMPV3_HMAC_SHA1,               // SHA-1 is being calculated
    /* No authentication is supported */
    SNMPV3_NO_HMAC_AUTH
} SNMPV3_HMAC_HASH_TYPE;

// *****************************************************************************
/*
  Enumeration:
    SNMPV3_USM_CONFIG_ERROR_TYPE

  Summary:
    USM Configuration error code type enumeration.  

  Description:
    The following error codes describes the return types errors for SNMPv3 USM
    configuration.   
    
  Remarks:
    None.
*/
typedef enum
{
    SNMPV3_USM_SUCCESS=0,
    /* Error return value if pointer argument is NULL */
    SNMPV3_USM_INVALID_INPUTCONFIG,
    /* Error return value if user index is more than TCPIP_SNMPV3_USM_MAX_USER */
    SNMPV3_USM_INVALID_USER,
    /* Error return value for UserName "initial", username "initial" is not 
    allowed */
    SNMPV3_USM_INVALID_USERNAME,
    /* Error return value if userName length is more than the value of 
    TCPIP_SNMPV3_USER_SECURITY_NAME_LEN */
    SNMPV3_USM_INVALID_USER_NAME_LENGTH,
    /* Error return value if Auth or Priv password length is more than the value
     of TCPIP_SNMPV3_PRIVAUTH_PASSWORD_LEN */
    SNMPV3_USM_INVALID_PRIVAUTH_PASSWORD_LEN,
     /* Error return value if Auth or Priv password length is more than the value
     of TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN */
    SNMPV3_USM_INVALID_PRIVAUTH_LOCALIZED_PASSWORD_LEN,
    /* Error return value if Auth or Priv security level type is not matching to
    the enum USM_SECURITY_LEVEL*/
    SNMPV3_USM_INVALID_PRIVAUTH_TYPE,
    /*Authentication security level configuration not allowed if security level 
    type is noAuthNoPriv */
    SNMPV3_USM_INVALID_AUTH_CONFIG_NOT_ALLOWED,
    /*Privacy security level configuration allowed if security level 
    type is authPriv */
    SNMPV3_USM_INVALID_PRIV_CONFIG_NOT_ALLOWED,
    /* Error return value for invalid security level */
    SNMPV3_USM_INVALID_SECURITY_LEVEL,
    /*SNMPV3 USM SET configuration not supported */
    SNMPV3_USM_NOT_SUPPORTED,
    SNMPV3_USM_NO_ERROR,
    
}TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE;

// *****************************************************************************
/*
  Structure:
    TCPIP_SNMP_COMMUNITY_CONFIG

  Summary:
    SNMP community configuration.

  Description:
    This structure is used to configure community details during run-time.

  Remarks:
    None.
*/
typedef struct
{
    const char    *communityName;
}TCPIP_SNMP_COMMUNITY_CONFIG;

// *****************************************************************************
/*
  Structure:
    SNMPV3_USM_USER_CONFIG

  Summary:
    SNMPv3 USM configuration.

  Description:
    This structure is used to configure predefined SNMPv3 USM details
    for run-time configuration.

  Remarks:
    None.
*/
typedef struct
{
    const char    *username;           /**< user name string */
    STD_BASED_SNMPV3_SECURITY_LEVEL security_level;      /**< security level: auth, priv combination */

    /* auth protocol */
    SNMPV3_HMAC_HASH_TYPE     usm_auth_proto;      /**< auth type: md5, sha1 */
    const char *usm_auth_password;  /**< passphrase string for authentication */

    /* priv protocol */
    SNMPV3_PRIV_PROT_TYPE     usm_priv_proto;      /**< priv type: DES */
    const char *usm_priv_password;  /**< passphrase string for privacy */

} TCPIP_SNMPV3_USM_USER_CONFIG;
// *****************************************************************************
/*
  Structure:
    SNMPV3_TARGET_ENTRY_CONFIG

  Summary:
    SNMP module trap target address configuration.

  Description:
    This structure is used to configure SNMP target details during runtime .

  Remarks:
    None.
*/
typedef struct
{
    const char *secname;
    STD_BASED_SNMP_SECURITY_MODEL  mp_model;
    STD_BASED_SNMP_SECURITY_MODEL  sec_model;
    STD_BASED_SNMPV3_SECURITY_LEVEL sec_level;
}TCPIP_SNMPV3_TARGET_ENTRY_CONFIG;

// *****************************************************************************
/*
  Structure:
    TCPIP_SNMP_MODULE_CONFIG

  Summary:
    SNMP module configuration.

  Description:
    This structure is used to configure SNMP details for runtime configuration.

  Remarks:
    None
*/
typedef struct
{
    bool trapEnable;    /* true = agent can send the trap, false = agent shouldn't send the trap */
    bool snmp_trapv2_use; /* true = agent uses Trap version v2 and false = uses Tarp version 1*/
    bool snmpv3_trapv1v2_use; /* SNMPv3 trap should be true , only if SNMP version is 3 */
    const char* snmp_bib_file;
    TCPIP_SNMP_COMMUNITY_CONFIG *read_community_config; /* read-only Community configuration*/
    TCPIP_SNMP_COMMUNITY_CONFIG *write_community_config; /* write-only Community configuration*/
    TCPIP_SNMPV3_USM_USER_CONFIG *usm_config; /* SNMPv3 USM configuration*/
    TCPIP_SNMPV3_TARGET_ENTRY_CONFIG *trap_target_config; /* SNMPv3 trap configuration*/
    // The SNMP server will access snmp.bib file or other MIB files located under this top directory.
    // No access is possible above this directory.
    // As a good practice to follow, use something like: "/mnt_point/snmp/"
    // to limit an external user access to this only directory in the file system
    // when accessing files.
    // NOTE: to allow access to the whole file system use the root directory:
    // "/mnt_point/" or "/" depending on your SYS_FS settings/mounting
    // But this usage should be discouraged because it gives SNMP Server access
    // to all system files.
    const char* mountPath;
} TCPIP_SNMP_MODULE_CONFIG;

// *****************************************************************************
// *****************************************************************************
// Section: SNMPV3 GET and SET Request Function
// *****************************************************************************
// *****************************************************************************

//****************************************************************************
/*
  Function:
    bool TCPIP_SNMPV3_EngineUserDataBaseGet(
        TCPIP_SNMPV3_USERDATABASECONFIG_TYPE userDataBaseType,
        uint8_t len,
        uint8_t userIndex,
        void *val);

  Summary:
    Get SNMPv3 USM engine data base details.

  Description:
    This function is used to get SNMPv3 Engine data base details using 
    TCPIP_SNMPV3_USERDATABASECONFIG_TYPE enumeration. 

  Precondition:
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    userDataBaseType - SNMPv3 data base configuration type
    len - Number of bytes need to be read from data base
    userIndex - SNMPv3 user index
    val - void pointer to a any data type. Length parameter value is changed according to
            data type. 

  Returns:
    true  - success
    false - failure
    
  Example:
  <code>
    bool TCPIP_SNMP_VarbindGet(SNMP_ID var, SNMP_INDEX index, uint8_t* ref, SNMP_VAL* val)
    {
        switch(var)
        {
            case USER_SECURITY_NAME:
                if(index < TCPIP_SNMPV3_USM_MAX_USER)
                {
                    if(TCPIP_SNMPV3_EngineUserDataBaseGet(SNMPV3_USERNAME_CONFIG_TYPE,myRef,index,&val->byte) == false)
                    {
                        *ref = SNMP_END_OF_VAR;
                    }
                    else
                    {
                        myRef++;
                        *ref = myRef;
                    }
                    return true;
                }
                break;
        }
    }
    
  </code>
  Remarks:
    None
*/
bool TCPIP_SNMPV3_EngineUserDataBaseGet(
    TCPIP_SNMPV3_USERDATABASECONFIG_TYPE userDataBaseType,
    uint8_t len,
    uint8_t userIndex,
    void *val);

//****************************************************************************
/*
  Function:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_USMAuthPrivLocalization(
                                       SNMP_INDEX index)

  Summary:
    Calculate Auth and Priv  password to the localized key with present USM
    details.

  Description:
    This routine converts MD5 or SHA1 authentication and AES privacy password 
    string to localized key. 

  Precondition:
    SNMPV3 initialization is required

  Parameters:
    userIndex - SNMPv3 user index

  Returns:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE 
    
  Example:
  <code>
    uint8_t index=0;
    // The below API should be called after all the USM SET opeartion.
    TCPIP_SNMPV3_USMAuthPrivLocalization(SNMP_INDEX index);
  </code>
  
  Remarks:
    TCPIP_SNMPV3_USMAuthPrivLocaziation(SNMP_INDEX index) should be called after 
 setting USM Authentication and Privacy password configuration.
 */
TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_USMAuthPrivLocalization(SNMP_INDEX index);

//****************************************************************************
/*
  Function:
    bool TCPIP_SNMPV3_EngineUserDataBaseSet(
            TCPIP_SNMPV3_USERDATABASECONFIG_TYPE userDataBaseType,
            uint8_t len, uint8_t userIndex, void *val)

  Summary:
    Set SNMPv3 engine data base details.

  Description:
    This function is used to set SNMPv3 Engine data base details using 
    TCPIP_SNMPV3_USERDATABASECONFIG_TYPE enumeration. 

  Precondition:
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    userDataBaseType - SNMPv3 data base configuration type
    len - Number of bytes need to be read from data base
    userIndex - SNMPv3 user index
    val - void pointer to a any data type. Length parameter value is changed according to
            data type. 

  Returns:
    true  - success
    false - failure 
    
  Example:
  <code>
  bool TCPIP_SNMP_VarbindSet(SNMP_ID var, SNMP_INDEX index, uint8_t ref, SNMP_VAL val)
  {
    switch(var)
    {
        case USER_SECURITY_NAME:
            if(ref ==  SNMP_END_OF_VAR)
            {
                if(TCPIP_SNMPV3_EngineUserDataBaseSet(SNMPV3_USERNAME_CONFIG_TYPE,strlen((char*)gSnmpv3UserSecurityName),
                                                index,gSnmpv3UserSecurityName) != true)
                    return false;
            }
            // Make sure that index is within our range.
            if(index < TCPIP_SNMPV3_USM_MAX_USER)
            {
                // Copy given value into local buffer.
                gSnmpv3UserSecurityName[ref]=val.byte;
                return true;
            }
            break;
    }
  }
   // The below API should be called after all the USM SET opeartion.
    TCPIP_SNMPV3_USMAuthPrivLocalization(SNMP_INDEX index);
  </code>
  
  Remarks:
    TCPIP_SNMPV3_USMAuthPrivLocaziation(SNMP_INDEX index) shoudl be called after 
 setting USM attributes.
 */
bool TCPIP_SNMPV3_EngineUserDataBaseSet(
    TCPIP_SNMPV3_USERDATABASECONFIG_TYPE userDataBaseType,
    uint8_t len,
    uint8_t userIndex,
    void *val);

//****************************************************************************
//****************************************************************************
// Section: SNMP TRAP Related Functions
//****************************************************************************
//****************************************************************************

//****************************************************************************
/*
  Function:
    bool TCPIP_SNMPv3_Notify(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index,
                             uint8_t targetIndex)

  Summary:
    Creates and Sends SNMPv3 TRAP PDU.

  Description:
    This function creates SNMPv3 trap PDU and sends it to previously specified
    remoteHost.

  Precondition:
    TRAP event is triggered.

  Parameters:
    var     - SNMP var ID that is to be used in notification
    val     - Value of var. Only value of uint8_t, uint16_t or uint32_t can be sent.
    index   - Index of var. If this var is a single,index would be 0, or else
              if this var Is a sequence, index could be any value
              from 0 to 127
    targetIndex -index of the 'Snmpv3TrapConfigData' table's security user name
                 for which the TRAP PDU message header to constructed.

  Return Values:
    - true  - If SNMP notification was successfully sent. However, this does
              not guarantee that the remoteHost received it.
    - false - Notification sent failed. This would fail under the following conditions:
              - The specified SNMP_BIB_FILE does not exist in the file system
              - The specified var does not exist
              - The previously provided agentID does not exist
              - The data type of the specified var is unknown. This is only
                possible if the file system itself was corrupted.
    - SNMPV3_MSG_PRIV_FAIL - encryption of the trap message failed
    - SNMPV3_MSG_AUTH_FAIL - HAMC of the trap message failed

  Remarks:
    None.
  */
bool TCPIP_SNMPv3_Notify(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index,
    uint8_t targetIndex, SNMP_TRAP_IP_ADDRESS_TYPE eTrapMultiAddressType);

//****************************************************************************
/*
  Function:
    void TCPIP_SNMPv3_TrapConfigDataGet(
        uint8_t userIndex,
        uint8_t *msgModelType,
        uint8_t *securityModelType)

  Summary:
    Gets the SNMPv3 Trap configuration details using the user name index.

  Description:
    This function is used to get SNMPv3 message model type and security model type
    using user index.

  Precondition:
    TCPIP_SNMP_Initialize is already called.

  Parameters:
    userIndex - user name index
    msgModelType - message processing type
    securityModelType - security model type

  Returns:
    None.
    
  Remarks:
    None.
*/
void TCPIP_SNMPv3_TrapConfigDataGet(uint8_t userIndex, 
        uint8_t *msgModelType, uint8_t *securityModelType);

//****************************************************************************
/*
  Function:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_SetUSMUserName(
            char *userName, uint8_t userLen, uint8_t userIndex)

  Summary:
    Configure the SNMPv3 user name for a specific index.

  Description:
    This function is used to set the SNMPv3 user name for a specific index.
 
  Precondition:
    TCPIP_SNMP_Initialize and SNMPv3 module initialization should be completed.

  Parameters:
    userName - USM user name
    userLen - user length should be less than TCPIP_SNMPV3_USER_SECURITY_NAME_LEN
    userIndex - user name index

  Returns:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE
    
  Remarks:
    None.
*/
TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_SetUSMUserName(char *userName, 
            uint8_t userLen, uint8_t userIndex);

//****************************************************************************
/*
  Function:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_GetUSMUserName(
            char *userName, uint8_t *userLen, uint8_t userIndex)

  Summary:
    Get the configured SNMPv3 user name for a specific index.

  Description:
    This function is used to get the SNMPv3 user name for a specific index.
 
  Precondition:
    TCPIP_SNMP_Initialize and SNMPv3 module initialization should be completed.

  Parameters:
    userName - USM user name
    userLen - returns username length 
    userIndex - user name index

  Returns:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE
    
  Remarks:
    None.
*/
TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_GetUSMUserName(char *userName, 
            uint8_t *userLen, uint8_t userIndex);

//****************************************************************************
/*
  Function:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_SetUSMAuth(char *userName, 
    uint8_t userLen, char *authPasswd, uint8_t authPasswdLen, 
    SNMPV3_HMAC_HASH_TYPE hashType)

  Summary:
    Configure the SNMPv3 authentication password for a configured USM user name.

  Description:
    This function is used to configure the SNMPv3 authentication password for a
    configured USM user name. Authentication password configuration should not be 
    allowed for noAuthNoPRiv security level.
 
  Precondition:
    TCPIP_SNMP_Initialize and SNMPv3 module initialization should be completed.

  Parameters:
    userName - USM user name
    userLen - username length 
    authPasswd - authentication password
    authPasswdLen - authentication password length
    hashType - set authentication hash type (MD5 or SHA)

  Returns:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE
    
  Remarks:
    TCPIP_SNMPV3_USMAuthPrivLocalization() should be called after configuring 
    authentication password.
*/
TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_SetUSMAuth(char *userName, 
    uint8_t userLen, char *authPasswd, uint8_t authPasswdLen, SNMPV3_HMAC_HASH_TYPE hashType);

//****************************************************************************
/*
  Function:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_GetUSMAuth(char *userName, 
    uint8_t userLen, char *authPasswd, uint8_t *authPasswdLen, 
    SNMPV3_HMAC_HASH_TYPE *hashType)

  Summary:
    Get the SNMPv3 authentication password for a configured USM user name.

  Description:
    This function is used to get the SNMPv3 authentication password for a
    configured USM user name. 
 
  Precondition:
    TCPIP_SNMP_Initialize and SNMPv3 module initialization should be completed.

  Parameters:
    userName - USM user name
    userLen - username length 
    authPasswd - get authentication password
    authPasswdLen - get authentication password length
    hashType - get authentication hash type (MD5 or SHA)

  Returns:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE
    
  Remarks:
    None
*/
TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_GetUSMAuth(char *userName, 
    uint8_t userLen, char *authPasswd, uint8_t *authPasswdLen, SNMPV3_HMAC_HASH_TYPE *hashType);

//****************************************************************************
/*
  Function:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_SetUSMPrivacy(char *userName, 
    uint8_t userLen, char *privPasswd, uint8_t privPasswdLen, SNMPV3_PRIV_PROT_TYPE privType)

  Summary:
    Configure the SNMPv3 privacy password for a configured USM user name.

  Description:
    This function is used to configure the SNMPv3 authentication password for a
    configured USM user name. Privacy password configuration should not be 
    allowed for noAuthNoPRiv and AuthNoPriv security levels.
 
  Precondition:
    TCPIP_SNMP_Initialize and SNMPv3 module initialization should be completed.

  Parameters:
    userName - USM user name
    userLen -  username length 
    privPasswd - privacy password
    privPasswdLen - privacy password length
    privType - set privacy protocol type (AES-CFG or DES)

  Returns:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE
    
  Remarks:
    TCPIP_SNMPV3_USMAuthPrivLocalization() should be called after configuring 
    privacy password.
*/
TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_SetUSMPrivacy(char *userName, 
    uint8_t userLen, char *privPasswd, uint8_t privPasswdLen, SNMPV3_PRIV_PROT_TYPE privType);


//****************************************************************************
/*
  Function:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_GetUSMPrivacy(char *userName, 
    uint8_t userLen, char *privPasswd, uint8_t *privPasswdLen, SNMPV3_PRIV_PROT_TYPE *privType)

  Summary:
    Get the SNMPv3 privacy password for a configured USM user name.

  Description:
    This function is used to get the SNMPv3 privacy password for a
    configured USM user name. 
 
  Precondition:
    TCPIP_SNMP_Initialize and SNMPv3 module initialization should be completed.

  Parameters:
    userName - USM user name
    userLen - username length 
    privPasswd - get privacy password
    privPasswdLen - get privacy password length
    privType - get privacy protocol type (AES-CFG or DES)

  Returns:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE
    
  Remarks:
    None
*/
TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_GetUSMPrivacy(char *userName, 
    uint8_t userLen, char *privPasswd, uint8_t *privPasswdLen, SNMPV3_PRIV_PROT_TYPE *privType);

//****************************************************************************
/*
  Function:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_SetUSMSecLevel(
    char *userName, uint8_t userLen, STD_BASED_SNMPV3_SECURITY_LEVEL secLevel)

  Summary:
    Configure the SNMPv3 USM security level for a configured USM user name.

  Description:
    This function is used to configure the SNMPv3 USM security level for a
    configured USM user name. 
  
  Precondition:
    TCPIP_SNMP_Initialize and SNMPv3 module initialization should be completed.

  Parameters:
    userName - USM user name
    userLen - username length 
    secLevel - security level

  Returns:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE
    
  Remarks:
    None
*/
TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_SetUSMSecLevel(char *userName,
        uint8_t userLen, STD_BASED_SNMPV3_SECURITY_LEVEL secLevel);

//****************************************************************************
/*
  Function:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_GetUSMSecLevel(
    char *userName, uint8_t userLen, STD_BASED_SNMPV3_SECURITY_LEVEL *secLevel)

  Summary:
    Get SNMPv3 USM security level for a configured USM user name.

  Description:
    This function is used to get the SNMPv3 USM security level for a
    configured USM user name. 
  
  Precondition:
    TCPIP_SNMP_Initialize and SNMPv3 module initialization should be completed.

  Parameters:
    userName - USM user name
    userLen -  username length 
    secLevel - returns security level

  Returns:
    TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE
    
  Remarks:
    None
*/
TCPIP_SNMPV3_USM_CONFIG_ERROR_TYPE TCPIP_SNMPV3_GetUSMSecLevel(char *userName, 
        uint8_t userLen, STD_BASED_SNMPV3_SECURITY_LEVEL *secLevel);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif//#ifndef _SNMPV3_H_
