/* robomatic.c: Rob-O-Matic I (CMU) Mon Oct 14 11:51:01 EDT 1985 - mlm */
/*****************************************************************
 *
 * Rob-O-Matic: Play the robots game
 * Copyright (C) 1985 by Mauldin
 * The right is granted to any person, university, or company 
 * to copy, modify, or distribute (for free) this file, providing
 * that this notice is not removed.
 *
 * HISTORY
 * 11-Oct-85  Michael Mauldin (mlm) at Carnegie-Mellon University
 *	Created.
 *
 *****************************************************************/

# include <curses.h>
#ifndef __STDC__
#ifndef A_REVERSE
#define chtype	char
#include <fcntl.h>
nodelay(scr, tf)
{	static int fcntlflgs;

	fcntlflgs = fcntl(0, F_GETFL, 0);
	if (fcntlflgs < 0) {
		perror("fcntl");
		exit(1);
	}
	if (tf == TRUE)
		fcntl(0, F_SETFL, fcntlflgs|O_NDELAY);
	else
		fcntl(0, F_SETFL, fcntlflgs&~O_NDELAY);
}
#endif
#endif
# include <signal.h>

#ifdef __STDC__
#define STR(x)		#x
#define STRING(x)	STR(x)
#define FILENM(x)	STRING(GAMDIR) "/" STRING(x)
#else
#define STRING(x)	"x
#define FILENM(x)	STRING(GAMDIR)/x"
#endif
# define NEWROBOT	FILENM(robots)
# define ROBOT		FILENM(robots)

# define READ		0
# define WRITE		1
# define abs(A)		((A)>0?(A):-(A))
# define max(A,B)	((A)>(B)?(A):(B))
# define min(A,B)	((A)<(B)?(A):(B))
# define sgn(A)		((A)==0?0:((A)>0?1:-1))

# define ROWS	25
# define COLS	80

/* Define the Rob-O-Matic pseudo-terminal */

# define ctrl(C) ((C)&037)
#ifdef OLDWAY
# define ROBOTTERM "rterm"
# define ROBOTCAP "rb|rterm:am:bs:ce=^E:cl=^L:cm=^F%+ %+ :co#80:li#24:pt:ta=^I:up=^A:do=^B:nd=^C:db:xn:"
# define CE ctrl('E')
# define CL ctrl('L')
# define CM ctrl('F')
# define DO ctrl('B')
# define LF ctrl('J')
# define ND ctrl('C')
# define UP ctrl('A')
#else	/* Use a real terminal type to keep terminfo happy */
# define ROBOTTERM "vc404"
# define ROBOTCAP "M8|vc404:am:bs:ce=^V:cl=^X:cm=^P%+ %+ :co#80:li#24:up=^Z:do=^J:nd=^U:"
# define CE ctrl('V')
# define CL ctrl('X')
# define CM ctrl('P')
# define DO ctrl('J')
# define ND ctrl('U')
# define UP ctrl('Z')
#endif
# define BL ctrl('G')
# define BS ctrl('H')
# define CR ctrl('M')
# define NL ctrl('@')
# define TA ctrl('I')

int   child;
int   frobot, trobot;
int   debug=0;
FILE *tracefp=NULL;
/****************************************************************
 * Main routine
 ****************************************************************/

main (argc, argv)
int   argc;
char *argv[];

{ int   ptc[2], ctp[2];
  char *rfile=NULL;

  setbuf(stdout, 0);
  /* Get the options from the command line */
  while (--argc > 0 && (*++argv)[0] == '-')
  { while (*++(*argv))
    { switch (**argv)
      { case 'd': debug++;					break;
        default:  printf ("Usage: robomatic [-d]\n");		exit (1);
      }
    }
  }

  /* Open tracing file (if needed) */
  if (debug)
  { if ((tracefp = fopen ("trace.log", "w")) == NULL)
    { perror ("trace.log");
      exit (1);
    }
  }

  /* Find an executable of the robots game */
  if (access ("robots", 1) == 0)	rfile = "robots";
  else if (access (NEWROBOT, 1) == 0)	rfile = NEWROBOT;
  else if (access (ROBOT, 1) == 0)	rfile = ROBOT;
  else
  { perror ("robots");
    exit (1);
  }

  /* Get two pipes to attach to the Robots process */
  if ((pipe (ptc) < 0) || (pipe (ctp) < 0))
  { fprintf (stderr, "Cannot get pipes!\n");
    exit (1);
  }

  trobot = ptc[WRITE];
  frobot = ctp[READ];

  /* Now fork a child process, update the TERMCAP, and exec the Robots */
  if ((child = fork ()) == 0)
  { close (0);
    dup (ptc[READ]);
    close (1);
    dup (ctp[WRITE]);

    put_env ("TERM", ROBOTTERM);
    put_env ("TERMCAP", ROBOTCAP);
    execl (rfile, rfile, (char *)0);
    exit (1);
  }
  /* Call Robomatic as the Parent Process */
  robomatic ();
  return (0);
}

