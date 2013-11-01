/* echoclient.cpp */

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

#include <string.h>
#include <cnaiapi.h>

int readln(char *, int);	/* Read from stdin						*/

/*-----------------------------------------------------------------------
 * Program: echoclient
 * Purpose: contact echo service provider, send user input and print
 *	server response
 * Usage:   echoclient <computer name> [appnum or port#]
 * Note:    Appnum is optional. If not specified the standard echo appnum
 *          (7) is used.
 *
 *	@param	int argc	Number of command line arguments
 *	@param	char* argv[]	Array of arguments
 *-----------------------------------------------------------------------
 */
int	main(int argc, char *argv[])	{
	/** Input buffer size													*/
	int const	BUFFER_SIZE		= 256;
	/** Input prompt.															*/
	char* const INPUT_PROMPT	= "   Input>";
	/** Recieved message.													*/
	char* const RECEIVED_PROMPT = "Received>";

	/** Computer address.													*/
	computer comp;
	/** Port number for associated application.						*/
	appnum app;
	/** Socket descriptor for service.									*/
	connection conn;

	/** Input buffer.															*/
	char buff[BUFFER_SIZE];
	/** Expected number of bytes to receive.							*/
	int	expect;
	/** Number of bytes received.											*/
	int	received;
	/** Input buffer size.													*/
	int	len;

#ifdef	_debug
	cout << "\n\tCommand line arguments:" << argc << endl;
	for (int idx=0; idx<argc; idx++)	{
		cout << "\t\targv[" << idx << "]:" << argv[idx] << endl;
	}
#endif

	/* Verify that the user passed the command line arguments.		*/
	if (argc < 2 || argc > 3) {
		cout << "\n\tUsage: " << argv[0]
			 << " <computer name> [appnum or port#]"
			 << "\n\t\t  or " << argv[0]
			 << " <computer name> [port default:7]"
			 << "\n\n" << endl;
		exit(1);
	}

	/*	Convert the arguments to binary format, 1st the computer name
	 *	If host can't be found, exit in error
	 */
	comp = cname_to_comp(argv[1]);
	if (comp == -1)	exit(1);

	/*	2nd convert the appnum (port) to an int
	 *	If host can't be found, exit in error
	 */
	if (argc == 3)	{
		/*	Convert string to integer								*/
		app = (appnum) atoi(argv[2]);
	}	else	{
		/*	Find the echo service by name							*/
		if ((app = appname_to_appnum("echo")) == -1)	{
			exit(1);
		}
	}
	
	/*	Make a connection with the echoserver, get the socket descriptor
	 *	for the echo service.  If we can't connect to echo server,
	 *	exit in error
	 */
	conn = make_contact(comp, app);
	if (conn < 0) exit(1);

	cout << "\n" << INPUT_PROMPT;

	/*	Read input from the user, send to the echo service provider
	 *	... receive a reply from the server
	 *	... and display for user
	 */
	while ((len = readln(buff, BUFFER_SIZE)) > 0)	{
		/* send the input to the echoserver			*/
		(void) send(conn, buff, len, 0);

		/*	Initialize the buffer
		 *	... Set the number of bytes we expect to receive
		 *	(based on what we sent)
		 *	... set the buffer size
		 *	Read from the soscket and display
		 *	the same no. of bytes we sent.
		 */
		(void) memset(buff, 0, sizeof(buff));
		//expect = len;

		//for (received = 0; received < expect;)	{
			len = recv(conn,		/* Socket descrptor				*/
						buff,		/* Receive buffer				
						(expect - received) < BUFFER_SIZE
							? (expect - received)	:				*/
							 BUFFER_SIZE,	/* Buffer size			*/
						0);		/* Flags							*/

		   /* If no more to read then send an eof, and exit			*/
			if (len < 0)	{
				send_eof(conn);
				return 1;
			}

			/*	Output the buffer
			 *	and increment the bytes received
			 */
			cout << "\n" << RECEIVED_PROMPT << buff << "\n";
			//received += len;
		//}

		cout	<< "\n" << INPUT_PROMPT;
	}

	/*	Iteration ends when EOF found on stdin						*/
	(void) send_eof(conn);
	cout	<< "\n" << endl;

	return 0;

}	/*	End of	echoclient.cpp					*/
