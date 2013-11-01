/* echoserver.c */

/*-----------------------------------------------------------------------
 *	Original code in "c" format from:
 *		Computer Networks and Internets, 3rd Edition.
 *		Douglas E. Comer. (2001)	Prentice Hall.
 *	Modified:	R. Dyer	2002.
 *-----------------------------------------------------------------------
 */

#include <iostream>
using std::cout;
using std::cin;

#include <iomanip>
using std::setw;
using std::endl;

#include  <cnaiapi.h>

#include <fstream>
using namespace std;
#include <process.h>

/*-----------------------------------------------------------------------
 *
 * Program: echoserver
 * Purpose: wait for a connection from an echoclient and echo data
 * Usage:   echoserver <appnum>
 *
 *-----------------------------------------------------------------------
 */

/** Buffer size.																*/
	const int BUFFER_SIZE = 256;
	/** Socket descriptor for service.										*/
	connection	conn;
	sockaddr_in saddress;
	int slen = sizeof(saddress);
	//std::ofstream outfile ("log.txt");
	

	#define MSG_BUF_SIZE  128

	#define  WITH_SYNCHRONIZATION

class MessageBuffer 		{
	/*
	 * The functions in this MessageBuffer class must be synchronized
	 * because they are called by both the producer and consumer threads.
	 * Only one of these two threads must be allowed access to the
	 * message buffer at any one time.

	 * A "synchronization object" is an object whose handle can be
	 * specified in one of the wait functions to coordinate the
	 * execution of multiple threads. More than one process can have
	 * a handle to the same synchronization object, making interprocess
	 * synchronization possible.

	 * A "mutex object" is a synchronization object whose state is set
	 * to signaled when it is not owned by any thread, and nonsignaled
	 * when it is owned. Only one thread at a time can own a mutex object,
	 * whose name comes from the fact that it is useful in coordinating
	 * mutually exclusive access to a shared resource.
	 */

	char  messageText[MSG_BUF_SIZE];		// the message buffer
	BOOL  m_bContinueProcessing;			// used to control thread lifetime

#ifdef WITH_SYNCHRONIZATION
	HANDLE  m_hMutex;
	HANDLE  m_hEvent;
#endif

public:
	/**
	 *	Default Constructor
	 */
	MessageBuffer()	{
		memset( messageText, '\0', sizeof( messageText ) );  // initially the message buffer is empty

		m_bContinueProcessing = true;   // will be changed upon a call to DieDieDie()

#ifdef WITH_SYNCHRONIZATION
		cout << "Creating mutex in MessageBuffer ctor." << endl;
		m_hMutex = CreateMutex( NULL,							// no security attributes
										false,						// BOOL bInitialOwner, we don't want the
																		// thread that creates the mutex to
																		// immediately own it.
										"MessageBufferMutex"		// lpName
										);
		if ( m_hMutex == NULL )		{
			cout << "CreateMutex() failed in MessageBuffer ctor." << endl;
		}

		/*		Create the auto-reset event.		*/
		m_hEvent = CreateEvent( NULL,							// no security attributes
										FALSE,						// auto-reset event
										FALSE,						// initial state is non-signaled
										"MessageBufferEvent" );	// lpName

		if (m_hEvent == NULL)	{
			cout << "CreateEvent() failed in MessageBuffer ctor." << endl;
		}
#endif
	}

	/**
	 *	Destructor for the MessageBuffer class
	 *	Do all wrapup/finalization here
	 */
	~MessageBuffer()	{
#ifdef WITH_SYNCHRONIZATION
		CloseHandle( m_hMutex );
		CloseHandle( m_hEvent );
#endif
	}

	/**
	 *	Set the message value
	 *	@param	s	The new message value
	 */
	void SetMessage(char * s)	{
		cout << "MessageBuffer::SetMessage()" << endl;

#ifdef WITH_SYNCHRONIZATION
		DWORD   dwWaitResult = WaitForSingleObject( m_hMutex, INFINITE ); // Jaeschke's Monitor::Enter(this)
		if ( dwWaitResult != WAIT_OBJECT_0 )	{
			cout << "WaitForSingleObject() failed in MessageBuffer::SetMessage()." << endl;
			return;
		} 
		cout << "SetMessage() acquired mutex" << endl;
#endif

		/*		I intentionally use a very non-atomic method of copying
		 *		the new message into the message buffer, in order to
		 *		exacerbate the problem which occurs if the program doesn't
		 *		use a synchronization object between the producer and consumer.
		 */
		if ( strlen( s ) >= MSG_BUF_SIZE )
				s[MSG_BUF_SIZE-1] = '\0';    // make sure the caller doesn't overfill our buffer

		char * pch = &messageText[0];
		while ( *s )	{
			*pch++ = *s++;
			Sleep( 5 );
		}

		*pch = '\0';

		/*		Since the message buffer now holds a message we can
		 *		allow the consumer thread to run.
		 */

//		cout << "Set new message: " << messageText << endl;

#ifdef WITH_SYNCHRONIZATION
		cout << "SetMessage() pulsing Event" << endl;
		/*	Jaeschke's Monitor::Pulse(this)	*/
		if ( ! SetEvent( m_hEvent ) )		{
			cout << "SetEvent() failed in SetMessage()" << endl;
		}

		cout << "SetMessage() releasing mutex" << endl;
		/*		Jaeschke's Monitor::Exit(this)	*/
		if ( ! ReleaseMutex( m_hMutex ))		{ 
			cout << "ReleaseMutex() failed in MessageBuffer::SetMessage()." << endl;
		} 
#endif
	}


