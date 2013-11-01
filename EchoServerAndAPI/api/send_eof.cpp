/* send_eof.cpp - send_eof */

/*-----------------------------------------------------------------------
 *	Original code in "c" format from:
 *		Computer Networks and Internets, 3rd Edition.
 *		Douglas E. Comer. (2001)	Prentice Hall.
 *	Modified:	R. Dyer	2002.
 *-----------------------------------------------------------------------
 */

#include <cnaiapi.h>

/*------------------------------------------------------------------------
 * send_eof - signal EOF by shutting down send side of connection
 *------------------------------------------------------------------------
 */
int	send_eof(connection conn)	{
	/*	Shutdown the socket identified by connection.				*/
	return shutdown(conn, 1);
}	/*	End of	send_eof()		*/
