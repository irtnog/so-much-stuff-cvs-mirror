/* popd.c - the POP server */
#ifndef	lint
static char ident[] = "@(#)$Id: popd.c,v 1.1 1990-04-12 13:29:22 vrs Exp $";
#endif	lint

/* Author:	Marshall T. Rose	<MRose@UDel>	(MTR)
		Department of Computer Science and Information Sciences
		University of Delaware
		Newark, DE  19716
		302/451-1951

   Date:	Sun Oct 28 16:23:26 1984
 */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include "../h/strings.h"
#include "syslog.h"
#include <sys/types.h>
#include <sys/file.h>
#ifndef	NOIOCTLH
#include <sys/ioctl.h>
#endif	NOIOCTLH
#include <sys/socket.h>
#include <sys/time.h>
#ifdef	SIGTSTP
#include <sys/resource.h>
#include <sys/wait.h>
#endif	SIGTSTP
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#ifdef	SYS5
#include <fcntl.h>
#endif	SYS5


#ifdef	SYS5
#define	u_short	ushort
#endif	SYS5

#define	NOTOK	(-1)
#define	OK	0

#define	NULLCP	((char *) 0)
#define NULLRP	((struct rusage *) 0)

#define	FAST			/* fast start-up of BBoards */

/*  */

extern int  errno;
extern int  sys_nerr;
extern char *sys_errlist[];
extern char *sys_siglist[];


int  debug = 0;
static int  nbits = ((sizeof (int)) * 8);
static int  options = 0;

#ifndef	POPSERVICE
#define	POPSERVICE	"pop"
#endif

char *myname = "popd";
char myhost[BUFSIZ];
static char *myprotocol = "tcp";
static char *myservice = POPSERVICE;

static struct sockaddr_in   in_socket;
static struct sockaddr_in  *isock = &in_socket;


static int	chldser ();
void	padios (), padvise ();

static	server(), arginit(), envinit();
/*  */

/* ARGSUSED */

main (argc, argv, envp)
int     argc;
char  **argv,
      **envp;
{
    int     fd,
            sd;
    int	    on = 1;
    struct servent *sp;
    struct sockaddr_in  out_socket,
                       *osock = &out_socket;

    if ((sp = getservbyname (myservice, myprotocol)) == NULL)
	padios (NULLCP, "%s/%s: unknown service", myprotocol, myservice);
    isock -> sin_family = AF_INET;
    isock -> sin_port = sp -> s_port;
    isock -> sin_addr.s_addr = INADDR_ANY;
    arginit (argv);
    envinit ();

#ifdef	RESTART
    for (;;) {
	char    reason[BUFSIZ];
	union wait status;

	switch (fork ()) {
	    case NOTOK: 
		sleep (5);
		continue;

	    case OK: 
		break;

	    default: 
		sleep (60);
		(void) wait3 (&status, 0, NULLRP);
		if (WIFEXITED (status))
		    (void) sprintf (reason, "exit=0%o", status.w_retcode);
		else
		    if (WIFSIGNALED (status))
			(void) sprintf (reason, "signal=%s%s",
				status.w_termsig < NSIG
				? sys_siglist[status.w_termsig] : "unknown",
				status.w_coredump ? " (core dumped)" : NULL);
		    else
			(void) strcpy (reason, "stopped(!!)");
		padvise (NULLCP, LOG_WARNING, "%s/%s server has terminated -- %s",
			sp -> s_proto, sp -> s_name, reason);
		continue;
	}
	break;
    }

    closelog ();
#ifndef	BSD43
    openlog (myname, LOG_PID);
#else	BSD43
    openlog (myname, LOG_PID, LOG_DAEMON);
#endif	BSD43
    padvise (NULLCP, LOG_INFO, "restart");
#endif	RESTART

/*  */

    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == NOTOK)
	padios ("socket", "unable to create");
#ifndef	BSD43
    if (options & SO_DEBUG)
	if (setsockopt (sd, SOL_SOCKET, SO_DEBUG, NULL, 0) == NOTOK)
	    padvise ("SO_DEBUG", LOG_WARNING, "unable to set socket option");
    if (setsockopt (sd, SOL_SOCKET, SO_KEEPALIVE, NULL, 0) == NOTOK)
	padvise ("SO_KEEPALIVE", LOG_WARNING, "unable to set socket option");
#else	BSD43
    if (options & SO_DEBUG)
	if (setsockopt (sd, SOL_SOCKET, SO_DEBUG, &on, sizeof on) == NOTOK)
	    padvise ("SO_DEBUG", LOG_WARNING, "unable to set socket option");
    if (setsockopt (sd, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof on) == NOTOK)
	padvise ("SO_KEEPALIVE", LOG_WARNING, "unable to set socket option");
#endif	BSD43
    if (bind (sd, (struct sockaddr *) isock, sizeof *isock) == NOTOK)
	padios ("socket", "unable to bind");

#ifdef	SIGCHLD
    (void) signal (SIGCHLD, chldser);
#endif	SIGCHLD
    (void) listen (sd, SOMAXCONN);
#ifdef	FAST
    popinit ();
#endif	FAST
    for (;;) {
	int     i = sizeof *osock;

	if ((fd = accept (sd, (struct sockaddr *) osock, &i)) == NOTOK) {
	    if (errno != EINTR)
		padvise ("socket", LOG_WARNING,
		    "unable to accept connection on");
	    continue;
	}
#ifdef	FAST
	popassert ();
#endif	FAST
	switch (fork ()) {
	    case OK: 
		(void) close (sd);
#ifdef	SIGCHLD
		(void) signal (SIGCHLD, SIG_DFL);
#endif	SIGCHLD
		server (fd, osock);
		_exit (0);

	    case NOTOK: 
		padvise ("socket", LOG_WARNING,
		    "no forks, so rejecting connection on");
	    default: 
		(void) close (fd);
	}
    }
}

