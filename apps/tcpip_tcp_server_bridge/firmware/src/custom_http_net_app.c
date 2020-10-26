/*******************************************************************************
  Application to Demo HTTP NET Server

  Summary:
    Support for HTTP NET module in Microchip TCP/IP Stack

  Description:
    -Implements the application
 *******************************************************************************/

/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

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


#include "system_config.h"
#include "system_definitions.h"
#include "http_net_print.h"
#if defined(TCPIP_STACK_USE_HTTP_NET_SERVER)

//#include "crypto/crypto.h"
#include "net_pres/pres/net_pres_socketapi.h"
#include "system/sys_random_h2_adapter.h"
#include "system/sys_time_h2_adapter.h"
#include "tcpip/tcpip.h"
#include "tcpip/src/common/helpers.h"

/****************************************************************************
  Section:
    Definitions
 ****************************************************************************/
#ifndef APP_SWITCH_1StateGet
#define APP_SWITCH_1StateGet() 0
#endif

#ifndef APP_SWITCH_2StateGet
#define APP_SWITCH_2StateGet() 0
#endif

#ifndef APP_SWITCH_3StateGet
#define APP_SWITCH_3StateGet() 0
#endif

#ifndef APP_LED_1StateGet
#define APP_LED_1StateGet() 0
#endif
#ifndef APP_LED_2StateGet
#define APP_LED_2StateGet() 0
#endif
#ifndef APP_LED_3StateGet
#define APP_LED_3StateGet() 0
#endif

#ifndef APP_LED_1StateSet
#define APP_LED_1StateSet()
#endif
#ifndef APP_LED_2StateSet
#define APP_LED_2StateSet()
#endif
#ifndef APP_LED_3StateSet
#define APP_LED_3StateSet()
#endif

#ifndef APP_LED_1StateClear
#define APP_LED_1StateClear()
#endif
#ifndef APP_LED_2StateClear
#define APP_LED_2StateClear()
#endif
#ifndef APP_LED_3StateClear
#define APP_LED_3StateClear()
#endif

#ifndef APP_LED_1StateToggle
#define APP_LED_1StateToggle()
#endif
#ifndef APP_LED_2StateToggle
#define APP_LED_2StateToggle()
#endif
#ifndef APP_LED_3StateToggle
#define APP_LED_3StateToggle()
#endif

// Use the web page in the Demo App (~2.5kb ROM, ~0b RAM)
#define HTTP_APP_USE_RECONFIG

#if !defined( NO_MD5 )        // no MD5 if crypto_config.h says NO_MD5   
// Use the MD5 Demo web page (~5kb ROM, ~160b RAM)
//#define HTTP_APP_USE_MD5
#endif

// Use the e-mail demo web page
#if defined(TCPIP_STACK_USE_SMTPC)
#define HTTP_APP_USE_EMAIL  1
#else
#define HTTP_APP_USE_EMAIL  0
#endif

/****************************************************************************
Section:
Function Prototypes and Memory Globalizers
 ****************************************************************************/
#if defined(TCPIP_HTTP_NET_USE_POST)
    #if defined(SYS_OUT_ENABLE)
        static TCPIP_HTTP_NET_IO_RESULT HTTPPostLCD(TCPIP_HTTP_NET_CONN_HANDLE connHandle);
    #endif
    #if defined(HTTP_APP_USE_MD5)
        static TCPIP_HTTP_NET_IO_RESULT HTTPPostMD5(TCPIP_HTTP_NET_CONN_HANDLE connHandle);
    #endif
    #if defined(HTTP_APP_USE_RECONFIG)
        static TCPIP_HTTP_NET_IO_RESULT HTTPPostConfig(TCPIP_HTTP_NET_CONN_HANDLE connHandle);
        #if defined(TCPIP_STACK_USE_SNMP_SERVER)
        static TCPIP_HTTP_NET_IO_RESULT HTTPPostSNMPCommunity(TCPIP_HTTP_NET_CONN_HANDLE connHandle);
        #endif
    #endif
    #if (HTTP_APP_USE_EMAIL != 0) 
        static TCPIP_HTTP_NET_IO_RESULT HTTPPostEmail(TCPIP_HTTP_NET_CONN_HANDLE connHandle);
    #endif
    #if defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)
        static TCPIP_HTTP_NET_IO_RESULT HTTPPostDDNSConfig(TCPIP_HTTP_NET_CONN_HANDLE connHandle);
    #endif
#endif

extern const char *const ddnsServiceHosts[];
// RAM allocated for DDNS parameters
#if defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)
    static uint8_t DDNSData[100];
#endif

// Sticky status message variable.
// This is used to indicated whether or not the previous POST operation was
// successful.  The application uses these to store status messages when a
// POST operation redirects.  This lets the application provide status messages
// after a redirect, when connection instance data has already been lost.
static bool lastSuccess = false;

// Sticky status message variable.  See lastSuccess for details.
static bool lastFailure = false;

/****************************************************************************
  Section:
    Customized HTTP NET Functions
 ****************************************************************************/

// processing the HTTP buffer acknowledgment
void TCPIP_HTTP_NET_DynAcknowledge(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const void *buffer, const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = (HTTP_APP_DYNVAR_BUFFER*)((const uint8_t *)buffer - offsetof(HTTP_APP_DYNVAR_BUFFER, data));

    pDynBuffer->busy = 0;
}

// processing the HTTP reported events
void TCPIP_HTTP_NET_EventReport(TCPIP_HTTP_NET_CONN_HANDLE connHandle, TCPIP_HTTP_NET_EVENT_TYPE evType, const void *evInfo, const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    char *evMsg = (char *)evInfo;

    if(evType < 0)
    {   // display errors only
        if(evMsg == 0)
        {
            evMsg = "none";
        }
        SYS_CONSOLE_PRINT("HTTP event: %d, info: %s\r\n", evType, evMsg);
    }
}

// example of processing an SSI notification
// return false for standard processing of this SSI command by the HTTP module
// return true if the processing is done by you and HTTP need take no further action
bool TCPIP_HTTP_NET_SSINotification(TCPIP_HTTP_NET_CONN_HANDLE connHandle, TCPIP_HTTP_SSI_NOTIFY_DCPT *pSSINotifyDcpt, const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    static  int newVarCount = 0;

    char    *cmdStr, *varName;
    char    newVarVal[] = "Page Visits: ";
    char    scratchBuff[100];

    cmdStr = pSSINotifyDcpt->ssiCommand;

    if(strcmp(cmdStr, "include") == 0)
    {   // here a standard SSI include directive is processed
        return false;
    }

    if(strcmp(cmdStr, "set") == 0)
    {   // a SSI variable is set; let the standard processing take place
        return false;
    }

    if(strcmp(cmdStr, "echo") == 0)
    {   // SSI echo command
        // inspect the variable name
        varName = pSSINotifyDcpt->pAttrDcpt->value;
        if(strcmp(varName, "myStrVar") == 0)
        {   // change the value of this variable
            sprintf(scratchBuff, "%s%d", newVarVal, ++newVarCount);

            if(!TCPIP_HTTP_NET_SSIVariableSet(varName, TCPIP_HTTP_DYN_ARG_TYPE_STRING, scratchBuff, 0))
            {
                SYS_CONSOLE_MESSAGE("SSI set myStrVar failed!!!\r\n");
            }
            // else success
            return false;
        }
    }

    return false;
}

/****************************************************************************
  Section:
    GET Form Handlers
 ****************************************************************************/

/*****************************************************************************
  Function:
    TCPIP_HTTP_NET_IO_RESULT TCPIP_HTTP_NET_ConnectionGetExecute(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_NET_USER_CALLBACK *pCBack)

  Internal:
    See documentation in the TCP/IP Stack APIs or http_net.h for details.
 ****************************************************************************/
TCPIP_HTTP_NET_IO_RESULT TCPIP_HTTP_NET_ConnectionGetExecute(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    const uint8_t *ptr;
    uint8_t *httpDataBuff;
    uint8_t filename[20];

    // Load the file name
    // Make sure uint8_t filename[] above is large enough for your longest name
    SYS_FS_FileNameGet(TCPIP_HTTP_NET_ConnectionFileGet(connHandle), filename, 20);

    httpDataBuff = TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle);

    // If its the forms.htm page
    if(!memcmp(filename, "forms.htm", 9))
    {
        // Seek out each of the four LED strings, and if it exists set the LED states
        ptr = TCPIP_HTTP_NET_ArgGet(httpDataBuff, (const uint8_t *)"led2");
        if(ptr)
        {
            if(*ptr == '1')
            {
                APP_LED_2StateSet();
            }
            else
            {
                APP_LED_2StateClear();
            }
        }

        ptr = TCPIP_HTTP_NET_ArgGet(httpDataBuff, (const uint8_t *)"led1");
        if(ptr)
		{
            if(*ptr == '1')
            {
                APP_LED_1StateSet();
            }
            else
            {
                APP_LED_1StateClear();
            }
        }
    }

    else if(!memcmp(filename, "cookies.htm", 11))
    {
        // This is very simple.  The names and values we want are already in
        // the data array.  We just set the hasArgs value to indicate how many
        // name/value pairs we want stored as cookies.
        // To add the second cookie, just increment this value.
        // remember to also add a dynamic variable callback to control the printout.
        TCPIP_HTTP_NET_ConnectionHasArgsSet(connHandle, 0x01);
    }

    // If it's the LED updater file
    else if(!memcmp(filename, "leds.cgi", 8))
    {
        // Determine which LED to toggle
        ptr = TCPIP_HTTP_NET_ArgGet(httpDataBuff, (const uint8_t *)"led");

        // Toggle the specified LED
        switch(*ptr) {
            case '0':
                APP_LED_1StateToggle();
                break;
            case '1':
                APP_LED_2StateToggle();
                break;
            case '2':
                APP_LED_3StateToggle();
                break;
        }
    }

    return TCPIP_HTTP_NET_IO_RES_DONE;
}

