/*
 * Phantasia 3.3.2 -- Interterminal fantasy game
 *
 * Edward A. Estes
 * AT&T, March 12, 1986
 */

/* DISCLAIMER:
 *
 * This game is distributed for free as is.  It is not guaranteed to work
 * in every conceivable environment.  It is not even guaranteed to work
 * in ANY environment.
 *
 * This game is distributed without notice of copyright, therefore it
 * may be used in any manner the recipient sees fit.  However, the
 * author assumes no responsibility for maintaining or revising this
 * game, in its original form, or any derivitives thereof.
 *
 * The author shall not be responsible for any loss, cost, or damage,
 * including consequential damage, caused by reliance on this material.
 *
 * The author makes no warranties, express or implied, including warranties
 * of merchantability or fitness for a particular purpose or use.
 *
 * AT&T is in no way connected with this game.
 */

/*
 * The program allocates as much file space as it needs to store characters,
 * so the possibility exists for the character file to grow without bound.
 * The file is purged upon normal entry to try to avoid that problem.
 * A similar problem exists for energy voids.  To alleviate the problem here,
 * the void file is cleared with every new king, and a limit is placed
 * on the size of the energy void file.
 */

/*
 * Put one line of text into the file 'motd' for announcements, etc.
 */

/*
 * If ENEMY is #defined, a list of restricted login names is checked
 * in the file 'enemy'.  These names are listed, one per line, with
 * no trailing blanks.
 */

/*
 * The scoreboard file is updated when someone dies, and keeps track
 * of the highest character to date for that login.
 * Being purged from the character file does not cause the scoreboard
 * to be updated.
 */

/*
 * All source files are set up for 'vi' with shiftwidth=4, tabstop=8. 
 */

/**/

/*
 * main.c	Main routines for Phantasia
 */

#include "include.h"

/***************************************************************************
/ FUNCTION NAME: main()
/
/ FUNCTION: initialize state, and call main process
/
/ AUTHOR: E. A. Estes, 12/4/85
/
/ ARGUMENTS:
/	int	argc - argument count
/	char	**argv - argument vector
/
/ RETURN VALUE: none
/
/ MODULES CALLED: monstlist(), checkenemy(), ok_to_play(), activelist(), 
/	throneroom(), checkbattle(), readmessage(), changestats(), writerecord(), 
/	tradingpost(), adjuststats(), recallplayer(), displaystats(), checktampered(), 
/	fabs(), rollnewplayer(), time(), exit(), sqrt(), floor(), wmove(), 
/	signal(), strcat(), purgeoldplayers(), getuid(), isatty(), wclear(), 
/	strcpy(), system(), altercoordinates(), cleanup(), waddstr(), procmain(), 
/	playinit(), leavegame(), localtime(), getanswer(), neatstuff(), initstate(), 
/	scorelist(), titlelist()
/
/ GLOBAL INPUTS: *Login, Throne, Wizard, Player, *stdscr, Changed, Databuf[], 
/	Fileloc, Helpfile[], Stattable[]
/
/ GLOBAL OUTPUTS: Wizard, Player, Changed, Fileloc, Timeout, *Statptr
/
/ DESCRIPTION:
/	Process arguments, initialize program, and loop forever processing
/	player input.
/
/***************************************************************************/

