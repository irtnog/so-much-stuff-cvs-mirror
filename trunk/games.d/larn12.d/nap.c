/* nap.c		 Larn is copyrighted 1986 by Noah Morgan. */
#include <signal.h>
#ifdef M_XENIX
#include <sys/param.h>
#else
#include <sys/types.h>
#endif
#ifdef SYS5
#include <sys/times.h>
#else
#ifdef BSD
#include <sys/timeb.h>
#else
long times();
#endif
#endif

#ifndef HZ
#define HZ 60	/* Take a guess */
#endif
#define MS	(1000/HZ)	/* Milliseconds per clock tick	*/

/*
 *	routine to take a nap for n milliseconds
 */
nap(x)
	register int x;
	{
	if (x<=0) return; /* eliminate chance for infinite loop */
	lflush();
	if (x > 999) sleep(x/1000); else napms(x);
	}

#ifdef NONAP
napms(x)	/* do nothing */
	int x;
	{
	}
#else
#ifdef SYS5
/*	napms - sleep for time milliseconds - uses times() */
/* this assumes that times returns a relative time in 60ths of a second */
/* this will do horrible things if your times() returns seconds! */
napms(time)
	int time;
	{
	long matchclock;
	struct tms stats;

	if (time<=0) time=1; /* eliminate chance for infinite loop */
	if ((matchclock = times(&stats)) == -1 || matchclock == 0)
		return;	/* error, or BSD style times() */
	matchclock += (time / MS);		/* MS mlliseconds per tic */

	while(matchclock > times(&stats))
		;
	}

#else
#ifdef BSD
#ifdef SIGVTALRM
/* This must be BSD 4.2!  */
#include <sys/time.h>
#define bit(_a) (1<<((_a)-1))

static  nullf()
    {
    }

/*	napms - sleep for time milliseconds - uses setitimer() */
napms(time)
	int time;
    {
    struct itimerval    timeout;
    int     (*oldhandler) ();
    int     oldsig;

	if (time <= 0) return;

    timerclear(&timeout.it_interval);
    timeout.it_value.tv_sec = time / 1000;
    timeout.it_value.tv_usec = (time % 1000) * 1000;

    oldsig = sigblock(bit(SIGALRM));
    setitimer(ITIMER_REAL, &timeout, (struct itimerval *)0);
    oldhandler = signal(SIGALRM, nullf);
    sigpause(oldsig);
    signal(SIGALRM, oldhandler);
    sigsetmask(oldsig);
    }

#else
/*	napms - sleep for time milliseconds - uses ftime() */

static napms(time)
	int time;
	{
	/* assumed to be BSD UNIX */
	struct timeb _gtime;
	time_t matchtime;
	unsigned short matchmilli;
	register struct timeb *tp = & _gtime;

	if (time <= 0) return;
	ftime(tp);
	matchmilli = tp->millitm + time;
	matchtime  = tp->time;
	while (matchmilli >= 1000)
		{
		++matchtime;
		matchmilli -= 1000;
		}

	while(1)
		{
		ftime(tp);
		if ((tp->time > matchtime) ||
			((tp->time == matchtime) && (tp->millitm >= matchmilli)))
			break;
		}
	}
#endif
#else
static napms(time) int time; {}	/* do nothing, forget it */
#endif
#endif
#endif