/****************************************************************************
  Section:
    POST Form Handlers
 ****************************************************************************/
#if defined(TCPIP_HTTP_NET_USE_POST)

/*****************************************************************************
  Function:
    TCPIP_HTTP_NET_IO_RESULT TCPIP_HTTP_NET_ConnectionPostExecute(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_NET_USER_CALLBACK *pCBack)

  Internal:
    See documentation in the TCP/IP Stack APIs or http_net.h for details.
 ****************************************************************************/
TCPIP_HTTP_NET_IO_RESULT TCPIP_HTTP_NET_ConnectionPostExecute(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    // Resolve which function to use and pass along
    uint8_t filename[20];

    // Load the file name
    // Make sure uint8_t filename[] above is large enough for your longest name
    SYS_FS_FileNameGet(TCPIP_HTTP_NET_ConnectionFileGet(connHandle), filename, sizeof(filename));

#if defined(SYS_OUT_ENABLE)
    if(!memcmp(filename, "forms.htm", 9))
        return HTTPPostLCD(connHandle);
#endif

#if defined(HTTP_APP_USE_MD5)
    if(!memcmp(filename, "upload.htm", 10))
        return HTTPPostMD5(connHandle);
#endif

#if defined(HTTP_APP_USE_RECONFIG)
    if(!memcmp(filename, "protect/config.htm", 18))
        return HTTPPostConfig(connHandle);
    #if defined(TCPIP_STACK_USE_SNMP_SERVER)
    else if(!memcmp(filename, "snmp/snmpconfig.htm", 19))
        return HTTPPostSNMPCommunity(connHandle);
    #endif
#endif

#if (HTTP_APP_USE_EMAIL != 0) 
    if(!strcmp((char *)filename, "email/index.htm"))
        return HTTPPostEmail(connHandle);
#endif

#if defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)
    if(!strcmp((char *)filename, "dyndns/index.htm"))
        return HTTPPostDDNSConfig(connHandle);
#endif

    return TCPIP_HTTP_NET_IO_RES_DONE;
}

/*****************************************************************************
  Function:
    static TCPIP_HTTP_NET_IO_RESULT HTTPPostLCD(TCPIP_HTTP_NET_CONN_HANDLE connHandle)

  Summary:
    Processes the LCD form on forms.htm

  Description:
    Locates the 'lcd' parameter and uses it to update the text displayed
    on the board's LCD display.

    This function has four states.  The first reads a name from the data
    string returned as part of the POST request.  If a name cannot
    be found, it returns, asking for more data.  Otherwise, if the name
    is expected, it reads the associated value and writes it to the LCD.
    If the name is not expected, the value is discarded and the next name
    parameter is read.

    In the case where the expected string is never found, this function
    will eventually return TCPIP_HTTP_NET_IO_RES_NEED_DATA when no data is left.  In that
    case, the HTTP server will automatically trap the error and issue an
    Internal Server Error to the browser.

  Precondition:
    None

  Parameters:
    connHandle  - HTTP connection handle

  Return Values:
    TCPIP_HTTP_NET_IO_RES_DONE - the parameter has been found and saved
    TCPIP_HTTP_NET_IO_RES_WAITING - the function is pausing to continue later
    TCPIP_HTTP_NET_IO_RES_NEED_DATA - data needed by this function has not yet arrived
 ****************************************************************************/
#if defined(SYS_OUT_ENABLE)
static TCPIP_HTTP_NET_IO_RESULT HTTPPostLCD(TCPIP_HTTP_NET_CONN_HANDLE connHandle)
{
    uint8_t *cDest;
    uint8_t *httpDataBuff;
    uint16_t httpBuffSize;

#define SM_POST_LCD_READ_NAME       (0u)
#define SM_POST_LCD_READ_VALUE      (1u)

    httpDataBuff = TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle);
    httpBuffSize = TCPIP_HTTP_NET_ConnectionDataBufferSizeGet(connHandle);
    switch(TCPIP_HTTP_NET_ConnectionPostSmGet(connHandle))
    {
        // Find the name
        case SM_POST_LCD_READ_NAME:

            // Read a name
            if(TCPIP_HTTP_NET_ConnectionPostNameRead(connHandle, httpDataBuff, httpBuffSize) == TCPIP_HTTP_NET_READ_INCOMPLETE)
                return TCPIP_HTTP_NET_IO_RES_NEED_DATA;

            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_POST_LCD_READ_VALUE);
            // No break...continue reading value

        // Found the value, so store the LCD and return
        case SM_POST_LCD_READ_VALUE:

            // If value is expected, read it to data buffer,
            // otherwise ignore it (by reading to NULL)
            if(!strcmp((char *)httpDataBuff, (const char *)"lcd"))
                cDest = httpDataBuff;
            else
                cDest = NULL;

            // Read a value string
            if(TCPIP_HTTP_NET_ConnectionPostValueRead(connHandle, cDest, httpBuffSize) == TCPIP_HTTP_NET_READ_INCOMPLETE)
                return TCPIP_HTTP_NET_IO_RES_NEED_DATA;

            // If this was an unexpected value, look for a new name
            if(!cDest)
            {
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_POST_LCD_READ_NAME);
                break;
            }

            SYS_OUT_MESSAGE((char *)cDest);

            // This is the only expected value, so callback is done
            strcpy((char *)httpDataBuff, "/forms.htm");
            TCPIP_HTTP_NET_ConnectionStatusSet(connHandle, TCPIP_HTTP_NET_STAT_REDIRECT);
            return TCPIP_HTTP_NET_IO_RES_DONE;
    }

    // Default assumes that we're returning for state machine convenience.
    // Function will be called again later.
    return TCPIP_HTTP_NET_IO_RES_WAITING;
}
#endif

/*****************************************************************************
  Function:
    static TCPIP_HTTP_NET_IO_RESULT HTTPPostMD5(TCPIP_HTTP_NET_CONN_HANDLE connHandle)

  Summary:
    Processes the file upload form on upload.htm

  Description:
    This function demonstrates the processing of file uploads.  First, the
    function locates the file data, skipping over any headers that arrive.
    Second, it reads the file 64 bytes at a time and hashes that data.  Once
    all data has been received, the function calculates the MD5 sum and
    stores it in current connection data buffer.

    After the headers, the first line from the form will be the MIME
    separator.  Following that is more headers about the file, which we
    discard.  After another CRLFCRLF, the file data begins, and we read
    it 16 bytes at a time and add that to the MD5 calculation.  The reading
    terminates when the separator string is encountered again on its own
    line.  Notice that the actual file data is trashed in this process,
    allowing us to accept files of arbitrary size, not limited by RAM.
    Also notice that the data buffer is used as an arbitrary storage array
    for the result.  The ~uploadedmd5~ callback reads this data later to
    send back to the client.

  Precondition:
    None

  Parameters:
    connHandle  - HTTP connection handle

  Return Values:
    TCPIP_HTTP_NET_IO_RES_DONE - all parameters have been processed
    TCPIP_HTTP_NET_IO_RES_WAITING - the function is pausing to continue later
    TCPIP_HTTP_NET_IO_RES_NEED_DATA - data needed by this function has not yet arrived
 ****************************************************************************/
