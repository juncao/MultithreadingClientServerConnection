/* cnaiapi.h */

#ifndef _CNAIAPI_H_
#define _CNAIAPI_H_

#if defined(LINUX) || defined(SOLARIS)
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#endif /* defined(LINUX) || defined(SOLARIS) */

#if defined(WIN32)
#include <winsock2.h>
#include <cnaiapi_win32.h>
#endif /* defined(WIN32) */

#include <sys/types.h>

typedef	short	appnum;
#ifdef __alpha__
typedef	int	computer;
#else
typedef	long	computer;
#endif /* defined(__alpha__) */
typedef	int	connection;

struct port2sock {
	short	port;
	int	sock;
};

#define P2S_SIZE 64 /* number of entries in port to socket map table	*/
#define LISTEN_Q_LEN 5

appnum	appname_to_appnum(char *appname);
computer	cname_to_comp(char *cname);
connection	await_contact(appnum a);
connection	make_contact(computer c, appnum a);
int	send_eof(connection c);
void  cnaiapi_init(void);

#if defined(LINUX) || defined(SOLARIS)
extern pthread_mutex_t await_contact_mutex, cname_mutex, appname_mutex;
#elif defined(WIN32)
extern HANDLE await_contact_mutex, cname_mutex, appname_mutex;
#endif

#endif /* !defined(_CNAIAPI_H_) */
