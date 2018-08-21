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

#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************


extern void SNMPV2TrapDemo(void);
extern void SNMPTrapDemo(void);

APP_LED_STATE LEDstate = APP_LED_STATE_OFF;
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

void APP_Initialize ( void )
{
    /* Intialize the app state to wait for its initial state. */
    appData.state = APP_MOUNT_DISK;
    

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
    const char          *netName, *netBiosName;
    static IPV4_ADDR    dwLastIP[2] = { {-1}, {-1} };
    IPV4_ADDR           ipAddr;
    int                 i, nNets;
    TCPIP_NET_HANDLE    netH;
    bool                authTrapFlag = false;
    uint32_t	        startTick = 0;

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_MOUNT_DISK:
            if(SYS_FS_Mount(SYS_FS_SD_VOL, LOCAL_WEBSITE_PATH_FS, FAT, 0, NULL) == 0)
            {
                SYS_CONSOLE_PRINT("SYS_Initialize: The %s File System is mounted.\r\n", SYS_FS_FATFS_STRING);
                appData.state = APP_TCPIP_WAIT_INIT;

            }
            else
            {
    //            appData.state = APP_MOUNT_DISK;
            }
            break;

        case APP_TCPIP_WAIT_INIT:
            tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
            if(tcpipStat < 0)
            {   // some error occurred
                SYS_CONSOLE_MESSAGE(" APP: TCP/IP stack initialization failed!\r\n");
                appData.state = APP_TCPIP_ERROR;
            }
            else if(tcpipStat == SYS_STATUS_READY)
            {
                // now that the stack is ready we can check the 
                // available interfaces 
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
#endif  // defined(TCPIP_STACK_USE_NBNS)

                }
                appData.state = APP_TCPIP_TRANSACT;

            }
            break;

        case APP_TCPIP_TRANSACT:
            if(SYS_TMR_TickCountGet() - startTick >= SYS_TMR_TickCounterFrequencyGet()/2ul)
            {
                startTick = SYS_TMR_TickCountGet();

                LEDstate ^= APP_LED_STATE_ON;
                if(LEDstate == 1)
                {
                    APP_LED_1StateSet();
                }
                if(LEDstate == 0)
                {
                    APP_LED_1StateClear();
                }
            }


            // if the IP address of an interface has changed
            // display the new value on the system console
            nNets = TCPIP_STACK_NumberOfNetworksGet();
#if defined(TCPIP_STACK_USE_SNMP_SERVER)
            if(TCPIP_SNMP_IsTrapEnabled())
            {
            // User should use one of the following SNMP demo
            // This routine demonstrates V1 or V2 trap formats with one variable binding.

                //SNMPTrapDemo(); //This function sends the both SNMP trap version 1 and 2 type of notifications

                if(TCPIP_SNMPV3_TrapTypeGet() || TCPIP_SNMP_TRAPTypeGet())
                {
                    //This routine provides V2 format notifications with multiple (3) variable bindings
                    //User should modify this routine to send v2 trap format notifications with the required varbinds.
                    SNMPV2TrapDemo(); //This function sends the SNMP trap version 2 type of notifications
                }
                else
                {
                    SNMPTrapDemo(); //This function sends the both SNMP trap version1 and 2 type of notifications
                }

        /*
             TCPIP_SNMP_SendFailureTrap() is used to send trap notification to previously configured ip address if trap notification is enabled.
             There are different trap notification code. The current implementation sends trap for authentication failure (4).
             PreCondition: If application defined event occurs to send the trap. Declare a notification flag and update as the event occurs.
             Uncomment the below function if the application requires.

         * */
                TCPIP_SNMP_AuthTrapFlagGet(&authTrapFlag);
                if(authTrapFlag)
                {
                    TCPIP_SNMP_SendFailureTrap();
                }
            }
#endif
            for (i = 0; i < nNets; i++)
            {
                netH = TCPIP_STACK_IndexToNet(i);
                if(!TCPIP_STACK_NetIsReady(netH))
                {
                    return;    // interface not ready yet!
                }
                ipAddr.Val = TCPIP_STACK_NetAddress(netH);
                if(dwLastIP[i].Val != ipAddr.Val)
                {
                    dwLastIP[i].Val = ipAddr.Val;

                    SYS_CONSOLE_MESSAGE(TCPIP_STACK_NetNameGet(netH));
                    SYS_CONSOLE_MESSAGE(" IP Address: ");
                    SYS_CONSOLE_PRINT("%d.%d.%d.%d \r\n", ipAddr.v[0], ipAddr.v[1], ipAddr.v[2], ipAddr.v[3]);
                }
            }
            SYS_CMD_READY_TO_READ();
            break;

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
