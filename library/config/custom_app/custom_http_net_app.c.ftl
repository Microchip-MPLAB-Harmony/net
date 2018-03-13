/*******************************************************************************
  Application to Demo HTTP NET Server

  Summary:
    Support for HTTP NET module in Microchip TCP/IP Stack

  Description:
    -Implements the application
 *******************************************************************************/

/*******************************************************************************
File Name: custom_http_net_app.c
Copyright (C) 2012 released Microchip Technology Inc.  All rights
reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
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

<#if CONFIG_DRV_WIFI_HTTP_CUSTOM_TEMPLATE!"niyas" == "Easy Configuration Demo">
#include <ctype.h>
</#if>
#include "system_config.h"
#include "system_definitions.h"

#if defined(TCPIP_STACK_USE_HTTP_NET_SERVER)

#include "crypto/crypto.h"
#include "net/pres/net_pres_socketapi.h"
#include "system/random/sys_random.h"
#include "system/tmr/sys_tmr.h"
#include "tcpip/tcpip.h"
<#if CONFIG_DRV_WIFI_HTTP_CUSTOM_TEMPLATE!"niyas" == "Easy Configuration Demo">
#include "tcpip/src/tcpip_types.h"
</#if>
#include "tcpip/src/common/helpers.h"

/****************************************************************************
  Section:
    Definitions
 ****************************************************************************/
// Use the web page in the Demo App (~2.5kb ROM, ~0b RAM)
#define HTTP_APP_USE_RECONFIG

// Use the MD5 Demo web page (~5kb ROM, ~160b RAM)
#define HTTP_APP_USE_MD5

// Use the e-mail demo web page
#if defined(TCPIP_STACK_USE_SMTP_CLIENT)
#define HTTP_APP_USE_EMAIL
#endif

<#if CONFIG_DRV_WIFI_HTTP_CUSTOM_TEMPLATE!"niyas" == "Easy Configuration Demo">
#if defined(TCPIP_IF_MRF24WN) || defined(TCPIP_IF_WINC1500) || defined(TCPIP_IF_WILC1000)
#define HTTP_APP_USE_WIFI
#endif

#define HTTP_APP_REDIRECTION_DELAY_TIME (1ul) /* second */

</#if>
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
    #if defined(HTTP_APP_USE_EMAIL) || defined(TCPIP_STACK_USE_SMTP_CLIENT)
        static TCPIP_HTTP_NET_IO_RESULT HTTPPostEmail(TCPIP_HTTP_NET_CONN_HANDLE connHandle);
    #endif
    #if defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)
        static TCPIP_HTTP_NET_IO_RESULT HTTPPostDDNSConfig(TCPIP_HTTP_NET_CONN_HANDLE connHandle);
    #endif
<#if CONFIG_DRV_WIFI_HTTP_CUSTOM_TEMPLATE!"niyas" == "Easy Configuration Demo">
    #if defined(HTTP_APP_USE_WIFI)
        static TCPIP_HTTP_NET_IO_RESULT HTTPPostWIFIConfig(TCPIP_HTTP_NET_CONN_HANDLE connHandle);
    #endif
</#if>
#endif

<#if CONFIG_DRV_WIFI_HTTP_CUSTOM_TEMPLATE!"niyas" == "Easy Configuration Demo">
extern bool g_wifi_redirection_signal;
extern WF_CONFIG g_wifi_cfg;
extern WF_DEVICE_INFO g_wifi_deviceInfo;
extern WF_REDIRECTION_CONFIG g_wifi_redirectionConfig;
extern WF_SCAN_CONTEXT g_wifi_scanContext;

static const WF_SCAN_RESULT *sp_scanResult;
static bool s_scanResultIsValid = false;
static IWPRIV_GET_PARAM s_httpapp_get_param;
static IWPRIV_SET_PARAM s_httpapp_set_param;
</#if>

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

// Stick status message variable.  See lastSuccess for details.
static bool lastFailure = false;
<#-- niyas CONFIG_TCPIP_HTTP_NET_DYNVAR_PROCESS -->
<#if TCPIP_HTTP_NET_DYNVAR_PROCESS == true>
// Number of buffers to be used by the app for dynamic variable callbacks
#define HTTP_APP_DYNVAR_BUFFERS_NO      4

// size of an allocated dynamic variable call back buffer
#define HTTP_APP_DYNVAR_BUFFER_SIZE     100

typedef struct
{
    uint16_t    busy;           // buffer is currently in process
    uint16_t    bufferSize;     // size of the associated buffer
    char        data[HTTP_APP_DYNVAR_BUFFER_SIZE];  // buffer for writing the dynamic variable callback content
}HTTP_APP_DYNVAR_BUFFER;

static HTTP_APP_DYNVAR_BUFFER httpDynVarBuffers[HTTP_APP_DYNVAR_BUFFERS_NO];

</#if>
<#if CONFIG_DRV_WIFI_HTTP_CUSTOM_TEMPLATE!"niyas" == "Easy Configuration Demo">
/****************************************************************************
  Section:
    Helper Functions
 ****************************************************************************/

/*******************************************************************************
 * FUNCTION: Helper_HEXCharToBIN
 *
 * RETURNS: binary value associated with ASCII HEX input value
 *
 * PARAMS: hex_char -- ASCII HEX character
 *
 * NOTES: Converts an input ASCII HEX character to its binary value.  Function
 *         does not error check; it assumes only hex characters are passed in.
 *******************************************************************************/
static uint8_t Helper_HEXCharToBIN(uint8_t hex_char)
{
    if ((hex_char >= 'a') && (hex_char <= 'f'))
    {
        return (0x0a + (hex_char - 'a'));
    }
    else if ((hex_char >= 'A') && (hex_char <= 'F'))
    {
        return (0x0a + (hex_char - 'A'));
    }
    else // ((hex_char >= '0') && (hex_char <= '9'))
    {
        return (0x00 + (hex_char - '0'));
    }
}

/*******************************************************************************
 * FUNCTION: Helper_HEXStrToBIN
 *
 * RETURNS: true if conversion successful, else false
 *
 * PARAMS: p_ascii_hex_str -- ASCII HEX string to be converted
 *         p_bin -- binary value if conversion successful
 *
 * NOTES: Converts an input ASCII HEX string to binary value (up to 32-bit value).
 *******************************************************************************/
static bool Helper_HEXStrToBIN(char *p_ascii_hex_str, uint16_t *p_bin)
{
    int8_t i;
    uint32_t multiplier = 1;

    *p_bin = 0;

    // not allowed to have a string of more than 4 nibbles
    if (strlen((char *)p_ascii_hex_str) > 8u)
    {
        return false;
    }

    // first, ensure all characters are a hex digit
    for (i = (uint8_t)strlen((char *)p_ascii_hex_str) - 1; i >= 0 ; --i)
    {
        if (!isxdigit(p_ascii_hex_str[i]))
        {
            return false;
        }
        *p_bin += multiplier * Helper_HEXCharToBIN(p_ascii_hex_str[i]);
        multiplier *= 16;
    }

    return true;
}

