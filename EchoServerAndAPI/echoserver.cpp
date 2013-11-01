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
	char logbuff[BUFFER_SIZE];
	sockaddr_in saddress;
	int slen = sizeof(saddress);
	std::ofstream outfile ("new.txt");


class Conninfo{ 
	connection conn;
public:
	Conninfo(connection c){
		conn = c;
	}

	connection getConn(){
		return conn;
	}
};
/**
 *	The Producer
 */
class CreateMessages		{
	int con;
	/** Length of buffer.														*/
	int len;
	/** Input buffer.																*/
	
	char msgbuff[BUFFER_SIZE+100];
	char defbuff[5];
	char valuebuff[BUFFER_SIZE];

public:
	CreateMessages(connection c)	{
		con=c;
	}

void CreateMessagesEntryPoint()	{

		char buff[BUFFER_SIZE] = "";
		if (getpeername(con, (sockaddr *) &saddress, &slen)){
		perror("getpeername() failed");
	}else{

		/* iterate, echoing all data received until end of file */
		while ((len = recv(con, buff, BUFFER_SIZE, 0)) > 0){
			
			strncpy_s(defbuff, buff, 4 );
			memcpy(valuebuff, buff+4, sizeof(buff)-4 );
			if (strcmp(defbuff, "MSG:") == 0){
				
				sprintf_s(msgbuff,"%s:%s -- %s \n",
					" Your msg", valuebuff, "is received");
			}else if(strcmp(defbuff, "NUM:") == 0){
				int r = rand() % 100 + 1;
				int x ; 
				sscanf_s(valuebuff," %d",&x);
				sprintf_s(msgbuff,"%s:%s -- %s %d %s\n",
					" Your num", valuebuff, "is received. multiplication is ",x*r ,", can you guess my secret num?");
			}else{
				strcpy_s(msgbuff, "your message is invalid, try again");
			}

			(void) send(con, msgbuff, len+100, 0);
				
			sprintf_s(logbuff,"%s: %hu %s:%hu -- %s \n",
				"client number", con,
				inet_ntoa(saddress.sin_addr), ntohs(saddress.sin_port),buff);
			cout << inet_ntoa(saddress.sin_addr)<< ":"<< ntohs(saddress.sin_port)
				<< "--" << buff <<endl;
			outfile <<logbuff;
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
 *		The CONSUMER
 */
class ProcessMessages		{
	connection conn;
public:
	ProcessMessages(connection c)	{
		conn = c;
	}

void ProcessMessagesEntryPoint()	{


		if (getpeername(conn, (sockaddr *) &saddress, &slen)){
		perror("getpeername() failed");
	}else{

	}
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
	HANDLE hdlConsumer;
	unsigned uiConsumerThreadID;

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


	/*	Wait for a connection from an echo client,
	 *	if no client connects exit in error.
	 */
	while(true){
		if (conn < 0)	exit(1);
	conn = await_contact((appnum) atoi(argv[1]));
	cout << "New client connected \n";
	//if (conn < 0)	exit(1);
	CreateMessages* pm = new CreateMessages(conn);

	//ProcessMessages::ProcessMessagesEntryPoint(c);
	hdlConsumer = (HANDLE)_beginthreadex( NULL,
										0,
										CreateMessages::CreateMessagesStaticEntryPoint,
										pm,
										CREATE_SUSPENDED, // so we can later call ResumeThread()
										&uiConsumerThreadID );

	ResumeThread( hdlConsumer ); 	
	
	}
	outfile.close();
	return 0;
	
}
