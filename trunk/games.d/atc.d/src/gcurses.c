/*
 * Copyright (c) 1987 by Ed James, UC Berkeley.  All rights reserved.
 *
 * Copy permission is hereby granted provided that this notice is
 * retained on all partial or complete copies.
 *
 * For more info on this and all of my stuff, mail edjames@berkeley.edu.
 */

#include "include.h"
#include <curses.h>
#ifndef A_REVERSE		/* If not using termio */
#  define erasechar()	(_tty.sg_erase)
#  define killchar()	(_tty.sg_kill)
#endif

#define C_TOPBOTTOM		'-'
#define C_LEFTRIGHT		'|'
#define C_AIRPORT		'='
#define C_LINE			'+'
#define C_BACKROUND		'.'
#define C_BEACON		'*'
#define C_CREDIT		'*'

WINDOW	*radar, *cleanradar, *credit, *input, *planes;

char erase_char, kill_char;

getAChar()
{
	int ch;
#ifdef M_XENIX
	fixtty();
#endif
	ch = getchar();
	return (ch == '\r'? '\n' : ch);
}

putAChar(c)
char c;
{
	putchar(c);
}

erase_all()
{
	PLANE	*pp;

	for (pp = air.head; pp != NULL; pp = pp->next) {
		wmove(cleanradar, pp->ypos, pp->xpos * 2);
		wmove(radar, pp->ypos, pp->xpos * 2);
		waddch(radar, winch(cleanradar));
		wmove(cleanradar, pp->ypos, pp->xpos * 2 + 1);
		wmove(radar, pp->ypos, pp->xpos * 2 + 1);
		waddch(radar, winch(cleanradar));
	}
}

draw_all()
{
	PLANE	*pp;

	for (pp = air.head; pp != NULL; pp = pp->next) {
		if (pp->status == S_MARKED)
			wstandout(radar);
		wmove(radar, pp->ypos, pp->xpos * 2);
		waddch(radar, name(pp));
		waddch(radar, '0' + pp->altitude);
		if (pp->status == S_MARKED)
			wstandend(radar);
	}
	wrefresh(radar);
	planewin();
	wrefresh(input);		/* return cursor */
	fflush(stdout);
}

init_gr()
{
	static char	buffer[BUFSIZ];

	setbuf(stdout, buffer);
	initscr();
	noraw();
	crmode();
	noecho();
	erase_char = erasechar();
	kill_char = killchar();
	input = newwin(INPUT_LINES, COLS - PLANE_COLS, LINES - INPUT_LINES, 0);
	credit = newwin(INPUT_LINES, PLANE_COLS, LINES - INPUT_LINES, 
		COLS - PLANE_COLS);
	planes = newwin(LINES - INPUT_LINES, PLANE_COLS, 0, COLS - PLANE_COLS);
}

setup_screen(scp)
	screen_t	*scp;
{
	register int	i, j;
	char		str[3], *airstr;

	str[2] = '\0';

	if (radar != NULL)
		delwin(radar);
	radar = newwin(scp->height, scp->width * 2, 0, 0);

	if (cleanradar != NULL)
		delwin(cleanradar);
	cleanradar = newwin(scp->height, scp->width * 2, 0, 0);

	/* minus one here to prevent a scroll */
	for (i = 0; i < PLANE_COLS - 1; i++) {
		wmove(credit, 0, i);
		waddch(credit, C_CREDIT);
		wmove(credit, INPUT_LINES - 1, i);
		waddch(credit, C_CREDIT);
	}
	wmove(credit, INPUT_LINES / 2, 1);
	waddstr(credit, AUTHOR_STR);

	for (i = 1; i < scp->height - 1; i++) {
		for (j = 1; j < scp->width - 1; j++) {
			wmove(radar, i, j * 2);
			waddch(radar, C_BACKROUND);
		}
	}

	/*
	 * Draw the lines first, since people like to draw lines
	 * through beacons and exit points.
	 */
	str[0] = C_LINE;
	for (i = 0; i < scp->num_lines; i++) {
		str[1] = ' ';
		draw_line(radar, scp->line[i].p1.x, scp->line[i].p1.y,
			scp->line[i].p2.x, scp->line[i].p2.y, str);
	}

	str[0] = C_TOPBOTTOM;
	str[1] = C_TOPBOTTOM;
	wmove(radar, 0, 0);
	for (i = 0; i < scp->width - 1; i++)
		waddstr(radar, str);
	waddch(radar, C_TOPBOTTOM);

	str[0] = C_TOPBOTTOM;
	str[1] = C_TOPBOTTOM;
	wmove(radar, scp->height - 1, 0);
	for (i = 0; i < scp->width - 1; i++)
		waddstr(radar, str);
	waddch(radar, C_TOPBOTTOM);

	for (i = 1; i < scp->height - 1; i++) {
		wmove(radar, i, 0);
		waddch(radar, C_LEFTRIGHT);
		wmove(radar, i, (scp->width - 1) * 2);
		waddch(radar, C_LEFTRIGHT);
	}

	str[0] = C_BEACON;
	for (i = 0; i < scp->num_beacons; i++) {
		str[1] = '0' + i;
		wmove(radar, scp->beacon[i].y, scp->beacon[i].x * 2);
		waddstr(radar, str);
	}

	for (i = 0; i < scp->num_exits; i++) {
		wmove(radar, scp->exit[i].y, scp->exit[i].x * 2);
		waddch(radar, '0' + i);
	}

	airstr = "^?>?v?<?";
	for (i = 0; i < scp->num_airports; i++) {
		str[0] = airstr[scp->airport[i].dir];
		str[1] = '0' + i;
		wmove(radar, scp->airport[i].y, scp->airport[i].x * 2);
		waddstr(radar, str);
	}
	
	overwrite(radar, cleanradar);
	wrefresh(radar);
	wrefresh(credit);
	fflush(stdout);
}