static bool Helper_HEXStrToBINInplace(char *p_str, uint8_t expected_binary_size)
{
    char str_buffer[3];
    char *ascii_hex_str_start = p_str;
    uint8_t binary_index = 0;
    uint16_t bin_buffer = 0;

    /* gobble up any hex prefix */
    if (memcmp(ascii_hex_str_start, (const char *)"0x", 2) == 0)
    {
        ascii_hex_str_start += 2;
    }

    if (strlen((char *)ascii_hex_str_start) != (expected_binary_size * 2))
    {
        return false;
    }

    while (binary_index < expected_binary_size)
    {
        memcpy(str_buffer, (const char *)ascii_hex_str_start, 2);
        str_buffer[2] = '\0';

        /* convert the hex string to binary value */
        if (!Helper_HEXStrToBIN(str_buffer, &bin_buffer))
        {
            return false;
        }

        p_str[binary_index++] = (uint8_t)bin_buffer;
        ascii_hex_str_start += 2;
    }

    return true;
}

static bool Helper_WIFI_SecuritySelect(WF_REDIRECTION_CONFIG *cfg, const char *str)
{
    bool ascii_key = false;
    uint8_t key_size = 0;

    switch (cfg->securityMode) {
        case WF_SECURITY_WEP_40:
            key_size = 10; /* Assume hex size. */
            if (strlen(str) == 5) {
                ascii_key = true;
                key_size = 5; /* ASCII key support. */
            }
            cfg->wepKeyIndex = 0; /* Example only uses key index 0. */
            break;
        case WF_SECURITY_WEP_104:
            key_size = 26; /* Assume hex size. */
            if (strlen(str) == 13) {
                ascii_key = true;
                key_size = 13; /* ASCII key support. */
            }
            cfg->wepKeyIndex = 0; /* Example only uses key index 0. */
            break;
#if defined(TCPIP_IF_MRF24WN)
        case WF_SECURITY_WPA_WITH_PASS_PHRASE:
        case WF_SECURITY_WPA2_WITH_PASS_PHRASE:
#endif
        case WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE:
            ascii_key = true;
            key_size = strlen(str);
            /* Valid WPA passphrase has 8 - 63 characters. */
            if ((key_size < 8 ) || (key_size > 63))
                return false;
            break;
        default:
            ascii_key = true;
            break;
    }
    if (strlen(str) != key_size) {
        SYS_CONSOLE_MESSAGE("\r\nIncomplete key received\r\n");
        return false;
    }
    memcpy(cfg->securityKey, (void *)str, key_size);
    cfg->securityKey[key_size] = 0; /* terminate string */
    if (!ascii_key) {
        key_size /= 2;
        if (!Helper_HEXStrToBINInplace((char *)cfg->securityKey, key_size)) {
            SYS_CONSOLE_MESSAGE("\r\nFailed to convert ASCII string (representing HEX digits) to real HEX string!\r\n");
            return false;
        }
    }
    cfg->securityKeyLen = key_size;
    return true;
}

static void Helper_WIFI_KeySave(WF_REDIRECTION_CONFIG *redirectCfg, WF_CONFIG *cfg)
{
    uint8_t key_size =0;

    switch ((uint8_t)redirectCfg->securityMode) {
        case WF_SECURITY_WEP_40:
            key_size = 5;
            break;
        case WF_SECURITY_WEP_104:
            key_size = 13;
            break;
#if defined(TCPIP_IF_MRF24WN)
        case WF_SECURITY_WPA_WITH_PASS_PHRASE:
        case WF_SECURITY_WPA2_WITH_PASS_PHRASE:
#endif
        case WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE:
            key_size = strlen((const char *)(redirectCfg->securityKey)); // ascii so use strlen
            break;
        default:
            break;
    }
    memcpy(cfg->securityKey, redirectCfg->securityKey, key_size);
    cfg->securityKey[strlen((const char *)(redirectCfg->securityKey))] = 0;
    cfg->securityKeyLen = key_size;
}

static bool Helper_WIFI_IsScanNeeded(void)
{
    /*
     * If current network type is SoftAP and there are valid scan
     * results stored in buffer, no need to scan again.
     * Otherwise, initiate a new scan.
     */
    iwpriv_get(NETWORKTYPE_GET, &s_httpapp_get_param);
    return !(s_httpapp_get_param.netType.type == WF_NETWORK_TYPE_SOFT_AP && g_wifi_scanContext.numberOfResults > 0 );
}

// helper triggered by a system timer callback function to set the redirection
// flag which is used in app.c or app_wifi to true
static void HTTP_APP_RedirectionFlagSet(uintptr_t context, uint32_t currTick)
{
    g_wifi_redirection_signal = true;
}

// helper to switch to error.htm webpage
static TCPIP_HTTP_NET_IO_RESULT HTTP_APP_ConfigFailure(TCPIP_HTTP_NET_CONN_HANDLE connHandle, uint8_t *httpDataBuff)
{
    lastFailure = true;
    if (httpDataBuff)
        strcpy((char *)httpDataBuff, "/error.htm");
    TCPIP_HTTP_NET_ConnectionStatusSet(connHandle, TCPIP_HTTP_NET_STAT_REDIRECT);
    return HTTP_IO_DONE;
}

</#if><#-- CONFIG_DRV_WIFI_HTTP_CUSTOM_TEMPLATE == "Easy Configuration Demo" -->
<#-- niyas CONFIG_TCPIP_HTTP_NET_DYNVAR_PROCESS -->
<#if TCPIP_HTTP_NET_DYNVAR_PROCESS == true>
// helper to get one of the application's dynamic buffer that are used in the
// dynamic variables processing
static HTTP_APP_DYNVAR_BUFFER *HTTP_APP_GetDynamicBuffer(void)
{
    int ix;
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer;

    pDynBuffer = httpDynVarBuffers;
    for(ix = 0; ix < sizeof(httpDynVarBuffers)/sizeof(*httpDynVarBuffers); ++ix, pDynBuffer++)
    {
        if(pDynBuffer->busy == 0)
        {
            pDynBuffer->busy = 1;
            return pDynBuffer;
        }
    }
    return 0;
}

</#if>
/****************************************************************************
  Section:
    Application initialization and HTTP registration.
    Here the application registers with the HTTP module the functions
    that will process the HTTP events (dynamic variables, SSI events, Post, Get, etc.).
    Note that without registering the process functions with HTTP, there won't be any web page processing.
    There is no default processing for a web page!

    See http_net.h for details regarding each of these functions.
 ****************************************************************************/