main(argc, argv)
int	argc;
char	**argv;
{
bool	noheader = FALSE;	/* set if don't want header */
bool	headeronly = FALSE;	/* set if only want header */
bool	examine = FALSE;	/* set if examine a character */
long	seconds;		/* for time of day */
double	dtemp;			/* for temporary calculations */

    initstate();		/* init globals */

#ifdef ENEMY
    checkenemy();		/* check if denied access */
#endif

    /* process arguments */
    while (--argc && (*++argv)[0] == '-')
	switch ((*argv)[1])
	    {
	    case 's':	/* short */
		noheader = TRUE;
		break;

	    case 'H':	/* Header */
		headeronly = TRUE;
		break;

	    case 'a':	/* all users */
		activelist();
		cleanup(TRUE);
		/*NOTREACHED*/

	    case 'p':	/* purge old players */
		purgeoldplayers();
		cleanup(TRUE);
		/*NOTREACHED*/

	    case 'S':	/* set 'Wizard' */
#ifdef __STDC__
		Wizard = (getuid() == UIDA);
		if (!Wizard) {
		    struct passwd *pw = getpwuid(getuid());
		    Wizard = (pw && !strcmp(pw->pw_name, WIZARDB));
		}
#else
		Wizard = ((getuid() == UIDA) || (getuid() == UIDB));
#endif
		break;

	    case 'x':	/* examine */
		examine = TRUE;
		break;

	    case 'm':	/* monsters */
		monstlist();
		cleanup(TRUE);
		/*NOTREACHED*/

	    case 'b':	/* scoreboard */
		scorelist();
		cleanup(TRUE);
		/*NOTREACHED*/

	    case 'h':	/* help */
		cleanup(FALSE);
		strcpy(Databuf, "cat ");
		system(strcat(Databuf, Helpfile));
		exit(0);
		/*NOTREACHED*/
		}

    if (!isatty(0))		/* don't let non-tty's play */
	cleanup(TRUE);
	/*NOTREACHED*/

    playinit();			/* set up to catch signals, init curses */

    if (examine)
	{
	changestats(FALSE);
	cleanup(TRUE);
	/*NOTREACHED*/
	}

    if (!noheader)
	{
	titlelist();
	purgeoldplayers();    /* clean up old characters */
	}

    if (headeronly)
	cleanup(TRUE);
	/*NOTREACHED*/

#ifdef OK_TO_PLAY
    if (!ok_to_play())
	{
	mvaddstr(23, 27, "Sorry, you can't play now.\n");
	cleanup(TRUE);
	/*NOTREACHED*/
	}
#endif

    do
	/* get the player structure filled */
	{
	Fileloc = -1L;

	mvaddstr(22, 17, "Do you have a character to run [Q = Quit] ? ");

	switch (getanswer("NYQ", FALSE))
	    {
	    case 'Y':
		Fileloc = recallplayer();
		break;

	    case 'Q':
		cleanup(TRUE);
		/*NOTREACHED*/

	    default:
		Fileloc = rollnewplayer();
		break;
	    }
	clear();
	}
    while (Fileloc < 0L);

    if (Player.p_level > 5.0)
	/* low level players have long timeout */
	Timeout = TRUE;

    /* update some important player statistics */
    strcpy(Player.p_login, Login);
    time(&seconds);
    Player.p_lastused = localtime(&seconds)->tm_yday;
    Player.p_status = S_PLAYING;
    writerecord(&Player, Fileloc);

    Statptr = &Stattable[Player.p_type];	/* initialize pointer */

    /* catch interrupts */
#ifdef	BSD41
    sigset(SIGINT, interrupt);
#endif
#ifdef	BSD42
    signal(SIGINT, interrupt);
#endif
#ifdef	SYS3
    signal(SIGINT, interrupt);
#endif
#ifdef	SYS5
    signal(SIGINT, interrupt);
#endif

    altercoordinates(Player.p_x, Player.p_y, A_FORCED);	/* set some flags */

    clear();

    for (;;)
	/* loop forever, processing input */
	{
#ifdef OK_TO_PLAY
	if (!ok_to_play())
	    {
	    mvaddstr(6, 0, "Whoops!  Can't play now.\n");
	    leavegame();
	    /*NOTREACHED*/
	    }
#endif

	adjuststats();		/* cleanup stats */

	if (Throne && Player.p_crowns == 0 && Player.p_specialtype != SC_KING)
	    /* not allowed on throne -- move */
	    {
	    mvaddstr(5,0,"You're not allowed in the Lord's Chamber without a crown.\n");
	    altercoordinates(0.0, 0.0, A_NEAR);
	    }

	if (Player.p_status != S_CLOAKED
	    /* not cloaked */
	    && (dtemp = fabs(Player.p_x)) == fabs(Player.p_y)
	    /* |x| = |y| */
	    && !Throne)
	    /* not on throne */
	    {
	    dtemp = sqrt(dtemp / 100.0);
	    if (floor(dtemp) == dtemp)
		/* |x| / 100 == n*n; at a trading post */
		{
		tradingpost();
		clear();
		}
	    }

	checktampered();	/* check for energy voids, etc. */
	checkbattle();		/* check for player to player battle */
	neatstuff();		/* gurus, medics, etc. */

	if (Player.p_status == S_CLOAKED)
	    /* costs 3 mana per turn to be cloaked */
	    if (Player.p_mana > 3.0)
		Player.p_mana -= 3.0;
	    else
		/* ran out of mana, uncloak */
		{
		Player.p_status = S_PLAYING;
		Changed = TRUE;
		}

	if (Player.p_status != S_PLAYING && Player.p_status != S_CLOAKED)
	    /* change status back to S_PLAYING */
	    {
	    Player.p_status = S_PLAYING;
	    Changed = TRUE;
	    }

	if (Changed)
	    /* update file only if important stuff has changed */
	    {
	    writerecord(&Player, Fileloc);
	    Changed = FALSE;
	    continue;
	    }

	readmessage();			/* read message, if any */

	displaystats();			/* print statistics */

	move(6, 0);

	if (Throne)
	    /* maybe make king, print prompt, etc. */
	    throneroom();

	/* print status line */
	addstr("1:Move  2:Players  3:Talk  4:Stats  5:Quit  ");
	if (Player.p_level >= MEL_CLOAK && Player.p_magiclvl >= ML_CLOAK)
	    addstr("6:Cloak  ");
	if (Player.p_level >= MEL_TELEPORT && Player.p_magiclvl >= ML_TELEPORT)
	    addstr("7:Teleport  ");
	if (Player.p_specialtype >= SC_COUNCIL || Wizard)
	    addstr("8:Intervene  ");

	procmain();			/* process input */
	}
}
/**/
/************************************************************************
/
/ FUNCTION NAME: initstate()
/
/ FUNCTION: initialize some important global variable
/
/ AUTHOR: E. A. Estes, 12/4/85
/
/ ARGUMENTS: none
/
/ RETURN VALUE: none
/
/ MODULES CALLED: time(), fopen(), srand(), error(), getuid(), getlogin(), 
/	getpwuid()
/
/ GLOBAL INPUTS: Peoplefile[], Voidfile[], Messfile[], Monstfile[]
/
/ GLOBAL OUTPUTS: *Energyvoidfp, Echo, Marsh, *Login, Users, Beyond, 
/	Throne, Wizard, Changed, Okcount, Timeout, Windows, *Monstfp, *Messagefp, 
/	*Playersfp
/
/ DESCRIPTION:
/	Set global flags, and open files which remain open.
/
/************************************************************************/

