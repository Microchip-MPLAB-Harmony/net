/*******************************************************************************
  Simple Mail Transfer Protocol (SMTP) Client
  Module for Microchip TCP/IP Stack

  Summary:
    Mail Client Library
    
  Description:
    -Provides ability to send Emails
    -Reference: RFC 2821
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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_SMTP_CLIENT

#include "tcpip/src/tcpip_private.h"
#include "net_pres/pres/net_pres_socketapi.h"

#if defined(TCPIP_STACK_USE_SMTP_CLIENT)

#warning "The SMTP module is obsolete. The new SMTPC module should be used!"

// character string to be expanded for SMTP
#define TCPIP_SMTP_EXPAND_STR    "\r\n."

// max number of characters that could be expanded for a TCPIP_SMTP_ArrayPut operation
// The email string "\r\n." is internally expanded to "\r\n.."
#define TCPIP_SMTP_MAX_EXPAND_CHARACTERS    20

// SMTP debug levels
#define TCPIP_SMTP_DEBUG_MASK_BASIC           (0x0001)

// enable SMTP debugging levels
#define TCPIP_SMTP_DEBUG_LEVEL  (0)

/****************************************************************************
  Section:
	SMTP Client Public Variables
  ***************************************************************************/
// The global set of TCPIP_SMTP_CLIENT_MESSAGE.
// Set these parameters after calling TCPIP_SMTP_UsageBegin successfully.
static TCPIP_SMTP_CLIENT_MESSAGE SMTPClient;	

/****************************************************************************
  Section:
	SMTP Client Internal Variables
  ***************************************************************************/
static IPV4_ADDR SMTPServer;						// IP address of the remote SMTP server
static NET_PRES_SKT_HANDLE_T MySocket = INVALID_SOCKET;	// Socket currently in use by the SMTP client
static int  smtpSktTxSize = 0;              // current socket size

// State machine for the CR LF Period replacement
// Used by TCPIP_SMTP_Put to transparently replace "\r\n." with "\r\n.."
static union
{
	uint8_t *Pos;
	enum
	{
		CR_PERIOD_SEEK_CR = 0,		// Idle state, waiting for '\r'
		CR_PERIOD_SEEK_LF,			// "\r" has been written, so check next byte for '\n'
		CR_PERIOD_SEEK_PERIOD,		// "\r\n" has been written, so check next byte for '.'
		CR_PERIOD_NEED_INSERTION	// "\r\n." has been written, so an additional '.'
									//   must be written before the next byte.
	} State;
} CRPeriod;

// State of the transport for the SMTP Client
static enum
{
	TRANSPORT_HOME = 0,			// Idle state
	TRANSPORT_BEGIN,			// Preparing to make connection
	TRANSPORT_NAME_RESOLVE,		// Resolving the SMTP server address
	TRANSPORT_OBTAIN_SOCKET,	// Obtaining a socket for the SMTP connection
	TRANSPORT_SOCKET_OBTAINED,	// SMTP connection successful
	TRANSPORT_CLOSE				// STMP socket is closed
} TransportState = TRANSPORT_HOME;

// Message state machine for the SMTP Client
static enum
{
	SMTP_HOME = 0,				// Idle start state for SMTP client (application is preparing message)
	SMTP_HELO,					// HELO is being sent to server
	SMTP_HELO_ACK,				// Received an ACK for the HELO
	SMTP_AUTH_LOGIN,			// Requesting to log in
	SMTP_AUTH_LOGIN_ACK,		// Log in request accepted
	SMTP_AUTH_USERNAME,			// Sending user name
	SMTP_AUTH_USERNAME_ACK,		// User name accepted
	SMTP_AUTH_PASSWORD,			// Sending password
	SMTP_AUTH_PASSWORD_ACK,		// Password was accepted
	SMTP_MAILFROM,				// Sending inital MAIL FROM command
	SMTP_MAILFROM_ACK,			// MAIL FROM was accepted
	SMTP_RCPTTO_INIT,			// Preparing to send RCPT TO
	SMTP_RCPTTO,				// Sending RCPT TO command
	SMTP_RCPTTO_ACK,			// RCPT TO was accepted
	SMTP_RCPTTO_ISDONE,			// Done sending RCPT TO commands
	SMTP_RCPTTOCC_INIT,			// Preparing to send RCPT TO CC commands
	SMTP_RCPTTOCC,				// Sending RCPT TO CC commands
	SMTP_RCPTTOCC_ACK,			// RCPT TO CC was accepted
	SMTP_RCPTTOCC_ISDONE,		// Done sending RCPT TO CC
	SMTP_RCPTTOBCC_INIT,		// Preparing to send RCPT TO BCC commands
	SMTP_RCPTTOBCC,				// Sending RCPT TO BCC commands
	SMTP_RCPTTOBCC_ACK,			// RCPT TO BCC was accepted
	SMTP_RCPTTOBCC_ISDONE,		// Done sending RCPT TO BCC
	SMTP_DATA,					// Sending DATA command
	SMTP_DATA_ACK,				// DATA command accpted
	SMTP_DATA_HEADER,			// Sending message headers
	SMTP_DATA_BODY_INIT,		// Preparing for message body
	SMTP_DATA_BODY,				// Sending message body
	SMTP_DATA_BODY_ACK,			// Message body accepted
	SMTP_QUIT_INIT,				// Sending QUIT command
	SMTP_QUIT					// QUIT accepted, connection closing
} SMTPState;

// State machine for writing the SMTP message headers
static enum
{
	PUTHEADERS_FROM_INIT = 0,	// Preparing to send From header
	PUTHEADERS_FROM,			// Sending From header
	PUTHEADERS_TO_INIT,			// Preparing to send To header
	PUTHEADERS_TO,				// Sending To header
	PUTHEADERS_CC_INIT,			// Preparing to send CC header
	PUTHEADERS_CC,				// Sending CC header
	PUTHEADERS_SUBJECT_INIT,	// Preparing to send Subject header
	PUTHEADERS_SUBJECT,			// Sending Subject header
	PUTHEADERS_OTHER_INIT,		// Preparing to send additional headers
	PUTHEADERS_OTHER,			// Sending additional headers
	PUTHEADERS_DONE				// Done writing all headers
} PutHeadersState;