void HTTP_APP_Initialize(void)
{
<#-- niyas CONFIG_TCPIP_HTTP_NET_DYNVAR_PROCESS -->
<#if TCPIP_HTTP_NET_DYNVAR_PROCESS == true>
    int ix;

    for(ix = 0; ix < sizeof(httpDynVarBuffers)/sizeof(*httpDynVarBuffers); ++ix)
    {
        httpDynVarBuffers[ix].busy = 0;
        httpDynVarBuffers[ix].bufferSize = HTTP_APP_DYNVAR_BUFFER_SIZE;
    }
</#if>

    TCPIP_HTTP_NET_USER_CALLBACK appHttpCBack =
    {
        .getExecute = TCPIP_HTTP_NET_ConnectionGetExecute,              // Process the "GET" command
        .postExecute = TCPIP_HTTP_NET_ConnectionPostExecute,            // Process the "POST" command
<#-- niyas CONFIG_TCPIP_HTTP_NET_USE_AUTHENTICATION -->
<#if TCPIP_HTTP_NET_USE_AUTHENTICATION == true>
        .fileAuthenticate = TCPIP_HTTP_NET_ConnectionFileAuthenticate,  // Process the file authentication
        .userAuthenticate = TCPIP_HTTP_NET_ConnectionUserAuthenticate,  // Process the user authentication
<#else>
        .fileAuthenticate = 0,
        .userAuthenticate = 0,
</#if>
<#-- niyas CONFIG_TCPIP_HTTP_NET_DYNVAR_PROCESS -->
<#if TCPIP_HTTP_NET_DYNVAR_PROCESS == true>
        .dynamicPrint = TCPIP_HTTP_NET_DynPrint,                        // Process the dynamic variable callback
        .dynamicAck = TCPIP_HTTP_NET_DynAcknowledge,                    // Acknowledgment function for when the dynamic variable processing is completed
<#else>
        .dynamicPrint = 0,
        .dynamicAck = 0,
</#if>
        .eventReport = TCPIP_HTTP_NET_EventReport,                      // HTTP Event notification callback
<#-- niyas CONFIG_TCPIP_HTTP_NET_SSI_PROCESS -->
<#if TCPIP_HTTP_NET_SSI_PROCESS == true>
        .ssiNotify = TCPIP_HTTP_NET_SSINotification,                    // SSI command calback
<#else>
        .ssiNotify = 0,
</#if>
    };

    TCPIP_HTTP_NET_USER_HANDLE httpH = TCPIP_HTTP_NET_UserHandlerRegister(&appHttpCBack);
    if(httpH == 0)
        SYS_CONSOLE_MESSAGE("APP: Failed to register the HTTP callback!\r\n");
}

/****************************************************************************
  Section:
    Customized HTTP NET Functions
 ****************************************************************************/
<#-- niyas CONFIG_TCPIP_HTTP_NET_DYNVAR_PROCESS -->
<#if TCPIP_HTTP_NET_DYNVAR_PROCESS == true>
// processing the HTTP buffer acknowledgment
void TCPIP_HTTP_NET_DynAcknowledge(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const void *buffer, const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = (HTTP_APP_DYNVAR_BUFFER*)((const uint8_t *)buffer - offsetof(HTTP_APP_DYNVAR_BUFFER, data));

    pDynBuffer->busy = 0;
}

</#if>
// processing the HTTP reported events
void TCPIP_HTTP_NET_EventReport(TCPIP_HTTP_NET_CONN_HANDLE connHandle, TCPIP_HTTP_NET_EVENT_TYPE evType, const void *evInfo, const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    char *evMsg = (char *)evInfo;

    if(evMsg == 0)
    {
        evMsg = "none";
    }
    SYS_CONSOLE_PRINT("HTTP event: %d, info: %s\r\n", evType, evMsg);
}
<#-- niyas CONFIG_TCPIP_HTTP_NET_SSI_PROCESS -->
<#if TCPIP_HTTP_NET_SSI_PROCESS == true>
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

</#if>
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
            BSP_LEDStateSet(APP_LED_3, (*ptr == '1'));
            //LED2_IO = (*ptr == '1');

        ptr = TCPIP_HTTP_NET_ArgGet(httpDataBuff, (const uint8_t *)"led1");
        if(ptr)
            BSP_LEDStateSet(APP_LED_2, (*ptr == '1'));
            //LED1_IO = (*ptr == '1');
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
                BSP_LEDToggle(APP_LED_1);
                //LED0_IO ^= 1;
                break;
            case '1':
                BSP_LEDToggle(APP_LED_2);
                //LED1_IO ^= 1;
                break;
            case '2':
                BSP_LEDToggle(APP_LED_3);
                //LED2_IO ^= 1;
                break;
        }
    }

<#if CONFIG_DRV_WIFI_HTTP_CUSTOM_TEMPLATE!"niyas" == "Easy Configuration Demo">
    else if(!memcmp(filename, "scan.cgi", 8))
    {
        uint8_t bssIdx;
        TCPIP_UINT16_VAL bssIdxStr;

        s_httpapp_get_param.cfg.config = &g_wifi_cfg;
        iwpriv_get(CONFIG_GET, &s_httpapp_get_param);

        ptr = TCPIP_HTTP_NET_ArgGet(httpDataBuff, (const uint8_t *)"scan");
        if (ptr) {
            // scan request
            s_scanResultIsValid = false;

            if (Helper_WIFI_IsScanNeeded()) {
                uint8_t i = 0;
                iwpriv_execute(SCAN_START, NULL);
                do {
                    iwpriv_get(SCANSTATUS_GET, &s_httpapp_get_param);
                } while (s_httpapp_get_param.scan.scanStatus == IWPRIV_SCAN_IN_PROGRESS);
                iwpriv_get(SCANRESULTS_COUNT_GET, &s_httpapp_get_param);
                if (s_httpapp_get_param.scan.numberOfResults > WF_SCAN_RESULTS_BUFFER_SIZE)
                    g_wifi_scanContext.numberOfResults = WF_SCAN_RESULTS_BUFFER_SIZE;
                else
                    g_wifi_scanContext.numberOfResults = s_httpapp_get_param.scan.numberOfResults;
                while (i < g_wifi_scanContext.numberOfResults) {
                    s_httpapp_get_param.scan.index = i;
                    s_httpapp_get_param.scan.result = &(g_wifi_scanContext.results[i]);
                    iwpriv_get(SCANRESULT_GET, &s_httpapp_get_param);
                    ++i;
                }
            }
        }

        ptr = TCPIP_HTTP_NET_ArgGet(httpDataBuff, (const uint8_t *)"getBss");
        if (ptr) {
            // getBss request
            // use the value to get the nth BSS stored in buffer
            s_scanResultIsValid = false;

            bssIdxStr.v[1] = *(ptr);
            bssIdxStr.v[0] = *(ptr + 1);
            bssIdx = hexatob(bssIdxStr.Val);

            if (bssIdx < g_wifi_scanContext.numberOfResults) {
                sp_scanResult = &(g_wifi_scanContext.results[bssIdx]);
                s_scanResultIsValid = true;
            }
        }
    }

</#if>
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

#if defined(TCPIP_STACK_USE_SMTP_CLIENT)
    if(!strcmp((char *)filename, "email/index.htm"))
        return HTTPPostEmail(connHandle);
#endif

#if defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)
    if(!strcmp((char *)filename, "dyndns/index.htm"))
        return HTTPPostDDNSConfig(connHandle);
#endif

<#if CONFIG_DRV_WIFI_HTTP_CUSTOM_TEMPLATE!"niyas" == "Easy Configuration Demo">
#if defined(HTTP_APP_USE_WIFI)
    if(!memcmp(filename, "configure.htm", 13))
        return HTTPPostWIFIConfig(connHandle);
#endif

