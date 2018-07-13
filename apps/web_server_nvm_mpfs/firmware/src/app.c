/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "app.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
// Buffers for USART2
#define UART_BUFFER_SIZE  200
char uartBuf [UART_BUFFER_SIZE + 1]; // UART buffer
// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

APP_LED_STATE LEDstate = APP_LED_STATE_OFF;
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */
int _mon_getc(int canblock)
{
    return 0;
}

void _mon_putc(char c)
{
    
}
void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_MOUNT_DISK;

    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
    appData.appCmdState = APP_TCPIP_WAIT_FOR_COMMAND;
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    SYS_STATUS          tcpipStat;
    TCPIP_NET_HANDLE    netH;
    int                 nNets;
    static IPV4_ADDR    dwLastIP[2] = { {-1}, {-1} };
    IPV4_ADDR           ipAddr;
    int                 i;
    const char          *netName, *netBiosName;
    static uint32_t     startTick = 0;


    switch(appData.state)
    {
        case APP_MOUNT_DISK:
            if(SYS_FS_Mount(SYS_FS_NVM_VOL, LOCAL_WEBSITE_PATH_FS, MPFS2, 0, NULL) == 0)
            {
                SYS_CONSOLE_PRINT("SYS_Initialize: The %s File System is mounted\r\n", SYS_FS_MPFS_STRING);
                appData.state = APP_TCPIP_WAIT_INIT;
            }
            break;

        case APP_TCPIP_WAIT_INIT:
            tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
            if(tcpipStat < 0)
            {   // some error occurred
                SYS_CONSOLE_MESSAGE("APP: TCP/IP stack initialization failed!\r\n");
                appData.state = APP_TCPIP_ERROR;
            }
            else if(tcpipStat == SYS_STATUS_READY)
            {
                // now that the stack is ready we can check the
                // available interfaces and register
                // a Bonjour service

                nNets = TCPIP_STACK_NumberOfNetworksGet();

                for(i = 0; i < nNets; i++)
                {
                    netH = TCPIP_STACK_IndexToNet(i);
                    netName = TCPIP_STACK_NetNameGet(netH);
                    netBiosName = TCPIP_STACK_NetBIOSName(netH);

#if defined(TCPIP_STACK_USE_NBNS)
                    SYS_CONSOLE_PRINT("    Interface %s on host %s - NBNS enabled\r\n", netName, netBiosName);
#else
                    SYS_CONSOLE_PRINT("    Interface %s on host %s - NBNS disabled\r\n", netName, netBiosName);
#endif // defined(TCPIP_STACK_USE_NBNS)

#if defined(TCPIP_STACK_USE_ZEROCONF_MDNS_SD)
                    // base name of the service Must not exceed 16 bytes long
                    // the last digit will be incremented by interface
                    char mDNSServiceName[] = "MyWebServiceNameX ";

                    mDNSServiceName[sizeof(mDNSServiceName) - 2] = '1' + i;
                    TCPIP_MDNS_ServiceRegister(netH
                            , mDNSServiceName                     // name of the service
                            ,"_http._tcp.local"                   // type of the service
                            ,80                                   // TCP or UDP port, at which this service is available
                            ,((const uint8_t *)"path=/index.htm") // TXT info
                            ,1                                    // auto rename the service when if needed
                            ,NULL                                 // no callback function
                            ,NULL);                               // no application context
#endif // defined(TCPIP_STACK_USE_ZEROCONF_MDNS_SD)
                }

                appData.state = APP_TCPIP_TRANSACT;
            }

            break;

        case APP_TCPIP_TRANSACT:
            if(SYS_TMR_TickCountGet() - startTick >= SYS_TMR_TickCounterFrequencyGet()/2ul)
            {
                startTick = SYS_TMR_TickCountGet();
                LEDstate ^= APP_LED_STATE_ON;
                BSP_LEDStateSet(APP_LED_1, LEDstate);
            }

            // if the IP address of an interface has changed
            // display the new value on the system console
            nNets = TCPIP_STACK_NumberOfNetworksGet();

            for(i = 0; i < nNets; i++)
            {
                netH = TCPIP_STACK_IndexToNet(i);
                ipAddr.Val = TCPIP_STACK_NetAddress(netH);
                if(dwLastIP[i].Val != ipAddr.Val)
                {
                    dwLastIP[i].Val = ipAddr.Val;
                    SYS_CONSOLE_PRINT("%s IP Address: %d.%d.%d.%d \r\n",
                            TCPIP_STACK_NetNameGet(netH),
                            ipAddr.v[0], ipAddr.v[1], ipAddr.v[2], ipAddr.v[3]);
                }
            }

            //SYS_CMD_READY_TO_READ();

            break;

        default:
            break;
    }
}