initstate()
{
    Beyond = FALSE;
    Marsh = FALSE;
    Throne = FALSE;
    Changed = FALSE;
    Wizard = FALSE;
    Timeout = FALSE;
    Users = 0;
    Windows = FALSE;
    Echo = TRUE;
#ifdef OK_TO_PLAY
    Okcount = 0;
#endif

    /* setup login name */
    if ((Login = getlogin()) == NULL)
	Login = getpwuid(getuid())->pw_name;
    
    /* open some files */
    if ((Playersfp = fopen(Peoplefile, "r+")) == NULL)
	error(Peoplefile);
	/*NOTREACHED*/

    if ((Monstfp = fopen(Monstfile, "r+")) == NULL)
	error(Monstfile);
	/*NOTREACHED*/

    if ((Messagefp = fopen(Messfile, "r")) == NULL)
	error(Messfile);
	/*NOTREACHED*/

    if ((Energyvoidfp = fopen(Voidfile, "r")) == NULL)
	error(Voidfile);
	/*NOTREACHED*/

    srand((unsigned) time((long *) NULL));	/* prime random numbers */
}
/**/
/************************************************************************
/
/ FUNCTION NAME: rollnewplayer()
/
/ FUNCTION: roll up a new character
/
/ AUTHOR: E. A. Estes, 12/4/85
/
/ ARGUMENTS: none
/
/ RETURN VALUE: none
/
/ MODULES CALLED: initplayer(), allocrecord(), truncstring(), fabs(), wmove(), 
/	wclear(), sscanf(), strcmp(), genchar(), waddstr(), findname(), mvprintw(), 
/	getanswer(), getstring()
/
/ GLOBAL INPUTS: Other, Wizard, Player, *stdscr, Databuf[]
/
/ GLOBAL OUTPUTS: Echo
/
/ DESCRIPTION:
/	Prompt player, and roll up new character.
/
/************************************************************************/

long
rollnewplayer()
{
int	chartype;	/* character type */
int	ch;		/* input */

    initplayer(&Player);		/* initialize player structure */

    clear();
    mvaddstr(4, 21, "Which type of character do you want:");
    mvaddstr(8, 4, "1:Magic User  2:Fighter  3:Elf  4:Dwarf  5:Halfling  6:Experimento  ");
    if (Wizard)
	addstr("7:Super  ? ");
    else
	addstr("?  ");

    chartype = getanswer("2134567", FALSE);

    do
	{
	genchar(chartype);		/* roll up a character */

	/* print out results */
	mvprintw(12, 14,
	    "Strength    :  %2.0f  Quickness:  %2.0f  Mana       :  %2.0f\n",
	    Player.p_strength, Player.p_quickness, Player.p_mana);
	mvprintw(13, 14,
	    "Energy Level:  %2.0f  Brains   :  %2.0f  Magic Level:  %2.0f\n",
	    Player.p_energy, Player.p_brains, Player.p_magiclvl);

	if (Player.p_type == C_EXPER || Player.p_type == C_SUPER)
	    break;

	mvaddstr(14, 14, "Type '1' to keep >");
	ch = getanswer(" ", TRUE);
	}
    while (ch != '1');

    if (Player.p_type == C_EXPER || Player.p_type == C_SUPER)
	/* get coordinates for experimento */
	for (;;)
	    {
	    mvaddstr(16, 0, "Enter the X Y coordinates of your experimento ? ");
	    getstring(Databuf, SZ_DATABUF);
	    sscanf(Databuf, "%F %F", &Player.p_x, &Player.p_y);

	    if (fabs(Player.p_x) > D_EXPER || fabs(Player.p_y) > D_EXPER)
		mvaddstr(17, 0, "Invalid coordinates.  Try again.\n");
	    else
		break;
	    }

    for (;;)
	/* name the new character */
	{
	mvprintw(18, 0,
	    "Give your character a name [up to %d characters] ?  ", SZ_NAME - 1);
	getstring(Player.p_name, SZ_NAME);
	truncstring(Player.p_name);		/* remove trailing blanks */

	if (Player.p_name[0] == '\0')
	    /* no null names */
	    mvaddstr(19, 0, "Invalid name.");
	else if (findname(Player.p_name, &Other) >= 0L)
	    /* cannot have duplicate names */
	    mvaddstr(19, 0, "Name already in use.");
	else
	    /* name is acceptable */
	    break;

	addstr("  Pick another.\n");
	}

    /* get a password for character */
    Echo = FALSE;

    do
	{
	mvaddstr(20, 0, "Give your character a password [up to 8 characters] ? ");
	getstring(Player.p_password, SZ_PASSWORD);
	mvaddstr(21, 0, "One more time to verify ? ");
	getstring(Databuf, SZ_PASSWORD);
	}
    while (strcmp(Player.p_password, Databuf) != 0);

    Echo = TRUE;

    return(allocrecord());
}
/**/
/************************************************************************
/
/ FUNCTION NAME: procmain()
/
/ FUNCTION: process input from player
/
/ AUTHOR: E. A. Estes, 12/4/85
/
/ ARGUMENTS: none
/
/ RETURN VALUE: none
/
/ MODULES CALLED: dotampered(), changestats(), inputoption(), allstatslist(), 
/	fopen(), wmove(), random(), sscanf(), fclose(), altercoordinates(), 
/	waddstr(), fprintf(), distance(), userlist(), leavegame(), encounter(), 
/	getstring(), wclrtobot()
/
/ GLOBAL INPUTS: Circle, Illcmd[], Throne, Wizard, Player, *stdscr, 
/	Databuf[], Illmove[], Messfile[]
/
/ GLOBAL OUTPUTS: Player, Changed
/
/ DESCRIPTION:
/	Process main menu options.
/
/************************************************************************/

