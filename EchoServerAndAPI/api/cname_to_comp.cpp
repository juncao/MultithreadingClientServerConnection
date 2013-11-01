/* cname_to_comp.cpp */

/*-----------------------------------------------------------------------
 *	Original code in "c" format from:
 *		Computer Networks and Internets, 3rd Edition.
 *		Douglas E. Comer. (2001)	Prentice Hall.
 *	Modified:	R. Dyer	2002.
 *-----------------------------------------------------------------------
 */

#include <cnaiapi.h>

#include <iostream>
using std::cout;
using std::cin;

#include <iomanip>
using std::setw;
using std::endl;

/*-----------------------------------------------------------------------
 *	<p>Look up a host by name and return its IP address
 *	@param	char* cname	Name of the host you wish to lookup
 *	@return	computer	Address of host.
 *-----------------------------------------------------------------------
 */
computer	cname_to_comp(char *cname)	{
	/**	Host address.													*/
	computer	compAddr;

	/*	Structure used by functions to store information about a
	 *	given host, such as host name, IP address, and so forth.
	 *	Only one copy of the hostent structure is allocated per
	 *	thread, and an application should therefore copy any
	 *	information that it needs before issuing any other
	 *	Windows Sockets API calls.
	 */	
	struct hostent	*hp;		/* Host information structure			*/

	cnaiapi_init();				/* Initialize CNAIAPI					*/

	/*	Get a mutex lock object											*/
#if defined(LINUX) || defined(SOLARIS)
	pthread_mutex_lock(&cname_mutex);
#elif defined(WIN32)
	WaitForSingleObject(cname_mutex, INFINITE);
#endif

	/*	Retrieve host information from a host database corresponding
	 *	to the host name in cname.
	 */
	hp = gethostbyname(cname);
	/*	If host can't be found, release ownership of the specified
	 *	mutext object.
	 */
	if (hp == NULL) {
#if defined(LINUX) || defined(SOLARIS)
		pthread_mutex_unlock(&cname_mutex);
#elif defined(WIN32)
		ReleaseMutex(cname_mutex);
#endif
		return -1;
	}

	/*	If this host is not an internet address or the host name
	 *	size is different than requested, ERROR getting host
	 *	release ownership of the specified mutext object.
	 */
	if (hp->h_addrtype != AF_INET
			|| hp->h_length != sizeof(computer))	{

#if defined(LINUX) || defined(SOLARIS)
		pthread_mutex_unlock(&cname_mutex);
#elif defined(WIN32)
		ReleaseMutex(cname_mutex);
#endif
		return -1;
	}

	/* convert the host address to long format							*/
	compAddr = *((long *) hp->h_addr_list[0]);

	/*	Release ownership of the specified mutex object.				*/
#if defined(LINUX) || defined(SOLARIS)
	pthread_mutex_unlock(&cname_mutex);
#elif defined(WIN32)
	ReleaseMutex(cname_mutex);
#endif

#ifdef	_debug
	cout << "cname_to_comp()"
		<< "\n\t'hostent' Structure Elements:"
		<<	"\n\t        Name:" << hp->h_name
		<<	"\n\t     Aliases:"
		<<	((hp->h_aliases[0] == 0) ? "None" : hp->h_aliases[0])
		<<	"\n\tAddress Type:" << hp->h_addrtype
		<<	"\n\t      Length:" << hp->h_length
		<<	"\n\tAddress list:"	<< compAddr
		<<	endl;
#endif

	return compAddr;		/* Return the host address						*/

}	/*	End of	cname_to_comp()			*/
