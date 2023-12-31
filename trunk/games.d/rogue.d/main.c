/*
 * #     #
 * #    #   #    #  #   ##  #    #   # 
 *		         #
 *
 * Exploring the dungeons of doom
 * Copyright (C) 1981 by Michael Toy, Ken Arnold, and Glenn Wichman
 * All rights reserved
 *
 * @(#)main.c	6.2 (Berkeley) 5/28/84
 */

#include <curses.h>
#include <signal.h>
#include <pwd.h>
#include "rogue.h"

int end_win = 0;

/*
 * leave:
 *	Leave quickly, but curteously
 */
SIG_T
leave(dummy)
{
	if (end_win)
	{
		mvcur(0, s_COLS - 1, s_LINES - 1, 0);
		refresh();
		endwin();
		end_win = 0;
	}
	(void) putchar('\n');
	exit(0);
}

/*
 * main:
 *	The main program, of course
 */
main(argc, argv, envp)
char **argv;
char **envp;
{
	register char *env;
	register struct passwd *pw;
#ifndef __STDC__
	struct passwd *getpwuid();
#endif
	char *getpass(), *crypt();
	int lowtime;

#ifndef DUMP
	signal(SIGQUIT, leave);
	signal(SIGILL, leave);
	signal(SIGTRAP, leave);
#ifndef SIGIOT
#define SIGIOT	SIGABRT
#endif
	signal(SIGIOT, leave);
#ifdef SIGEMT
	signal(SIGEMT, leave);
#endif
	signal(SIGFPE, leave);
	signal(SIGBUS, leave);
	signal(SIGSEGV, leave);
	signal(SIGSYS, leave);
#endif

#ifdef WIZARD
	/*
	 * Check to see if he is a wizard
	 */
	if (argc >= 2 && argv[1][0] == '\0')
		if (strcmp(PASSWD, crypt(getpass("Wizard's password: "), "mT")) == 0)
		{
			wizard = TRUE;
			player.t_flags |= SEEMONST;
			argv++;
			argc--;
		}
#endif

	/*
	 * get home and options from environment
	 */
	if ((env = getenv("HOME")) != NULL)
		strcpy(home, env);
	else if ((pw = getpwuid(getuid())) != NULL)
		strcpy(home, pw->pw_dir);
	else
		home[0] = '\0';
	strcat(home, "/");

	strcpy(file_name, home);
	strcat(file_name, "rogue.save");

	if ((env = getenv("ROGUEOPTS")) != NULL)
		parse_opts(env);
	if (env == NULL || whoami[0] == '\0')
		if ((pw = getpwuid(getuid())) == NULL)
		{
			printf("Say, who are you?\n");
			leave();
		}
		else
			strucpy(whoami, pw->pw_name, strlen(pw->pw_name));
	if (env == NULL || fruit[0] == '\0')
		strcpy(fruit, "slime-mold");
	initscr();			/* Start up cursor package */
	end_win = 1;			/* Remember to endwin() */
	crmode();

	/*
	 * check for print-score option
	 */
	open_score();
	if (argc == 2 && strcmp(argv[1], "-s") == 0)
	{
		noscore = TRUE;
		score(0, -1);
		leave();
	}
	init_check();			/* check for legal startup */
	if (argc == 2)
		if (!restore(argv[1], envp))	/* Note: restore will never return */
			leave();
	lowtime = (int) time((long *)NULL);
#ifdef WIZARD
	if (wizard && getenv("SEED") != NULL)
		dnum = atoi(getenv("SEED"));
	else
#endif
		dnum = lowtime + getpid();
#ifdef WIZARD
	if (wizard)
		printf("Hello %s, welcome to dungeon #%d", whoami, dnum);
	else
#endif
		printf("Hello %s, just a moment while I dig the dungeon...", whoami);
	fflush(stdout);
	seed = dnum;

	init_player();			/* Set up initial player stats */
	init_things();			/* Set up probabilities of things */
	init_names();			/* Set up names of scrolls */
	init_colors();			/* Set up colors of potions */
	init_stones();			/* Set up stone settings of rings */
	init_materials();		/* Set up materials of wands */
	setup();
	/*
	 * Set up windows
	 */
	if ((hw = newwin(s_LINES, s_COLS, 0, 0)) == 0)
				printf("Not enough memory"), endwin(), leave();
#ifdef WIZARD
	noscore = wizard;
#endif
	new_level();			/* Draw current level */
	/*
	 * Start up daemons and fuses
	 */
	daemon(doctor, 0, AFTER);
	fuse(swander, 0, WANDERTIME, AFTER);
	daemon(stomach, 0, AFTER);
	daemon(runners, 0, AFTER);
	playit();
}

