# include	"mille.h"
# include	<signal.h>

SIG_T	rub();

char	sobuf[BUFSIZ];

main(ac, av)
reg int		ac;
reg char	*av[]; {

	reg bool	restore;

# if 0
	if (getuid() == GURP) {
		printf("%s: Permission denied\n", av[0]);
		exit(1);
	}
# endif
	if (strcmp(av[0], "a.out") == 0) {
		outf = fopen("q", "w");
		setbuf(outf, 0);
		Debug = TRUE;
	}
	restore = FALSE;
# ifdef pdp11
	if (geteuid() != ARNOLD)
		maxusers(MAXUSERS, NULL);
# endif
	switch (ac) {
	  case 2:
		rest_f(av[1]);
		restore = TRUE;
	  case 1:
		break;
	  default:
		printf("usage: milles [ restore_file ]\n");
		exit(-1);
		/* NOTREACHED */
	}
	setbuf(stdout, sobuf);
	Play = PLAYER;
	initscr();
	delwin(stdscr);
	Board = newwin(BOARD_Y, BOARD_X, 0, 0);
	overwrite(stdscr, Board);
	Score = newwin(SCORE_Y, SCORE_X, 0, 40);
	Miles = newwin(MILES_Y, MILES_X, 17, 0);
	leaveok(Score, TRUE);
	leaveok(Miles, TRUE);
	clearok(curscr, TRUE);
# ifndef PROF
	srand(getpid());
# else
	srand(0);
# endif
	crmode();
	noecho();
	nonl();
	signal(SIGINT, rub);
	for (;;) {
		if (!restore || (Player[PLAYER].total >= 5000
		    || Player[COMP].total >= 5000)) {
			if (Player[COMP].total < Player[PLAYER].total)
				Player[PLAYER].games++;
			else if (Player[COMP].total > Player[PLAYER].total)
				Player[COMP].games++;
			Player[COMP].total = 0;
			Player[PLAYER].total = 0;
		}
		do {
			if (!restore)
				Handstart = Play = other(Handstart);
			if (!restore || On_exit) {
				shuffle();
				init();
			}
			newboard();
			if (restore)
				mvwaddstr(Score, ERR_Y, ERR_X, Initstr);
			prboard();
			do {
				domove();
				if (Finished)
					newscore();
				prboard();
			} while (!Finished);
			check_more();
			restore = On_exit = FALSE;
		} while (Player[COMP].total < 5000
		    && Player[PLAYER].total < 5000);
	}
}

/*
 *	Routine to trap rubouts, and make sure they really want to
 * quit.
 */
SIG_T
rub(dummy) {

	signal(SIGINT, SIG_IGN);
	if (getyn("Really? "))
		die();
	signal(SIGINT, rub);
}

/*
 *	Time to go beddy-by
 */
die() {

	signal(SIGINT, SIG_IGN);
	if (outf)
		fflush(outf);
	mvcur(0, COLS - 1, LINES - 1, 0);
	endwin();
	exit(1);
}