/****************************************************************
 * Panic: Abort child, reset terminal, and exit.
 ****************************************************************/
SIG_T
panic(dummy)
{ send('q');
  send('\n');
  close(frobot);
  while (wait((int *)0) != child)
    perror("panic");
  move (ROWS-1, 0); clrtoeol (); refresh ();
  nocrmode (); noraw (); echo (); resetty (); endwin ();
  exit (0);
}

/****************************************************************
 * Robomatic: Play Robots.  Read the screen, choose a move, and send it
 ****************************************************************/

char screen[ROWS][COLS];
int playing=1;
int row=0, col=0, atrow= -1, atcol= -1;

robomatic ()
{ int cmd;
  
  /* Clean up if SIGINT seen */
  signal(SIGINT, panic);

  /* Initialize the Curses package */
  initscr (); savetty (); crmode (); noecho (); clear (); refresh ();

  /* Clear the screen array */
  blankscreen ();

  /* Read the first screen of output */
  send (';');	/* Cause robots to send a bell when ready to read cmd */
  getrobot ();	/* Read screen updates until a bell */

  /* Main loop, send a command and then read the screen */
  while (playing) {
    int ch;
    /* If the user types anything, execute his commands */
    nodelay(stdscr, TRUE);
    while ((ch = getch()) != ERR) {
      switch (ch) {
        case 'd':	debug++; 
			break;
        case 'r':	clear (); drawscreen (); refresh (); 
			break;
	default:	break;
      }
    }
    nodelay(stdscr, FALSE);

    /* Choose a command and send it */
    cmd = strategy ();
    if (cmd == 0) { debug++; dwait ("command is zero"); }
    dwait ("Sending command '%c'.", cmd);
    send (cmd);

    /* Send the semicolon and read screen updates until a bell */
    send (';');
    getrobot ();
  }

  /* Clear the scoreboard on some Robots */
  send ('\n');

  /* Wait for robots to get through, just in case */
  close(frobot);
  while (wait((int *)0) != child)
    perror("dead");

  /* Now wait for the user to type a character before finishing */
  refresh ();
  getchar ();

  /* Print termination messages */
  move (ROWS-1, 0); clrtoeol (); refresh ();
  nocrmode (); noraw (); echo (); resetty (); endwin ();

  deadrobot ();

  exit (0);
}

/****************************************************************
 * blankscreen: Fill the screen array with blanks
 ****************************************************************/

blankscreen ()
{ register int i, j;

  for (i=0; i<ROWS; i++)
    for (j=0; j<COLS; j++)
      screen[i][j] = ' ';
}

/****************************************************************
 * getrobot: Read the Robot screen; stop when you read a bell
 ****************************************************************/

