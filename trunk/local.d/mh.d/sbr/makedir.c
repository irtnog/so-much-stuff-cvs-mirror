/* makedir.c - make a directory */

#if defined (BSD42) || defined (hpux)
/* Modified to try recursive create.  Really, this should be broken
 * out into a subr so that SYS5 systems can do this too.  I don't 
 * have a SYS5 machine around to test anymore, so someone else will
 * have to send me the code.
 */
#endif

#include "../h/mh.h"
#include <stdio.h>

#ifdef	SYS5DIR
#include <sys/types.h>
#include <sys/stat.h>
#endif	SYS5DIR
#if defined (BSD42)  || defined (hpux) || defined(SYS5)
#include <errno.h>
#include <sys/param.h>
#include <sys/file.h>
#endif BDS42
#if defined(SYS5)
#include <limits.h>
#include <unistd.h>
#ifndef MAXPATHLEN
#define MAXPATHLEN PATH_MAX
#endif
#endif SYS5

extern int  errno;
	
makedir (dir)
register char *dir;
{
    int     pid;
    register char  *cp;
#if defined (BSD42)  || defined (hpux) || defined (SYS5DIR)
    register char  *c;
    char path[MAXPATHLEN];
#endif BSD42

    m_update ();
    (void) fflush (stdout);

#if	defined (BSD42) ||  defined (hpux) || defined (SYS5DIR)
    if (getuid () == geteuid ()) {
	    c = strcpy(path, dir);     

	    while ((c = index((c + 1), '/')) != NULL) {	
		    *c = (char)0;
		    if (access(path, X_OK)) {
			    if (errno != ENOENT){
				    advise (dir, "unable to create directory");
				    return 0;
			    }			    
			    if (mkdir(path, 0775)) {
				    advise (dir, "unable to create directory");
				    return 0;
			    }
		    }
		    *c = '/';
	    }
 
	    if (mkdir (dir, 0755) == NOTOK) {
		    advise (dir, "unable to create directory");
		    return 0;
	    }
    }
    else
#endif	BSD42 or hpux or SYS5DIR
    switch (pid = vfork ()) {
	case NOTOK: 
	    advise ("fork", "unable to");
	    return 0;

	case OK: 
	    (void) setgid (getgid ());
	    (void) setuid (getuid ());

	    execl ("/bin/mkdir", "mkdir", dir, NULLCP);
	    execl ("/usr/bin/mkdir", "mkdir", dir, NULLCP);
	    fprintf (stderr, "unable to exec ");
	    perror ("mkdir");
	    _exit (-1);

	default: 
	    if (pidXwait (pid, "mkdir"))
		return 0;
	    break;
    }

    if ((cp = m_find ("folder-protect")) == NULL)
	cp = foldprot;
    (void) chmod (dir, atooi (cp));
    return 1;
}
