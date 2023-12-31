/*
** timing.c -	functions dealing with the "smooth" running of the game
**		it is important not for the game to get ahead of the screen
**		and the necessary `slowing down' of the game is done here
**
**	[pm by Peter Costantinidis, Jr. @ University of California at Davis]
*/

#include <signal.h>
#include "pm.h"
#include <sys/times.h>
#ifdef __STDC__
#include <termios.h>
#endif

/*
** delay()	- coordinate with tty speed (let the driver do it)
*/
void	delay ()
{
#ifndef TCGETA
#ifdef __STDC__
	struct termios buf;
	tcgetattr(1, &buf);
	tcsetattr(1, TCSADRAIN, &buf);
#else
	struct sgttyb buf;
	ioctl(1, TIOCGETP, &buf);
	ioctl(1, TIOCSETP, &buf);
#endif
#else
	struct termio buf;
	ioctl(1, TCGETA, &buf);
	ioctl(1, TCSETAW, &buf);
#endif
}

/*
** msleep()	- sleep the specified number of milliseconds
*/
void	msleep (u)
reg	unss	u;
{
#ifdef	ITIMER_REAL
	static	struct	itimerval	oitv, itv =
	{
		{	0L, 0L },
		{	0L, 0L }
	};
	extern	int	wakeup();

#ifndef	LINT
	signal(SIGALRM, wakeup);
#endif
	itv.it_value.tv_sec = u / 1000;
	itv.it_value.tv_usec = u % 1000;
	if (setitimer(ITIMER_REAL, &itv, &oitv))
	{
		fprintf(stderr, "%s: setitimer() error\n", argv0);
		perror("setitimer");
		quitit();
	}
	pause();
#else
	struct tms buf;
	time_t end;

	end = times(&buf) + ((long)u*HZ+999)/1000;
	while (times(&buf) < end)
		;
#endif
}

#ifdef	ITIMER_REAL
/*
** wakeup()	- someplace to go when the alarm goes off
*/
static	int	wakeup ()
{
	return(0);
}
#endif

static	int	rates[] =	/* these were `tuned' after much playing */
{
/*        0    1    2    3    4    5    6    7    8    9	*/
	430, 395, 380, 350, 340, 310, 325, 330, 325, 300,
/*       10   11   12   13   14   15   16   17   18   19	*/
	305, 325, 360, 320, 290, 260, 220, 190, 140, 120,
/*       20   21   22   23   24   25   26   27   28   29	*/
	 95,  70,  45,  20, 100, 200,  50,  99, 200, 300,
/*       30   31   32   33   34   35   36   37   38   39	*/
	  5,   0,   0,   0,   0,   0,   0,   0,   0,   0,
/*       40   41   42   43   44   45   46   47   48   49	*/
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
/*       50   51   52   53   54   55   56   57   58   59	*/
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

/*
** slow()	- make the game go faster as they go deeper
**		- assuming that they WILL NOT GET DEEPER THAN
**		  50 LEVELS!!!!  if they are, then they have been
**		  tieing up the computer long enough and should stop
**		  playing anyways
*/
void	slow (flag)
reg	int	flag;
{
	reg	time_t	ms;
	auto	struct	tms	buf;
	static	time_t	tp;
	auto	time_t	tp2;

	if (flag)
	{
		tp = times(&buf);
		return;
	}
	if (level > 59)
		quitit();
	tp2 = times(&buf);
	ms = (int) ((tp2-tp)*1000L)/HZ;
	if (fast)
	{
		if (ms < rates[level]/2)
			msleep((rates[level]/2) - ms);
	}
	else
	{
		if (ms < rates[level])
			msleep(rates[level] - ms);
	}
	tp = times(&buf);
}

/*
** slowness()	- sets delay padding for number of nulls to be printed
**		  each loop
*/
void	slowness ()
{
	auto	char	buf[BUFSIZ];

	clear();
	echo();
	nocrmode();
	printw("old delay: %d, new delay: ", rates[level]);
	if (!gets(buf))
		msg("EOF in slowness");
	if (buf[0])
		if (sscanf(buf, "%d", &(rates[level])) == EOF)
			msg("EOF2 in slowness");
	noecho();
	crmode();
	redraw();
}

/*
** to_baud()	- convert the given string to an appropriate baud define
*/
char	to_baud (s)
reg	char	*s;
{
	reg	int	i;
	static	char	*sbauds[] =
	{
		"0", "50", "75", "110", "134", "150",
		"200", "300", "600", "1200", "1800", "2400",
		"4800", "9600", "EXTA", "EXTB",
		0
	};

	for (i = 0; sbauds[i]; i++)
		if (!strcmp(sbauds[i], s))
			return((char) i);
	return('\0');
}
