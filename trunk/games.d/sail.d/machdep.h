/*
 * sccsid = "@(#)machdep.h	2.5 4/28/84";
 */
#ifdef __STDC__
#define STR(x)		#x
#define STRING(x)	STR(x)
#define FILENM(x)	STRING(GAMLIB) "/" STRING(x)
#else
#define STRING(x)	"x
#define FILENM(x)	STRING(GAMLIB)/x"
#endif
#define LOGFILE FILENM(saillog)
#define DRIVER1 "driver"
#define DRIVER2 FILENM(saildriver)
#define DRIVER3 FILENM(.driver)
#define DRIVERNAME "driver"
#define SETUID			/* player and driver run setuid */

#define TIMEOUT 300		/* Sync() time out */

#define BUFSIZE 4096

#ifdef BERKELEY
/* for 4.2bsd machines */
#define blockalarm()	((void) sigblock(1 << SIGALRM-1))
#define unblockalarm()	((void) sigsetmask(sigblock(0) & ~(1 << SIGALRM-1)))

/* for 2.9bsd machines (onyx)
typedef int void;
#define blockalarm()	((void) sighold(SIGALRM))
#define unblockalarm()	((void) sigrelse(SIGALRM))
*/
#else
/* Do these right */
extern void blockalarm();
extern void unblockalarm();
#endif