</#if>
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
    This function sends an e-mail message using the SMTP client and
    optionally encrypts the connection to the SMTP server.  It
    demonstrates the use of the SMTP client, waiting for asynchronous
    processes in an HTTP callback, and how to send e-mail attachments using
    the stack.

    Messages with attachments are sent using multipart/mixed MIME encoding,
    which has three sections.  The first has no headers, and is only to be
    displayed by old clients that cannot interpret the MIME format.  (The
    overwhelming majority of these clients have been obseleted, but the
    so-called "ignored" section is still used.)  The second has a few
    headers to indicate that it is the main body of the message in plain-
    text encoding.  The third section has headers indicating an attached
    file, along with its name and type.  All sections are separated by a
    boundary string, which cannot appear anywhere else in the message.

  Precondition:
    None

  Parameters:
    connHandle  - HTTP connection handle

  Return Values:
    TCPIP_HTTP_NET_IO_RES_DONE - the message has been sent
    TCPIP_HTTP_NET_IO_RES_WAITING - the function is waiting for the SMTP process to complete
    TCPIP_HTTP_NET_IO_RES_NEED_DATA - data needed by this function has not yet arrived
 ****************************************************************************/
#if defined(TCPIP_STACK_USE_SMTP_CLIENT)
static TCPIP_HTTP_NET_IO_RESULT HTTPPostEmail(TCPIP_HTTP_NET_CONN_HANDLE connHandle)
{
    static uint8_t *ptrData;
    static uint8_t *szPort;
    static TCPIP_SMTP_CLIENT_MESSAGE mySMTPClient;
    uint16_t len, rem;
    uint8_t cName[8];
    uint8_t *httpDataBuff;
    uint16_t httpBuffSize;
    char putBuffer[40];
    char *msgPtr;

    #define SM_EMAIL_CLAIM_MODULE               (0u)
    #define SM_EMAIL_READ_PARAM_NAME            (1u)
    #define SM_EMAIL_READ_PARAM_VALUE           (2u)
    #define SM_EMAIL_PUT_IGNORED                (3u)
    #define SM_EMAIL_PUT_BODY                   (4u)
    #define SM_EMAIL_PUT_ATTACHMENT_HEADER      (5u)
    #define SM_EMAIL_PUT_ATTACHMENT_DATA_BTNS   (6u)
    #define SM_EMAIL_PUT_ATTACHMENT_DATA_LEDS   (7u)
    #define SM_EMAIL_PUT_ATTACHMENT_DATA_POT    (8u)
    #define SM_EMAIL_PUT_TERMINATOR             (9u)
    #define SM_EMAIL_FINISHING                  (10u)

    httpDataBuff = TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle);
    httpBuffSize = TCPIP_HTTP_NET_ConnectionDataBufferSizeGet(connHandle);
    switch(TCPIP_HTTP_NET_ConnectionPostSmGet(connHandle))
    {
        case SM_EMAIL_CLAIM_MODULE:
            // Try to claim module
            if(TCPIP_SMTP_UsageBegin())
            {// Module was claimed, so set up static parameters
                memset(&mySMTPClient, 0, sizeof(mySMTPClient));
                mySMTPClient.Subject = "Microchip TCP/IP Stack Status Update";
                mySMTPClient.From = "\"SMTP Service\" <mchpboard@picsaregood.com>";

                // The following two lines indicate to the receiving client that
                // this message has an attachment.  The boundary field *must not*
                // be included anywhere in the content of the message.  In real
                // applications it is typically a long random string.
                mySMTPClient.OtherHeaders = "MIME-version: 1.0\r\nContent-type: multipart/mixed; boundary=\"frontier\"\r\n";

                // Move our state machine forward
                ptrData = httpDataBuff;
                szPort = NULL;
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_READ_PARAM_NAME);
            }
            return TCPIP_HTTP_NET_IO_RES_WAITING;

        case SM_EMAIL_READ_PARAM_NAME:
            // Search for a parameter name in POST data
            if(TCPIP_HTTP_NET_ConnectionPostNameRead(connHandle, cName, sizeof(cName)) == TCPIP_HTTP_NET_READ_INCOMPLETE)
                return TCPIP_HTTP_NET_IO_RES_NEED_DATA;

            // Try to match the name value
            if(!strcmp((char *)cName, (const char *)"server"))
            {// Read the server name
                mySMTPClient.Server = (char *)ptrData;
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_READ_PARAM_VALUE);
            }
            else if(!strcmp((char *)cName, (const char *)"port"))
            {// Read the server port
                szPort = ptrData;
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_READ_PARAM_VALUE);
            }
            else if(!strcmp((char *)cName, (const char *)"user"))
            {// Read the user name
                mySMTPClient.Username = (char *)ptrData;
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_READ_PARAM_VALUE);
            }
            else if(!strcmp((char *)cName, (const char *)"pass"))
            {// Read the password
                mySMTPClient.Password = (char *)ptrData;
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_READ_PARAM_VALUE);
            }
            else if(!strcmp((char *)cName, (const char *)"to"))
            {// Read the To string
                mySMTPClient.To = (char *)ptrData;
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_READ_PARAM_VALUE);
            }
            else if(!strcmp((char *)cName, (const char *)"msg"))
            {// Done with headers, move on to the message
                // Delete paramters that are just null strings (no data from user) or illegal (ex: password without username)
                if(mySMTPClient.Server )
                    if(*mySMTPClient.Server == 0x00u)
                        mySMTPClient.Server = NULL;
                if(mySMTPClient.Username )
                    if(*mySMTPClient.Username == 0x00u)
                        mySMTPClient.Username = NULL;
                if(mySMTPClient.Password)
                    if((*mySMTPClient.Password == 0x00u) || (mySMTPClient.Username == NULL))
                        mySMTPClient.Password = NULL;

                // Decode server port string if it exists
                if(szPort)
                    if(*szPort)
                        mySMTPClient.ServerPort = (uint16_t)atol((char *)szPort);

                // Start sending the message
                TCPIP_SMTP_MailSend(&mySMTPClient);
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_PUT_IGNORED);
                return TCPIP_HTTP_NET_IO_RES_WAITING;
            }
            else
            {// Don't know what we're receiving
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_READ_PARAM_VALUE);
            }

            // No break...continue to try reading the value

        case SM_EMAIL_READ_PARAM_VALUE:
            // Search for a parameter value in POST data
            rem = httpBuffSize - (ptrData - httpDataBuff);
            if(TCPIP_HTTP_NET_ConnectionPostValueRead(connHandle, ptrData, rem) == TCPIP_HTTP_NET_READ_INCOMPLETE)
                return TCPIP_HTTP_NET_IO_RES_NEED_DATA;

            // Move past the data that was just read
            ptrData += strlen((char *)ptrData);
            if(ptrData < httpDataBuff + httpBuffSize - 1)
                ptrData += 1;

            // Try reading the next parameter
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_READ_PARAM_NAME);
            return TCPIP_HTTP_NET_IO_RES_WAITING;

        case SM_EMAIL_PUT_IGNORED:
            // This section puts a message that is ignored by compatible clients.
            // This text will not display unless the receiving client is obselete
            // and does not understand the MIME structure.
            // The "--frontier" indicates the start of a section, then any
            // needed MIME headers follow, then two CRLF pairs, and then
            // the actual content (which will be the body text in the next state).

            // Check to see if a failure occured
            if(!TCPIP_SMTP_IsBusy())
            {
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_FINISHING);
                return TCPIP_HTTP_NET_IO_RES_WAITING;
            }
            // See if we're ready to write data
            msgPtr = "This is a multi-part message in MIME format.\r\n--frontier\r\nContent-type: text/plain\r\n\r\n";
            if(TCPIP_SMTP_IsPutReady() < strlen(msgPtr))
            {
                return TCPIP_HTTP_NET_IO_RES_WAITING;
            }

            // Write the ignored text
            TCPIP_SMTP_StringPut(msgPtr);
            TCPIP_SMTP_Flush();

            // Move to the next state
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_PUT_BODY);

        case SM_EMAIL_PUT_BODY:
            // Write as much body text as is available from the TCP buffer
            // return TCPIP_HTTP_NET_IO_RES_NEED_DATA or TCPIP_HTTP_NET_IO_RES_WAITING
            // On completion, => PUT_ATTACHMENT_HEADER and continue

            // Check to see if a failure occurred
            if(!TCPIP_SMTP_IsBusy())
            {
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_FINISHING);
                return TCPIP_HTTP_NET_IO_RES_WAITING;
            }

            // Loop as long as data remains to be read
            while(TCPIP_HTTP_NET_ConnectionByteCountGet(connHandle))
            {
                // See if space is available to write
                len = TCPIP_SMTP_IsPutReady();
                if(len == 0u)
                {
                    return TCPIP_HTTP_NET_IO_RES_WAITING;
                }

                // See if data is ready to be read
                rem = TCPIP_HTTP_NET_ConnectionReadIsReady(connHandle);
                if(rem == 0u)
                {
                    return TCPIP_HTTP_NET_IO_RES_NEED_DATA;
                }

                // Only write as much as we can handle
                if(len > rem)
                {
                    len = rem;
                }
                if(len > httpBuffSize - 2)
                {
                    len = httpBuffSize - 2;
                }

                // Read the data from HTTP POST buffer and send it to SMTP
                TCPIP_HTTP_NET_ConnectionByteCountDec(connHandle, TCPIP_HTTP_NET_ConnectionRead(connHandle, httpDataBuff, len));
                httpDataBuff[len] = '\0';
                TCPIP_HTTP_NET_URLDecode(httpDataBuff);
                TCPIP_SMTP_StringPut((char *)httpDataBuff);
                TCPIP_SMTP_Flush();
            }

            // We're done with the POST data, so continue
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_PUT_ATTACHMENT_HEADER);

        case SM_EMAIL_PUT_ATTACHMENT_HEADER:
            // This section writes the attachment to the message.
            // This portion generally will not display in the reader, but
            // will be downloadable to the local machine.  Use caution
            // when selecting the content-type and file name, as certain
            // types and extensions are blocked by virus filters.

            // The same structure as the message body is used.
            // Any attachment must not include high-bit ASCII characters or
            // binary data.  If binary data is to be sent, the data should
            // be encoded using Base64 and a MIME header should be added:
            // Content-transfer-encoding: base64

            // Check to see if a failure occurred
            if(!TCPIP_SMTP_IsBusy())
            {
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_FINISHING);
                return TCPIP_HTTP_NET_IO_RES_WAITING;
            }

            // See if we're ready to write data
            msgPtr = "\r\n--frontier\r\nContent-type: text/csv\r\nContent-Disposition: attachment; filename=\"status.csv\"\r\n\r\n";
            if(TCPIP_SMTP_IsPutReady() < strlen(msgPtr))
            {
                return TCPIP_HTTP_NET_IO_RES_WAITING;
            }

            // Write the attachment header
            TCPIP_SMTP_StringPut(msgPtr);
            TCPIP_SMTP_Flush();

            // Move to the next state
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_PUT_ATTACHMENT_DATA_BTNS);

        case SM_EMAIL_PUT_ATTACHMENT_DATA_BTNS:
            // The following states output the system status as a CSV file.

            // Check to see if a failure occurred
            if(!TCPIP_SMTP_IsBusy())
            {
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_FINISHING);
                return TCPIP_HTTP_NET_IO_RES_WAITING;
            }

            // Write the header and button strings
            sprintf(putBuffer, "SYSTEM STATUS\r\nButtons:,%c,%c,%c\r\n", APP_SWITCH_1StateGet() + '0', APP_SWITCH_2StateGet() + '0', APP_SWITCH_3StateGet() + '0');
            // See if we're ready to write data
            if(TCPIP_SMTP_IsPutReady() < strlen(putBuffer))
            {
                return TCPIP_HTTP_NET_IO_RES_WAITING;
            }

            TCPIP_SMTP_StringPut(putBuffer);
            TCPIP_SMTP_Flush();

            // Move to the next state
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_PUT_ATTACHMENT_DATA_LEDS);

        case SM_EMAIL_PUT_ATTACHMENT_DATA_LEDS:
            // Check to see if a failure occurred
            if(!TCPIP_SMTP_IsBusy())
            {
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_FINISHING);
                return TCPIP_HTTP_NET_IO_RES_WAITING;
            }

            // Write the header and button strings
            sprintf(putBuffer, "LEDs:,%c,%c,%c\r\n", BSP_LEDStateGet(APP_LED_1) + '0', BSP_LEDStateGet(APP_LED_2) + '0', BSP_LEDStateGet(APP_LED_3) + '0');
            // See if we're ready to write data
            if(TCPIP_SMTP_IsPutReady() < strlen(putBuffer))
            {
                return TCPIP_HTTP_NET_IO_RES_WAITING;
            }

            TCPIP_SMTP_StringPut(putBuffer);
            TCPIP_SMTP_Flush();

            // Move to the next state
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_PUT_ATTACHMENT_DATA_POT);

        case SM_EMAIL_PUT_ATTACHMENT_DATA_POT:
            // Check to see if a failure occurred
            if(!TCPIP_SMTP_IsBusy())
            {
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_FINISHING);
                return TCPIP_HTTP_NET_IO_RES_WAITING;
            }

            // Display Random Number
            len = (uint16_t)SYS_RANDOM_PseudoGet();

            uitoa(len, (uint8_t *)&httpDataBuff[1]);

            // Write the header and button strings
            sprintf(putBuffer, "Pot:,%s\r\n", (char *)(httpDataBuff + 1));
            // See if we're ready to write data
            if(TCPIP_SMTP_IsPutReady() < strlen(putBuffer))
            {
                return TCPIP_HTTP_NET_IO_RES_WAITING;
            }

            TCPIP_SMTP_StringPut(putBuffer);
            TCPIP_SMTP_Flush();

            // Move to the next state
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_PUT_TERMINATOR);

        case SM_EMAIL_PUT_TERMINATOR:
            // This section finishes the message
            // This consists of two dashes, the boundary, and two more dashes
            // on a single line, followed by a CRLF pair to terminate the message.

            // Check to see if a failure occured
            if(!TCPIP_SMTP_IsBusy())
            {
                TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_FINISHING);
                return TCPIP_HTTP_NET_IO_RES_WAITING;
            }

            // Write the ignored text
            msgPtr = "--frontier--\r\n";
            // See if we're ready to write data
            if(TCPIP_SMTP_IsPutReady() < strlen(msgPtr))
            {
                return TCPIP_HTTP_NET_IO_RES_WAITING;
            }

            // Write the ignored text
            TCPIP_SMTP_StringPut(msgPtr);
            TCPIP_SMTP_PutIsDone();
            TCPIP_SMTP_Flush();

            // Move to the next state
            TCPIP_HTTP_NET_ConnectionPostSmSet(connHandle, SM_EMAIL_FINISHING);

        case SM_EMAIL_FINISHING:
            // Wait for status
            if(!TCPIP_SMTP_IsBusy())
            {
                // Release the module and check success
                // Redirect the user based on the result
                if(TCPIP_SMTP_UsageEnd() == SMTP_SUCCESS)
                    lastSuccess = true;
                else
                    lastFailure = true;

                // Redirect to the page
                strcpy((char *)httpDataBuff, "/email/index.htm");
                TCPIP_HTTP_NET_ConnectionStatusSet(connHandle, TCPIP_HTTP_NET_STAT_REDIRECT);
                return TCPIP_HTTP_NET_IO_RES_DONE;
            }

            return TCPIP_HTTP_NET_IO_RES_WAITING;
    }

    return TCPIP_HTTP_NET_IO_RES_DONE;
}
#endif // #if defined(TCPIP_STACK_USE_SMTP_CLIENT)

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