draw_line(w, x, y, lx, ly, s)
	WINDOW	*w;
	char	*s;
{
	int	dx, dy;

	dx = SGN(lx - x);
	dy = SGN(ly - y);
	for (;;) {
		wmove(w, y, x * 2);
		waddstr(w, s);
		if (x == lx && y == ly)
			break;
		x += dx;
		y += dy;
	}
}

ioclrtoeol(pos)
{
	wmove(input, 0, pos);
	wclrtoeol(input);
	wrefresh(input);
	fflush(stdout);
}

iomove(pos)
{
	wmove(input, 0, pos);
	wrefresh(input);
	fflush(stdout);
}

ioaddstr(pos, str)
	char	*str;
{
	wmove(input, 0, pos);
	waddstr(input, str);
	wrefresh(input);
	fflush(stdout);
}

ioclrtobot()
{
	wclrtobot(input);
	wrefresh(input);
	fflush(stdout);
}

ioerror(pos, len, str)
	char	*str;
{
	int	i;

	wmove(input, 1, pos);
	for (i = 0; i < len; i++)
		waddch(input, '^');
	wmove(input, 2, 0);
	waddstr(input, str);
	wrefresh(input);
	fflush(stdout);
}

SIG_T
quit(dummy)
{
	int			c, y, x;

	signal(SIGINT, quit);
	signal(SIGQUIT, quit);
	getyx(input, y, x);
	wmove(input, 2, 0);
	waddstr(input, "Really quit? (y/n) ");
	wclrtobot(input);
	wrefresh(input);
	fflush(stdout);
	c = getchar();
	if (c == EOF || c == 'y') {
		alarm(0); /* disable timer */
		fflush(stdout);
		clear();
		refresh();
		endwin();
		log_score(0);
		exit(0);
	}
	wmove(input, 2, 0);
	wclrtobot(input);
	wmove(input, y, x);
	wrefresh(input);
	fflush(stdout);
	return;
}

planewin()
{
	PLANE	*pp;
	char	*command();
	int	warning = 0;

	werase(planes);
	wmove(planes, 0,0);
	wprintw(planes, "Time: %-4d Safe: %d", gclock, safe_planes);
	wmove(planes, 2, 0);

	waddstr(planes, "pl dt  comm");
	for (pp = air.head; pp != NULL; pp = pp->next) {
		if (waddch(planes, '\n') == ERR) {
			warning++;
			break;
		}
		waddstr(planes, command(pp));
	}
	waddch(planes, '\n');
	for (pp = ground.head; pp != NULL; pp = pp->next) {
		if (waddch(planes, '\n') == ERR) {
			warning++;
			break;
		}
		waddstr(planes, command(pp));
	}
	if (warning) {
		wmove(planes, LINES - INPUT_LINES - 1, 0);
		waddstr(planes, "---- more ----");
		wclrtoeol(planes);
	}
	wrefresh(planes);
	fflush(stdout);
}

loser(p, s)
	PLANE	*p;
	char	*s;
{
	int			c;

	/* disable timer */
	alarm(0);
	wmove(input, 0, 0);
	wclrtobot(input);
	wprintw(input, "Plane '%c' %s\n\nHit space for top players list...",
		name(p), s);
	wrefresh(input);
	fflush(stdout);
	while ((c = getchar()) != EOF && c != ' ')
		;
	clear();	/* move to top of screen */
	refresh();
	endwin();
	log_score(0);
	exit(0);
}

redraw()
{
	clear();
	refresh();

	touchwin(radar);
	wrefresh(radar);
	touchwin(planes);
	wrefresh(planes);
	touchwin(credit);
	wrefresh(credit);

	/* refresh input last to get cursor in right place */
	touchwin(input);
	wrefresh(input);
	fflush(stdout);
}

#ifndef A_REVERSE	/* If not using terminfo */
flash()
{
	if (VB != 0)
		tputs(VB, 1, putAChar);
	else
		putAChar('\007');
	fflush(stdout);
}
#endif
