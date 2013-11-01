/* make_contact.cpp */

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
 *	<p>Open a new TCP connection to the specified IP address.
 *	@param	computer c	Host address of computer.
 *	@param	appnum a	Port number of Service.
 *	@return	connection	Socket descriptor.
 *-----------------------------------------------------------------------
 */
connection	make_contact(computer c, appnum a)	{
	/*	Structure used by Windows Sockets to specify a local
	 *	or remote endpoint address to which to connect a socket.
	 *	This is the form of the SOCKADDR structure specific to the 
	 *	Internet address family and can be cast to SOCKADDR.
	 */
	struct sockaddr_in	sockaddr;
	/** Socket descriptor.											*/
	int	sock;

	cnaiapi_init();				/*	Initialize CNAIAPI				*/
  
	/*	Create a stream socket.
	 *	... if unable to create it, exit in error
	 */
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0)	return -1;

	/* Initialize the socket address structure to zeros.			*/
	(void) memset(&sockaddr, 0, sizeof(struct sockaddr_in));
	/*	Set address family: Internet								*/
	/*	... use htons() to convert the u_short port number from
	 *		little-endian to TCP/IP network byte order (big-endian)
	 */
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(a);		/* Port number (bigendian)	*/
	sockaddr.sin_addr.s_addr = c;		/* Host address				*/

	/*	Try to connect, if failed, exit in error					*/
	if (connect(sock,
				(struct sockaddr *) &sockaddr,
				sizeof(struct sockaddr_in)) < 0) {
#if defined(LINUX) || defined(SOLARIS)
		close(sock);
#elif defined(WINDOWS)
		closesocket(sock);
#endif
		return -1;
	}  

#ifdef	_debug
	cout << "make_contact()"
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

	return sock;	/* Success, return socket descriptor			*/
}	/*	End of	make_contact()				*/