<#if CONFIG_DRV_WIFI_HTTP_CUSTOM_TEMPLATE!"niyas" == "Easy Configuration Demo">
/*******************************************************************************
  Function:
    static TCPIP_HTTP_NET_IO_RESULT HTTPPostWIFIConfig(TCPIP_HTTP_NET_CONN_HANDLE connHandle)

  Summary:
    Processes the Wi-Fi configuration data

  Description:
    Accepts wireless configuration data from the www site and saves them to a
    structure to be applied by the Wi-Fi module configuration manager.

    The following configurations are possible:
         i) Mode: adhoc or infrastructure
        ii) Security:
               - None
               - WEP 64-bit
               - WEP 128-bit
               - WPA Auto pre-calculated key
               - WPA1 passphrase
               - WPA2 passphrase
               - WPA Auto passphrase
       iii) Key material

    If an error occurs, such as data is invalid they will be redirected to a page
    informing the user of such results.

    NOTE: This code is modified originally from HTTPPostWIFIConfig as distributed
           by Microchip.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    TCPIP_HTTP_NET_IO_RES_DONE - all parameters have been processed
    TCPIP_HTTP_NET_IO_RES_NEED_DATA - data needed by this function has not yet arrived
 *******************************************************************************/
#if defined(HTTP_APP_USE_WIFI)
static TCPIP_HTTP_NET_IO_RESULT HTTPPostWIFIConfig(TCPIP_HTTP_NET_CONN_HANDLE connHandle)
{
    uint8_t *httpDataBuff = 0;
    uint8_t ssidLen;
    uint16_t httpBuffSize;
    uint32_t byteCount;
    TCP_SOCKET sktHTTP;

    byteCount = TCPIP_HTTP_NET_ConnectionByteCountGet(connHandle);
    if (byteCount > TCPIP_HTTP_NET_ConnectionReadBufferSize(connHandle))
        return HTTP_APP_ConfigFailure(connHandle, httpDataBuff);

    // Ensure that all data is waiting to be parsed.  If not, keep waiting for
    // all of it to arrive.
    if (TCPIP_HTTP_NET_ConnectionReadIsReady(connHandle) < byteCount)
        return TCPIP_HTTP_NET_IO_RES_NEED_DATA;

    httpDataBuff = TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle);
    httpBuffSize = TCPIP_HTTP_NET_ConnectionDataBufferSizeGet(connHandle);
    // Read all browser POST data.
    while (TCPIP_HTTP_NET_ConnectionByteCountGet(connHandle))
    {
        // Read a form field name.
        if (TCPIP_HTTP_NET_ConnectionPostNameRead(connHandle, httpDataBuff, 6) != TCPIP_HTTP_NET_READ_OK)
            return HTTP_APP_ConfigFailure(connHandle, httpDataBuff);

        // Read a form field value.
        if (TCPIP_HTTP_NET_ConnectionPostValueRead(connHandle, httpDataBuff + 6, httpBuffSize - 6 - 2) != TCPIP_HTTP_NET_READ_OK)
            return HTTP_APP_ConfigFailure(connHandle, httpDataBuff);

        // Parse the value that was read.
        if (!strcmp((const char *)httpDataBuff, (const char *)"wlan"))
        {
            // Get the network type: Ad hoc or Infrastructure.
            char networkType[6];
            if (strlen((const char *)(httpDataBuff + 6)) > 5) /* Sanity check. */
                return HTTP_APP_ConfigFailure(connHandle, httpDataBuff);

            memcpy(networkType, httpDataBuff + 6, strlen((const char *)(httpDataBuff + 6)));
            networkType[strlen((const char *)(httpDataBuff + 6))] = 0; /* Terminate string. */
            if (!strcmp(networkType, "infra"))
            {
                g_wifi_redirectionConfig.networkType = WF_NETWORK_TYPE_INFRASTRUCTURE;
            }
            else
            {
                // Network type no good. :-(
                SYS_CONSOLE_MESSAGE("\r\nInvalid redirection network type\r\n");
                return HTTP_APP_ConfigFailure(connHandle, httpDataBuff);
            }

            // Save old network type.
            iwpriv_get(NETWORKTYPE_GET, &s_httpapp_get_param);
            g_wifi_redirectionConfig.prevNetworkType = s_httpapp_get_param.netType.type;
        }
        else if (!strcmp((const char *)httpDataBuff, (const char *)"ssid"))
        {
            // Get new SSID and make sure it is valid.
            if (strlen((const char *)(httpDataBuff + 6)) < 33u)
            {
                memcpy(g_wifi_redirectionConfig.ssid, httpDataBuff + 6, strlen((const char *)(httpDataBuff + 6)));
                g_wifi_redirectionConfig.ssid[strlen((const char *)(httpDataBuff + 6))] = 0; /* Terminate string. */

                /* Save current profile SSID for displaying later. */
                s_httpapp_get_param.ssid.ssid = g_wifi_redirectionConfig.prevSSID;
                iwpriv_get(SSID_GET, &s_httpapp_get_param);
                ssidLen = s_httpapp_get_param.ssid.ssidLen;
                g_wifi_redirectionConfig.prevSSID[ssidLen] = 0;
            }
            else
            {
                // Invalid SSID... :-(
                return HTTP_APP_ConfigFailure(connHandle, httpDataBuff);
            }
        }
        else if (!strcmp((const char *)httpDataBuff, (const char *)"sec"))
        {
            char securityMode[7]; // Read security mode.

            if (strlen((const char *)(httpDataBuff + 6)) > 6) /* Sanity check. */
                return HTTP_APP_ConfigFailure(connHandle, httpDataBuff);

            memcpy(securityMode, httpDataBuff + 6, strlen((const char *)(httpDataBuff + 6)));
            securityMode[strlen((const char *)(httpDataBuff + 6))] = 0; /* Terminate string. */

            if (!strcmp(securityMode, "no"))
            {
                g_wifi_redirectionConfig.securityMode = WF_SECURITY_OPEN;
            }
            else if (!strcmp(securityMode, "wep40"))
            {
                g_wifi_redirectionConfig.securityMode = WF_SECURITY_WEP_40;
            }
            else if (!strcmp(securityMode, "wep104"))
            {
                g_wifi_redirectionConfig.securityMode = WF_SECURITY_WEP_104;
            }
            else if (!strcmp(securityMode, "wpa1"))
            {
                if (g_wifi_deviceInfo.deviceType == MRF24WN_MODULE) {
                    g_wifi_redirectionConfig.securityMode = WF_SECURITY_WPA_WITH_PASS_PHRASE;
                } else if (g_wifi_deviceInfo.deviceType == WINC1500_MODULE) {
                    g_wifi_redirectionConfig.securityMode = WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE;
                } else if (g_wifi_deviceInfo.deviceType == WILC1000_MODULE) {
                    g_wifi_redirectionConfig.securityMode = WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE;
                } else {
                    WF_ASSERT(false, "Incorrect Wi-Fi Device Info");
                }
            }
            else if (!strcmp(securityMode, "wpa2"))
            {
                if (g_wifi_deviceInfo.deviceType == MRF24WN_MODULE) {
                    g_wifi_redirectionConfig.securityMode = WF_SECURITY_WPA2_WITH_PASS_PHRASE;
                } else if (g_wifi_deviceInfo.deviceType == WINC1500_MODULE) {
                    g_wifi_redirectionConfig.securityMode = WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE;
                } else if (g_wifi_deviceInfo.deviceType == WILC1000_MODULE) {
                    g_wifi_redirectionConfig.securityMode = WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE;
                } else {
                    WF_ASSERT(false, "Incorrect Wi-Fi Device Info");
                }
            }
            else if (!strcmp(securityMode, "wpa"))
            {
                WF_ASSERT(g_wifi_deviceInfo.deviceType != MRF24WN_MODULE, "Incorrect Wi-Fi Device Info");
                g_wifi_redirectionConfig.securityMode = WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE;
            }
            else
            {
                // Security mode no good. :-(
                SYS_CONSOLE_MESSAGE("\r\nInvalid redirection security mode\r\n\r\n");
                return HTTP_APP_ConfigFailure(connHandle, httpDataBuff);
            }
        }
        else if (!strcmp((const char *)httpDataBuff, (const char *)"key"))
        {
            // Read new key material.
            if (!Helper_WIFI_SecuritySelect(&g_wifi_redirectionConfig, (const char *)(httpDataBuff + 6)))
                return HTTP_APP_ConfigFailure(connHandle, httpDataBuff);
        }
    }

    /* Check if WPA hasn't been selected with Ad hoc, if it has we choke! */
    if ((g_wifi_redirectionConfig.networkType == WF_NETWORK_TYPE_ADHOC) && (
        (g_wifi_redirectionConfig.securityMode == WF_SECURITY_WPA_WITH_PASS_PHRASE) ||
        (g_wifi_redirectionConfig.securityMode == WF_SECURITY_WPA2_WITH_PASS_PHRASE) ||
        (g_wifi_redirectionConfig.securityMode == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE)))
        return HTTP_APP_ConfigFailure(connHandle, httpDataBuff);

    /*
     * All parsing complete!  If we have got to here all data has been validated and
     * We can handle what is necessary to start the reconfigure process of the Wi-Fi device.
     */

    /* Copy Wi-Fi cfg data to be committed to NVM. */
    s_httpapp_get_param.cfg.config = &g_wifi_cfg;
    iwpriv_get(CONFIG_GET, &s_httpapp_get_param);
    strcpy((char *)g_wifi_cfg.ssid, (const char *)g_wifi_redirectionConfig.ssid);
    g_wifi_cfg.ssidLen = strlen((const char *)g_wifi_redirectionConfig.ssid);
    /* Going to set security type. */
    g_wifi_cfg.securityMode = g_wifi_redirectionConfig.securityMode;
    /* Going to save the key, if required. */
    if (g_wifi_redirectionConfig.securityMode != WF_SECURITY_OPEN)
        Helper_WIFI_KeySave(&g_wifi_redirectionConfig, &g_wifi_cfg);
    /* Going to save the network type. */
    g_wifi_cfg.networkType = g_wifi_redirectionConfig.networkType;
    s_httpapp_set_param.cfg.config = &g_wifi_cfg;
    iwpriv_set(CONFIG_SET, &s_httpapp_set_param);

    strcpy((char *)httpDataBuff, "/reconnect.htm");
    TCPIP_HTTP_NET_ConnectionStatusSet(connHandle, TCPIP_HTTP_NET_STAT_REDIRECT);

    /* Set 1s delay before redirection, goal is to display the redirection web page. */
    uint16_t redirection_delay = SYS_TMR_TickCounterFrequencyGet() * HTTP_APP_REDIRECTION_DELAY_TIME;
    SYS_TMR_CallbackSingle(redirection_delay, 0, HTTP_APP_RedirectionFlagSet);

    return TCPIP_HTTP_NET_IO_RES_DONE;
}
#endif // #if defined(HTTP_APP_USE_WIFI)