#if defined(HTTP_APP_USE_MD5)
static TCPIP_HTTP_NET_IO_RESULT HTTPPostMD5(TCPIP_HTTP_NET_CONN_HANDLE connHandle)
{
    static CRYPT_MD5_CTX md5;
    uint8_t *httpDataBuff;
    uint32_t lenA, lenB;

    #define SM_MD5_READ_SEPARATOR   (0u)
    #define SM_MD5_SKIP_TO_DATA     (1u)
    #define SM_MD5_READ_DATA        (2u)
    #define SM_MD5_POST_COMPLETE    (3u)

    switch(TCPIP_HTTP_NET_ConnectionPostSmGet(connHandle))
    {
        // Just started, so try to find the separator string
        case SM_MD5_READ_SEPARATOR:
            // Reset the MD5 calculation
            CRYPT_MD5_Initialize(&md5);

            // See if a CRLF is in the buffer
            lenA = TCPIP_HTTP_NET_ConnectionStringFind(connHandle, "\r\n", 0, 0);

            if(lenA == 0xffff)
            {   // if not, ask for more data
                return TCPIP_HTTP_NET_IO_RES_NEED_DATA;
            }

            // If so, figure out where the last byte of data is
            // Data ends at CRLFseparator--CRLF, so len + 6 bytes
            TCPIP_HTTP_NET_ConnectionByteCountDec(connHandle, lenA + 6);

            // Read past the CRLF
            TCPIP_HTTP_NET_ConnectionByteCountDec(connHandle, TCPIP_HTTP_NET_ConnectionRead(connHandle, NULL, lenA + 2));

            // Save the next state (skip to CRLFCRLF)
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_MD5_SKIP_TO_DATA);

            // No break...continue reading the headers if possible

        // Skip the headers
        case SM_MD5_SKIP_TO_DATA:
            // Look for the CRLFCRLF
            lenA = TCPIP_HTTP_NET_ConnectionStringFind(connHandle, "\r\n\r\n", 0, 0);

            if(lenA != 0xffff)
            {// Found it, so remove all data up to and including
                lenA = TCPIP_HTTP_NET_ConnectionRead(connHandle, NULL, lenA + 4);
                TCPIP_HTTP_NET_ConnectionByteCountDec(connHandle, lenA);
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_MD5_READ_DATA);
            }
            else
            {// Otherwise, remove as much as possible
                lenA = TCPIP_HTTP_NET_ConnectionRead(connHandle, NULL, TCPIP_HTTP_NET_ConnectionReadIsReady(connHandle) - 4);
                TCPIP_HTTP_NET_ConnectionByteCountDec(connHandle, lenA);

                // Return the need more data flag
                return TCPIP_HTTP_NET_IO_RES_NEED_DATA;
            }

            // No break if we found the header terminator

        // Read and hash file data
        case SM_MD5_READ_DATA:
            // Find out how many bytes are available to be read
            httpDataBuff = TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle);
            lenA = TCPIP_HTTP_NET_ConnectionReadIsReady(connHandle);
            lenB = TCPIP_HTTP_NET_ConnectionByteCountGet(connHandle);
            if(lenA > lenB)
                lenA = lenB;

            while(lenA > 0u)
            {// Add up to 64 bytes at a time to the sum
                lenB = TCPIP_HTTP_NET_ConnectionRead(connHandle, httpDataBuff, (lenA < 64u)?lenA:64);
                TCPIP_HTTP_NET_ConnectionByteCountDec(connHandle, lenB);
                lenA -= lenB;
                CRYPT_MD5_DataAdd(&md5,httpDataBuff, lenB);
            }

            // If we've read all the data
            if(TCPIP_HTTP_NET_ConnectionByteCountGet(connHandle) == 0u)
            {// Calculate and copy result data buffer for printout
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_MD5_POST_COMPLETE);
                CRYPT_MD5_Finalize(&md5, httpDataBuff);
                return TCPIP_HTTP_NET_IO_RES_DONE;
            }

            // Ask for more data
            return TCPIP_HTTP_NET_IO_RES_NEED_DATA;
    }

    return TCPIP_HTTP_NET_IO_RES_DONE;
}
#endif // #if defined(HTTP_APP_USE_MD5)

/*****************************************************************************
  Function:
    static TCPIP_HTTP_NET_IO_RESULT HTTPPostConfig(TCPIP_HTTP_NET_CONN_HANDLE connHandle)

  Summary:
    Processes the configuration form on config/index.htm

  Description:
    Accepts configuration parameters from the form, saves them to a
    temporary location in RAM, then eventually saves the data to EEPROM or
    external Flash.

    When complete, this function redirects to config/reboot.htm, which will
    display information on reconnecting to the board.

    This function creates a shadow copy of a network info structure in
    RAM and then overwrites incoming data there as it arrives.  For each
    name/value pair, the name is first read to cur connection data[0:5].  Next, the
    value is read to newNetConfig.  Once all data has been read, the new
    network info structure is saved back to storage and the browser is redirected to
    reboot.htm.  That file includes an AJAX call to reboot.cgi, which
    performs the actual reboot of the machine.

    If an IP address cannot be parsed, too much data is POSTed, or any other
    parsing error occurs, the browser reloads config.htm and displays an error
    message at the top.

  Precondition:
    None

  Parameters:
    connHandle  - HTTP connection handle

  Return Values:
    TCPIP_HTTP_NET_IO_RES_DONE - all parameters have been processed
    TCPIP_HTTP_NET_IO_RES_NEED_DATA - data needed by this function has not yet arrived
 ****************************************************************************/
#if defined(HTTP_APP_USE_RECONFIG)
// network configuration/information storage space
static struct
{
    TCPIP_NET_HANDLE    currNet;            // current working interface + valid flag
    char                ifName[10 + 1];     // interface name
    char                nbnsName[16 + 1];   // host name
    char                ifMacAddr[17 + 1];  // MAC address
    char                ipAddr[15 +1];      // IP address
    char                ipMask[15 + 1];     // mask
    char                gwIP[15 + 1];       // gateway IP address
    char                dns1IP[15 + 1];     // DNS IP address
    char                dns2IP[15 + 1];     // DNS IP address

    TCPIP_NETWORK_CONFIG    netConfig;  // configuration in the interface requested format
}httpNetData;