// State machine for parsing incoming responses
static enum
{
	RX_BYTE_0 = 0,
	RX_BYTE_1,
	RX_BYTE_2,
	RX_BYTE_3,
	RX_SEEK_CR,
	RX_SEEK_LF
} RXParserState;

// Internal flags used by the SMTP Client
static union
{
	uint8_t Val;
	struct
	{
		unsigned char RXSkipResponse:1;
		unsigned char SMTPInUse:1;
		unsigned char SentSuccessfully:1;
		unsigned char ReadyToStart:1;
		unsigned char ReadyToFinish:1;
		unsigned char ConnectedOnce:1;
		unsigned char isEncrypted:1;
		unsigned char filler:1;
	} bits;
} SMTPFlags = {0x00};
	
// Response code from server when an error exists
static uint16_t         ResponseCode;

static int              smtpInitCount = 0;

static tcpipSignalHandle smtpTimerHandle = 0;

/****************************************************************************
  Section:
	SMTP Client Internal Function Prototypes
  ***************************************************************************/
static const uint8_t *FindEmailAddress(const uint8_t *str, uint16_t *wLen);

static char* FindEmailServer(const TCPIP_SMTP_CLIENT_MESSAGE* pClient, TCPIP_DNS_RESOLVE_TYPE* pDnsType);

static void TCPIP_SMTP_ClientProcess(void);

static void _SMTPSocketRxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param);

#if ((TCPIP_SMTP_DEBUG_LEVEL & TCPIP_SMTP_DEBUG_MASK_BASIC) != 0)
// a debug condition, not really assertion
volatile int _SMTPStayCondLoop = 0;
static void _SMTPDbgCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("SMTP Cond: %s, in line: %d, \r\n", message, lineNo);
        while(_SMTPStayCondLoop != 0);
    }
}

#else
#define _SMTPDbgCond(cond, message, lineNo)
#endif  // (TCPIP_SMTP_DEBUG_LEVEL & TCPIP_SMTP_DEBUG_MASK_BASIC)

/****************************************************************************
  Section:
	SMTP Function Implementations
  ***************************************************************************/


bool TCPIP_SMTP_ClientInitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_SMTP_CLIENT_MODULE_CONFIG* pSmtpConfig)
{
    int maxWriteSize;

    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {  // interface restart
        return true;
    }
    
    // stack init
    if(smtpInitCount == 0)
    {   // first time we run
        // create the SMTP timer
        smtpTimerHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_SMTP_ClientTask, TCPIP_SMTP_TASK_TICK_RATE); 
        if(smtpTimerHandle == 0)
        {   // cannot create the SMTP timer
            return false;
        }
        // calculate the needed socket TX buffer
        maxWriteSize = TCPIP_SMTP_MAX_WRITE_SIZE * 2;
        if(maxWriteSize > 1024)
        {
            smtpSktTxSize = 2048;
        }
        else if(maxWriteSize > 512)
        {
            smtpSktTxSize = 1024;
        }
        else
        {
            smtpSktTxSize = 512;
        }
    }
    
    // Reset per interface state machine and flags to default values
	
    smtpInitCount++;
	return true;
}

#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_SMTP_ClientDeinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down


    if(smtpInitCount > 0)
    {   // we're up and running
        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--smtpInitCount == 0)
            {   // all closed
                // release resources
                if(smtpTimerHandle)
                {
                    _TCPIPStackSignalHandlerDeregister(smtpTimerHandle);
                    smtpTimerHandle = 0;
                }
            }
        }
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)



/*****************************************************************************
  Function:
	bool TCPIP_SMTP_UsageBegin(void)

  Summary:
	Requests control of the SMTP client module.

  Description:
	Call this function before calling any other SMTP Client APIs.  This 
	function obtains a lock on the SMTP Client, which can only be used by
	one stack application at a time.  Once the application is finished
	with the SMTP client, it must call TCPIP_SMTP_UsageEnd to release control
	of the module to any other waiting applications.
	
	This function initializes all the SMTP state machines and variables
	back to their default state.

  Precondition:
	None

  Parameters:
	None

  Return Values:
	true - The application has successfully obtained control of the module
	false - The SMTP module is in use by another application.  Call 
		TCPIP_SMTP_UsageBegin again later, after returning to the main program loop
  ***************************************************************************/
bool TCPIP_SMTP_UsageBegin(void)
{
	if(SMTPFlags.bits.SMTPInUse)
		return false;

	SMTPFlags.Val = 0x00;
	SMTPFlags.bits.SMTPInUse = true;
	TransportState = TRANSPORT_BEGIN;
	RXParserState = RX_BYTE_0;
	SMTPState = SMTP_HOME;
	memset((void*)&SMTPClient, 0x00, sizeof(SMTPClient));
		
	return true;
}

/*****************************************************************************
  Function:
	uint16_t TCPIP_SMTP_UsageEnd(void)

  Summary:
	Releases control of the SMTP client module.

  Description:
	Call this function to release control of the SMTP client module once
	an application is finished using it.  This function releases the lock
	obtained by TCPIP_SMTP_UsageBegin, and frees the SMTP client to be used by 
	another application.

  Precondition:
	TCPIP_SMTP_UsageBegin returned true on a previous call.

  Parameters:
	None

  Return Values:
	SMTP_SUCCESS - A message was successfully sent
	SMTP_RESOLVE_ERROR - The SMTP server could not be resolved
	SMTP_CONNECT_ERROR - The connection to the SMTP server failed or was
		prematurely terminated
	1-199 and 300-399 - The last SMTP server response code
  ***************************************************************************/