getrobot ()
{ int ch, done=0;
  register int i;
  char *d, *h, buf[BUFSIZ];

  d = "robot food";			/* FSM to check for death */
  h = "scrap heaps";			/* FSM to check for new level */

  while (!done)
  {
    /* Read a character from the Robots process */
    if (read (frobot, buf, 1) < 1)
    { ch = EOF; if (tracefp) fclose (tracefp); }
    else
    { ch = buf[0]; if (tracefp) { fputc (ch, tracefp); fflush (tracefp); } }

    /* Check for the words "robot food" to see if we died */
    if (ch == *d) { if (0 == *++d) { done++; playing=0; } }
    else d = "robot food";

    /* Check for the words "scrap heaps" to see if we survived a level */
    if (ch == *h) { if (0 == *++h) { send (';'); } }
    else h = "scrap heaps";

    /* Now figure out what the character means */
    switch (ch)
    { case BL:	/* Bell */
        done++;	/* FALL THROUGH */
      case NL:	/* Pad character */
        break;

      case BS: /* BackSpace */
        col--;
        break;

      case CE: /* Clear to end of line */
        for (i = col; i < COLS; i++)
          screen[row][i] = ' ';

        move (row, col);
        clrtoeol ();
	break;

      case CL: /* Clear Screen */
        clear ();
        blankscreen ();
	row = col = 0;
        break;

      case CM:	/* Cursor motion command */
        if (read (frobot, buf, 2) < 2)
	{ deadrobot (); }
	else
	{ row = buf[0] - ' ';
	  col = buf[1] - ' ';
          if (tracefp) { fprintf (tracefp, "%c%c", buf[0], buf[1]);
		       fflush (tracefp); }
	}
        break;

      case CR:	/* Carriage Return */
        col = 0;
        break;

      case DO:	/* Cursor down */
        row++;
        break;

#ifdef LF
      case LF:	/* Line Feed */
        row++;
        col = 0;
        break;
#endif

      case ND:	/* Non-destructive space (cursor right) */
        col++;
        break;

      case TA:	/* Tab */
        col = 8 * (1 + col / 8);
        break;

      case EOF:	/* End of file */
	playing = 0;
	return;

      case UP:	/* Cursor up */
        row--;
        break;

      default: /* Other */

	/* Control character */
        if (ch < ' ')
        { fprintf (stderr, "Unknown character '\\%o'\n", ch);
	  panic();
        }

	/* Printing character */
	if (ch == 'I')
	{  atrow = row; atcol = col; }
        mvaddch (row, col, (chtype)ch);
        screen[row][col++] = ch;
        if (col > COLS)
	{ row++;
	  col = 0;
	}
        break;
    }
  }

  resetty();
  drawscreen ();
  move (row, col); 
  refresh ();
}

/****************************************************************
 * deadrobot: We died, read our score and print it out
 ****************************************************************/

deadrobot ()
{ int level=0;
  long score=0;
  char junk[BUFSIZ];

  sscanf (screen[ROWS-1], "%s level: %d score: %ld", junk, &level, &score);
  printf ("Died on level %d with a score of %ld.\n", level, score);
}

/****************************************************************
 * send: Send a command to the Robots process
 ****************************************************************/

send (ch)
int ch;
{ char buf[BUFSIZ];

  *buf = ch;
  write (trobot, buf, 1);
}

/****************************************************************
 * drawscreen: Draw the screen array on the user's terminal
 ****************************************************************/

drawscreen ()
{ register int r, c;

  for (r=0; r<ROWS; r++)
    for (c=0; c<COLS; c++)
      mvaddch (r, c, (chtype)screen[r][c]);
}

/****************************************************************
 * strategy: Five basic rules
 *
 * 1. Find the nearest enemy robot.
 * 2. Can you hide behind a scrap heap and cause him to crash?
 * 3. Can you find another robot close to the first and move in.
 *    such a way so as to cause them to collide?
 * 4. Can you make any move toward center?
 * 5. Teleport.
 ****************************************************************/

/* keymap[dy+1][dx+1] == character to get you there */
char keymap[3][3] = {
  'y', 'k', 'u',
  'h', '.', 'l',
  'b', 'j', 'n'
};

strategy ()
{ int cmd, tr=0, tc=0, hr=0, hc=0;

  /* Find closest robot and find a heap to block him with */
  if (closest (&tr, &tc, '=') )
    if (!findheap (tr, tc, &hr, &hc) && !findcollide (tr, tc, &hr, &hc)) {
      /* None found, try to go toward the center of the screen */
      hr = ROWS/2;
      hc = COLS/2;
    }
  if (cmd = makemove (hr, hc)) return (cmd);

  return ('t');
}

/****************************************************************
 * safe: Is it safe to move to a given square
 ****************************************************************/

safe (r, c)
register int r, c;
{
  return ((screen[r][c] == ' ' || screen[r][c] == 'I') &&
	   screen[r-1][c-1] != '=' && screen[r-1][  c] != '=' &&
	   screen[r-1][c+1] != '=' && screen[  r][c-1] != '=' &&
	   screen[  r][c+1] != '=' && screen[r+1][c-1] != '=' &&
	   screen[r+1][  c] != '=' && screen[r+1][c+1] != '=');
}

/****************************************************************
 * find the closest object of a given type
 ****************************************************************/

closest (rp, cp, type)
int *rp, *cp, type;
{ register int r, c, dist=999, newdist, tr=0, tc=0;

  /* Find closest robot */
  for (r=1; r<ROWS-1; r++)
  { for (c=1; c<COLS; c++)
    { if (screen[r][c] == type)
      { newdist = distance (atrow, atcol, r, c);
	if (newdist < dist)
	{ tr=r; tc=c; dist=newdist; }
      }
    }
  }

  *rp=tr; *cp=tc;

  dwait ("Closest %c is at (%d,%d).", type, tr, tc);

  return (dist < 999);
}

