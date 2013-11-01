/* await_contact.cpp */

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

#include <cnaiapi.h>


/*-----------------------------------------------------------------------
 *	<p>Accept a connection on port a. If no master sock is
 *	already open for the port, create one and record it in the port-to-
 *	socket table.
 *	@param	appnum aport	Port number for accepting a connection.
 *	@return	connection	Socket descriptor.
 *-----------------------------------------------------------------------
 */
connection	await_contact(appnum aport)		{
	/**	Structure used by Windows Sockets to specify a local
	 *	or remote endpoint address to which to connect a socket.
	 *	This is the form of the SOCKADDR structure specific to the 
	 *	Internet address family and can be cast to SOCKADDR.
	 */
	struct sockaddr_in	sockaddr, csockaddr;
	/** Socket descriptor.											*/
	int	sock;
	/** Another socket descriptor.									*/
	int	newsock;
	/** Index or subscript.											*/
	int	i;
	/** Length of socket address.									*/
	int	csockaddrlen;
	/** Initialization flag, flase 1st time thru, then true.		*/
	static int	p2sinit = 0;

	/*	Create the port-to-socket table for all sockets.			*/
	static struct port2sock	p2s[P2S_SIZE];

	cnaiapi_init();					/*	Initialize CNAIAPI			*/

	/*	Exit of appnumber id is 0									*/
	if (aport == 0)	return -1;

	/*	Get a mutex lock object										*/
#if defined(LINUX) || defined(SOLARIS)
	pthread_mutex_lock(&await_contact_mutex);
#elif defined(WIN32)
	WaitForSingleObject(await_contact_mutex, INFINITE);
#endif

	/*	If 1st time thru, initialize the socket structures to 0's
	 *	and set the p2sinit flag to true
	 */
	if (p2sinit == 0) {
		/*	SET buffer to 0'S										*/
		(void) memset(p2s, 0, sizeof(p2s));
		p2sinit = 1;
	}

	/*	Look up master socket in port-to-socket table				*/
	for (sock = -1, i = 0; i < P2S_SIZE; i++) {
		if (p2s[i].port == aport) {
			sock = p2s[i].sock;
			break;
		}
	}

	/*	If no master socket for the service, create one.			*/
	if (sock == -1) {
		/* look for room in p2s table */
		for (i = 0; i < P2S_SIZE; i++) {
			if (p2s[i].port == 0)
				break;
		}

		if (i == P2S_SIZE) {
			/*	No room left in p2s, Release the mutext object
			 *	and exit in error
			 */
#if defined(LINUX) || defined(SOLARIS)
			pthread_mutex_unlock(&await_contact_mutex);
#elif defined(WIN32)
			ReleaseMutex(await_contact_mutex);
#endif
			return -1;
		}

		/*	Create a new socket
		 *	... If unsuccessful, release the mutex object
		 *	... and return in error.
		 */
		sock = socket(PF_INET, SOCK_STREAM, 0);
		if (sock < 0) {
#if defined(LINUX) || defined(SOLARIS)
			pthread_mutex_unlock(&await_contact_mutex);
#elif defined(WIN32)
			ReleaseMutex(await_contact_mutex);
#endif
			return -1;
		}

		/*	SET buffer to 0'S
		 *	... set the socket type to an Internet socket
		 *	... Convert the port number and the address
		 *	to Big Endian format
		 */
		(void) memset(&sockaddr, 0, sizeof(struct sockaddr_in));
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_port = htons(aport);
		sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

		/*	Bind the unconnected socket before subsequent calls
		 *	to the connect or listen functions.  Used to bind
		 *	either connection-oriented (stream) or connectionless
		 *	(datagram) sockets. When a socket is created with a call
		 *	to the socket function, it exists in a name space
		 *	(address family), but it has no name assigned to it.
		 *	Use the bind function to establish the local association
		 *	of the socket by assigning a local name to an unnamed
		 *	socket.
		 */		
		if (bind(sock,(struct sockaddr *) &sockaddr,
						sizeof(struct sockaddr_in)) < 0
				|| listen(sock, LISTEN_Q_LEN) < 0)	{
			/*	If unsuccesful bind then close the socket
			 *	release the mutex and return in error
			 */
#if defined(LINUX) || defined(SOLARIS)
			close(sock);
			pthread_mutex_unlock(&await_contact_mutex);
#elif defined(WIN32)
			closesocket(sock);
			ReleaseMutex(await_contact_mutex);
#endif
			return -1;
		}

		/* Record master socket in table							*/
		p2s[i].sock = sock;		/*	Socket descriptor.				*/
		p2s[i].port = aport;	/*	Port number						*/
	}

	/*	Release the mutext object.									*/
#if defined(LINUX) || defined(SOLARIS)
	pthread_mutex_unlock(&await_contact_mutex);
#elif defined(WIN32)
	ReleaseMutex(await_contact_mutex);
#endif

	/*	Set the length of the socket address structure.				*/
	csockaddrlen = sizeof(struct sockaddr_in);
	/*	Accept the connection request.								*/
	newsock = accept(sock, (struct sockaddr *) &csockaddr, &csockaddrlen);

#ifdef	_debug
	cout << "await_contact()"
		<<	"\n\t'sockaddr' Structure Elements:"
		<<	"\n\t      Family:" << sockaddr.sin_family
		<<	"\n\t        Port:" << ntohs(sockaddr.sin_port)
		<<	"\n\t     Address:"
			<<	inet_ntoa(sockaddr.sin_addr)
//						<< sockaddr.sin_addr.S_un.S_un_b.s_b1 << "."
//						<< sockaddr.sin_addr.S_un.S_un_b.s_b2 << "."
//						<< sockaddr.sin_addr.S_un.S_un_b.s_b3 << "."
//						<< sockaddr.sin_addr.S_un.S_un_b.s_b4
		<<	endl;
#endif

	return newsock;	/* Return the socket descriptor					*/
}	/*	End of	await_contact()				*/