//void APP_Command_Task()
//{
//    switch(appData.appCmdState)
//    {
//        case APP_TCPIP_WAIT_FOR_COMMAND:                                     //2
//            if ( bPromptForNextCommand )
//            {
//                SYS_CONSOLE_MESSAGE("Enter ^C to enter command mode...\r\n");
//                bPromptForNextCommand = false;
//            }
//            if ( U2STAbits.URXDA == 1 ) // Input found
//            {
//                c = U2RXREG;
//                if ( c == '\x3')
//                { // If found a ^C
//                    SYS_CONSOLE_MESSAGE("\r\nCommand:");
//                    appData.adcState =  APP_ADC_STATE_IDLE;  // Turn off ADC captures
//                    pNextChar = command;
//                    appData.state = APP_TCPIP_GET_COMMAND;
//                }
//            }
//            break;
//
//        case APP_TCPIP_GET_COMMAND:                                          //3
//            while ( U2STAbits.URXDA == 1 )
//            {
//                c = U2RXREG;
//                if ( c == '\x3' )
//                { // Extra ^C
//                    // Do nothing
//                }
//                else if ( c == '\r' )
//                { // End of the command
//                    *pNextChar = '\0';
//                  //SYS_CONSOLE_MESSAGE("\r\nEnd Command: ");
//                  //SYS_CONSOLE_MESSAGE(command);
//                    SYS_CONSOLE_MESSAGE("\r\n");
//                    appData.state = APP_TCPIP_PROCESS_COMMAND;
//                }
//                else
//                {
//                    while(U2STAbits.UTXBF == 1)
//                    {
//                        // Wait for transmit buffer to open up
//                    }
//                    if ( c == '\b' || c == '\x7f')
//                    { //backspace or delete
//                        pNextChar--;
//                        U2TXREG = '\b'; // backspace
//                    }
//                    else
//                    {
//                        *pNextChar++ = c;
//                        U2TXREG = c; // Repeat character just typed
//                    }
//                }
//            }
//            bPromptForNextCommand = true;
//            break;
//
//        case APP_TCPIP_PROCESS_COMMAND:                                      //4
//            pCommand = strtok(command,tokenWhiteSpace);
//            if ( pCommand != NULL )
//            {
//                
//            }
//        default:
//            break;
//    }
//}
/* Call Tree:
 * Everywhere -> SYS_CONSOLE_PRINT (= USART2_MSG)
 */
void USART_MSG(const char* str)
{
    uint32_t len = 0;
    unsigned char ch;
    char* lStr = (char*)str;
    ch = *str;
    while((ch != '\0'))
    {
        len++;
        ch = *str++;
    }
    //USART1_Write("\r\n",2);
    USART1_Write((void*)lStr,len);
    //USART1_Write("\r\n",2);
}

/* Call Tree:
 * Everywhere -> SYS_CONSOLE_PRINT (= USART2_PRINT)
 */
int USART_PRINT(const char* format, ...)
{
    size_t len;
    va_list args;
    va_start( args, format );

    len = vsnprintf(uartBuf, UART_BUFFER_SIZE + 1, format, args);

    va_end( args );

    if (len >= UART_BUFFER_SIZE + 1 )
    {// Output truncated because it's too long.
        len = UART_BUFFER_SIZE;
        uartBuf[len] = '\0';  // Add NULL
    }

    if (len > 0)
    {
        USART_MSG(uartBuf);
    }

    return len;
}
/*******************************************************************************
 End of File
 */
