/* @(#)llib-lcurses	1.5 (Berkeley) 3/27/83 */
#ifdef __STDC__
/* LIBPROTO */
#else
/* LINTLIBRARY */
#endif
# include	"curses.h"
#ifndef __STDC__
/*
 * Fake declarations; all of these are declared as externs in curses.h,
 * but lint needs something a little more solid than that
 */

bool	AM, BS, CA, DA, DB, EO, GT, HZ, IN, MI, MS, NC, OS, UL, XN;

char	*AL, *BC, *BT, *CD, *CE, *CL, *CM, *CR, *DC, *DL, *DM, *DO, *ED,
	*EI, *HO, *IC, *IM, *IP, *LL,*NL,  *MA, *ND, *SE, *SF, *SO, *SR,
	*TA, *TE, *TI, *UC, *UE, *UP, *US, *VB, *VE, *VS, PC;

char *_unctrl[];

/*
 * From the tty modes...
 */

bool	NONL, UPPERCASE, normtty, _pfast;

# define	WINDOW	struct _win_st

bool	My_term, _echoit, _rawmode, _endwin;

char	*Def_term, ttytype[];

int	LINES, COLS, _tty_ch, _res_flg;

SGTTY	_tty;

WINDOW	*stdscr, *curscr;

	box(win,vert,hor) WINDOW *win; int vert, hor; {}
	delwin(win) WINDOW *win; {}
	endwin() {}
	gettmode() {}
WINDOW *initscr() {
	AL = AL;
	AM = AM;
	BC = BC;
	BS = BS;
	BT = BT;
	CA = CA;
	CD = CD;
	CE = CE;
	CL = CL;
	CM = CM;
	DA = DA;
	DB = DB;
	DC = DC;
	DL = DL;
	DM = DM;
	DO = DO;
	ED = ED;
	EO = EO;
	EI = EI;
	GT = GT;
	HO = HO;
	HZ = HZ;
	IC = IC;
	IN = IN;
	IM = IM;
	IP = IP;
	LL = LL;
	MA = MA;
	MI = MI;
	NC = NC;
	ND = ND;
	OS = OS;
	PC = PC;
	SE = SE;
	SF = SF;
	SO = SO;
	SR = SR;
	TA = TA;
	TE = TE;
	TI = TI;
	UC = UC;
	UE = UE;
	UL = UL;
	UP = UP;
	US = US;
	VB = VB;
	VE = VE;
	VS = VS;
	XN = XN;
	NONL = NONL;
	UPPERCASE = UPPERCASE;
	normtty = normtty;
	_pfast = _pfast;
	_tty = _tty;
	My_term = My_term;
	_echoit = _echoit;
	_rawmode = _rawmode;
	LINES = LINES;
	COLS = COLS;
	_tty_ch = _tty_ch;
	_res_flg = _res_flg;
	stdscr = stdscr;
	curscr = curscr;
	_echoit = _echoit;
	_rawmode = _rawmode;
	_tty_ch = _tty_ch;
	return (WINDOW *) 0;
}
char *	getcap() { return 0; }
char *	longname(bp, def) char *bp, *def; { return bp; }
	mvcur(ly,lx,y,x) int ly, lx, y, x; { }
/* VARARGS3 */
	mvprintw(y,x,fmt) int y, x; char *fmt; { return 0; }
/* VARARGS3 */
	mvscanw(y,x,fmt) int y, x; char *fmt; { return 0; }
/* VARARGS4 */
	mvwprintw(win,y,x,fmt) WINDOW *win; int y, x; char *fmt; { return 0; }
/* VARARGS4 */
	mvwscanw(win,y,x,fmt) WINDOW *win; int y, x; char *fmt; { return 0; }
WINDOW *newwin(Nl, Nc, by, bx) int Nl, Nc, by, bx; { return (WINDOW *) 0; }
	overlay(win1, win2) WINDOW *win1, *win2; { }
	overwrite(win1, win2) WINDOW *win1, *win2; { }
/* VARARGS1 */
	printw(fmt) char *fmt; { return 0; }
/* VARARGS1 */
	scanw(fmt) char *fmt; { return 0; }
	scroll(win) WINDOW *win; { return 0; }
	setterm(type) char *type; { return 0; }
WINDOW *subwin(o, Nl, Nc, by, bx) WINDOW *o; int Nl, Nc, by, bx; { return o; }
	waddch(win,ch) WINDOW *win; char ch; { return 0; }
	waddstr(win,str) WINDOW *win; char *str; { return 0; }
	wclear(win) WINDOW *win; { return 0; }
	wclrtobot(win) WINDOW *win; { return 0; }
	wclrtoeol(win) WINDOW *win; { return 0; }
	wdeleteln(win) WINDOW *win; { return 0; }
	werase(win) WINDOW *win; { return 0; }
	wgetch(win) WINDOW *win; { return '0'; }
	wgetstr(win,str) WINDOW *win; char *str; { return 0; }
	winsertln(win) WINDOW *win; { return 0; }
	wmove(win,y,x) WINDOW *win; int y, x; { return 0; }
/* VARARGS2 */
	wprintw(win,fmt) WINDOW *win; char *fmt; { return 0; }
	wrefresh(win) WINDOW *win; { return 0; }
/* VARARGS2 */
	wscanw(win,fmt) WINDOW *win; char *fmt; { return 0; }
	/* These really return char *'s but this will do. */
int	wstandout(win) WINDOW *win; { return 0; }
int	wstandend(win) WINDOW *win; { return 0; }
#endif