	/**
	 *
	 */
	void ProcessMessages()		{
//		cout << "MessageBuffer::ProcessMessages()" << endl;


		while ( m_bContinueProcessing )		{		/*	State variable used to control thread lifetime		*/

			/*		We now want to enter an "alertable wait state" so that
			 *		this consumer thread doesn't burn any cycles except
			 *		upon those occasions when the producer thread indicates
			 *		that a message waits for processing.
			 */

#ifdef WITH_SYNCHRONIZATION
			DWORD   dwWaitResult = WaitForSingleObject( m_hEvent, 2000 );
			if ( (dwWaitResult == WAIT_TIMEOUT)
					&& (m_bContinueProcessing == false) )		{	/*	WAIT_TIMEOUT = 258									*/
				break;    // we were told to die
			} else if ( dwWaitResult == WAIT_ABANDONED )		{	/*	WAIT_ABANDONED = 80									*/
				cout << "WaitForSingleObject(1) failed in MessageBuffer::ProcessMessages()." << endl;
				return;
			} else if ( dwWaitResult == WAIT_OBJECT_0 )		{	/*	WAIT_OBJECT_0 = 0										*/
				cout << "ProcessMessages() saw Event" << endl;
			} 

			dwWaitResult = WaitForSingleObject( m_hMutex, INFINITE );

			if ( dwWaitResult != WAIT_OBJECT_0 )	{
				cout << "WaitForSingleObject(2) failed in MessageBuffer::ProcessMessages()." << endl;
				return;
			} 
			cout << "ProcessMessages() acquired mutex" << endl;
#endif

			if ( strlen( messageText ) != 0 )		{
				cout << "Processed new message: " << messageText << endl;
				/*		We now empty the message buffer to show we have finished
				 *		processing the message:
				 */
				//write message into log file
				std::ofstream log;
				log.open("log.txt", std::ofstream::app);
				log << messageText;
				messageText[0] = '\0';   
				log.close();
			}

#ifdef WITH_SYNCHRONIZATION
			cout << "ProcessMessages() releasing mutex" << endl;
			if ( ! ReleaseMutex( m_hMutex ))		{ 
				cout << "ReleaseMutex() failed in MessageBuffer::ProcessMessages()." << endl;
			} 
#endif

		}	/*	END OF while ( m_bContinueProcessing ) loop			*/

		//outfile <<messageText;

	}


	/**
	 *
	 */
	void  DieDieDie( void )	{
		m_bContinueProcessing = false;   // ProcessMessages() watches for this in a loop
	}

};	/*	END OF CLASS:	MessageBuffer				*/

/**
 *	The Producer
 */
class CreateMessages		{
	int con;
	/** Length of buffer.														*/
	int len;
	/** Input buffer.																*/
	char logbuff[BUFFER_SIZE];
	/** Temp message buffer.																*/
	char msgbuff[BUFFER_SIZE+100];
	/** Message def buffer.																*/
	char defbuff[5];
	/** MSG message value buffer.																*/
	char valuebuff[BUFFER_SIZE];

	MessageBuffer* msg;
public:
	CreateMessages(connection c, MessageBuffer* m)	{
		con=c;
		msg = m;
	}