static TCPIP_HTTP_NET_IO_RESULT HTTPPostConfig(TCPIP_HTTP_NET_CONN_HANDLE connHandle)
{
    bool bConfigFailure = false;
    uint8_t i;
    uint8_t *httpDataBuff = 0;
    uint16_t httpBuffSize;
    uint32_t byteCount;
    IPV4_ADDR newIPAddress, newMask;
    TCPIP_MAC_ADDR newMACAddr;

    httpNetData.currNet = 0; // forget the old settings
    httpNetData.netConfig.startFlags = 0;   // assume DHCP is off

    // Check to see if the browser is attempting to submit more data than we
    // can parse at once.  This function needs to receive all updated
    // parameters and validate them all before committing them to memory so that
    // orphaned configuration parameters do not get written (for example, if a
    // static IP address is given, but the subnet mask fails parsing, we
    // should not use the static IP address).  Everything needs to be processed
    // in a single transaction.  If this is impossible, fail and notify the user.
    // As a web devloper, if you add parameters to the network info and run into this
    // problem, you could fix this by to splitting your update web page into two
    // seperate web pages (causing two transactional writes).  Alternatively,
    // you could fix it by storing a static shadow copy of network info someplace
    // in memory and using it when info is complete.
    // Lastly, you could increase the TCP RX FIFO size for the HTTP server.
    // This will allow more data to be POSTed by the web browser before hitting this limit.
    byteCount = TCPIP_HTTP_NET_ConnectionByteCountGet(connHandle);
    if(byteCount > TCPIP_HTTP_NET_ConnectionReadBufferSize(connHandle))
    {   // Configuration Failure
        lastFailure = true;
        TCPIP_HTTP_NET_ConnectionStatusSet(connHandle, TCPIP_HTTP_NET_STAT_REDIRECT);
        return TCPIP_HTTP_NET_IO_RES_DONE;
    }

    // Ensure that all data is waiting to be parsed.  If not, keep waiting for
    // all of it to arrive.
    if(TCPIP_HTTP_NET_ConnectionReadIsReady(connHandle) < byteCount)
        return TCPIP_HTTP_NET_IO_RES_NEED_DATA;

    // Use current config in non-volatile memory as defaults
    httpDataBuff = TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle);
    httpBuffSize = TCPIP_HTTP_NET_ConnectionDataBufferSizeGet(connHandle);

    // Read all browser POST data
    while(TCPIP_HTTP_NET_ConnectionByteCountGet(connHandle))
    {
        // Read a form field name
        if(TCPIP_HTTP_NET_ConnectionPostNameRead(connHandle, httpDataBuff, 6) != TCPIP_HTTP_NET_READ_OK)
        {
            bConfigFailure = true;
            break;
        }

        // Read a form field value
        if(TCPIP_HTTP_NET_ConnectionPostValueRead(connHandle, httpDataBuff + 6, httpBuffSize - 6 - 2) != TCPIP_HTTP_NET_READ_OK)
        {
            bConfigFailure = true;
            break;
        }

        // Parse the value that was read
        if(!strcmp((char *)httpDataBuff, (const char *)"ip"))
        {// Save new static IP Address
            if(!TCPIP_Helper_StringToIPAddress((char *)(httpDataBuff + 6), &newIPAddress))
            {
                bConfigFailure = true;
                break;
            }
            strncpy(httpNetData.ipAddr, (char *)httpDataBuff + 6, sizeof(httpNetData.ipAddr));
        }
        else if(!strcmp((char *)httpDataBuff, (const char *)"gw"))
        {// Read new gateway address
            if(!TCPIP_Helper_StringToIPAddress((char *)(httpDataBuff + 6), &newIPAddress))
            {
                bConfigFailure = true;
                break;
            }
            strncpy(httpNetData.gwIP, (char *)httpDataBuff + 6, sizeof(httpNetData.gwIP));
        }
        else if(!strcmp((char *)httpDataBuff, (const char *)"sub"))
        {// Read new static subnet
            if(!TCPIP_Helper_StringToIPAddress((char *)(httpDataBuff + 6), &newMask))
            {
                bConfigFailure = true;
                break;
            }
            strncpy(httpNetData.ipMask, (char *)httpDataBuff + 6, sizeof(httpNetData.ipMask));
        }
        else if(!strcmp((char *)httpDataBuff, (const char *)"dns1"))
        {// Read new primary DNS server
            if(!TCPIP_Helper_StringToIPAddress((char *)(httpDataBuff + 6), &newIPAddress))
            {
                bConfigFailure = true;
                break;
            }
            strncpy(httpNetData.dns1IP, (char *)httpDataBuff + 6, sizeof(httpNetData.dns1IP));
        }
        else if(!strcmp((char *)httpDataBuff, (const char *)"dns2"))
        {// Read new secondary DNS server
            if(!TCPIP_Helper_StringToIPAddress((char *)(httpDataBuff + 6), &newIPAddress))
            {
                bConfigFailure = true;
                break;
            }
            strncpy(httpNetData.dns2IP, (char *)httpDataBuff + 6, sizeof(httpNetData.dns2IP));
        }
        else if(!strcmp((char *)httpDataBuff, (const char *)"mac"))
        {   // read the new MAC address
            if(!TCPIP_Helper_StringToMACAddress((char *)(httpDataBuff + 6), newMACAddr.v))
            {
                bConfigFailure = true;
                break;
            }
            strncpy(httpNetData.ifMacAddr, (char *)httpDataBuff + 6, sizeof(httpNetData.ifMacAddr));
        }
        else if(!strcmp((char *)httpDataBuff, (const char *)"host"))
        {   // Read new hostname
            strncpy(httpNetData.nbnsName, (char *)httpDataBuff + 6, sizeof(httpNetData.nbnsName));
        }
        else if(!strcmp((char *)httpDataBuff, (const char *)"dhcp"))
        {// Read new DHCP Enabled flag
            httpNetData.netConfig.startFlags = httpDataBuff[6] == '1' ? TCPIP_NETWORK_CONFIG_DHCP_CLIENT_ON : 0;
        }
    }

    if(bConfigFailure == false)
    {
        // All parsing complete!  Save new settings and force an interface restart
        // Set the interface to restart and display reconnecting information
        strcpy((char *)httpDataBuff, "/protect/reboot.htm?");
        TCPIP_Helper_FormatNetBIOSName((uint8_t *)httpNetData.nbnsName);
        memcpy((void *)(httpDataBuff + 20), httpNetData.nbnsName, 16);
        httpDataBuff[20 + 16] = 0x00; // Force null termination
        for(i = 20; i < 20u + 16u; ++i)
        {
            if(httpDataBuff[i] == ' ')
                httpDataBuff[i] = 0x00;
        }
        // save current interface and mark as valid
        httpNetData.currNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
        strncpy(httpNetData.ifName, TCPIP_STACK_NetNameGet(httpNetData.currNet), sizeof(httpNetData.ifName));
    }
    else
    {   // Configuration error
        lastFailure = true;
        if(httpDataBuff)
        {
            strcpy((char *)httpDataBuff, "/protect/config.htm");
        }
    }

    TCPIP_HTTP_NET_ConnectionStatusSet(connHandle, TCPIP_HTTP_NET_STAT_REDIRECT);

    return TCPIP_HTTP_NET_IO_RES_DONE;
}

#if defined(TCPIP_STACK_USE_SNMP_SERVER)
static TCPIP_HTTP_NET_IO_RESULT HTTPPostSNMPCommunity(TCPIP_HTTP_NET_CONN_HANDLE connHandle)
{
    uint8_t len = 0;
    uint8_t vCommunityIndex;
    uint8_t *httpDataBuff;
    uint16_t httpBuffSize;

    #define SM_CFG_SNMP_READ_NAME   (0u)
    #define SM_CFG_SNMP_READ_VALUE  (1u)

    httpDataBuff = TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle);
    httpBuffSize = TCPIP_HTTP_NET_ConnectionDataBufferSizeGet(connHandle);
    switch(TCPIP_HTTP_NET_ConnectionPostSmGet(connHandle))
    {
        case SM_CFG_SNMP_READ_NAME:
            // If all parameters have been read, end
            if(TCPIP_HTTP_NET_ConnectionByteCountGet(connHandle) == 0u)
            {
                return TCPIP_HTTP_NET_IO_RES_DONE;
            }

            // Read a name
            if(TCPIP_HTTP_NET_ConnectionPostNameRead(connHandle, httpDataBuff, httpBuffSize) == TCPIP_HTTP_NET_READ_INCOMPLETE)
                return TCPIP_HTTP_NET_IO_RES_NEED_DATA;

            // Move to reading a value, but no break
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_CFG_SNMP_READ_VALUE);

        case SM_CFG_SNMP_READ_VALUE:
            // Read a value
            if(TCPIP_HTTP_NET_ConnectionPostValueRead(connHandle, httpDataBuff + 6, httpBuffSize) == TCPIP_HTTP_NET_READ_INCOMPLETE)
                return TCPIP_HTTP_NET_IO_RES_NEED_DATA;

            // Default action after this is to read the next name, unless there's an error
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_CFG_SNMP_READ_NAME);

            // See if this is a known parameter and legal (must be null
            // terminator in 4th field name byte, string must no greater than
            // TCPIP_SNMP_COMMUNITY_MAX_LEN bytes long, and TCPIP_SNMP_MAX_COMMUNITY_SUPPORT
            // must not be violated.
            vCommunityIndex = httpDataBuff[3] - '0';
            if(vCommunityIndex >= TCPIP_SNMP_MAX_COMMUNITY_SUPPORT)
                break;
            if(httpDataBuff[4] != 0x00u)
                break;
            len = strlen((char *)httpDataBuff + 6);
            if(len > TCPIP_SNMP_COMMUNITY_MAX_LEN)
            {
                break;
            }
            if(memcmp((void *)httpDataBuff, (const void *)"rcm", 3) == 0)
            {
                if(TCPIP_SNMP_ReadCommunitySet(vCommunityIndex,len,httpDataBuff + 6)!=true)
                    break;
            }
            else if(memcmp((void *)httpDataBuff, (const void *)"wcm", 3) == 0)
            {
                if(TCPIP_SNMP_WriteCommunitySet(vCommunityIndex,len,httpDataBuff + 6) != true)
                    break;
            }
            else
            {
                break;
            }

            break;
    }

    return TCPIP_HTTP_NET_IO_RES_WAITING; // Assume we're waiting to process more data
}
#endif // #if defined(TCPIP_STACK_USE_SNMP_SERVER)
#endif // #if defined(HTTP_APP_USE_RECONFIG)

/*****************************************************************************
  Function:
    static TCPIP_HTTP_NET_IO_RESULT HTTPPostEmail(void)

  Summary:
    Processes the e-mail form on email/index.htm

  Description:
    This function sends an e-mail message using the SMTPC client.
    If encryption is needed it is done by the SMTPC module communicating with the SMTP server.
    (the NET_PRES layer has to be configured for encryption support).
    
    It demonstrates the use of the SMTPC client, waiting for asynchronous
    processes in an HTTP callback.
    
  Precondition:
    None

  Parameters:
    connHandle  - HTTP connection handle

  Return Values:
    TCPIP_HTTP_NET_IO_RES_DONE - the message has been sent
    TCPIP_HTTP_NET_IO_RES_WAITING - the function is waiting for the SMTP process to complete
    TCPIP_HTTP_NET_IO_RES_NEED_DATA - data needed by this function has not yet arrived
 ****************************************************************************/
#if (HTTP_APP_USE_EMAIL != 0) 
// size of an email parameter
#define HTTP_APP_EMAIL_PARAM_SIZE           30 
// maximum size of the mail body
#define HTTP_APP_EMAIL_BODY_SIZE            200 
// maximum size of the mail attachment
#define HTTP_APP_EMAIL_ATTACHMENT_SIZE      200 

// handle of the mail message submitted to SMTPC
static TCPIP_SMTPC_MESSAGE_HANDLE postMailHandle = 0;

