/* appname_to_appnum.cpp */

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
 *	<p>Look up a port by service name
 *	@param	char* appname	character string representing the service
 *	request.
 *	@return	appnum (port) for the requested service
 *-----------------------------------------------------------------------
 */
appnum	appname_to_appnum(char *appname)	{
	struct servent	*sp;		/* Servent structure returned by
								 *	getservbyname()					*/
	appnum	port;				/*	Port number for app service		*/

	cnaiapi_init();				/*	Initialize the cnaiapi			*/

	/*	Get a mutex lock object										*/
#if defined(LINUX) || defined(SOLARIS)
	pthread_mutex_lock(&appname_mutex);
#elif defined(WIN32)
	WaitForSingleObject(appname_mutex, INFINITE);
#endif

	/*	Get a pointer to the SERVENT structure containing the name(s)
	 *	and service number that match the string in the name parameter.
	 *	All strings are null-terminated.
	 */
	sp = getservbyname(appname, "tcp");

	/*	If failure getting SERVENT structure
	 *	Release ownership of the specified mutex object.
	 */
	if (sp == NULL) {
#if defined(LINUX) || defined(SOLARIS)
		pthread_mutex_unlock(&appname_mutex);
#elif defined(WIN32)
		ReleaseMutex(appname_mutex);
#endif
		return -1;
	}
	
	/*	Convert a u_short from TCP/IP network byte order to host
	 *	byte order (which is little-endian on Intel processors).
	 */
	port = ntohs(sp->s_port);
	
	/*	Release ownership of the specified mutex object.			*/
#if defined(LINUX) || defined(SOLARIS)
		pthread_mutex_unlock(&appname_mutex);
#elif defined(WIN32)
		ReleaseMutex(appname_mutex);
#endif

#ifdef	_debug
	cout << "appname_to_appnum()"
		<< "\n\t'servent' Structure Elements:"
		<<	"\n\t        Name:" << sp->s_name
		<<	"\n\t     Aliases:"
		<<	((sp->s_aliases[0] == 0) ? "None" : sp->s_aliases[0])
		<<	"\n\t        Port:" << ntohs(sp->s_port)
		<<	"\n\t    Protocol:" << sp->s_proto
		<<	endl;
#endif

	return port;		/*	Return the port number for the service	*/

}	/*	End of	appname_to_appnum()				*/