/****************************************************************
 * distance: Calculate the distance between two points
 ****************************************************************/

distance (r1, c1, r2, c2)
int r1, c1, r2, c2;
{ register int dr, dc;

  if ((dr = r2-r1) < 0) dr = -dr;	/* Absolute row distance */
  if ((dc = c2-c1) < 0) dc = -dc;	/* Absolute col distance */
  return (max(dr,dc));			/* Max norm */
}

/****************************************************************
 * findheap: Find a heap to block a given robot
 ****************************************************************/

findheap (tr, tc, pr, pc)
int tr, tc, *pr, *pc;
{ int r, c;

  /* Check closest heap for blocking first */
  if (closest (&r, &c, '@') && willblock (r, c, tr, tc, pr, pc))
  { dwait ("Heading for nearby safe square at (%d,%d).", *pr, *pc);
    return (1);
  }

  /* Look through all heaps on the screen to find a block */
  for (r=1; r<ROWS-1; r++)
  { for (c=1; c<COLS; c++)
    { if (screen[r][c] == '@' && willblock (r, c, tr, tc, pr, pc))
      { dwait ("Heading for safe square at (%d,%d).", *pr, *pc);
        return (1);
      }
    }
  }

  /* Lose */  
  return (0);
}

/****************************************************************
 * willblock: returns true if the heap at (hr,hc) will block the enemy robot
 * located at (tr,tc).  The square to move to is returned in (pr, pc)
 ****************************************************************/

willblock (hr, hc, tr, tc, pr, pc)
int hr, hc, tr, tc, *pr, *pc;
{ register int dr, dc, sr,sc;

  /* First find the "shadow" behind the block from the enemy robot */
  dr = hr-tr; dc = hc-tc;

  if (abs (dr) == abs (dc))
  { sr = hr+sgn (dr); sc = hc+sgn (dc); }
  else if (abs (dr) > abs (dc))
  { sr = hr+sgn (dr); sc = hc; }
  else
  { sr = hr; sc = hc+sgn (dc); }

  /* If we are closer to the shadow square than the robot is, win */
  if (distance (sr, sc, tr, tc) > distance (hr, hc, atrow, atcol))
  { *pr = sr; *pc = sc; return (1); }
  
  /* Lose */
  return (0);
}

/****************************************************************
 * findcollide: Find a robot to collide with a given robot.  The target
 * square will be the nearest safe square to the collision point.
 ****************************************************************/

findcollide (tr, tc, pr, pc)
int tr, tc, *pr, *pc;
{ int r=0, c=0, d, cr1, cc1, cr2, cc2;

  /* Search from target robot for another on the same row or column */
  for (d=1; d<COLS; d++)
  { if (tc+d < COLS && screen[tr][tc+d] == '=')
    { r = tr; c = tc+d; break; }

    if (tc-d > 0 && screen[tr][tc-d] == '=')
    { r = tr; c = tc-d; break; }

    if (tr+d < LINES-1 && screen[tr+d][tc] == '=')
    { r = tr+d; c = tc; break; }

    if (tr-d > 0 && screen[tr-d][tc] == '=')
    { r = tr-d; c = tc; break; }
  }

  if (r || c)
  { dwait ("possible collision between (%d,%d) and (%d,%d)", tr, tc, r, c); }
  
  /* Check for two on same row */
  if (r == tr && r != atrow)
  { /* Check collision up */
    cr1 = tr - (d+1)/2 - 1;
    cr2 = tr + (d+1)/2 + 1;
    cc1 = cc2 = (tc + c) / 2;
    if (distance (atrow, atcol, cr1, cc1) < distance (atrow, atcol, cr2, cc2))
    { *pr = cr1; *pc = cc1; }
    else
    { *pr = cr2; *pc = cc2; }

    if (*pr < 1 || *pr >= ROWS || *pc < 1 || *pc > COLS) return (0);

    dwait ("predicting safe spot near collision (%d,%d)", *pr, *pc);
    return (1);
  }
  
  /* Check for two on same column */
  if (c == tc && c != atcol)
  { /* Check collision left */
    cr1 = cr2 = (tr + r) / 2;
    cc1 = tc - (d+1)/2 - 1;
    cc2 = tc + (d+1)/2 + 1;
    if (distance (atrow, atcol, cr1, cc1) < distance (atrow, atcol, cr2, cc2))
    { *pr = cr1; *pc = cc1; }
    else
    { *pr = cr2; *pc = cc2; }

    if (*pr < 1 || *pr >= ROWS || *pc < 1 || *pc > COLS) return (0);

    dwait ("predicting safe spot near collision (%d,%d)", *pr, *pc);
    return (1);
  }
  
  /* Fail */
  return (0);
}