/*  */

static	server (fd, sin)
int	fd;
struct sockaddr_in *sin;
{
    u_short port;
    struct hostent *hp;
    struct in_addr *addr;

    closelog ();
#ifndef	BSD43
    openlog (myname, LOG_PID);
#else	BSD43
    openlog (myname, LOG_PID, LOG_DAEMON);
#endif	BSD43
    port = ntohs (sin -> sin_port);
    addr = &sin -> sin_addr;
    hp = gethostbyaddr (addr, sizeof *addr, sin -> sin_family);
    padvise (NULLCP, LOG_INFO, "servicing %s/%d",
	    hp ? hp -> h_name : inet_ntoa (*addr), port);

    (void) dup2 (fd, 0);
    (void) close (fd);
    (void) dup2 (0, 1);

    pop (0, 1, sin -> sin_family == AF_INET && port < IPPORT_RESERVED && hp,
	    hp ? hp -> h_name : NULLCP);
}
	
/*  */

static	arginit (vec)
char	**vec;
{
    int	    port;
    register char  *ap;
    struct hostent *hp;

    if (myname = rindex (*vec, '/'))
	myname++;
    if (myname == NULL || *myname == NULL)
	myname = *vec;

    (void) gethostname (myhost, sizeof myhost);
    if (hp = gethostbyname (myhost))
	(void) strcpy (myhost, hp -> h_name);
    nbits = getdtablesize ();

    for (vec++; ap = *vec; vec++) {
	if (*ap == '-')
	    switch (*++ap) {
		case 'd': 
		    options |= SO_DEBUG;
		    continue;

		case 'p': 
		    if ((ap = *++vec) == NULL
			    || *ap == '-'
			    || (port = atoi (ap)) <= 0)
			padios (NULLCP, "usage: %s -p portno", myname);
		    isock -> sin_port = htons ((u_short) port);
		    continue;

		default: 
		    padios (NULLCP, "-%s: unknown switch", ap);
	    }

	padios (NULLCP, "usage: %s [switches]", myname);
    }
}

/*  */

static  envinit () {
    int     i,
            sd;

    if (!(debug = isatty (2))) {
	for (i = 0; i < 5; i++) {
	    switch (fork ()) {
		case NOTOK: 
		    sleep (5);
		    continue;

		case OK: 
		    break;

		default: 
		    _exit (0);
	    }
	    break;
	}

	(void) chdir ("/");

	if ((sd = open ("/dev/null", O_RDWR)) == NOTOK)
	    padios ("/dev/null", "unable to read");
	if (sd != 0)
	    (void) dup2 (sd, 0), (void) close (sd);
	(void) dup2 (0, 1);
	(void) dup2 (0, 2);

	if ((sd = open ("/dev/tty", O_RDWR)) != NOTOK) {
#ifdef	TIOCNOTTY
	    (void) ioctl (sd, TIOCNOTTY, NULLCP);
#endif	TIOCNOTTY
	    (void) close (sd);
	}
    }

    for (sd = 3; sd < nbits; sd++)
	(void) close (sd);

    (void) signal (SIGPIPE, SIG_IGN);

#ifndef	BSD43
    openlog (myname, LOG_PID);
#else	BSD43
    openlog (myname, LOG_PID, LOG_DAEMON);
#endif	BSD43
    padvise (NULLCP, LOG_INFO, "starting");
    if (debug)
	padvise (NULLCP, LOG_DEBUG, "options=0x%x port=%d",
		options, ntohs (isock -> sin_port));
}

/*  */

/* ARGSUSED */

#ifdef	SIGCHLD

static int chldser (sig, code, sc)
int	sig;
long    code;
struct sigcontext *sc;
{
    union wait status;

    while (wait3 (&status, WNOHANG, NULLRP) > 0)
	continue;
}

#endif	SIGCHLD

/*  */

/* VARARGS2 */

void	padios (what, fmt, a, b, c, d, e, f, g, h, i, j)
char   *what,
       *fmt,
       *a,
       *b,
       *c,
       *d,
       *e,
       *f,
       *g,
       *h,
       *i,
       *j;
{
    padvise (what, LOG_ERR, fmt, a, b, c, d, e, f, g, h, i, j);
    _exit (1);
}

/*  */

/* VARARGS3 */

void	padvise (what, code, fmt, a, b, c, d, e, f, g, h, i, j)
char   *what,
       *fmt,
       *a,
       *b,
       *c,
       *d,
       *e,
       *f,
       *g,
       *h,
       *i,
       *j;
int	code;
{
    int     eindex = errno;
    char    buffer[BUFSIZ];

    (void) sprintf (buffer, fmt, a, b, c, d, e, f, g, h, i, j);
    if (what)
	if (eindex > 0 && eindex < sys_nerr)
	    syslog (code, "%s %s: %s", buffer, what, sys_errlist[eindex]);
	else
	    syslog (code, "%s %s: Error %d", buffer, what, eindex);
    else
	syslog (code, "%s", buffer);

    if (debug) {
	fprintf (stderr, "[%d] %s", code, buffer);
	if (what)
	    (void) fputc (' ', stderr), perror (what);
	else
	    (void) fputc ('\n', stderr);
	(void) fflush (stderr);
    }

    errno = eindex;
}
