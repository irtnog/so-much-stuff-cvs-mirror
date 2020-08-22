/* config.h 1.1	(c) copyright 1986 (Dan Heller) */

#define MAILRC		".mushrc"
#define ALTERNATE_RC	".mailrc"
#define LIBDIR		"/usr/lib
#define DEFAULT_RC	LIBDIR/Mail.rc"
#define COMMAND_HELP	LIBDIR/cmd_help"
#ifdef SUNTOOL
#    define TOOL_HELP	LIBDIR/tool_help"
#endif SUNTOOL
#define ALTERNATE_HOME	"/tmp"       /* Path must be read/write to EVERYONE */
#define EDFILE  	".edXXXXXX"  /* file/pathname added to user's "home" */

/* mail delivery system */
#define MAIL_DELIVERY	"/bin/rmail"
/*#define VERBOSE_ARG	"-v"    /* undef if none exists */
/*#define METOO_ARG	"-m"    /* man sendmail for more info. */

#define	MAXMSGS		1000	/* maximum number of messages we can read */
#ifndef MAXPATHLEN
#define MAXPATHLEN 256
#endif

#define VPRINTF		/* If your system supports the vprintf() functions,
			 * True for sys-v and later sun versions (3.0+ ?).
			 */

/* The maximum number of hostnames YOUR host may have (usually found in
 * /etc/hosts for BSD systems).  System-V/XENIX doesn't use this (use 1).
 */
#define MAX_HOST_NAMES  1

#define LS_COMMAND	"ls"
#define FORTUNE		"/usr/games/fortune"
#define LPR		"lpr"
#define SIGNATURE	".signature"
#define MAILDIR		"/usr/spool/mail"

/* default settings for some variable strings */
#define DEF_PROMPT	"Msg %m of %t: "
#define DEF_PAGER	"more"
#define DEF_SHELL	"csh"
#define DEF_EDITOR	"vi"
#define DEF_FOLDER	"~/Mail"        /* default Mail folder */
#define DEF_MBOX	"~/mbox"	/* default mbox */
#define DEF_INDENT_STR	"> "		/* indent included mail */
#define DEF_PRINTER	"lp"
#define DEF_ESCAPE	"~"
#define DEF_HDR_FMT	"%S %25f %7d (%l/%c) %25s" /* headers format string */