procmain()
{
int	ch;			/* input */
double	x;			/* desired new x coordinate */
double	y;			/* desired new y coordinate */
double	temp;			/* for temporary calculations */
FILE	*fp;			/* for opening files */
register int	loop;		/* a loop counter */
bool	hasmoved = FALSE;	/* set if player has moved */

    ch = inputoption();
    mvaddstr(4, 0, "\n\n");		/* clear status area */

    move(7, 0);
    clrtobot();			/* clear data on bottom area of screen */

    if (Player.p_specialtype == SC_VALAR && (ch == '1' || ch == '7'))
	/* valar cannot move */
	ch = ' ';

    switch (ch)
	{
	case 'K':		/* move up/north */
	case 'N':
	    x = Player.p_x;
	    y = Player.p_y + MAXMOVE();
	    hasmoved = TRUE;
	    break;

	case 'J':		/* move down/south */
	case 'S':
	    x = Player.p_x;
	    y = Player.p_y - MAXMOVE();
	    hasmoved = TRUE;
	    break;

	case 'L':		/* move right/east */
	case 'E':
	    x = Player.p_x + MAXMOVE();
	    y = Player.p_y;
	    hasmoved = TRUE;
	    break;

	case 'H':		/* move left/west */
	case 'W':
	    x = Player.p_x - MAXMOVE();
	    y = Player.p_y;
	    hasmoved = TRUE;
	    break;

	default:    /* rest */
	    Player.p_energy += (Player.p_maxenergy + Player.p_shield) / 15.0
		+ Player.p_level / 3.0 + 2.0;
	    Player.p_energy =
		MIN(Player.p_energy, Player.p_maxenergy + Player.p_shield);

	    if (Player.p_status != S_CLOAKED)
		/* cannot find mana if cloaked */
		{
		Player.p_mana += (Circle + Player.p_level) / 4.0;

		if (random() < 0.2 && Player.p_status == S_PLAYING && !Throne)
		    /* wandering monster */
		    encounter(-1);
		}
	    break;

	case 'X':		/* change/examine a character */
	    changestats(TRUE);
	    break;

	case '1':		/* move */
	    for (loop = 3; loop; --loop)
		{
		mvaddstr(4, 0, "X Y Coordinates ? ");
		getstring(Databuf, SZ_DATABUF);

		if (sscanf(Databuf, "%F %F", &x, &y) != 2)
		    mvaddstr(5, 0, "Try again\n");
		else if (distance(Player.p_x, x, Player.p_y, y) > MAXMOVE())
		    ILLMOVE();
		else
		    {
		    hasmoved = TRUE;
		    break;
		    }
		}
	    break;

	case '2':		/* players */
	    userlist(TRUE);
	    break;

	case '3':		/* message */
	    mvaddstr(4, 0, "Message ? ");
	    getstring(Databuf, SZ_DATABUF);
	    /* we open the file for writing to erase any data which is already there */
	    fp = fopen(Messfile, "w");
	    if (Databuf[0] != '\0')
		fprintf(fp, "%s: %s", Player.p_name, Databuf);
	    fclose(fp);
	    break;

	case '4':		/* stats */
	    allstatslist();
	    break;

	case '5':		/* good-bye */
	    leavegame();
	    /*NOTREACHED*/

	case '6':		/* cloak */
	    if (Player.p_level < MEL_CLOAK || Player.p_magiclvl < ML_CLOAK)
		ILLCMD();
	    else if (Player.p_status == S_CLOAKED)
		Player.p_status = S_PLAYING;
	    else if (Player.p_mana < MM_CLOAK)
		mvaddstr(5, 0, "No mana left.\n");
	    else
		{
		Changed = TRUE;
		Player.p_mana -= MM_CLOAK;
		Player.p_status = S_CLOAKED;
		}
	    break;

	case '7':	/* teleport */
	    /*
	     * conditions for teleport
	     *	- 20 per (level plus magic level)
	     *	- OR council of the wise or valar or ex-valar
	     *	- OR transport from throne
	     * transports from throne cost no mana
	     */
	    if (Player.p_level < MEL_TELEPORT || Player.p_magiclvl < ML_TELEPORT)
		ILLCMD();
	    else 
		for (loop = 3; loop; --loop)
		    {
		    mvaddstr(4, 0, "X Y Coordinates ? ");
		    getstring(Databuf, SZ_DATABUF);

		    if (sscanf(Databuf, "%F %F", &x, &y) == 2)
			{
			temp = distance(Player.p_x, x, Player.p_y, y);
			if (!Throne
			    /* can transport anywhere from throne */
			    && Player.p_specialtype <= SC_COUNCIL
			    /* council, valar can transport anywhere */
			    && temp > (Player.p_level + Player.p_magiclvl) * 20.0)
			    /* can only move 20 per exp. level + mag. level */
			    ILLMOVE();
			else
			    {
			    temp = (temp / 75.0 + 1.0) * 20.0;	/* mana used */

			    if (!Throne && temp > Player.p_mana)
				mvaddstr(5, 0, "Not enough power for that distance.\n");
			    else
				{
				if (!Throne)
				    Player.p_mana -= temp;
				hasmoved = TRUE;
				break;
				}
			    }
			}
		    }
	    break;

	case 'C':
	case '9':		/* monster */
	    if (Throne)
		/* no monsters while on throne */
		mvaddstr(5, 0, "No monsters in the chamber!\n");
	    else if (Player.p_specialtype != SC_VALAR)
		/* the valar cannot call monsters */
		{
		Player.p_sin += 1e-6;
		encounter(-1);
		}
	    break;

	case '0':		/* decree */
	    if (Wizard || Player.p_specialtype == SC_KING && Throne)
		/* kings must be on throne to decree */
		dotampered();
	    else
		ILLCMD();
	    break;

	case '8':		/* intervention */
	    if (Wizard || Player.p_specialtype >= SC_COUNCIL)
		dotampered();
	    else
		ILLCMD();
	    break;
	}

    if (hasmoved)
	/* player has moved -- alter coordinates, and do random monster */
	{
	altercoordinates(x, y, A_SPECIFIC);

	if (random() < 0.2 && Player.p_status == S_PLAYING && !Throne)
	    encounter(-1);
	}
}
/**/
#ifdef	ENEMY
/************************************************************************
/
/ FUNCTION NAME: checkenemy()
/
/ FUNCTION: check login name against enemy list
/
/ AUTHOR: E. A. Estes, 12/4/85
/
/ ARGUMENTS: none
/
/ RETURN VALUE: none
/
/ MODULES CALLED: fopen(), fgets(), strcmp(), fclose(), printf(), cleanup()
/
/ GLOBAL INPUTS: *Login, Databuf[], Enemyfile[]
/
/ GLOBAL OUTPUTS: none
/
/ DESCRIPTION:
/	The enemy file has a list of login names which are denied
/	access to Phantasia.
/	We scan this list and exit if the current login name is
/	found in the list.
/
/************************************************************************/