// structure describing the post email operation
typedef struct
{
    char*   ptrParam;       // pointer to the current parameter being retrieved
    int     paramSize;      // size of the buffer to retrieve the parameter
    int     attachLen;      // length of the attachment buffer
    bool    mailParamsDone; // flag that signals that all parameters were retrieved
    TCPIP_SMTPC_ATTACH_BUFFER attachBuffer; // descriptor for the attachment
    TCPIP_SMTPC_MESSAGE_RESULT mailRes;     // operation outcome

    // storage area
    char serverName[HTTP_APP_EMAIL_PARAM_SIZE + 1];
    char username[HTTP_APP_EMAIL_PARAM_SIZE + 1];
    char password[HTTP_APP_EMAIL_PARAM_SIZE + 1];
    char mailTo[HTTP_APP_EMAIL_PARAM_SIZE + 1];
    char serverPort[10 + 1];
    char mailBody[HTTP_APP_EMAIL_BODY_SIZE + 1];
    char mailAttachment[HTTP_APP_EMAIL_ATTACHMENT_SIZE];

}HTTP_POST_EMAIL_DCPT;

static HTTP_POST_EMAIL_DCPT postEmail;

// callback for getting the signal of mail completion
static void postMailCallback(TCPIP_SMTPC_MESSAGE_HANDLE messageHandle, const TCPIP_SMTPC_MESSAGE_REPORT* pMailReport)
{
    postEmail.mailRes = pMailReport->messageRes;
    if(postEmail.mailRes < 0)
    {
        SYS_CONSOLE_PRINT("SMTPC mail FAILED! Callback result: %d\r\n", postEmail.mailRes);
    }
    else
    {
        SYS_CONSOLE_MESSAGE("SMTPC mail SUCCESS!\r\n");
    }
}

static TCPIP_HTTP_NET_IO_RESULT HTTPPostEmail(TCPIP_HTTP_NET_CONN_HANDLE connHandle)
{

    TCPIP_SMTPC_MAIL_MESSAGE mySMTPMessage;
    char paramName[HTTP_APP_EMAIL_PARAM_SIZE + 1];

    #define SM_EMAIL_INIT                       (0)
    #define SM_EMAIL_READ_PARAM_NAME            (1)
    #define SM_EMAIL_READ_PARAM_VALUE           (2)
    #define SM_EMAIL_SEND_MESSAGE               (3)
    #define SM_EMAIL_WAIT_RESULT                (4)

    switch(TCPIP_HTTP_NET_ConnectionPostSmGet(connHandle))
    {
        case SM_EMAIL_INIT:
            if(postMailHandle != 0)
            {   // some other operation on going
                return TCPIP_HTTP_NET_IO_RES_ERROR;
            }


            memset(&postEmail, 0, sizeof(postEmail));
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_READ_PARAM_NAME);
            return TCPIP_HTTP_NET_IO_RES_WAITING;


        case SM_EMAIL_READ_PARAM_NAME:
            // Search for a parameter name in POST data
            if(TCPIP_HTTP_NET_ConnectionPostNameRead(connHandle, (uint8_t*)paramName, sizeof(paramName)) == TCPIP_HTTP_NET_READ_INCOMPLETE)
            {
                return TCPIP_HTTP_NET_IO_RES_NEED_DATA;
            }

            // Try to match the name value
            if(!strcmp(paramName, (const char *)"server"))
            {   // Read the server name
                postEmail.ptrParam = postEmail.serverName;
                postEmail.paramSize = sizeof(postEmail.serverName) - 1;
            }
            else if(!strcmp(paramName, (const char *)"user"))
            {   // Read the user name
                postEmail.ptrParam = postEmail.username;
                postEmail.paramSize = sizeof(postEmail.username) - 1;
            }
            else if(!strcmp(paramName, (const char *)"pass"))
            {   // Read the password
                postEmail.ptrParam = postEmail.password;
                postEmail.paramSize = sizeof(postEmail.password) - 1;
            }
            else if(!strcmp(paramName, (const char *)"to"))
            {   // Read the To string
                postEmail.ptrParam = postEmail.mailTo;
                postEmail.paramSize = sizeof(postEmail.mailTo) - 1;
            }
            else if(!strcmp(paramName, (const char *)"port"))
            {   // Read the server port
                postEmail.ptrParam = postEmail.serverPort;
                postEmail.paramSize = sizeof(postEmail.serverPort) - 1;
            }
            else if(!strcmp(paramName, (const char *)"msg"))
            {   // Read the server port
                postEmail.ptrParam = postEmail.mailBody;
                postEmail.paramSize = sizeof(postEmail.mailBody) - 1;
                postEmail.mailParamsDone = true;
            }
            else
            {   // unknown parameter
                postEmail.ptrParam = 0;
                postEmail.paramSize = 0;
            }

            // read the parameter now
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_READ_PARAM_VALUE);
            return TCPIP_HTTP_NET_IO_RES_WAITING;


        case SM_EMAIL_READ_PARAM_VALUE:
            // Search for a parameter value in POST data
            if(TCPIP_HTTP_NET_ConnectionPostValueRead(connHandle, (uint8_t*)postEmail.ptrParam, postEmail.paramSize) == TCPIP_HTTP_NET_READ_INCOMPLETE)
                return TCPIP_HTTP_NET_IO_RES_NEED_DATA;

            // end parameter properly
            postEmail.ptrParam[postEmail.paramSize] = 0;

            // check if we're done with the parameters
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, postEmail.mailParamsDone == true ? SM_EMAIL_SEND_MESSAGE : SM_EMAIL_READ_PARAM_NAME);
            return TCPIP_HTTP_NET_IO_RES_WAITING;

        case SM_EMAIL_SEND_MESSAGE:
            // prepare the message attachment
            // output the system status as a CSV file.
            // Write the header and button strings
            { // avoid volatile evaluation warnings by using a temp variable
                int xtch1, xtch2, xtch3;
                xtch1 = (int)APP_SWITCH_1StateGet() + '0';
                xtch2 = (int)APP_SWITCH_2StateGet() + '0';
                xtch3 = (int)APP_SWITCH_3StateGet() + '0';
                postEmail.attachLen = sprintf(postEmail.mailAttachment, "SYSTEM STATUS\r\nButtons:,%c,%c,%c\r\n", xtch1, xtch2, xtch3 );
                // Write the header and button strings
                xtch1 = (int)APP_LED_1StateGet() + '0';
                xtch2 = (int)APP_LED_2StateGet() + '0';
                xtch3 = (int)APP_LED_3StateGet() + '0';
                postEmail.attachLen += sprintf(postEmail.mailAttachment + postEmail.attachLen, "LEDs:,%c,%c,%c\r\n", xtch1, xtch2, xtch3 );
                // add a potentiometer read: a random string
                postEmail.attachLen += sprintf(postEmail.mailAttachment + postEmail.attachLen, "Pot:,%u\r\n", (unsigned int)SYS_RANDOM_PseudoGet());
            }

            // prepare the message itself
            memset(&mySMTPMessage, 0, sizeof(mySMTPMessage));
            mySMTPMessage.body = (const uint8_t*)postEmail.mailBody;
            mySMTPMessage.bodySize = strlen(postEmail.mailBody);
            mySMTPMessage.smtpServer = postEmail.serverName;
            mySMTPMessage.serverPort = (uint16_t)atol(postEmail.serverPort);
            mySMTPMessage.username = postEmail.username;
            mySMTPMessage.password = postEmail.password;
            mySMTPMessage.to = postEmail.mailTo;
            mySMTPMessage.from = "\"SMTP Service\" <mchpboard@picsaregood.com>";
            mySMTPMessage.subject = "Microchip TCP/IP Stack Status Update";

            // set the buffer attachment
            postEmail.attachBuffer.attachType = TCPIP_SMTPC_ATTACH_TYPE_TEXT;
            postEmail.attachBuffer.attachEncode = TCPIP_SMTPC_ENCODE_TYPE_7BIT;
            postEmail.attachBuffer.attachName = "status.csv";
            postEmail.attachBuffer.attachBuffer = (const uint8_t*)postEmail.mailAttachment;
            postEmail.attachBuffer.attachSize = postEmail.attachLen;
            mySMTPMessage.attachBuffers = &postEmail.attachBuffer;
            mySMTPMessage.nBuffers = 1;
            // set the notification function
            mySMTPMessage.messageCallback = postMailCallback;
            
            postMailHandle = TCPIP_SMTPC_MailMessage(&mySMTPMessage, &postEmail.mailRes);
            if(postMailHandle == 0)
            {   // failed
                SYS_CONSOLE_PRINT("SMTPC mail: Failed to submit message: %d!\r\n", postEmail.mailRes);
            }
            else
            {
                postEmail.mailRes = TCPIP_SMTPC_RES_PENDING;
                SYS_CONSOLE_MESSAGE("SMTPC mail: Submitted the mail message!\r\n");
            }

            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_WAIT_RESULT);
            return TCPIP_HTTP_NET_IO_RES_WAITING;

        case SM_EMAIL_WAIT_RESULT:
            // Wait for status done
            if(postEmail.mailRes == TCPIP_SMTPC_RES_PENDING)
            {   // not done yet
                return TCPIP_HTTP_NET_IO_RES_WAITING;
            }

            // done
            postMailHandle = 0;

            if(postEmail.mailRes == TCPIP_SMTPC_RES_OK)
            {
                lastSuccess = true;
            }
            else
            {
                lastFailure = true;
            }

            // Redirect to the page
            strcpy((char *)TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle), "/email/index.htm");
            TCPIP_HTTP_NET_ConnectionStatusSet(connHandle, TCPIP_HTTP_NET_STAT_REDIRECT);
            return TCPIP_HTTP_NET_IO_RES_DONE;
    }

    return TCPIP_HTTP_NET_IO_RES_DONE;
}
#endif // (HTTP_APP_USE_EMAIL != 0) 