/****************************************************************
 * makemove: Return the direction to move toward a given square.
 * If not possible to move straight toward given square, try to
 * get as close as possible, or at least make a legal move. This
 * routine checks to make sure the move returned is a safe one.
 * Returns 0 if no move is safe, and the command if one is found.
 ****************************************************************/

typedef struct {
  int h[9][2];
} heur;
/*
 * Evaluation function when goal is on the diagonal.
 *  G			<- Goal
 *   123
 *   467		<- 6 is (atrow, atcol)
 *   589
*/
heur diag = {{
  {  1,  1 },			/* Prefer straight to goal	*/
  {  1,  0 },			/* Then toward goal on Y axis	*/
  {  1, -1 },			/* even if compromises X	*/
  {  0,  1 },			/* Then toward goal on X axis	*/
  { -1,  1 },			/* even if compromises Y	*/
  {  0,  0 },			/* Then staying put		*/
  {  0, -1 },			/* Then compromise X axis	*/
  { -1,  0 },			/* Then compromise Y axis	*/
  { -1, -1 }			/* Then compromise both		*/
}};
/*
 * Evaluation function when goal on same Y
 *   258
 *  G147			Rotate 90 degrees for view 3
 *   369
 *
 * Note that the Y delta cannot be zero, it need not be signed
 * relative to the direction to G.  In fact, the center heuristic
 * suggests that we pick the sign for the Y delta such that it
 * would move us toward the center.  Be careful that, if we are 
 * already centered in Y, we consider moving off center.
*/
heur samey = {{
  {  0,  1 },			/* Prefer straight to goal	*/
  {  1,  1 },			/* then toward goal X, +center	*/
  { -1,  1 },			/* then toward goal X, -center	*/
  {  0,  0 },			/* Prefer staying put		*/
  {  1,  0 },			/* then compromize Y, +center	*/
  { -1,  0 },			/* then compromise Y, -center	*/
  {  0, -1 },			/* then compromise X		*/
  {  1, -1 },			/* then compromise both, +center*/
  { -1, -1 },			/* then compromise both, -center*/
}};
/*
 * Evaluation function when goal on same X
 *   This is just like samey with X and Y deltas swapped.
*/
heur samex = {{
  {  1,  0 },			/* Prefer straight to goal	*/
  {  1,  1 },			/* then toward goal Y, +center	*/
  {  1, -1 },			/* then toward goal Y, -center	*/
  {  0,  0 },			/* Prefer staying put		*/
  {  0,  1 },			/* then compromize X, +center	*/
  {  0, -1 },			/* then compromise X, -center	*/
  { -1,  0 },			/* then compromise Y		*/
  { -1,  1 },			/* then compromise both, +center*/
  { -1, -1 },			/* then compromise both, -center*/
}};

makemove(gy, gx)
int gy, gx;
{ register int r, c;
  int dr, dc, i;
  heur *h;

  h = &diag;
  r = sgn(gy-atrow);
  c = sgn(gx-atcol);
  if (c && !r) {
    h = &samey;
    r = sgn(ROWS/2 - atrow);
    if (r == 0)
      r = -1;
  } else if (r && !c) {
    h = &samex;
    c = sgn(COLS/2 - atcol);
    if (c == 0)
      c = -1;
  }
  for (i = 0; i < 9; i++) {
    dr = r*h->h[i][0];
    dc = c*h->h[i][1];
    if (safe(atrow+dr, atcol+dc))
      return(keymap[1+dr][1+dc]);
  }
  return(0);
}

/****************************************************************
 * dwait: Debugging message
 ****************************************************************/
/*VARARGS1*/
dwait (f, a1, a2, a3, a4)
char *f;
int a1, a2, a3, a4;
{ register int c;

  if (debug)
  { mvprintw (0, 0, "[%d,%d] ", atrow, atcol);
    printw (f, a1, a2, a3, a4);
    addch (' ');
    move (row, col);
    refresh ();
    switch (getchar ())
    { case 'd':	debug=0; break;
      case 'r': clear (); refresh (); drawscreen (); refresh (); break;
      default:	break;
    }
    for (c=0; c<COLS; c++)
    { mvaddch (0, c, (chtype)screen[0][c]); }
  }
}