/*
 * endit:
 *	Exit the program abnormally.
 */
SIG_T
endit(dummy)
{
	fatal("Ok, if you want to leave that badly, I'll have to allow it\n");
}

/*
 * fatal:
 *	Exit the program, printing a message.
 */
fatal(s)
char *s;
{
	clear();
	move(s_LINES-2, 0);
	printw("%s", s);
	leave();
}

/*
 * rnd:
 *	Pick a very random number.
 */
rnd(range)
register int range;
{
	return range == 0 ? 0 : abs((int) RN) % range;
}

/*
 * roll:
 *	Roll a number of dice
 */
roll(number, sides)
register int number, sides;
{
	register int dtotal = 0;

	while (number--)
		dtotal += rnd(sides)+1;
	return dtotal;
}
#ifdef SIGTSTP
/*
 * tstp:
 *	Handle stop and start signals
 */
SIG_T
tstp(dummy)
{
	register int y, x;
	register int oy, ox;

	getyx(curscr, oy, ox);
	mvcur(0, s_COLS - 1, s_LINES - 1, 0);
	endwin();
	fflush(stdout);
	kill(0, SIGTSTP);
	signal(SIGTSTP, tstp);
	crmode();
	noecho();
	clearok(curscr, TRUE);
	wrefresh(curscr);
	getyx(curscr, y, x);
	mvcur(y, x, oy, ox);
	fflush(stdout);
#ifndef CYGWIN
	curscr->_cury = oy;
	curscr->_curx = ox;
#endif
}
#endif

#ifndef A_REVERSE	/* Don't have terminfo? */
unsigned  bauds[] = {
	   0,   50,    75,   110,
	 134,  150,   200,   300,
	 600, 1200,  1800,  2400,
	4800, 9600, 19200, 38400
};
#  define baudrate()	(bauds[_tty.sg_ospeed])
#endif
/*
 * playit:
 *	The main loop of the program.  Loop until the game is over,
 *	refreshing things and looking at the proper times.
 */
playit()
{
	register char *opts;
	extern short ospeed;

	/*
	 * set up defaults for slow terminals
	 */
	if (baudrate() <= 1200)
		jump = TRUE;

	/*
	 * parse environment declaration of options
	 */
	if ((opts = getenv("ROGUEOPTS")) != NULL)
		parse_opts(opts);


	oldpos = hero;
	oldrp = roomin(&hero);
	while (playing)
		command();			/* Command execution */
	endit();
}

/*
 * quit:
 *	Have player make certain, then leave.
 */
SIG_T
quit(dummy)
{
	register int oy, ox;
	register char c;

	/*
	 * Reset the signal in case we got here via an interrupt
	 */
	if (signal(SIGINT, quit) != quit)
		mpos = 0;
	getyx(curscr, oy, ox);
	msg("Really quit?");
	if ((c = readchar()) == 'y')
	{
		signal(SIGINT, leave);
		clear();
		mvprintw(s_LINES - 2, 0, "You quit with %d gold pieces", purse);
		move(s_LINES - 1, 0);
		refresh();
		score(purse, 1);
		leave();
	}
	else
	{
#ifdef WIZARD
		if (wizard && c == MYCTRL('P'))
				no_command = 0;
#endif
		move(0, 0);
		clrtoeol();
		status();
		move(oy, ox);
		refresh();
		mpos = 0;
		count = 0;
	}
}

/*
 * shell:
 *	Let him escape for a while
 */
shell()
{
	register int pid;
	register char *sh;
	int ret_status;

	/*
	 * Set the terminal back to original mode
	 */
	move(s_LINES-1, 0);
	refresh();
	(void) putchar('\n');
	in_shell = TRUE;
	after = FALSE;
	sh = getenv("SHELL");
	fflush(stdout);
	/*
	 * Fork and do a shell
	 */
	while ((pid = fork()) < 0)
		sleep(1);
	if (pid == 0)
	{
		execl(sh == NULL ? "/bin/sh" : sh, "shell", "-i", 0);
		printf("No shell");
		exit(-1);
	}
	else
	{
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		while (wait(&ret_status) != pid)
			continue;
		signal(SIGINT, quit);
		signal(SIGQUIT, endit);
		printf("\n[Press return to continue]");
		noecho();
		crmode();
		in_shell = FALSE;
		wait_for('\n');
		clearok(stdscr, TRUE);
	}
}