</#if>
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
<#-- niyas CONFIG_TCPIP_HTTP_NET_DYNVAR_PROCESS -->
<#if TCPIP_HTTP_NET_DYNVAR_PROCESS == true>
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

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_drive(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)SYS_FS_DRIVE, false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_fstype(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)SYS_FS_MPFS_STRING, false);
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
                nLed = BSP_LEDStateGet(APP_LED_1);
                break;

            case 1:
                nLed = BSP_LEDStateGet(APP_LED_2);
                break;

            case 2:
                nLed = BSP_LEDStateGet(APP_LED_3);
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
                nLed = BSP_LEDStateGet(APP_LED_1);
                break;
            case 1:
                nLed = BSP_LEDStateGet(APP_LED_2);
                break;
            case 2:
                nLed = BSP_LEDStateGet(APP_LED_3);
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
    char *pMd5;
    uint8_t i;
    uint8_t *httpDataBuff;
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer;

    // Check for flag set in HTTPPostMD5
#if defined(HTTP_APP_USE_MD5)
    if(TCPIP_HTTP_NET_ConnectionPostSmGet(connHandle) != SM_MD5_POST_COMPLETE)
#endif
    {// No file uploaded, so just return
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "<b>Upload a File</b>", false);
        return TCPIP_HTTP_DYN_PRINT_RES_DONE;
    }

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
<#-- niyas CONFIG_TCPIP_STACK_USE_IPV4 -->
<#if CONFIG_TCPIP_STACK_USE_IPV4!true == true>
    TCPIP_NET_HANDLE hNet;

    hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);

    if(TCPIP_DHCP_IsEnabled(hNet))
    {
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "checked", false);
    }