	void CreateMessagesEntryPoint()	{

		char buff[BUFFER_SIZE] = "";
		if (getpeername(con, (sockaddr *) &saddress, &slen)){
		perror("getpeername() failed");
	}else{

		/* iterate, echoing all data received until end of file */
		while ((len = recv(con, buff, BUFFER_SIZE, 0)) > 0){
			
			strncpy_s(defbuff, buff, 4 );//get the first 4 chars of received message
			memcpy(valuebuff, buff+4, sizeof(buff)-4 );// get the value after first 4 chars
			if (strcmp(defbuff, "MSG:") == 0){
				//If first 4 chars are MSG: consider received message value as a 'message'
				sprintf_s(msgbuff,"%s: %s -- %s \n",
					" Your msg", valuebuff, "is received");
			}else if(strcmp(defbuff, "NUM:") == 0){
				//If first 4 chars are NUM: consider received message value as a 'number'
				int r = rand() % 100 + 1;
				int x ; 
				sscanf_s(valuebuff," %d",&x);// convert value string to a number
				sprintf_s(msgbuff,"%s: %s -- %s %d %s\n",
					" Your num", valuebuff, "is received. multiplication is ",x*r ,", can you guess my secret num?");
			}else{
				// If first 4 chars not MSG: or NUM:, consider received message as a invalid message
				strcpy_s(msgbuff, "your message is invalid, try again");
			}
			// Send response message to client
			(void) send(con, msgbuff, len+100, 0);
			// Create log message	
			sprintf_s(logbuff,"%s: %hu -- %s:%hu -- %s \n",
				"client number", con,
				inet_ntoa(saddress.sin_addr), ntohs(saddress.sin_port),buff);
			// Pass log message to messagebuffer instance
			msg->SetMessage( logbuff );
			cout << inet_ntoa(saddress.sin_addr)<< ":"<< ntohs(saddress.sin_port)
				<< "--" << buff <<endl;
			// Clean buff memory
			(void) memset(buff, 0, sizeof(buff));
			
		}
		send_eof(con);
		
	}
	}

static unsigned __stdcall CreateMessagesStaticEntryPoint( void * pThis )		{
		CreateMessages * pPM = (CreateMessages*)pThis;
		pPM->CreateMessagesEntryPoint();	// calls MessageBuffer::ProcessMessages()
														// which has a while(m_bContinueProcessing) loop

		cout << "ProcessMessages thread terminating." << endl;
		return 1;   // the thread exit code
	}
};

/**
 *	The Ccosumer
 */
class ProcessMessages		{
	MessageBuffer* msg;
public:
	ProcessMessages(MessageBuffer* m)	{
		msg = m ;
	}

void ProcessMessagesEntryPoint()	{
		msg->ProcessMessages();
	}

static unsigned __stdcall ProcessMessagesStaticEntryPoint( void * pThis )		{
		ProcessMessages * pPM = (ProcessMessages*)pThis;
		pPM->ProcessMessagesEntryPoint();	// calls MessageBuffer::ProcessMessages()
														// which has a while(m_bContinueProcessing) loop

		cout << "ProcessMessages thread terminating." << endl;
		return 1;   // the thread exit code
	}
};


int	main(int argc, char *argv[])	{
	HANDLE  hdlProducer;
	unsigned  uiProducerThreadID;

	if (argc != 2) {
		(void) fprintf(stderr, "usage: %s <appnum>\n", argv[0]);
		exit(1);
	}

	/* Verify that the user passed the command line arguments.		*/
	if (argc != 2)	{
		cout << "\n\tUsage: " << argv[0]
			 << " <appnum or port#>"
			 << "\n\tExample:"
			 << "\n\t" << argv[0]
			 << " 7"
			 << "\n\n\t(Start the server on port number 7"
			 << "\n\n" << endl;
		exit(1);
	}

	MessageBuffer* m = new MessageBuffer();
	ProcessMessages* pm = new ProcessMessages( m );
	HANDLE   hdlConsumer;
	unsigned  uiConsumerThreadID;
	hdlConsumer = (HANDLE)_beginthreadex( NULL,
													0,
													ProcessMessages::ProcessMessagesStaticEntryPoint,
													pm,
													CREATE_SUSPENDED, // so we can later call ResumeThread()
													&uiConsumerThreadID );


	ResumeThread( hdlConsumer );   // Jaeschke's pmt->Start()

	/*	Wait for a connection from echo clients,
	 *	Create new thread for new connected client
	 *  
	 */
	while(true){
		if (conn < 0)	exit(1);
	conn = await_contact((appnum) atoi(argv[1]));
	cout << "New client connected \n";
	//if (conn < 0)	exit(1);
	CreateMessages* pm = new CreateMessages(conn, m);

	//ProcessMessages::ProcessMessagesEntryPoint(c);
	hdlProducer = (HANDLE)_beginthreadex( NULL,
										0,
										CreateMessages::CreateMessagesStaticEntryPoint,
										pm,
										CREATE_SUSPENDED, // so we can later call ResumeThread()
										&uiProducerThreadID );

	ResumeThread( hdlProducer ); 	
	
	}
	return 0;
	
}