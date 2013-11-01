/*	readln.cpp - readln, recvln */

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

/*------------------------------------------------------------------------
 *	<p>Read from stdin until newline or EOF, or buffer is full.
 *	Flush to newline or EOF and return on full buffer.
 *	@param	char *buff	Input buffer.
 *	@param	int buffz	Input buffer size.
 *	@return	int	Data length.
 *------------------------------------------------------------------------
 */
int	readln(char *buff, int buffsz)	{
	/**	Save the pointer to the input buffer.						*/	
	char	*bp = buff;

	cin.getline(bp,buffsz);
//	cout << "length of input:" << strlen(buff);
	return	strlen(bp);

}	/*	End of	readln()			*/


/*------------------------------------------------------------------------
 *	<p>Receive from socket until newline or EOF is encountered.
 *	Flush to newline or EOF and return on full buffer.
 *	@param	connection conn	Socket descriptor.
 *	@param	char *buff	Output buffer.
 *	@param	int buffz	Output buffer size.
 *	@return	int	Length of data.
 *------------------------------------------------------------------------
 */
int	recvln(connection conn, char *buff, int buffsz)	{
	/** Save the output buffer pointer.								*/
	char	*bp = buff;
	/** Spare character, used when we've exceeded our output
	 *	buffer size and we have bytes left to read from the socket.
	 */
	char	c;
	/** Number of bytes received from socket read.					*/
	int	bytesReceived;

	cout	<< "recvln()" << endl;

	/*	Use the recv() function to read incoming data on a
	 *	connection-oriented or connectionless socket identified by
	 *	the socket descriptor "conn".
	 *	When using a connection-oriented protocol, the sockets must be
	 *	connected before calling recv.
	 *	When using a connectionless protocol, the sockets must be bound
	 *	before calling recv.
	 */

	 /* Read from our socket until we max out the size of the buffer
	 *	or we don't reveive any data
	 */
	while ( (bp - buff < buffsz)
				&& (bytesReceived = recv(conn, bp, 1, 0)) > 0)	{
		/*	If we've reached a newline character, exit				*/
		if (*bp++ == '\n')	{
			cout	<< "recvln:" << (bp-buff) << endl;
			return (bp - buff);
		}
	}

	/* If we read no data, Exit in error,							*/
	if (bytesReceived < 0)	return -1;

	/* If buffer is full, read from the socket byte by byte
	 *	and discard the input until we finish reading
	 *	all the data or we read an newline character
	 */
	if (bp - buff == buffsz)	{
		while (recv(conn, &c, 1, 0) > 0 && c != '\n');
	}

	cout	<< "recvln:" << bp-buff;

	return (bp - buff);		/* Return the current buffer size		*/
}
