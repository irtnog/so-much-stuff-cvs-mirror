/* config.h
 * This file was produced by running the Configure script.
 * Feel free to modify any of this as the need arises.
 */

/* name of the site.  May be overridden by gethostname, uname, etc. */
#define SITENAME "snuggles"

/* name of the organization, may be a file name */
#define ORGNAME "/usr/lib/news/organization"

/* login name of news administrator, if any. */
#define NEWSADMIN "news"

/* news library, may use only ~ and %l expansion */
#define LIB "/usr/lib/news"

/* rn private library, may use ~ expansion, %x and %l */
#define RNLIB "/usr/lib/news/rn"

/* location of the news spool directory, may use ~ expansion, %x and %l */
#define SPOOL "/usr/spool/news"

/* location of the active file, may use ~ expansion, %x and %l */
#define ACTIVE "/usr/lib/news/active"

/* location of spooled mail */
#define MAILFILE "/usr/spool/mail/%L"

/* default shell--ok to be a slow shell like csh */
#define PREFSHELL "/bin/sh"

/* default editor */
#define DEFEDITOR "/bin/vi"

/* root uid */
#define ROOTID 0

/* what is the first character of a mailbox? */
#define MBOXCHAR 'F'

/* how to cancel an article */
#define CANCEL "/usr/lib/news/inews -h <%h"

/* distribution groups */
#define LOCDIST "iso"
#define ORGDIST "intel"
#define CITYDIST "pdx"
#define STATEDIST "or"
#define CNTRYDIST "usa"
#define CONTDIST "na"

#define	index strchr	/* cultural */
#define	rindex strrchr	/*  differences? */
/*#undef	void int	/* is void to be avoided? */
#define	vfork fork	/* is vfork too virtual? */
#undef	EUNICE		/* no linking? */
#undef	VMS		/* not currently used, here just in case */
#define	USENDIR		/* include ndir.c? */
#undef	LIBNDIR		/* include /usr/include/ndir.h? */
#define	MININACT	/* include 2.10.2 optimization? */
#define	PORTABLE	/* do we do extra lookups to start up? */
#define	PASSNAMES	/* do names come from the passwd file? */
				/*  (undef to take name from ~/.fullname) */
#define	BERKNAMES	/* if so, are they Berkeley format? */
				/* (that is, ":name,stuff:") */
#undef	USGNAMES	/* or are they USG format? */
				/* (that is, ":stuff-name(stuff):") */
#undef	WHOAMI		/* should we include whoami.h? */
#define	TERMIO		/* is this a termio system? */
#define		FCNTL		/* should we include fcntl.h? */
#define		IOCTL		/* are ioctl args all defined in one place? */
#define	NORMSIG		/* use signal rather than sigset? */
#define	HAVETERMLIB	/* do we have termlib-style routines? */
#undef	GETPWENT	/* need we include slow getpwent? */
#undef	INTERNET	/* does our mailer do INTERNET addressing? */
#undef	GETHOSTNAME	/* do we have a gethostname function? */
#define	DOUNAME		/* do we have a uname function? */
/*#undef	PHOSTNAME "hostname"	/* how to get host name with popen */
#define	NORELAY		/* 2.10.3 doesn't have Relay-Version line */
#define ZEROGLOB
