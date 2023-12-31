/*
** make_moves -	code relating to player movement
**
**	[pm by Peter Costantinidis, Jr. @ University of California at Davis]
*/
#include "pm.h"
#include <fcntl.h>
#include <signal.h>

/*
** make_moves -	`ch' is the global variable designating the move of the
**		player.  perhaps it would have been better to pass this
**		character to this function instead of using a global.
*/
int	make_moves ()
{
	reg	char	what;
	reg	int	quit = FALSE;
	auto	coord	tmp_pos;

	if (pm_tunn)
	{	/* in tunnel	*/
		if (pm_extunn--)
		{	/* still in tunnel, move over a square	*/
			switch (ch)
			{
				when MLEFT:
					if (--pm_pos.x < LEFT)
						pm_pos.x = RIGHT;
				when MRIGHT:
					if (++pm_pos.x > RIGHT)
						pm_pos.x = LEFT;
				otherwise:
					msg("case error in tunnel");
			}
			/*
			** check for monsters here!!!!
			** to see if they have run into any in the tunnel
			*/
Above:	/* sorry about this! */
			switch (what = tunn_look(&pm_pos))
			{
				case EMPTY:
					return(msg("In and out of tunn"),quit);
				case TUNNEL:
					return(quit);
				case PM:	/* nothing here but me! */
					return(quit);
			}
			if (!is_mons(what))
				return(msg("found: %s in tunn", punctrl(what)),quit);
			if (islower(what))  /* we have caught a monster	*/
				return(pm_eat_m(what), quit);
			if (isupper(what))
			{
				pm_eaten = TRUE;
				m_eat_pm(wh_mons(what));
				return(quit);
			}
			msg("What was that???");
		}
		else
		{
			pm_tunn = FALSE;
			tmp_pos.x = pm_pos.x;
			tmp_pos.y = pm_pos.y;
			newch = ch;
			goto here;
		}
	}
	else
		mvaddch(pm_pos.y, pm_pos.x, ' ');
	if (pending())
	{
		if (isupper(newch = getch()))
			newch = tolower(newch);
		else if (isdigit(newch))
			newch = toletter(newch);
		move_cntr++;
	}
	if (newch)
	{
		oldch = ch;
		ch = newch;
	}
	if (!ch)
		return(quit);
top:
	tmp_pos.x = pm_pos.x;
	tmp_pos.y = pm_pos.y;
	switch (moveit(ch, &tmp_pos))
	{
		when FINE:
		when ERROR:
			ch = oldch;
			oldch = '\0';
			newch = '\0';
			goto top;
		when QUIT:
			quit = TRUE;
	}
here:
	if (is_safe(&tmp_pos))
		pm_pos.x = tmp_pos.x, pm_pos.y = tmp_pos.y;
	else if (pm_eaten)
		return(0);
	else if (pm_tunn)
		goto Above;
	else
	{
#ifdef	USELESS
/*
** maybe the next `if' statement should have been an `else if',
** but until that has been ascertained, this one is useless
*/
		if ((newch == ch) && oldch)
		{
			ch = oldch;
			oldch = '\0';
			goto top;
		}
#endif
		if (oldch)
		{
			ch = oldch;
			oldch = '\0';
			goto top;
		}
	}
	/*
	** redraw pm and leave cursor there if not in tunnel
	*/
	if (!pm_tunn)
	{
		move(pm_pos.y, pm_pos.x);
		addch(PM);
		move(pm_pos.y, pm_pos.x);
	}
	draw();
	return(quit);
}

/*
** pending()	- return TRUE if the user wants service, else return FALSE
**		- i realize that this function could be simpler (ex. just
**		  return((int) l)), but i wanted to minimize lint's complaints
*/
int	pending ()
{
#ifdef	FIONREAD
	auto	long	l;

	if (ioctl(0, FIONREAD, &l) == -1)
		return(FALSE);
	return(l > 0 ? TRUE : FALSE);
#else
	int flags, ch;
	void (*intr)() = signal(SIGINT, SIG_IGN);
	void (*hup)()  = signal(SIGHUP, SIG_IGN);
	void (*quit)() = signal(SIGQUIT, SIG_IGN);
	unsigned alrm  = alarm(0);

	flags = fcntl(0, F_GETFL, 0);
	fcntl(0, F_SETFL, flags|O_NDELAY);
	if ((ch = getch()) != EOF) {
		ungetch(ch);
		ch = 1;
	} else
		ch = 0;
	fcntl(0, F_SETFL, flags);
	(void) signal(SIGINT, intr);
	(void) signal(SIGHUP, hup);
	(void) signal(SIGQUIT, quit);
	(void) alarm(alrm);
	return(ch);
#endif
}