checkenemy()
{
FILE	*fp;		/* to open enemy file */

    /* check hit list of restricted accounts */
    if ((fp = fopen(Enemyfile, "r")) != NULL)
	{
	while (fgets(Databuf, SZ_DATABUF, fp) != NULL)
	    if (strcmp(Login, Databuf) == 0)
		{
		printf ("The Phantasia privileges for the account \"%s\" have been revoked.\n", Login);
		printf ("Mail comments to %s or %s.\n", WIZARDA, WIZARDB);
		fclose(fp);
		cleanup(TRUE);
		/*NOTREACHED*/
		}
	fclose (fp);
	}
}
#endif

/**/
/************************************************************************
/
/ FUNCTION NAME: titlelist()
/
/ FUNCTION: print title page
/
/ AUTHOR: E. A. Estes, 12/4/85
/
/ ARGUMENTS: none
/
/ RETURN VALUE: none
/
/ MODULES CALLED: fread(), fseek(), fopen(), fgets(), wmove(), strcpy(), 
/	fclose(), strlen(), waddstr(), sprintf(), wrefresh()
/
/ GLOBAL INPUTS: Lines, Other, *stdscr, Databuf[], Lastdead[], Motdfile[], 
/	*Playersfp
/
/ GLOBAL OUTPUTS: Lines
/
/ DESCRIPTION:
/	Print important information about game, players, etc.
/
/************************************************************************/

titlelist()
{
register FILE	*fp;		/* used for opening various files */
bool	councilfound = FALSE;	/* set if we find a member of the council */
bool	kingfound = FALSE;	/* set if we find a king */
double	hiexp, nxtexp;		/* used for finding the two highest players */
double	hilvl, nxtlvl;		/* used for finding the two highest players */
char	hiname[21], nxtname[21];/* used for finding the two highest players */

    mvaddstr(0, 14, "W e l c o m e   t o   P h a n t a s i a (vers. 3.3.2)!");

    /* print message of the day */
    if ((fp = fopen(Motdfile, "r")) != NULL
	&& fgets(Databuf, SZ_DATABUF, fp) != NULL)
	{
	mvaddstr(2, 40 - strlen(Databuf) / 2, Databuf);
	fclose(fp);
	}

    /* search for king */
    fseek(Playersfp, 0L, 0);
    while (fread((char *) &Other, SZ_PLAYERSTRUCT, 1, Playersfp) == 1)
	if (Other.p_specialtype == SC_KING && Other.p_status != S_NOTUSED)
	    /* found the king */
	    {
	    sprintf(Databuf, "The present ruler is %s  Level:%.0f",
		Other.p_name, Other.p_level);
	    mvaddstr(4, 40 - strlen(Databuf) / 2, Databuf);
	    kingfound = TRUE;
	    break;
	    }

    if (!kingfound)
	mvaddstr(4, 24, "There is no ruler at this time.");

    /* search for valar */
    fseek(Playersfp, 0L, 0);
    while (fread((char *) &Other, SZ_PLAYERSTRUCT, 1, Playersfp) == 1)
	if (Other.p_specialtype == SC_VALAR && Other.p_status != S_NOTUSED)
	    /* found the valar */
	    {
	    sprintf(Databuf, "The Valar is %s   Login:  %s", Other.p_name, Other.p_login);
	    mvaddstr(6, 40 - strlen(Databuf) / 2 , Databuf);
	    break;
	    }

    /* search for council of the wise */
    fseek(Playersfp, 0L, 0);
    Lines = 10;
    while (fread((char *) &Other, SZ_PLAYERSTRUCT, 1, Playersfp) == 1)
	if (Other.p_specialtype == SC_COUNCIL && Other.p_status != S_NOTUSED)
	    /* found a member of the council */
	    {
	    if (!councilfound)
		{
		mvaddstr(8, 30, "Council of the Wise:");
		councilfound = TRUE;
		}

	    /* This assumes a finite (<=5) number of C.O.W.: */
	    sprintf(Databuf, "%s   Login:  %s", Other.p_name, Other.p_login);
	    mvaddstr(Lines++, 40 - strlen(Databuf) / 2, Databuf);
	    }

    /* search for the two highest players */
    nxtname[0] = hiname[0] = '\0';
    hiexp = 0.0;
    nxtlvl = hilvl = 0;

    fseek(Playersfp, 0L, 0);
    while (fread((char *) &Other, SZ_PLAYERSTRUCT, 1, Playersfp) == 1)
	if (Other.p_experience > hiexp && Other.p_specialtype <= SC_KING && Other.p_status != S_NOTUSED)
	    /* highest found so far */
	    {
	    nxtexp = hiexp;
	    hiexp = Other.p_experience;
	    nxtlvl = hilvl;
	    hilvl = Other.p_level;
	    strcpy(nxtname, hiname);
	    strcpy(hiname, Other.p_name);
	    }
	else if (Other.p_experience > nxtexp
	    && Other.p_specialtype <= SC_KING
	    && Other.p_status != S_NOTUSED)
	    /* next highest found so far */
	    {
	    nxtexp = Other.p_experience;
	    nxtlvl = Other.p_level;
	    strcpy(nxtname, Other.p_name);
	    }

    mvaddstr(15, 28, "Highest characters are:");
    sprintf(Databuf, "%s  Level:%.0f   and   %s  Level:%.0f",
	hiname, hilvl, nxtname, nxtlvl);
    mvaddstr(17, 40 - strlen(Databuf) / 2, Databuf);

    /* print last to die */
    if ((fp = fopen(Lastdead,"r")) != NULL
	&& fgets(Databuf, SZ_DATABUF, fp) != NULL)
	{
	mvaddstr(19, 25, "The last character to die was:");
	mvaddstr(20, 40 - strlen(Databuf) / 2,Databuf);
	fclose(fp);
	}

    refresh();
}
/**/
/************************************************************************
/
/ FUNCTION NAME: recallplayer()
/
/ FUNCTION: find a character on file
/
/ AUTHOR: E. A. Estes, 12/4/85
/
/ ARGUMENTS: none
/
/ RETURN VALUE: none
/
/ MODULES CALLED: writerecord(), truncstring(), more(), death(), wmove(), 
/	wclear(), strcmp(), printw(), cleanup(), waddstr(), findname(), mvprintw(), 
/	getanswer(), getstring()
/
/ GLOBAL INPUTS: Player, *stdscr, Databuf[]
/
/ GLOBAL OUTPUTS: Echo, Player
/
/ DESCRIPTION:
/	Search for a character of a certain name, and check password.
/
/************************************************************************/