/****************************************************************************
  Function:
    TCPIP_HTTP_NET_IO_RESULT HTTPPostDDNSConfig(TCPIP_HTTP_NET_CONN_HANDLE connHandle)

  Summary:
    Parsing and collecting http data received from http form

  Description:
    This routine will be excuted every time the Dynamic DNS Client
    configuration form is submitted.  The http data is received
    as a string of the variables seperated by '&' characters in the TCP RX
    buffer.  This data is parsed to read the required configuration values,
    and those values are populated to the global array (DDNSData) reserved
    for this purpose.  As the data is read, DDNSPointers is also populated
    so that the dynamic DNS client can execute with the new parameters.

  Precondition:
    cur HTTP connection is loaded

  Parameters:
    connHandle  - HTTP connection handle

  Return Values:
    TCPIP_HTTP_NET_IO_RES_DONE      -  Finished with procedure
    TCPIP_HTTP_NET_IO_RES_NEED_DATA -  More data needed to continue, call again later
    TCPIP_HTTP_NET_IO_RES_WAITING   -  Waiting for asynchronous process to complete,
                                        call again later
 ****************************************************************************/
#if defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)
static TCPIP_HTTP_NET_IO_RESULT HTTPPostDDNSConfig(TCPIP_HTTP_NET_CONN_HANDLE connHandle)
{
    static uint8_t *ptrDDNS;
    uint8_t *httpDataBuff;
    uint16_t httpBuffSize;
    uint8_t smPost;

    #define SM_DDNS_START           (0u)
    #define SM_DDNS_READ_NAME       (1u)
    #define SM_DDNS_READ_VALUE      (2u)
    #define SM_DDNS_READ_SERVICE    (3u)
    #define SM_DDNS_DONE            (4u)

    #define DDNS_SPACE_REMAINING    (sizeof(DDNSData) - (ptrDDNS - DDNSData))

    httpDataBuff = TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle);
    httpBuffSize = TCPIP_HTTP_NET_ConnectionDataBufferSizeGet(connHandle);
    smPost = TCPIP_HTTP_NET_ConnectionPostSmGet(connHandle);
    switch(smPost)
    {
        // Sets defaults for the system
        case SM_DDNS_START:
            ptrDDNS = DDNSData;
            TCPIP_DDNS_ServiceSet(0);
            DDNSClient.Host.szROM = NULL;
            DDNSClient.Username.szROM = NULL;
            DDNSClient.Password.szROM = NULL;
            DDNSClient.ROMPointers.Host = 0;
            DDNSClient.ROMPointers.Username = 0;
            DDNSClient.ROMPointers.Password = 0;
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, ++smPost);

        // Searches out names and handles them as they arrive
        case SM_DDNS_READ_NAME:
            // If all parameters have been read, end
            if(TCPIP_HTTP_NET_ConnectionByteCountGet(connHandle) == 0u)
            {
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_DDNS_DONE);
                break;
            }

            // Read a name
            if(TCPIP_HTTP_NET_ConnectionPostNameRead(connHandle, httpDataBuff, httpBuffSize) == TCPIP_HTTP_NET_READ_INCOMPLETE)
                return TCPIP_HTTP_NET_IO_RES_NEED_DATA;

            if(!strcmp((char *)httpDataBuff, (const char *)"service"))
            {
                // Reading the service (numeric)
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_DDNS_READ_SERVICE);
                break;
            }
            else if(!strcmp((char *)httpDataBuff, (const char *)"user"))
                DDNSClient.Username.szRAM = ptrDDNS;
            else if(!strcmp((char *)httpDataBuff, (const char *)"pass"))
                DDNSClient.Password.szRAM = ptrDDNS;
            else if(!strcmp((char *)httpDataBuff, (const char *)"host"))
                DDNSClient.Host.szRAM = ptrDDNS;

            // Move to reading the value for user/pass/host
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, ++smPost);

        // Reads in values and assigns them to the DDNS RAM
        case SM_DDNS_READ_VALUE:
            // Read a name
            if(TCPIP_HTTP_NET_ConnectionPostValueRead(connHandle, ptrDDNS, DDNS_SPACE_REMAINING) == TCPIP_HTTP_NET_READ_INCOMPLETE)
                return TCPIP_HTTP_NET_IO_RES_NEED_DATA;

            // Move past the data that was just read
            ptrDDNS += strlen((char *)ptrDDNS);
            if(ptrDDNS < DDNSData + sizeof(DDNSData) - 1)
                ptrDDNS += 1;

            // Return to reading names
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_DDNS_READ_NAME);
            break;

        // Reads in a service ID
        case SM_DDNS_READ_SERVICE:
            // Read the integer id
            if(TCPIP_HTTP_NET_ConnectionPostValueRead(connHandle, httpDataBuff, httpBuffSize) == TCPIP_HTTP_NET_READ_INCOMPLETE)
                return TCPIP_HTTP_NET_IO_RES_NEED_DATA;

            // Convert to a service ID
            TCPIP_DDNS_ServiceSet((uint8_t)atol((char *)httpDataBuff));

            // Return to reading names
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_DDNS_READ_NAME);
            break;

        // Sets up the DDNS client for an update
        case SM_DDNS_DONE:
            // Since user name and password changed, force an update immediately
            TCPIP_DDNS_UpdateForce();

            // Redirect to prevent POST errors
            lastSuccess = true;
            strcpy((char *)httpDataBuff, "/dyndns/index.htm");
            TCPIP_HTTP_NET_ConnectionStatusSet(connHandle, TCPIP_HTTP_NET_STAT_REDIRECT);
            return TCPIP_HTTP_NET_IO_RES_DONE;
    }

    return TCPIP_HTTP_NET_IO_RES_WAITING;     // Assume we're waiting to process more data
}
#endif // #if defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)

#endif // #if defined(TCPIP_HTTP_NET_USE_POST)

/****************************************************************************
  Section:
    Authorization Handlers
 ****************************************************************************/

/*****************************************************************************
  Function:
    uint8_t TCPIP_HTTP_NET_ConnectionFileAuthenticate(TCPIP_HTTP_NET_CONN_HANDLE connHandle, uint8_t *cFile, const TCPIP_HTTP_NET_USER_CALLBACK *pCBack)

  Internal:
    See documentation in the TCP/IP Stack APIs or http_net.h for details.
 ****************************************************************************/
#if defined(TCPIP_HTTP_NET_USE_AUTHENTICATION)
uint8_t TCPIP_HTTP_NET_ConnectionFileAuthenticate(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const char *cFile, const TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    // If the filename begins with the folder "protect", then require auth
    if(memcmp(cFile, "protect", 7) == 0)
        return 0x00;        // Authentication will be needed later

    // If the filename begins with the folder "snmp", then require auth
    if(memcmp(cFile, "snmp", 4) == 0)
        return 0x00;        // Authentication will be needed later

    #if defined(HTTP_MPFS_UPLOAD_REQUIRES_AUTH)
    if(memcmp(cFile, "mpfsupload", 10) == 0)
        return 0x00;
    #endif
    // You can match additional strings here to password protect other files.
    // You could switch this and exclude files from authentication.
    // You could also always return 0x00 to require auth for all files.
    // You can return different values (0x00 to 0x79) to track "realms" for below.

    return 0x80; // No authentication required
}
#endif

/*****************************************************************************
  Function:
    uint8_t TCPIP_HTTP_NET_ConnectionUserAuthenticate(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const char *cUser, const char *cPass, const TCPIP_HTTP_NET_USER_CALLBACK *pCBack)

  Internal:
    See documentation in the TCP/IP Stack APIs or http_net.h for details.
 ****************************************************************************/
#if defined(TCPIP_HTTP_NET_USE_AUTHENTICATION)
uint8_t TCPIP_HTTP_NET_ConnectionUserAuthenticate(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const char *cUser, const char *cPass, const TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    if(strcmp(cUser,(const char *)"admin") == 0
        && strcmp(cPass, (const char *)"microchip") == 0)
        return 0x80;        // We accept this combination

    // You can add additional user/pass combos here.
    // If you return specific "realm" values above, you can base this
    //   decision on what specific file or folder is being accessed.
    // You could return different values (0x80 to 0xff) to indicate
    //   various users or groups, and base future processing decisions
    //   in TCPIP_HTTP_NET_ConnectionGetExecute/Post or HTTPPrint callbacks on this value.

    return 0x00;            // Provided user/pass is invalid
}
#endif