/*
** is_safe()	- returns TRUE if location is safe
**		- also assumes that move will be made regardless
**		  of safeness
*/
int	is_safe (where)
reg	coord	*where;
{
	reg	char	what;

	move(where->y, where->x);
	what = INCH();
	move(pm_pos.y, pm_pos.x);
	switch (what)
	{
		case BLOCK:
		case DOOR:
			return(FALSE);
		case TUNNEL:
			pm_tunn = TRUE;
			pm_extunn = TUNN_TIME;
			return(TRUE);
		case DOT:
			thescore += V_DOT;
			d_left--;
			if (!quiet)
				beep();
			return(TRUE);
		case ENERGY:
			thescore += V_ENERGY;
			e_left--;
			if (!quiet)
				beep();
			submissive();
			return(TRUE);
		case EMPTY:
			return(TRUE);
		case PM:
			msg("I'm going skitzo");
			return(FALSE);
	}
	if (IS_FRUIT(what))		/* check to see if it is a fruit */
	{
		thescore += fr_val;
		if (!quiet)
			beep();
		fr_val = 0;		/* shows fruit has been eaten	*/
		add_fruit(fr_ch);
		return(TRUE);
	}
#ifdef	DEBUG
	if (!is_mons(what))
		return(msg("found a %s in @ 226", punctrl(what)), FALSE);
#endif
	if (islower(what))		/* we have caught a monster	*/
	{
#ifdef	DEBUG
		if (pm_run)		/* remove message later on###	*/
			msg("Eatable, but not running");
		/*
		** may need this
		pm_pos.x = where->x;
		pm_pos.y = where->y;
		*/
#endif
		return(pm_eat_m(what), TRUE);
	}
	pm_eaten = TRUE;
	pm_pos.x = where->x;
	pm_pos.y = where->y;
	m_eat_pm(wh_mons(what));
	return(FALSE);
}

/*
** pm_eat_m()	- the pm ate the m!!!
**		- the variable flag is used to indicate that the
**		  monsters (including the one eaten) must become
**		  submissive (after the eaten one has been initialized)
*/
void	pm_eat_m (who)
reg	char	who;
{
	reg	mons	*m;
	reg	int	flag = FALSE;

	thescore += mons_val[++mons_eaten];
	if (mons_eaten == 3)
	{	/* all the monsters are eaten, reset the timer	*/
		timer = 0;
		mons_eaten = -1;
	}
	if (!(m = wh_mons(who)))
	{
		msg("Lost monster in pm_eat_m()");	
		return;
	}
	switch (m->mo_inch)	/* check what was underneath him*/
	{
		when DOT:
			thescore += V_DOT;
			d_left--;
			if (!quiet)
				beep();
		when ENERGY:
			flag = TRUE;
			thescore += V_ENERGY;
			e_left--;
			if (!quiet)
				beep();
	}
	m->mo_name = toupper(who);
	m_init(m);
	place_m(m);
	m->mo_eaten = TRUE;
	if (flag)
		submissive();
}