long
recallplayer()
{
long	loc = 0L;		/* location in player file */
register int	loop;		/* loop counter */
int	ch;			/* input */

    clear();
    mvprintw(10, 0, "What was your character's name ? ");
    getstring(Databuf, SZ_NAME);
    truncstring(Databuf);

    if ((loc = findname(Databuf, &Player)) >= 0L)
	/* found character */
	{
	Echo = FALSE;

	for (loop = 0; loop < 2; ++loop)
	    {
	    /* prompt for password */
	    mvaddstr(11, 0, "Password ? ");
	    getstring(Databuf, SZ_PASSWORD);
	    if (strcmp(Databuf, Player.p_password) == 0)
		/* password good */
		{
		Echo = TRUE;

		if (Player.p_status != S_OFF)
		    /* player did not exit normally last time */
		    {
		    clear();
		    addstr("Your character did not exit normally last time.\n");
		    addstr("If you think you have good cause to have your character saved,\n");
		    printw("you may quit and mail your reason to '%s' or '%s'.\n", WIZARDA,WIZARDB);
		    addstr("Otherwise, continuing spells certain death.\n");
		    addstr("Do you want to quit ? ");
		    ch = getanswer("YN", FALSE);
		    if (ch == 'Y')
			{
			Player.p_status = S_HUNGUP;
			writerecord(&Player, loc);
			cleanup(TRUE);
			/*NOTREACHED*/
			}
		    death("Stupidity");
		    /*NOTREACHED*/
		    }
		return(loc);
		}
	    else
		mvaddstr(12, 0, "No good.\n");
	    }

	Echo = TRUE;
	}
    else
	mvaddstr(11, 0, "Not found.\n");

    more(13);
    return(-1L);
}
/**/
/************************************************************************
/
/ FUNCTION NAME: neatstuff()
/
/ FUNCTION: do random stuff
/
/ AUTHOR: E. A. Estes, 3/3/86
/
/ ARGUMENTS: none
/
/ RETURN VALUE: none
/
/ MODULES CALLED: collecttaxes(), floor(), wmove(), random(), infloat(), 
/	waddstr(), mvprintw(), getanswer()
/
/ GLOBAL INPUTS: Player, *stdscr, *Statptr
/
/ GLOBAL OUTPUTS: Player
/
/ DESCRIPTION:
/	Handle gurus, medics, etc.
/
/************************************************************************/