uint16_t TCPIP_SMTP_UsageEnd(void)
{
	if(!SMTPFlags.bits.SMTPInUse)
		return 0xFFFF;

	// Release the DNS module, if in use
	
	
	// Release the TCP socket, if in use
	if(MySocket != INVALID_SOCKET)
	{
        
		NET_PRES_SocketClose(MySocket);
		MySocket = INVALID_SOCKET;
	}
	
	// Release the SMTP module
	SMTPFlags.bits.SMTPInUse = false;
	TransportState = TRANSPORT_HOME;

	if(SMTPFlags.bits.SentSuccessfully)
	{
		return 0;
	}
	else
	{
		return ResponseCode;
	}
}

void TCPIP_SMTP_ClientTask(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != 0)
    { // TMO or RX signals occurred
        TCPIP_SMTP_ClientProcess();
    }

}


// send a signal to the SMTP module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
static void _SMTPSocketRxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param)
{
    if(sigType == TCPIP_TCP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}



static void TCPIP_SMTP_ClientProcess(void)
{
    uint8_t			    i;
    uint16_t			w;
    uint8_t			    vBase64Buffer[4];
    static uint32_t	    SMTPTimer;

    static uint8_t		RXBuffer[4];
    static const uint8_t *ROMStrPtr, *ROMStrPtr2;
    static const uint8_t *RAMStrPtr;
    static uint16_t		wAddressLength;
    TCPIP_DNS_RESULT    dnsRes;
    TCPIP_DNS_RESOLVE_TYPE    dnsType;
    bool                isConnected;

    switch(TransportState)
    {
        case TRANSPORT_HOME:
            // TCPIP_SMTP_UsageBegin() is the only function which will kick
            // the state machine into the next state
            break;

        case TRANSPORT_BEGIN:
            // Wait for the user to program all the pointers and then
            // call TCPIP_SMTP_MailSend()
            if(!SMTPFlags.bits.ReadyToStart)
            {
                break;
            }

            SMTPClient.Server = FindEmailServer(&SMTPClient, &dnsType);
            // See if we found a hostname anywhere which we could resolve
            if(!(SMTPClient.Server))
            {
                ResponseCode = SMTP_RESOLVE_ERROR;
                TransportState = TRANSPORT_HOME;
                break;
            }

            // check for a plain IP address
            if(TCPIP_Helper_StringToIPAddress(SMTPClient.Server, &SMTPServer))
            {
                TransportState = TRANSPORT_OBTAIN_SOCKET;
                break;
            }

            // use DNS to resolve the name
            TCPIP_DNS_Resolve(SMTPClient.Server, dnsType);
            SMTPTimer = SYS_TMR_TickCountGet();

            TransportState++;
            break;

        case TRANSPORT_NAME_RESOLVE:
            // Wait for the DNS server to return the requested IP address
            dnsRes = TCPIP_DNS_IsNameResolved((const char*)SMTPClient.Server, &SMTPServer, 0);
            if(dnsRes == TCPIP_DNS_RES_PENDING)
            {
                break;
            }
            if(dnsRes < 0)
            {   // some error occurred
                ResponseCode = SMTP_RESOLVE_ERROR;
                TransportState = TRANSPORT_HOME;
                break;
            }
            
            // TCPIP_DNS_RES_OK
            TransportState++;
            // No need to break here

        case TRANSPORT_OBTAIN_SOCKET:
            // Connect a TCP socket to the remote SMTP server
            // use TLS if requested, else decide by port number
            MySocket = NET_PRES_SocketOpen(0, SMTPClient.UseSSL ? NET_PRES_SKT_ENCRYPTED_STREAM_CLIENT : NET_PRES_SKT_DEFAULT_STREAM_CLIENT, 
                                         IP_ADDRESS_TYPE_IPV4, 
                                         SMTPClient.ServerPort, 
                                         (NET_PRES_ADDRESS*)&SMTPServer.Val,
                                         NULL);

            // Abort operation if no TCP socket could be opened.
            // If this ever happens, you need to update your tcp_config.h
            if(MySocket == INVALID_SOCKET)
                break;

            NET_PRES_SocketSignalHandlerRegister(MySocket, TCPIP_TCP_SIGNAL_RX_DATA, _SMTPSocketRxSignalHandler, 0);
            if(smtpSktTxSize != 0)
            {
                NET_PRES_SocketOptionsSet(MySocket, TCP_OPTION_TX_BUFF, (void*)smtpSktTxSize);
            }

            SMTPFlags.bits.isEncrypted = SMTPClient.UseSSL || TCPIP_Helper_TCPSecurePortGet(SMTPClient.ServerPort);
            TransportState++;
            SMTPTimer = SYS_TMR_TickCountGet();

            // No break; fall into TRANSPORT_SOCKET_OBTAINED
			
        case TRANSPORT_SOCKET_OBTAINED:
            isConnected = (SMTPFlags.bits.isEncrypted == 0) ? NET_PRES_SocketIsConnected(MySocket) : NET_PRES_SocketIsSecure(MySocket);

            if(!isConnected)
            {
                // Don't stick around in the wrong state if the
                // server was connected, but then disconnected us.
                // Also time out if we can't establish the connection
                // to the SMTP server
                if(SMTPFlags.bits.ConnectedOnce || ((SYS_TMR_TickCountGet()-SMTPTimer) > (TCPIP_SMTP_SERVER_REPLY_TIMEOUT * SYS_TMR_TickCounterFrequencyGet())))
                {
                    ResponseCode = SMTP_CONNECT_ERROR;
                    TransportState = TRANSPORT_CLOSE;
                }

                break;
            }
            SMTPFlags.bits.ConnectedOnce = true;

            // See if the server sent us anything
            while(NET_PRES_SocketReadIsReady(MySocket))
            {
                NET_PRES_SocketRead(MySocket, &i, 1);
                switch(RXParserState)
                {
                    case RX_BYTE_0:
                    case RX_BYTE_1:
                    case RX_BYTE_2:
                        RXBuffer[RXParserState] = i;
                        RXParserState++;
                        break;
	
                    case RX_BYTE_3:
                        switch(i)
                        {
                            case ' ':
                                SMTPFlags.bits.RXSkipResponse = false;
                                RXParserState++;
                                break;
                            case '-':
                                SMTPFlags.bits.RXSkipResponse = true;
                                RXParserState++;
                                break;
                            case '\r':
                                RXParserState = RX_SEEK_LF;
                                break;
                        }
                        break;
	
					case RX_SEEK_CR:
                                            if(i == '\r')
                                                    RXParserState++;
                                            break;
	
					case RX_SEEK_LF:
						// If we received the whole command
						if(i == '\n')
						{
							RXParserState = RX_BYTE_0;

							if(!SMTPFlags.bits.RXSkipResponse)
							{
								// The server sent us a response code
								// Null terminate the ASCII reponse code so we can convert it to an integer
								RXBuffer[3] = 0;
								ResponseCode = atoi((char*)RXBuffer);

								// Handle the response
								switch(SMTPState)
								{
									case SMTP_HELO_ACK:
										if(ResponseCode >= 200u && ResponseCode <= 299u)
										{
											if(SMTPClient.Username)
												SMTPState = SMTP_AUTH_LOGIN;
											else
												SMTPState = SMTP_MAILFROM;
										}
										else
											SMTPState = SMTP_QUIT_INIT;
										break;


									case SMTP_AUTH_LOGIN_ACK:
									case SMTP_AUTH_USERNAME_ACK:
										if(ResponseCode == 334u)
											SMTPState++;
										else
											SMTPState = SMTP_QUIT_INIT;
										break;

									case SMTP_AUTH_PASSWORD_ACK:
										if(ResponseCode == 235u)
											SMTPState++;
										else
											SMTPState = SMTP_QUIT_INIT;
										break;

									case SMTP_HOME:
									case SMTP_MAILFROM_ACK:
									case SMTP_RCPTTO_ACK:
									case SMTP_RCPTTOCC_ACK:
									case SMTP_RCPTTOBCC_ACK:
										if(ResponseCode >= 200u && ResponseCode <= 299u)
											SMTPState++;
										else
											SMTPState = SMTP_QUIT_INIT;
										break;
							
									case SMTP_DATA_ACK:
										if(ResponseCode == 354u)
											SMTPState++;
										else
											SMTPState = SMTP_QUIT_INIT;
										break;
							
									case SMTP_DATA_BODY_ACK:
										if(ResponseCode >= 200u && ResponseCode <= 299u)
											SMTPFlags.bits.SentSuccessfully = true;
							
										SMTPState = SMTP_QUIT_INIT;
										break;

									// Default case needed to supress compiler diagnostics
									default:
										break;
								}
							}
						}
						else if(i != '\r')
							RXParserState--;
	
						break;
				}
			}

			// Generate new data in the TX buffer, as needed, if possible
			if(NET_PRES_SocketWriteIsReady(MySocket, 64, 0) < 64)
				break;

            const char * mailStr;
            
			switch(SMTPState)
			{
				case SMTP_HELO:
					if(SMTPClient.Username == NULL)
                        mailStr = "HELO MCHPBOARD\r\n";
					else    
                        mailStr = "EHLO MCHPBOARD\r\n";

                    NET_PRES_SocketWrite(MySocket, mailStr, strlen(mailStr));
                    NET_PRES_SocketFlush(MySocket);
					SMTPState++;
					break;

				case SMTP_AUTH_LOGIN:
					// Note: This state is only entered from SMTP_HELO_ACK if the application 
					// has specified a Username to use (SMTPClient.Username is non-NULL)
                    mailStr = "AUTH LOGIN\r\n";
                    NET_PRES_SocketWrite(MySocket, mailStr, strlen(mailStr));
                    NET_PRES_SocketFlush(MySocket);
					SMTPState++;
					break;

				case SMTP_AUTH_USERNAME:
					// Base 64 encode and transmit the username.
                    RAMStrPtr = (uint8_t*)SMTPClient.Username;
                    w = strlen((char*)RAMStrPtr);

					while(w)
					{
						i = 0;
						while((i < w) && (i < sizeof(vBase64Buffer)*3/4))
						{
                            vBase64Buffer[i] = *RAMStrPtr++;
							i++;
						}
						w -= i;					
						TCPIP_Helper_Base64Encode(vBase64Buffer, i, vBase64Buffer, sizeof(vBase64Buffer));                        
						NET_PRES_SocketWrite(MySocket, vBase64Buffer, sizeof(vBase64Buffer));
					}
                    mailStr = "\r\n";
                    NET_PRES_SocketWrite(MySocket, mailStr, strlen(mailStr));
                    NET_PRES_SocketFlush(MySocket);
					SMTPState++;
					break;

				case SMTP_AUTH_PASSWORD:
					// Base 64 encode and transmit the password
                    RAMStrPtr = (uint8_t*)SMTPClient.Password;
                    w = strlen((char*)RAMStrPtr);

					while(w)
					{
						i = 0;
						while((i < w) && (i < sizeof(vBase64Buffer)*3/4))
						{
                            vBase64Buffer[i] = *RAMStrPtr++;
							i++;
						}
						w -= i;					
						TCPIP_Helper_Base64Encode(vBase64Buffer, i, vBase64Buffer, sizeof(vBase64Buffer));
						NET_PRES_SocketWrite(MySocket, vBase64Buffer, sizeof(vBase64Buffer));
					}
                    mailStr = "\r\n";
                    NET_PRES_SocketWrite(MySocket, mailStr, strlen(mailStr));
                    NET_PRES_SocketFlush(MySocket);
					SMTPState++;
					break;

				case SMTP_MAILFROM:
					// Send MAIL FROM header.  Note that this is for the SMTP server validation, 
					// not what actually will be displayed in the recipients mail client as a 
					// return address.
                    mailStr = "MAIL FROM:<";
                    NET_PRES_SocketWrite(MySocket, mailStr, strlen(mailStr));
                    RAMStrPtr = FindEmailAddress((uint8_t*)SMTPClient.From, &wAddressLength);
                    NET_PRES_SocketWrite(MySocket, RAMStrPtr, wAddressLength);
                    mailStr = ">\r\n";
                    NET_PRES_SocketWrite(MySocket, mailStr, strlen(mailStr));
                    NET_PRES_SocketFlush(MySocket);
					SMTPState++;
					break;

				case SMTP_RCPTTO_INIT:
					// See if there are any (To) recipients to process
					if(SMTPClient.To)
					{
						RAMStrPtr = FindEmailAddress((uint8_t*)SMTPClient.To, &wAddressLength);
						if(wAddressLength)
						{
							SMTPState = SMTP_RCPTTO;
							break;
						}
					}
					
					SMTPState = SMTP_RCPTTOCC_INIT;
					break;

				case SMTP_RCPTTO:
				case SMTP_RCPTTOCC:
				case SMTP_RCPTTOBCC:
                    mailStr = "RCPT TO:<";
                    NET_PRES_SocketWrite(MySocket, mailStr, strlen(mailStr));
                    NET_PRES_SocketWrite(MySocket, RAMStrPtr, wAddressLength);
                    mailStr = ">\r\n";
                    NET_PRES_SocketWrite(MySocket, mailStr, strlen(mailStr));
                    NET_PRES_SocketFlush(MySocket);
					SMTPState++;
					break;

				case SMTP_RCPTTO_ISDONE:
					// See if we have any more (To) recipients to process
					// If we do, we must roll back a couple of states
                    RAMStrPtr = FindEmailAddress(RAMStrPtr+wAddressLength, &wAddressLength);
	
					if(wAddressLength)
					{
						SMTPState = SMTP_RCPTTO;
						break;
					}

					// All done with To field
					SMTPState++;
					//No break

				case SMTP_RCPTTOCC_INIT:
					// See if there are any Carbon Copy (CC) recipients to process
					if(SMTPClient.CC)
					{
						RAMStrPtr = FindEmailAddress((uint8_t*)SMTPClient.CC, &wAddressLength);
						if(wAddressLength)
						{
							SMTPState = SMTP_RCPTTOCC;
							break;
						}
					}
					
					SMTPState = SMTP_RCPTTOBCC_INIT;
					break;

				case SMTP_RCPTTOCC_ISDONE:
					// See if we have any more Carbon Copy (CC) recipients to process
					// If we do, we must roll back a couple of states
                    RAMStrPtr = FindEmailAddress(RAMStrPtr+wAddressLength, &wAddressLength);

					if(wAddressLength)
					{
						SMTPState = SMTP_RCPTTOCC;
						break;
					}

					// All done with CC field
					SMTPState++;
					//No break

				case SMTP_RCPTTOBCC_INIT:
					// See if there are any Blind Carbon Copy (BCC) recipients to process
					if(SMTPClient.BCC)
					{
						RAMStrPtr = FindEmailAddress((uint8_t*)SMTPClient.BCC, &wAddressLength);
						if(wAddressLength)
						{
							SMTPState = SMTP_RCPTTOBCC;
							break;
						}
					}

					// All done with BCC field
					SMTPState = SMTP_DATA;
					break;

				case SMTP_RCPTTOBCC_ISDONE:
					// See if we have any more Blind Carbon Copy (CC) recipients to process
					// If we do, we must roll back a couple of states
                    RAMStrPtr = FindEmailAddress(RAMStrPtr+wAddressLength, &wAddressLength);

					if(wAddressLength)
					{
						SMTPState = SMTP_RCPTTOBCC;
						break;
					}

					// All done with BCC field
					SMTPState++;
					//No break

				case SMTP_DATA:
                    mailStr = "DATA\r\n";
                    NET_PRES_SocketWrite(MySocket, mailStr, strlen(mailStr));
					SMTPState++;
					PutHeadersState = PUTHEADERS_FROM_INIT;
                    NET_PRES_SocketFlush(MySocket);
					break;

				case SMTP_DATA_HEADER:
					while((PutHeadersState != PUTHEADERS_DONE) && (NET_PRES_SocketWriteIsReady(MySocket, 64, 0) > 64))
					{
						switch(PutHeadersState)
						{
							case PUTHEADERS_FROM_INIT:
								if(SMTPClient.From)
								{
									PutHeadersState = PUTHEADERS_FROM;
                                    mailStr = "From: ";
                                    NET_PRES_SocketWrite(MySocket, mailStr, strlen(mailStr));
								}
								else
								{
									PutHeadersState = PUTHEADERS_TO_INIT;
								}
								break;
								
							case PUTHEADERS_FROM:
                                SMTPClient.From += NET_PRES_SocketWrite(MySocket, (uint8_t*)SMTPClient.From, strlen(SMTPClient.From));
                                if(*SMTPClient.From == 0u)
                                    PutHeadersState = PUTHEADERS_TO_INIT;
								break;

							case PUTHEADERS_TO_INIT:
								if(SMTPClient.To)
								{
									PutHeadersState = PUTHEADERS_TO;
                                    mailStr = "\r\nTo: ";
									NET_PRES_SocketWrite(MySocket, mailStr, strlen(mailStr));
								}
								else
								{
									PutHeadersState = PUTHEADERS_CC_INIT;
								}
								break;
								
							case PUTHEADERS_TO:
                                SMTPClient.To += NET_PRES_SocketWrite(MySocket, (uint8_t*)SMTPClient.To, strlen(SMTPClient.To));
                                if(*SMTPClient.To == 0u)
                                    PutHeadersState = PUTHEADERS_CC_INIT;
								break;

							case PUTHEADERS_CC_INIT:
								if(SMTPClient.CC)
								{
									PutHeadersState = PUTHEADERS_CC;
                                    mailStr = "\r\nCC: ";
									NET_PRES_SocketWrite(MySocket, mailStr, strlen(mailStr));
								}
								else
								{
									PutHeadersState = PUTHEADERS_SUBJECT_INIT;
								}
								break;
								
							case PUTHEADERS_CC:
                                SMTPClient.CC += NET_PRES_SocketWrite(MySocket, (uint8_t*)SMTPClient.CC, strlen(SMTPClient.CC));
                                if(*SMTPClient.CC == 0u)
                                    PutHeadersState = PUTHEADERS_SUBJECT_INIT;
								break;

							case PUTHEADERS_SUBJECT_INIT:
								if(SMTPClient.Subject)
								{
									PutHeadersState = PUTHEADERS_SUBJECT;
                                    mailStr = "\r\nSubject: ";
									NET_PRES_SocketWrite(MySocket, mailStr, strlen(mailStr));
								}
								else
								{
									PutHeadersState = PUTHEADERS_OTHER_INIT;
								}
								break;
								
							case PUTHEADERS_SUBJECT:
                                SMTPClient.Subject += NET_PRES_SocketWrite(MySocket, (uint8_t*)SMTPClient.Subject, strlen(SMTPClient.Subject));
                                if(*SMTPClient.Subject == 0u)
                                    PutHeadersState = PUTHEADERS_OTHER_INIT;
								break;

							case PUTHEADERS_OTHER_INIT:
                                mailStr = "\r\n";
								NET_PRES_SocketWrite(MySocket, mailStr, strlen(mailStr));
								if(SMTPClient.OtherHeaders)
								{
									PutHeadersState = PUTHEADERS_OTHER;
								}
								else
								{
									NET_PRES_SocketWrite(MySocket, mailStr, strlen(mailStr));
									PutHeadersState = PUTHEADERS_DONE;
									SMTPState++;
								}
								break;
								
							case PUTHEADERS_OTHER:
                                SMTPClient.OtherHeaders += NET_PRES_SocketWrite(MySocket, (uint8_t*)SMTPClient.OtherHeaders, strlen(SMTPClient.OtherHeaders));
                                if(*SMTPClient.OtherHeaders == 0u)
                                {
                                    mailStr = "\r\n";
                                    NET_PRES_SocketWrite(MySocket, mailStr, strlen(mailStr));
                                    PutHeadersState = PUTHEADERS_DONE;
                                    SMTPState++;
                                }
								break;
							
							// Default case needed to supress compiler diagnostics
							default:
								break;
						}
					}
					NET_PRES_SocketFlush(MySocket);
					break;
		
				case SMTP_DATA_BODY_INIT:
					SMTPState++;
					RAMStrPtr = (uint8_t*)SMTPClient.Body;
					ROMStrPtr2 = (const uint8_t*)"\r\n.\r\n";
					CRPeriod.Pos = NULL;
					if(RAMStrPtr)
						CRPeriod.Pos = (uint8_t*)strstr((char*)RAMStrPtr, (const char*)"\r\n.");
					// No break here
		
				case SMTP_DATA_BODY:
					if(SMTPClient.Body)
					{
						if(*ROMStrPtr2)
						{
							// Put the application data, doing the transparancy replacement of "\r\n." with "\r\n.."
							while(CRPeriod.Pos)
							{
								CRPeriod.Pos += 3;
								RAMStrPtr += NET_PRES_SocketWrite(MySocket, RAMStrPtr, CRPeriod.Pos-RAMStrPtr);
								if(RAMStrPtr == CRPeriod.Pos)
								{
                                    mailStr = ".";
									if(!NET_PRES_SocketWrite(MySocket, mailStr, 1))
									{
										CRPeriod.Pos -= 3;
										break;
									}
								}
								else
								{
									CRPeriod.Pos -= 3;
									break;
								}
								CRPeriod.Pos = (uint8_t*)strstr((char*)RAMStrPtr, (const char*)"\r\n.");
							}
							
							// If we get down here, either all replacements have been made or there is no remaining space in the TCP output buffer
							RAMStrPtr += NET_PRES_SocketWrite(MySocket, RAMStrPtr, strlen((const char*)RAMStrPtr));
							ROMStrPtr2 += NET_PRES_SocketWrite(MySocket, (uint8_t*)ROMStrPtr2, strlen((const char*)ROMStrPtr2));
							NET_PRES_SocketFlush(MySocket);
						}
					}
					else
					{
						if(SMTPFlags.bits.ReadyToFinish)
						{
							if(*ROMStrPtr2)
							{
								ROMStrPtr2 += NET_PRES_SocketWrite(MySocket, (uint8_t*)ROMStrPtr2, strlen((const char*)ROMStrPtr2));
								NET_PRES_SocketFlush(MySocket);
							}
		
						}
					}

					if(*ROMStrPtr2 == 0u)
					{
						SMTPState++;
					}
					break;
		
				case SMTP_QUIT_INIT:
					SMTPState++;
					ROMStrPtr = (const uint8_t*)"QUIT\r\n";
					// No break here

				case SMTP_QUIT:
					if(*ROMStrPtr)
					{
						ROMStrPtr += NET_PRES_SocketWrite(MySocket, (uint8_t*)ROMStrPtr, strlen((const char*)ROMStrPtr));
						NET_PRES_SocketFlush(MySocket);
					}

					if(*ROMStrPtr == 0u)
					{
						TransportState = TRANSPORT_CLOSE;
					}
					break;
				
				// Default case needed to supress compiler diagnostics
				default:
					break;
			}
			break;

		case TRANSPORT_CLOSE:
			// Close the socket so it can be used by other modules
			NET_PRES_SocketClose(MySocket);
			MySocket = INVALID_SOCKET;

			// Go back to doing nothing
			TransportState = TRANSPORT_HOME;
			break;
	}
}

/*****************************************************************************
  Function:
	void TCPIP_SMTP_MailSend(TCPIP_SMTP_CLIENT_MESSAGE* smtpClientMessage)

  Summary:
	Initializes the message sending process.

  Description:
	This function starts the state machine that performs the actual
	transmission of the message.  Call this function after all the fields
	in SMTPClient have been set.

  Precondition:
	TCPIP_SMTP_UsageBegin returned true on a previous call.

  Parameters:
	smtpClientMessage   - pointer to a TCPIP_SMTP_CLIENT_MESSAGE structure that configures
                          the SMTP Client to send an e-mail to

  Returns:
	None

  Note:
    The fileds pointed by the smtpClientMessage have to be non-volatile
    until the SMTP send mail process is completed!
    
  ***************************************************************************/
void TCPIP_SMTP_MailSend(TCPIP_SMTP_CLIENT_MESSAGE* smtpClientMessage)
{
    memcpy(&SMTPClient, smtpClientMessage, sizeof(SMTPClient));	

	SMTPFlags.bits.ReadyToStart = true;
}

/*****************************************************************************
  Function:
	bool TCPIP_SMTP_IsBusy(void)

  Summary:
	Determines if the SMTP client is busy.

  Description:
	Call this function to determine if the SMTP client is busy performing
	background tasks.  This function should be called after any call to 
	TCPIP_SMTP_MailSend, TCPIP_SMTP_PutIsDone to determine if the stack has finished
	performing its internal tasks.  It should also be called prior to any
	call to TCPIP_SMTP_IsPutReady to verify that the SMTP client has not
	prematurely disconnected.  When this function returns false, the next
	call should be to TCPIP_SMTP_UsageEnd to release the module and obtain the
	status code for the operation.

  Precondition:
	TCPIP_SMTP_UsageBegin returned true on a previous call.

  Parameters:
	None

  Return Values:
	true - The SMTP Client is busy with internal tasks or sending an 
		on-the-fly message.
	false - The SMTP Client is terminated and is ready to be released.
  ***************************************************************************/
bool TCPIP_SMTP_IsBusy(void)
{
	return TransportState != TRANSPORT_HOME;
}

/*****************************************************************************
  Function:
	uint16_t TCPIP_SMTP_IsPutReady(void)

  Summary:
	Determines how much data can be written to the SMTP client.

  Description:
	Use this function to determine how much data can be written to the SMTP 
	client when generating an on-the-fly message.

  Precondition:
	TCPIP_SMTP_UsageBegin returned true on a previous call, and an on-the-fly 
	message is being generated.  This requires that TCPIP_SMTP_MailSend was called
	with SMTPClient.Body set to NULL.

  Parameters:
	None

  Returns:
	The number of free bytes the SMTP TX FIFO.

  Remarks:
	This function should only be called externally when the SMTP client is
	generating an on-the-fly message.  (That is, TCPIP_SMTP_MailSend was called
	with SMTPClient.Body set to NULL.)
  ***************************************************************************/
uint16_t TCPIP_SMTP_IsPutReady(void)
{
    uint16_t putReady = 0;

    if(SMTPState == SMTP_DATA_BODY)
    {
        uint16_t reqSpace = TCPIP_SMTP_WRITE_READY_SPACE;

        if((reqSpace * 18) / 10 > smtpSktTxSize)
        {   // make sure we don't exceed the socket space and always fail
            reqSpace = (smtpSktTxSize * 10) / 18;
        }

        putReady = NET_PRES_SocketWriteIsReady(MySocket, reqSpace, 0);
    }

    return putReady;

}

/*****************************************************************************
  Function:
	bool TCPIP_SMTP_Put(char c)

  Description:
	Writes a single byte to the SMTP client.

  Precondition:
	TCPIP_SMTP_UsageBegin returned true on a previous call.

  Parameters:
	c - The byte to be written

  Return Values:
	true - The byte was successfully written
	false - The byte was not written, most likely because the buffer was full

  Remarks:
	This function should only be called externally when the SMTP client is
	generating an on-the-fly message.  (That is, TCPIP_SMTP_MailSend was called
	with SMTPClient.Body set to NULL.)
  ***************************************************************************/
bool TCPIP_SMTP_Put(char c)
{
	if(CRPeriod.State == CR_PERIOD_NEED_INSERTION)
	{
        const char * mailStr = ".";
		if(NET_PRES_SocketWrite(MySocket, mailStr, strlen(mailStr)))
        {
			CRPeriod.State = CR_PERIOD_SEEK_CR;
        }
		else
			return false;
	}

	switch(CRPeriod.State)
	{
		case CR_PERIOD_SEEK_CR:
			if(c == '\r')
				CRPeriod.State++;
			break;

		case CR_PERIOD_SEEK_LF:
			if(c == '\n')
				CRPeriod.State++;
			else if(c != '\r')
				CRPeriod.State--;
			break;

		case CR_PERIOD_SEEK_PERIOD:
			if(c == '.')
				CRPeriod.State++;	// CR_PERIOD_NEED_INSERTION
			else if(c == '\r')
				CRPeriod.State--;
			else
				CRPeriod.State = CR_PERIOD_SEEK_CR;
			break;
		
		// Default case needed to supress compiler diagnostics 
		// (CR_PERIOD_NEED_INSERTION state already handled above)
		default:
			break;
	}

	if(!NET_PRES_SocketWrite(MySocket, &c, 1))
		return false;

	return true;
}

/*****************************************************************************
  Function:
	uint16_t TCPIP_SMTP_ArrayPut(uint8_t* dataStr, uint16_t dataLen)

  Description:
	Writes a series of bytes to the SMTP client.

  Precondition:
	TCPIP_SMTP_UsageBegin returned true on a previous call.

  Parameters:
	dataStr - The data to be written
	dataLen - How many bytes should be written

  Returns:
	The number of bytes written.  If less than dataLen, then the TX FIFO became
	full before all bytes could be written.
	
  Remarks:
	This function should only be called externally when the SMTP client is
	generating an on-the-fly message.  (That is, TCPIP_SMTP_MailSend was called
	with SMTPClient.Body set to NULL.)
	
  ***************************************************************************/
uint16_t TCPIP_SMTP_ArrayPut(uint8_t* dataStr, uint16_t dataLen)
{
    union
    {
        char out[TCPIP_SMTP_MAX_WRITE_SIZE + TCPIP_SMTP_MAX_EXPAND_CHARACTERS];
        char in[TCPIP_SMTP_MAX_WRITE_SIZE];
    }putBuffer;


    if(dataLen > sizeof(putBuffer.in))
    {   // truncate
        dataLen = sizeof(putBuffer.in);
    }

    char*       outPtr = putBuffer.out;
    char*       inPtr = (char*)dataStr;
    int         nOut = 0;
    char*       expand;
    int         copySize;
    uint16_t    putReady;
    uint16_t    dataIn = dataLen;

    while(dataIn && nOut < sizeof(putBuffer.out))
    {
        expand = strstr(inPtr, TCPIP_SMTP_EXPAND_STR);
        if(expand == 0)
        {
            copySize = dataIn;
        }
        else
        {
            copySize = (expand - outPtr) + strlen(TCPIP_SMTP_EXPAND_STR);
        }

        if(copySize)
        {
            memcpy(outPtr, inPtr, copySize);
        }

        inPtr += copySize;
        outPtr += copySize;
        nOut += copySize;
        if(expand)
        {
            *outPtr++ = '.';    // expand
            nOut++;
        }
        dataIn -=copySize;
    }


    if(nOut)
    {   // need to write something
        putReady = NET_PRES_SocketWriteIsReady(MySocket, nOut, 0);
        if(putReady < nOut)
        {   // try to avoid partial writes
            return 0;
        }

        putReady = NET_PRES_SocketWrite(MySocket, putBuffer.out, nOut);
        _SMTPDbgCond(putReady == nOut, __func__, __LINE__);
    }

    return dataLen;

}


/*****************************************************************************
  Function:
	uint16_t TCPIP_SMTP_StringPut(char* Data)

  Description:
	Writes a string to the SMTP client.

  Precondition:
	TCPIP_SMTP_UsageBegin returned true on a previous call.

  Parameters:
	Data - The data to be written

  Returns:
	The number of bytes written.  If less than the length of Data, then the 
	TX FIFO became full before all bytes could be written.
	
  Remarks:
	This function should only be called externally when the SMTP client is
	generating an on-the-fly message.  (That is, TCPIP_SMTP_MailSend was called
	with SMTPClient.Body set to NULL.)
	
  ***************************************************************************/
uint16_t TCPIP_SMTP_StringPut(char* Data)
{
    return TCPIP_SMTP_ArrayPut((uint8_t*)Data, strlen(Data));
}


/*****************************************************************************
  Function:
	void TCPIP_SMTP_Flush(void)

  Description:
	Flushes the SMTP socket and forces all data to be sent.

  Precondition:
	TCPIP_SMTP_UsageBegin returned true on a previous call.

  Parameters:
	None

  Returns:
	None
	
  Remarks:
	This function should only be called externally when the SMTP client is
	generating an on-the-fly message.  (That is, TCPIP_SMTP_MailSend was called
	with SMTPClient.Body set to NULL.)
  ***************************************************************************/
void TCPIP_SMTP_Flush(void)
{
	NET_PRES_SocketFlush(MySocket);
}

/*****************************************************************************
  Function:
	void TCPIP_SMTP_PutIsDone(void)

  Description:
	Indicates that the on-the-fly message is complete.

  Precondition:
	TCPIP_SMTP_UsageBegin returned true on a previous call, and the SMTP client is
	generated an on-the-fly message.  (That is, TCPIP_SMTP_MailSend was called
	with SMTPClient.Body set to NULL.)

  Parameters:
	None

  Returns:
	None
  ***************************************************************************/
void TCPIP_SMTP_PutIsDone(void)
{
	SMTPFlags.bits.ReadyToFinish = true;
}

/*****************************************************************************
  Function:
	static uint8_t *FindEmailAddress(uint8_t *str, uint16_t *wLen)

  Summary:
	Searches a string for an e-mail address.

  Description:
	This function locates an e-mail address in a string.  It is used 
	internally by the SMTP client to parse out the actual address from
	the From and To strings so that the MAIL FROM and RCPT TO commands
	can be sent to the SMTP server.

  Precondition:
	TCPIP_SMTP_UsageBegin returned true on a previous call.

  Parameters:
	str - The string in which to search for an e-mail address
	wLen - the length of str

  Returns:
	A pointer to the e-mail address
  ***************************************************************************/
static const uint8_t *FindEmailAddress(const uint8_t *str, uint16_t *wLen)
{
	const uint8_t *lpStart;
	uint8_t c;
	union
	{
		uint8_t Val;
		struct
		{
			uint8_t FoundOpenBracket	: 1;
			uint8_t FoundAt			: 1;
		} bits;
	} ParseStates;

	lpStart = str;
	*wLen = 0x0000;
	ParseStates.Val = 0x00;

	while((c = *str++))
	{	
		if(c == '<')
		{
			ParseStates.bits.FoundOpenBracket = 1;
			lpStart = str;
			*wLen = -1;
		}
		else if(c == '@')
			ParseStates.bits.FoundAt = 1;


		if(	!ParseStates.bits.FoundOpenBracket &&
			!ParseStates.bits.FoundAt &&
			(c == ' ' || c == ','))
		{
			lpStart = str;
			continue;
		}
		else if(c == ',')
			break;

		if(ParseStates.bits.FoundOpenBracket && ParseStates.bits.FoundAt)
		{
			if(c == '>')
				break;
		}
		
		// Advance to next character
		*wLen += 1;
	}

	if(!ParseStates.bits.FoundAt)
		*wLen = 0;

	return lpStart;
}

// Obtain the IP address associated with the SMTP mail server
static char* FindEmailServer(const TCPIP_SMTP_CLIENT_MESSAGE* pClient, TCPIP_DNS_RESOLVE_TYPE* pDnsType)
{
    char*   dnsServer;


    if(pClient->Server != 0)
    {
        *pDnsType = TCPIP_DNS_TYPE_A;
        return pClient->Server;
    }

    // If no mail server, try to send the mail
    // directly to the destination SMTP server
    *pDnsType = TCPIP_DNS_TYPE_MX;

    if(pClient->To && (dnsServer = strchr(pClient->To, '@')) != 0)
    {
        return dnsServer++;
    }

    if(pClient->CC && (dnsServer = strchr(pClient->CC, '@')) != 0)
    {
        return dnsServer++;
    }

    if(pClient->BCC && (dnsServer = strchr(pClient->BCC, '@')) != 0)
    {
        return dnsServer++;
    }

    // no hostname found
    return 0;
}

#endif //#if defined(TCPIP_STACK_USE_SMTP_CLIENT)