/*
** moveit()	- evaluate move and return status
*/
moveit (what, where)
reg	char	what;
reg	coord	*where;
{
	switch (what)
	{
		case MUP:
			return(where->y--, FINE);
		case MDOWN:
			return(where->y++, FINE);
		case MLEFT:
			return(where->x--, FINE);
		case MRIGHT:
			return(where->x++, FINE);
		case MSTOP:
			return(STOP);
		case MQUIT:
			return(QUIT);
		case MREDRAW:
			return(redraw(), ERROR);
		case MSHELL:
			return(shell(), ERROR);
		case MHELP:
			return(commands(), ERROR);
		case MFAST:
			return(fast = !fast, ERROR);
		case MQUIET:
			return(quiet = !quiet, ERROR);
		case MPAUSE:
			/*
			** they are not allowed to pause to examine
			** the board (for potential moves), so clear
			** the screen (and go to the bottom) while
			** they are paused
			*/
			if (is_wiz) /* wizard is an exception! */
				return(trash(getch()), ERROR);
			clear();
			move(LINES - 1, 0);
			draw();
			printw("[Hit return when ready] ");
			trash(getch());
			redraw();
			return(ERROR);
		case MHUH:
			return(re_msg(), ERROR);
		case MWIZARD:
			if (is_wiz)
			{
				msg("");
				is_wiz = FALSE;
				return(ERROR);
			}
			msg("Wizard's Password: ");
			if (!strcmp(W_PASSWD, crypt(get_pass(), SALT)))
			{
				was_wiz = TRUE;
				is_wiz = TRUE;
				if (getuid() != WIZARD_UID)
					msg("Are you trying to cheat?");
				else
					msg("Hi wiz!");
			}
			else
				msg("Who are you kidding?");
			return(ERROR);
		default:
			if (!is_wiz)
				return(ERROR);
	}
	/*
	** since they are wizard, lets try some of these
	*/
	switch (what)
	{
		when MPM:
			pms_left++, p_pms();
		when MSLOW:
			slowness();
		when MSTATUS:
			status();
		when MMONS:
			p_info(getch());
		when MUP_LVL:
			chg_lvl(1);
		when MDN_LVL:
			chg_lvl(-1);
		when MEAT:
			submissive();
		when MMEAN:
			aggressive();
	}
	return(ERROR);
}

/*
** commands()	- print a list of the users commands
**		- erase the screen by hand
*/
void	commands ()
{
	static	char	*cmds[] =
	{	"---------------------------------------------------",
		"|         Movement:         |   Misc:             |",
		"---------------------------------------------------",
		"|                           |                     |",
		"|             k             |    !       shell    |",
		"|             ^             |    q       quit     |",
		"|             ^             | <SPACE>    stop     |",
		"|             ^             |    f       faster   |",
		"|             ^             |    b       quiet    |",
		"|   h < < < < * > > > > l   |    p       pause    |",
		"|             v             |                     |",
		"|             v             |                     |",
		"|             v             |                     |",
		"|             v             |                     |",
		"|             j             |                     |",
		"|                           |                     |",
		"---------------------------------------------------",
		"\nHit return to continue",
		0
	};
	reg	char	**s = cmds;

	clear();
	while (*s)
		printw("%s\n", *s++);
	trash(getch());
	redraw();
}

/*
** status()	- print out a bunch of debugging info
*/
void	status ()
{
	alarm(0);
	clear();
	move(0, 0);
	printw("        Diagnostics\n\n");
	printw("Fruit:             %c\n", fr_ch);
	printw("Fruit value:       %d\n", fr_val);
	printw("Level:             %d\n", level);
	printw("Moves:             %ld\n", move_cntr);
	printw("Time:              %ld\n", demon);
	printw("Timeit:            %s\n", (timeit ? "Yes" : "No"));
	printw("Fast:              %s\n", (fast ? "Yes" : "No"));
	printw("Beeping:           %s\n", (quiet ? "No" : "Yes"));
	printw("Dots left:         %d\n", d_left);
	printw("Energizers left:   %d\n", e_left);
	printw("Pm's left:         %d\n", pms_left);
	printw("Time left:         %d\n", timer);
	printw("Score:             %ld\n", thescore);
	printw("Pos.:              (%d, %d)\n", pm_pos.x, pm_pos.y);
	printw("Tunn.:             %s\n",TF(pm_tunn));
	printw("Screen dimension   %d x %d\n", LINES, COLS);
	printw("High score:        %ld\n", hi_score);
	printw("Max's:             %d,%d\n", LINES, COLS);
	printw("\n");
	printw("\nHit return to continue\n");
	trash(getch());
	redraw();
}