neatstuff()
{
double	temp;	/* for temporary calculations */
int	ch;	/* input */

    switch ((int) ROLL(0.0, 100.0))
	{
	case 1:
	case 2:
	    if (Player.p_poison > 0.0)
		{
		mvaddstr(4, 0, "You've found a medic!  How much will you offer to be cured ? ");
		temp = floor(infloat());
		if (temp < 0.0 || temp > Player.p_gold)
		    /* negative gold, or more than available */
		    {
		    mvaddstr(6, 0, "He was not amused, and made you worse.\n");
		    Player.p_poison += 1.0;
		    }
		else if (random() / 2.0 > (temp + 1.0) / MAX(Player.p_gold, 1))
		    /* medic wants 1/2 of available gold */
		    mvaddstr(5, 0, "Sorry, he wasn't interested.\n");
		else
		    {
		    mvaddstr(5, 0, "He accepted.");
		    Player.p_poison = MAX(0.0, Player.p_poison - 1.0);
		    Player.p_gold -= temp;
		    }
		}
	    break;

	case 3:
	    mvaddstr(4, 0, "You've been caught raping and pillaging!\n");
	    Player.p_experience += 4000.0;
	    Player.p_sin += 0.5;
	    break;

	case 4:
	    temp = ROLL(10.0, 75.0);
	    mvprintw(4, 0, "You've found %.0f gold pieces, want them ? ", temp);
	    ch = getanswer("NY", FALSE);

	    if (ch == 'Y')
		collecttaxes(temp, 0.0);
	    break;

	case 5:
	    if (Player.p_sin > 1.0)
		{
		mvaddstr(4, 0, "You've found a Holy Orb!\n");
		Player.p_sin -= 0.25;
		}
	    break;

	case 6:
	    if (Player.p_poison < 1.0)
		{
		mvaddstr(4, 0, "You've been hit with a plague!\n");
		Player.p_poison += 1.0;
		}
	    break;

	case 7:
	    mvaddstr(4, 0, "You've found some holy water.\n");
	    ++Player.p_holywater;
	    break;

	case 8:
	    mvaddstr(4, 0, "You've met a Guru. . .");
	    if (random() * Player.p_sin > 1.0)
		addstr("You disgusted him with your sins!\n");
	    else if (Player.p_poison > 0.0)
		{
		addstr("He looked kindly upon you, and cured you.\n");
		Player.p_poison = 0.0;
		}
	    else
		{
		addstr("He rewarded you for your virtue.\n");
		Player.p_mana += 50.0;
		Player.p_shield += 2.0;
		}
	    break;

	case 9:
	    mvaddstr(4, 0, "You've found an amulet.\n");
	    ++Player.p_amulets;
	    break;

	case 10:
	    if (Player.p_blindness)
		{
		mvaddstr(4, 0, "You've regained your sight!\n");
		Player.p_blindness = FALSE;
		}
	    break;

	default:	/* deal with poison */
	    if (Player.p_poison > 0.0)
		{
		temp = Player.p_poison * Statptr->c_weakness
		    * Player.p_maxenergy / 600.0;
		if (Player.p_energy > Player.p_maxenergy / 10.0
		    && temp + 5.0 < Player.p_energy)
		    Player.p_energy -= temp;
		}
	    break;
	}
}
/**/
/************************************************************************
/
/ FUNCTION NAME: genchar()
/
/ FUNCTION: generate a random character
/
/ AUTHOR: E. A. Estes, 12/4/85
/
/ ARGUMENTS:
/	int type - ASCII value of character type to generate
/
/ RETURN VALUE: none
/
/ MODULES CALLED: floor(), random()
/
/ GLOBAL INPUTS: Wizard, Player, Stattable[]
/
/ GLOBAL OUTPUTS: Player
/
/ DESCRIPTION:
/	Use the lookup table for rolling stats.
/
/************************************************************************/

genchar(type)
int	type;
{
register int	subscript;		/* used for subscripting into Stattable */
register struct charstats	*statptr;/* for pointing into Stattable */

    subscript = type - '1';

    if (subscript < C_MAGIC || subscript > C_EXPER)
	if (subscript != C_SUPER || !Wizard)
	    /* fighter is default */
	    subscript = C_FIGHTER;
    
    statptr = &Stattable[subscript];

    Player.p_quickness =
	ROLL(statptr->c_quickness.base, statptr->c_quickness.interval);
    Player.p_strength =
	ROLL(statptr->c_strength.base, statptr->c_strength.interval);
    Player.p_mana =
	ROLL(statptr->c_mana.base, statptr->c_mana.interval);
    Player.p_maxenergy =
    Player.p_energy =
	ROLL(statptr->c_energy.base, statptr->c_energy.interval);
    Player.p_brains =
	ROLL(statptr->c_brains.base, statptr->c_brains.interval);
    Player.p_magiclvl =
	ROLL(statptr->c_magiclvl.base, statptr->c_magiclvl.interval);

    Player.p_type = subscript;

    if (Player.p_type == C_HALFLING)
	/* give halfling some experience */
	Player.p_experience = ROLL(600.0, 200.0);
}
/**/
/************************************************************************
/
/ FUNCTION NAME: playinit()
/
/ FUNCTION: initialize for playing game
/
/ AUTHOR: E. A. Estes, 12/4/85
/
/ ARGUMENTS: none
/
/ RETURN VALUE: none
/
/ MODULES CALLED: signal(), wclear(), noecho(), crmode(), initscr(), 
/	wrefresh()
/
/ GLOBAL INPUTS: *stdscr, ill_sig()
/
/ GLOBAL OUTPUTS: Windows
/
/ DESCRIPTION:
/	Catch a bunch of signals, and turn on curses stuff.
/
/************************************************************************/

