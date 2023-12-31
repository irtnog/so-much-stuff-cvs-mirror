static char *sccsid = "@(#)clear.c	4.1 (Berkeley) 10/1/80";
/* load me with -ltermlib
 * include <retrofit.h> on version 6
 *
 * clear - clear the screen
*/

#include <stdio.h>
#include <curses.h>
#ifdef SYS5
#include <termios.h>
#else
#include <sgtty.h>
#endif

char	*getenv();
char	*tgetstr();
char	PC;
#ifdef __STDC__
speed_t	ospeed;
#else
short	ospeed;
#endif
#undef	putchar
int	putchar();

main()
{
	char *cp = getenv("TERM");
	char clbuf[20];
	char pcbuf[20];
	char *clbp = clbuf;
	char *pcbp = pcbuf;
	char *clear;
	char buf[1024];
	char *pc;
#ifdef SYS5
	struct termios tty;

	tcgetattr(1, &tty);
	ospeed = tty.c_ospeed;
#else
	struct sgttyb tty;

	gtty(1, &tty);
	ospeed = tty.sg_ospeed;
#endif
	if (cp == (char *) 0)
		exit(1);
	if (tgetent(buf, cp) != 1)
		exit(1);
	pc = tgetstr("pc", &pcbp);
	if (pc)
		PC = *pc;
	clear = tgetstr("cl", &clbp);
	if (clear)
		tputs(clear, tgetnum("li"), putchar);
	exit (clear != (char *) 0);
}