/*
 *  put_env  --  put value into environment
 *
 *  Usage:  i = put_env (name,value)
 *	int i;
 *	char *name, *value;
 *
 *  Putenv associates "value" with the environment parameter "name".
 *  If "value" is 0, then "name" will be deleted from the environment.
 *  Putenv returns 0 normally, -1 on error (not enough core for malloc).
 *
 *  Putenv may need to add a new name into the environment, or to
 *  associate a value longer than the current value with a particular
 *  name.  So, to make life simpler, put_env() copies your entire
 *  environment into the heap (i.e. malloc()) from the stack
 *  (i.e. where it resides when your process is initiated) the first
 *  time you call it.
 *
 *  HISTORY
 * 14-Oct-85 Michael Mauldin (mlm) at Carnegie-Mellon University
 *      Ripped out of CMU lib for Rob-O-Matic portability
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Created for VAX.  It's unfortunate that you have to copy the whole
 *	environment onto the heap, but the bookkeeping-and-not-so-much-copying
 *	approach turns out to be much hairier.  So, I decided to do the simple
 *	thing, copying the entire environment onto the heap the first time you
 *	call put_env(), then doing realloc() uniformly later on.
 *	Note that "put_env(name,getenv(name))" is a no-op; that's the reason
 *	for the use of a 0 pointer to tell put_env() to delete an entry.
 *
 */

#define EXTRASIZE 5		/* increment to add to env. size */

extern char  *malloc (), *realloc (), *strcpy ();
extern int   strlen ();
extern void  free ();

static int  envsize = -1;	/* current size of environment */
extern char **environ;		/* the global which is your env. */

static int  findenv ();		/* look for a name in the env. */
static int  newenv ();		/* copy env. from stack to heap */
static int  moreenv ();		/* incr. size of env. */

int   put_env (name, value)
char *name, *value;
{ register int  i, j;
  register char *p;

  if (envsize < 0)
  {				/* first time put_env called */
    if (newenv () < 0)		/* copy env. to heap */
      return (-1);
  }

  i = findenv (name);		/* look for name in environment */

  if (value)
  {				/* put value into environment */
    if (i < 0)
    {				/* name must be added */
      for (i = 0; environ[i]; i++);
      if (i >= (envsize - 1))
      {				/* need new slot */
	if (moreenv () < 0)
	  return (-1);
      }
      p = malloc ((unsigned)(strlen (name) + strlen (value) + 2));
      if (p == 0)		/* not enough core */
	return (-1);
      environ[i + 1] = 0;	/* new end of env. */
    }
    else
    {				/* name already in env. */
      p = realloc (environ[i],
	  (unsigned)(strlen (name) + strlen (value) + 2));
      if (p == 0)
	return (-1);
    }
    sprintf (p, "%s=%s", name, value);/* copy into env. */
    environ[i] = p;
  }
  else
  {				/* delete name from environment */
    if (i >= 0)
    {				/* name is currently in env. */
      free (environ[i]);
      for (j = i; environ[j]; j++);
      environ[i] = environ[j - 1];
      environ[j - 1] = 0;
    }
  }

  return (0);
}

static int  findenv (name)
char *name;
{ register char *namechar, *envchar;
  register int  i, found;

  found = 0;
  for (i = 0; environ[i] && !found; i++)
  { envchar = environ[i];
    namechar = name;
    while (*namechar && (*namechar == *envchar))
    { namechar++;
      envchar++;
    }
    found = (*namechar == '\0' && *envchar == '=');
  }
  return (found ? i - 1 : -1);
}

static int  newenv ()
{ register char **env, *elem;
  register unsigned i, esize;

  for (i = 0; environ[i]; i++);
  esize = i + EXTRASIZE + 1;
  env = (char **) malloc (esize * sizeof (elem));
  if (env == 0)
    return (-1);

  for (i = 0; environ[i]; i++)
  { elem = malloc ((unsigned)(strlen (environ[i]) + 1));
    if (elem == 0)
      return (-1);
    env[i] = elem;
    strcpy (elem, environ[i]);
  }

  env[i] = 0;
  environ = env;
  envsize = esize;
  return (0);
}

static int  moreenv ()
{ register unsigned esize;
  register char **env;

  esize = envsize + EXTRASIZE;
  env = (char **) realloc ((char *)environ, esize * sizeof (*env));
  if (env == 0)
    return (-1);
  environ = env;
  envsize = esize;
  return (0);
}