playinit()
{
    /* catch/ingnore signals */

#ifdef	BSD41
    sigignore(SIGQUIT);
    sigignore(SIGALRM);
    sigignore(SIGTERM);
    sigignore(SIGTSTP);
    sigignore(SIGTTIN);
    sigignore(SIGTTOU);
    sighold(SIGINT);
    sigset(SIGHUP, ill_sig);
    sigset(SIGTRAP, ill_sig);
    sigset(SIGIOT, ill_sig);
#ifdef SIGEMT
    sigset(SIGEMT, ill_sig);
#endif
    sigset(SIGFPE, ill_sig);
    sigset(SIGBUS, ill_sig);
    sigset(SIGSEGV, ill_sig);
    sigset(SIGSYS, ill_sig);
    sigset(SIGPIPE, ill_sig);
#endif
#ifdef	BSD42
    signal(SIGQUIT, SIG_IGN);
    signal(SIGALRM, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGHUP, ill_sig);
    signal(SIGTRAP, ill_sig);
    signal(SIGIOT, ill_sig);
#ifdef SIGEMT
    signal(SIGEMT, ill_sig);
#endif
    signal(SIGFPE, ill_sig);
    signal(SIGBUS, ill_sig);
    signal(SIGSEGV, ill_sig);
    signal(SIGSYS, ill_sig);
    signal(SIGPIPE, ill_sig);
#endif
#ifdef	SYS3
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGALRM, SIG_IGN);
    signal(SIGHUP, ill_sig);
    signal(SIGTRAP, ill_sig);
    signal(SIGIOT, ill_sig);
    signal(SIGEMT, ill_sig);
    signal(SIGFPE, ill_sig);
    signal(SIGBUS, ill_sig);
    signal(SIGSEGV, ill_sig);
    signal(SIGSYS, ill_sig);
    signal(SIGPIPE, ill_sig);
#endif
#ifdef	SYS5
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGALRM, SIG_IGN);
    signal(SIGHUP, ill_sig);
    signal(SIGTRAP, ill_sig);
#ifndef SIGIOT
#define SIGIOT	SIGABRT
#endif
    signal(SIGIOT, ill_sig);
#ifdef SIGEMT
    signal(SIGEMT, ill_sig);
#endif
    signal(SIGFPE, ill_sig);
    signal(SIGBUS, ill_sig);
    signal(SIGSEGV, ill_sig);
    signal(SIGSYS, ill_sig);
    signal(SIGPIPE, ill_sig);
#endif

    initscr();		/* turn on curses */
    noecho();		/* do not echo input */
    crmode();		/* do not process erase, kill */
    clear();
    refresh();
    Windows = TRUE;	/* mark the state */
}

/**/
/************************************************************************
/
/ FUNCTION NAME: cleanup()
/
/ FUNCTION: close some files, and maybe exit
/
/ AUTHOR: E. A. Estes, 12/4/85
/
/ ARGUMENTS:
/	bool doexit - exit flag
/
/ RETURN VALUE: none
/
/ MODULES CALLED: exit(), wmove(), fclose(), endwin(), nocrmode(), wrefresh()
/
/ GLOBAL INPUTS: *Energyvoidfp, LINES, *stdscr, Windows, *Monstfp, 
/	*Messagefp, *Playersfp
/
/ GLOBAL OUTPUTS: none
/
/ DESCRIPTION:
/	Close all open files.  If we are "in curses" terminate curses.
/	If 'doexit' is set, exit, otherwise return.
/
/************************************************************************/

cleanup(doexit)
bool	doexit;
{
    if (Windows)
	{
	move(LINES - 2, 0);
	refresh();
	nocrmode();
	endwin();
	}

    fclose(Playersfp);
    fclose(Monstfp);
    fclose(Messagefp);
    fclose(Energyvoidfp);

    if (doexit)
	exit(0);
	/*NOTREACHED*/
}
/**/
#ifdef	OK_TO_PLAY
#include <sys/types.h>
#include <utmp.h>   /* used for counting users on system */

/************************************************************************
/
/ FUNCTION NAME: ok_to_play()
/
/ FUNCTION: indicate whether playing is allowed
/
/ AUTHOR: E. A. Estes, 12/4/85
/
/ ARGUMENTS: none
/
/ RETURN VALUE:
/	FALSE if playing is not allowed
/	TRUE if playing is allowed
/
/ MODULES CALLED: time(), fread(), fopen(), fclose(), localtime()
/
/ GLOBAL INPUTS: Wizard, Okcount
/
/ GLOBAL OUTPUTS: Okcount
/
/ DESCRIPTION: 
/	This function is provided to allow one to restrict access to the game.
/	Tailor this routine as appropriate.
/	Return FALSE if playing is not allowed at this time.
/
/************************************************************************/

ok_to_play()
{
register struct tm	*tp;	/* to get time of day */
register int	numusers = 0;	/* number of users on system */
FILE	*fp;			/* to open files */
long	now;			/* present time */
struct utmp	ubuf;		/* to read 'utmp' file */

    if (Wizard)
	/* wizard can always play */
	return(TRUE);

    if (Okcount >= 5)
	Okcount = 0;
    if (Okcount++ > 0)
	/* only check every 5 times */
	return(TRUE);

    /* check time of day */
    time(&now);
    tp = localtime(&now);
    if (((tp->tm_hour > 8 && tp->tm_hour < 12)		/* 8-noon */
	|| (tp->tm_hour > 11 && tp->tm_hour < 15))	/* noon-3 */
	&& (tp->tm_wday != 0 && tp->tm_wday != 6))	/* not a weekend */
	    return(FALSE);

    /* check # of users */
    if ((fp = fopen("/etc/utmp", "r")) != NULL)
	{
	while (fread((char *) &ubuf, sizeof(ubuf), 1, fp) == 1)
#ifdef	USER_PROCESS
	    if (ubuf.ut_type == USER_PROCESS)
#else
	    if (ubuf.ut_name[0] != '\0')
#endif
		++numusers;
	fclose(fp);

	if (numusers > N_MAXUSERS)
	    return(FALSE);
	}
    return(TRUE);
}
#endif