/****************************************************************************
  Section:
    Dynamic Variable Callback Functions
 ****************************************************************************/
TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_hellomsg(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    const uint8_t *ptr;

    ptr = TCPIP_HTTP_NET_ArgGet(TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle), (const uint8_t *)"name");

    if(ptr != NULL)
    {
        size_t nChars;
        HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
        if(pDynBuffer == 0)
        {   // failed to get a buffer; retry
            return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
        }

        nChars = sprintf(pDynBuffer->data, "Hello, %s", ptr);
        TCPIP_HTTP_NET_DynamicWrite(vDcpt, pDynBuffer->data, nChars, true);
    }

    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_builddate(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, __DATE__" "__TIME__, false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_version(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)TCPIP_STACK_VERSION_STR, false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_cookiename(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    const char *ptr;

    ptr = (const char *)TCPIP_HTTP_NET_ArgGet(TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle), (const uint8_t *)"name");

    if(ptr == 0)
        ptr = "not set";

    size_t nChars;
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    nChars = sprintf(pDynBuffer->data, "%s", ptr);
    TCPIP_HTTP_NET_DynamicWrite(vDcpt, pDynBuffer->data, nChars, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_cookiefav(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    const char *ptr;

    ptr = (const char *)TCPIP_HTTP_NET_ArgGet(TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle), (const uint8_t *)"fav");

    if(ptr == 0)
        ptr = "not set";

    size_t nChars;
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }
    nChars = sprintf(pDynBuffer->data, "%s", ptr);
    TCPIP_HTTP_NET_DynamicWrite(vDcpt, pDynBuffer->data, nChars, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_btn(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    // Determine which button
    if(vDcpt->nArgs != 0 && vDcpt->dynArgs->argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
    {
        int nBtn = vDcpt->dynArgs->argInt32;
        switch(nBtn)
        {
            case 0:
                nBtn = APP_SWITCH_1StateGet();
                break;
            case 1:
                nBtn = APP_SWITCH_2StateGet();
                break;
            case 2:
                nBtn = APP_SWITCH_3StateGet();
                break;
            default:
                nBtn = 0;
        }

        // Print the output
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (nBtn ? "up" : "dn"), false);
    }
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_led(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    // Determine which LED
    if(vDcpt->nArgs != 0 && vDcpt->dynArgs->argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
    {
        int nLed = vDcpt->dynArgs->argInt32;
        switch(nLed)
        {
            case 0:
                nLed = APP_LED_1StateGet();
                break;

            case 1:
                nLed = APP_LED_2StateGet();
                break;

            case 2:
                nLed = APP_LED_3StateGet();
                break;

            default:
                nLed = 0;
        }

        // Print the output
        const char *ledMsg = nLed ? "1": "0";

        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, ledMsg, false);
    }

    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_ledSelected(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    // Determine which LED to check
    if(vDcpt->nArgs >= 2 && vDcpt->dynArgs->argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32 && (vDcpt->dynArgs + 1)->argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
    {
        int nLed = vDcpt->dynArgs->argInt32;
        int state = (vDcpt->dynArgs + 1)->argInt32;

        switch(nLed)
        {
            case 0:
                nLed = APP_LED_1StateGet();
                break;
            case 1:
                nLed = APP_LED_2StateGet();
                break;
            case 2:
                nLed = APP_LED_3StateGet();
                break;
            default:
                nLed = 0;
        }

        // Print output if true and ON or if false and OFF
        if((state && nLed) || (!state && !nLed))
            TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "SELECTED", false);
    }

    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_pot(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    uint16_t RandVal;
    size_t nChars;

    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    RandVal = (uint16_t)SYS_RANDOM_PseudoGet();
    nChars = sprintf(pDynBuffer->data, "%d", RandVal);
    TCPIP_HTTP_NET_DynamicWrite(vDcpt, pDynBuffer->data, nChars, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_status_ok(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    const char *statMsg = lastSuccess ? "block" : "none";
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, statMsg, false);
    lastSuccess = false;
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_status_fail(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    const char *statMsg = lastFailure ? "block" : "none";
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, statMsg, false);
    lastFailure = false;
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_uploadedmd5(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
#if defined(HTTP_APP_USE_MD5)
    char *pMd5;
    uint8_t i;
    uint8_t *httpDataBuff;
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer;

    // Check for flag set in HTTPPostMD5
    if(TCPIP_HTTP_NET_ConnectionPostSmGet(connHandle) != SM_MD5_POST_COMPLETE)
#endif
    {// No file uploaded, so just return
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "<b>Upload a File</b>", false);
        return TCPIP_HTTP_DYN_PRINT_RES_DONE;
    }

#if defined(HTTP_APP_USE_MD5)
    if(HTTP_APP_DYNVAR_BUFFER_SIZE < 80)
    {
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "<b>Not enough room to output a MD5!</b>", false);
        return TCPIP_HTTP_DYN_PRINT_RES_DONE;
    }

    pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    pMd5 = pDynBuffer->data;
    strcpy(pMd5, "<b>Uploaded File's MD5 was:</b><br />");
    httpDataBuff = TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle);

    // Convert the md5 sum
    pMd5 += strlen(pMd5);
    for(i = 0; i < 16u; ++i)
    {
        *pMd5++ = btohexa_high(httpDataBuff[i]);
        *pMd5++ = btohexa_low(httpDataBuff[i]);

        if((i & 0x03) == 3u)
            *pMd5++ = ' ';
    }
    *pMd5 = 0;

    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, pDynBuffer->data, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
#endif
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_hostname(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    TCPIP_NET_HANDLE hNet;
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer;
    const char *nbnsName;

    hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    nbnsName = TCPIP_STACK_NetBIOSName(hNet);

    if(nbnsName == 0)
    {
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "Failed to get a Host name", false);
    }
    else
    {
        pDynBuffer = HTTP_APP_GetDynamicBuffer();
        if(pDynBuffer == 0)
        {   // failed to get a buffer; retry
            return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
        }
        strncpy(pDynBuffer->data, nbnsName, HTTP_APP_DYNVAR_BUFFER_SIZE);
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, pDynBuffer->data, true);
    }

    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_dhcpchecked(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{

    TCPIP_NET_HANDLE hNet;

    hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);

    if(TCPIP_DHCP_IsEnabled(hNet))
    {
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "checked", false);
    }
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_ip(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    IPV4_ADDR ipAddress;
    char *ipAddStr;
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    ipAddStr = pDynBuffer->data;
    TCPIP_NET_HANDLE hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    ipAddress.Val = TCPIP_STACK_NetAddress(hNet);

    TCPIP_Helper_IPAddressToString(&ipAddress, ipAddStr, HTTP_APP_DYNVAR_BUFFER_SIZE);
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, ipAddStr, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_gw(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    IPV4_ADDR gwAddress;
    char *ipAddStr;
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    ipAddStr = pDynBuffer->data;
    TCPIP_NET_HANDLE hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    gwAddress.Val = TCPIP_STACK_NetAddressGateway(hNet);
    TCPIP_Helper_IPAddressToString(&gwAddress, ipAddStr, HTTP_APP_DYNVAR_BUFFER_SIZE);
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, ipAddStr, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_subnet(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    IPV4_ADDR ipMask;
    char *ipAddStr;
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    ipAddStr = pDynBuffer->data;
    TCPIP_NET_HANDLE hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    ipMask.Val = TCPIP_STACK_NetMask(hNet);
    TCPIP_Helper_IPAddressToString(&ipMask, ipAddStr, HTTP_APP_DYNVAR_BUFFER_SIZE);
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, ipAddStr, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_dns1(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    IPV4_ADDR priDnsAddr;
    char *ipAddStr;
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    ipAddStr = pDynBuffer->data;
    TCPIP_NET_HANDLE hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    priDnsAddr.Val = TCPIP_STACK_NetAddressDnsPrimary(hNet);
    TCPIP_Helper_IPAddressToString(&priDnsAddr, ipAddStr, HTTP_APP_DYNVAR_BUFFER_SIZE);
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, ipAddStr, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_dns2(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    IPV4_ADDR secondDnsAddr;
    char *ipAddStr;
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    ipAddStr = pDynBuffer->data;

    TCPIP_NET_HANDLE hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    secondDnsAddr.Val = TCPIP_STACK_NetAddressDnsSecond(hNet);
    TCPIP_Helper_IPAddressToString(&secondDnsAddr, ipAddStr, HTTP_APP_DYNVAR_BUFFER_SIZE);
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, ipAddStr, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_mac(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    TCPIP_NET_HANDLE hNet;
    const TCPIP_MAC_ADDR *pMacAdd;
    char macAddStr[20];
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer;

    hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    pMacAdd = (const TCPIP_MAC_ADDR*)TCPIP_STACK_NetAddressMac(hNet);
    if(pMacAdd)
    {
        TCPIP_Helper_MACAddressToString(pMacAdd, macAddStr, sizeof(macAddStr));
        pDynBuffer = HTTP_APP_GetDynamicBuffer();
        if(pDynBuffer == 0)
        {   // failed to get a buffer; retry
            return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
        }
        strncpy(pDynBuffer->data, macAddStr, HTTP_APP_DYNVAR_BUFFER_SIZE);
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, pDynBuffer->data, true);
    }
    else
    {
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "Failed to get a MAC address", false);
    }

    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_ddns_user(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
#if defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)
    if(DDNSClient.ROMPointers.Username || !DDNSClient.Username.szRAM)
    {
        return TCPIP_HTTP_DYN_PRINT_RES_DONE;
    }

    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }
    strncpy(pDynBuffer->data, (char *)DDNSClient.Username.szRAM, HTTP_APP_DYNVAR_BUFFER_SIZE);
    TCPIP_HTTP_NET_DynamicWriteString(connHandle, pDynBuffer->data, true);
#endif
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_ddns_pass(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
#if defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)
    if(DDNSClient.ROMPointers.Password || !DDNSClient.Password.szRAM)
    {
        return TCPIP_HTTP_DYN_PRINT_RES_DONE;
    }

    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }
    strncpy(pDynBuffer->data, (char *)DDNSClient.Password.szRAM, HTTP_APP_DYNVAR_BUFFER_SIZE);
    TCPIP_HTTP_NET_DynamicWriteString(connHandle, pDynBuffer->data, true);
#endif
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_ddns_host(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
#if defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)
    if(DDNSClient.ROMPointers.Host || !DDNSClient.Host.szRAM)
    {
        return TCPIP_HTTP_DYN_PRINT_RES_DONE;
    }

    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }
    strncpy(pDynBuffer->data, (char *)DDNSClient.Host.szRAM, HTTP_APP_DYNVAR_BUFFER_SIZE);
    TCPIP_HTTP_NET_DynamicWriteString(connHandle, pDynBuffer->data, true);
#endif
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_ddns_service(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
#if defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)
    if(DDNSClient.ROMPointers.UpdateServer && DDNSClient.UpdateServer.szROM)
    {
        if(vDcpt->nArgs != 0 && vDcpt->dynArgs->argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
        {
            uint16_t nHost = vDcpt->dynArgs->argInt32;

            if((const char *)DDNSClient.UpdateServer.szROM == ddnsServiceHosts[nHost])
            {
                TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "selected", false);
            }
        }
    }
#endif
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_ddns_status(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    const char *ddnsMsg;

#if defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)
    DDNS_STATUS s;

    s = TCPIP_DDNS_LastStatusGet();
    if(s == DDNS_STATUS_GOOD || s == DDNS_STATUS_UNCHANGED || s == DDNS_STATUS_NOCHG)
    {
        ddnsMsg = "ok";
    }
    else if(s == DDNS_STATUS_UNKNOWN)
    {
        ddnsMsg = "unk";
    }
    else
    {
        ddnsMsg = "fail";
    }
#else
    ddnsMsg = "fail";
#endif

    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, ddnsMsg, false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_ddns_status_msg(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    const char *ddnsMsg;

#if defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)
    switch(TCPIP_DDNS_LastStatusGet())
    {
        case DDNS_STATUS_GOOD:
        case DDNS_STATUS_NOCHG:
            ddnsMsg = "The last update was successful.";
            break;

        case DDNS_STATUS_UNCHANGED:
            ddnsMsg = "The IP has not changed since the last update.";
            break;

        case DDNS_STATUS_UPDATE_ERROR:
        case DDNS_STATUS_CHECKIP_ERROR:
            ddnsMsg = "Could not communicate with DDNS server.";
            break;

        case DDNS_STATUS_INVALID:
            ddnsMsg = "The current configuration is not valid.";
            break;

        case DDNS_STATUS_UNKNOWN:
            ddnsMsg = "The Dynamic DNS client is pending an update.";
            break;

        default:
            ddnsMsg = "An error occurred during the update.<br />The DDNS Client is suspended.";
            break;
    }
#else
    ddnsMsg = "The Dynamic DNS Client is not enabled.";
#endif

    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, ddnsMsg, false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_reboot(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{

    // This is not so much a print function, but causes the interface to restart
    // when the configuration is changed.  If called via an AJAX call, this
    // will gracefully restart the interface and bring it back online immediately
    if(httpNetData.currNet != 0)
    {   // valid data
        httpNetData.netConfig.interface = httpNetData.ifName;
        httpNetData.netConfig.hostName = httpNetData.nbnsName;
        httpNetData.netConfig.macAddr = httpNetData.ifMacAddr;
        httpNetData.netConfig.ipAddr = httpNetData.ipAddr;
        httpNetData.netConfig.ipMask = httpNetData.ipMask;
        httpNetData.netConfig.gateway = httpNetData.gwIP;
        httpNetData.netConfig.priDNS = httpNetData.dns1IP;
        httpNetData.netConfig.secondDNS = httpNetData.dns2IP;
        httpNetData.netConfig.powerMode = TCPIP_STACK_IF_POWER_FULL;
        // httpNetData.netConfig.startFlags should be already set;
        httpNetData.netConfig.pMacObject = TCPIP_STACK_MACObjectGet(httpNetData.currNet);

        TCPIP_STACK_NetDown(httpNetData.currNet);
        TCPIP_STACK_NetUp(httpNetData.currNet, &httpNetData.netConfig);
    }
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_rebootaddr(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{   // This is the expected address of the board upon rebooting
    const char *rebootAddr = (const char *)TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle);

    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }
    strncpy(pDynBuffer->data, rebootAddr, HTTP_APP_DYNVAR_BUFFER_SIZE);
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, pDynBuffer->data, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_snmp_en(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
#if defined(TCPIP_STACK_USE_SNMP_SERVER)
    const char *snmpMsg = "none";
#else
    const char *snmpMsg = "block";
#endif
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, snmpMsg, false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

// SNMP Read communities configuration page
TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_read_comm(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
#if defined(TCPIP_STACK_USE_SNMP_SERVER)
    while(vDcpt->nArgs != 0 && vDcpt->dynArgs->argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
    {
        uint8_t *dest;
        HTTP_APP_DYNVAR_BUFFER *pDynBuffer;
        uint16_t num = vDcpt->dynArgs->argInt32;

        // Ensure no one tries to read illegal memory addresses by specifying
        // illegal num values.
        if(num >= TCPIP_SNMP_MAX_COMMUNITY_SUPPORT)
        {
            break;
        }

        if(HTTP_APP_DYNVAR_BUFFER_SIZE < TCPIP_SNMP_COMMUNITY_MAX_LEN + 1)
        {
            TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "<b>Not enough room to output SNMP info!</b>", false);
            break;
        }

        pDynBuffer = HTTP_APP_GetDynamicBuffer();
        if(pDynBuffer == 0)
        {   // failed to get a buffer; retry
            return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
        }

        dest = (uint8_t *)pDynBuffer->data;
        memset(dest, 0, TCPIP_SNMP_COMMUNITY_MAX_LEN + 1);
        if(TCPIP_SNMP_ReadCommunityGet(num, TCPIP_SNMP_COMMUNITY_MAX_LEN, dest) != true)
        {   // failed; release the buffer
            pDynBuffer->busy = 0;
            break;
        }

        // Send proper string
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)dest, true);

        break;
    }
#endif
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

// SNMP Write communities configuration page
TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_write_comm(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
#if defined(TCPIP_STACK_USE_SNMP_SERVER)
    while(vDcpt->nArgs != 0 && vDcpt->dynArgs->argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
    {
        uint8_t *dest;
        HTTP_APP_DYNVAR_BUFFER *pDynBuffer;
        uint16_t num = vDcpt->dynArgs->argInt32;

        // Ensure no one tries to read illegal memory addresses by specifying
        // illegal num values.
        if(num >= TCPIP_SNMP_MAX_COMMUNITY_SUPPORT)
        {
            break;
        }

        if(HTTP_APP_DYNVAR_BUFFER_SIZE < TCPIP_SNMP_COMMUNITY_MAX_LEN + 1)
        {
            TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "<b>Not enough room to output SNMP info!</b>", false);
            break;
        }

        pDynBuffer = HTTP_APP_GetDynamicBuffer();
        if(pDynBuffer == 0)
        {   // failed to get a buffer; retry
            return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
        }

        dest = (uint8_t *)pDynBuffer->data;
        memset(dest, 0, TCPIP_SNMP_COMMUNITY_MAX_LEN + 1);
        if(TCPIP_SNMP_WriteCommunityGet(num, TCPIP_SNMP_COMMUNITY_MAX_LEN, dest) != true)
        {   // failed; release the buffer
            pDynBuffer->busy = 0;
            break;
        }

        // Send proper string
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)dest, true);

        break;
    }
#endif
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

#endif // #if defined(TCPIP_STACK_USE_HTTP_SERVER)