</#if>
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
<#-- niyas CONFIG_TCPIP_STACK_USE_IPV4 -->
<#if CONFIG_TCPIP_STACK_USE_IPV4!true == true>
    TCPIP_NET_HANDLE hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    ipAddress.Val = TCPIP_STACK_NetAddress(hNet);
<#else>
    ipAddress.Val = 0;
</#if>

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
<#-- niyas CONFIG_TCPIP_STACK_USE_IPV4 -->
<#if CONFIG_TCPIP_STACK_USE_IPV4!true == true>
    TCPIP_NET_HANDLE hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    gwAddress.Val = TCPIP_STACK_NetAddressGateway(hNet);
<#else>
    gwAddress.Val = 0;
</#if>
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
<#-- niyas CONFIG_TCPIP_STACK_USE_IPV4 -->
<#if CONFIG_TCPIP_STACK_USE_IPV4!true == true>
    TCPIP_NET_HANDLE hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    ipMask.Val = TCPIP_STACK_NetMask(hNet);
<#else>
    ipMask.Val = 0;
</#if>
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
<#-- niyas CONFIG_TCPIP_STACK_USE_IPV4 -->
<#if CONFIG_TCPIP_STACK_USE_IPV4!true == true>
    TCPIP_NET_HANDLE hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    priDnsAddr.Val = TCPIP_STACK_NetAddressDnsPrimary(hNet);
<#else>
    priDnsAddr.Val = 0;
</#if>
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
<#-- niyas CONFIG_TCPIP_STACK_USE_IPV4 -->
<#if CONFIG_TCPIP_STACK_USE_IPV4!true == true>
    TCPIP_NET_HANDLE hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    secondDnsAddr.Val = TCPIP_STACK_NetAddressDnsSecond(hNet);
<#else>
    secondDnsAddr.Val = 0;
</#if>
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
<#-- niyas CONFIG_TCPIP_STACK_IF_UP_DOWN_OPERATION -->
<#if CONFIG_TCPIP_STACK_IF_UP_DOWN_OPERATION!false == true>
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
<#else>
    // The interface restart interface functions are not implemented.
    // Do nothing
</#if>
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

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_smtps_en(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "none", false);
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

<#if CONFIG_DRV_WIFI_HTTP_CUSTOM_TEMPLATE!"niyas" == "Easy Configuration Demo">
TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_fwver(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    static bool firstTime = true;
    static uint8_t fwverString[8];

    if (firstTime)
    {
        WF_DEVICE_INFO deviceInfo;

        firstTime = false;
        s_httpapp_get_param.devInfo.data = &deviceInfo;
        iwpriv_get(DEVICEINFO_GET, &s_httpapp_get_param);
        sprintf((char *)fwverString, "%02x%02x", deviceInfo.romVersion, deviceInfo.patchVersion);
    }
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)fwverString, false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_ssid(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    static uint8_t ssidString[33];

    // We don't need to check the connection state as the only way this function
    // is called is from the web server.  If the web server is requesting this,
    // then you can infer that we should be connected to the network.
    s_httpapp_get_param.ssid.ssid = ssidString;
    iwpriv_get(SSID_GET, &s_httpapp_get_param);
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)ssidString, false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_scan(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    uint8_t scanInProgressString[4]; /* TODO: use HTTP_APP_DYNVAR_BUFFER */

    iwpriv_get(SCANSTATUS_GET, &s_httpapp_get_param);
    if (s_httpapp_get_param.scan.scanStatus == IWPRIV_SCAN_IN_PROGRESS)
        uitoa((uint16_t)true, scanInProgressString);
    else
        uitoa((uint16_t)false, scanInProgressString);
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)scanInProgressString, false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_bssCount(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    uint8_t bssCountString[4]; /* TODO: use HTTP_APP_DYNVAR_BUFFER */

    if (Helper_WIFI_IsScanNeeded()) {
        iwpriv_get(SCANRESULTS_COUNT_GET, &s_httpapp_get_param);
        uitoa(s_httpapp_get_param.scan.numberOfResults, bssCountString);
    } else {
        uitoa(g_wifi_scanContext.numberOfResults, bssCountString);
    }
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)bssCountString, false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_valid(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    uint8_t s_scanResultIsValidString[4]; /* TODO: use HTTP_APP_DYNVAR_BUFFER */

    uitoa((uint8_t)s_scanResultIsValid, s_scanResultIsValidString);
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)s_scanResultIsValidString, false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_name(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    if (s_scanResultIsValid)
    {
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)sp_scanResult->ssid, false);
    }
    else
    {
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)"0", false);
    }
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_wlan(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    uint8_t bssTypeString[4]; /* TODO: use HTTP_APP_DYNVAR_BUFFER */

    if (s_scanResultIsValid)
    {
        uitoa(sp_scanResult->bssType, bssTypeString);
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)bssTypeString, false);
    }
    else
    {
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)"0", false);
    }
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_privacy(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    if (s_scanResultIsValid)
    {
        uint8_t secString[4]; /* TODO: use HTTP_APP_DYNVAR_BUFFER */
        uint8_t security = (sp_scanResult->apConfig & 0xd0) >> 4;
        uitoa(security, secString);
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)secString, false);
    }
    else
    {
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)"0", false);
    }
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_strength(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    uint8_t rssi;
    uint8_t strVal;
    uint8_t strString[4]; /* TODO: use HTTP_APP_DYNVAR_BUFFER */

    if (s_scanResultIsValid)
    {
        if (g_wifi_deviceInfo.deviceType == MRF24WN_MODULE)
            rssi = sp_scanResult->rssi;
        else
            rssi = sp_scanResult->rssi + 125;

        if (rssi < 61)
            strVal = 1;
        else if (rssi < 81)
            strVal = 2;
        else if (rssi < 101)
            strVal = 3;
        else
            strVal = 4;

        uitoa(strVal, strString);
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)strString, false);
    }
    else
    {
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)"0", false);
    }
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_prevSSID(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)g_wifi_redirectionConfig.prevSSID, false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_nextSSID(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)g_wifi_redirectionConfig.ssid, false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_prevWLAN(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    if (g_wifi_redirectionConfig.prevNetworkType == WF_NETWORK_TYPE_INFRASTRUCTURE)
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)"Infrastructure (BSS)", false);
    else if (g_wifi_redirectionConfig.prevNetworkType == WF_NETWORK_TYPE_ADHOC)
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)"Ad hoc (IBSS)", false);
    else if (g_wifi_redirectionConfig.prevNetworkType == WF_NETWORK_TYPE_SOFT_AP)
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)"SoftAP (BSS)", false);
    else
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)"Unknown", false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_currWLAN(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    s_httpapp_get_param.config.data = &g_wifi_cfg;
    iwpriv_get(CONFIG_GET, &s_httpapp_get_param);
    if (g_wifi_cfg.networkType == WF_NETWORK_TYPE_INFRASTRUCTURE)
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)"Infrastructure (BSS)", false);
    else if (g_wifi_cfg.networkType == WF_NETWORK_TYPE_ADHOC)
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)"Ad hoc (IBSS)", false);
    else if (g_wifi_cfg.networkType == WF_NETWORK_TYPE_SOFT_AP)
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)"SoftAP (BSS)", false);
    else if (g_wifi_cfg.networkType == WF_NETWORK_TYPE_P2P)
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)"Wi-Fi Direct (P2P)", false);
    else
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)"Unknown", false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_nextWLAN(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    if (g_wifi_redirectionConfig.networkType == WF_NETWORK_TYPE_INFRASTRUCTURE)
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)"Infrastructure (BSS)", false);
    else if (g_wifi_redirectionConfig.networkType == WF_NETWORK_TYPE_ADHOC)
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)"Ad hoc (IBSS)", false);
    else if (g_wifi_redirectionConfig.networkType == WF_NETWORK_TYPE_SOFT_AP)
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)"SoftAP (BSS)", false);
    else
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)"Unknown", false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

</#if><#-- CONFIG_DRV_WIFI_HTTP_CUSTOM_TEMPLATE == "Easy Configuration Demo" -->
</#if><#-- CONFIG_TCPIP_HTTP_NET_DYNVAR_PROCESS == true -->
#endif // #if defined(TCPIP_STACK_USE_HTTP_SERVER)
