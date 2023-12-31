/*	SCCS Id: @(#)cmd.c	1.4	87/08/08
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* cmd.c - version 1.0.3 */

#include	"hack.h"
#include	"func_tab.h"

int doredraw(),doredotopl(),dodrop(),dodrink(),doread(),dosearch(),dopickup(),
doversion(),doweararm(),dowearring(),doremarm(),doddoremarm(),doremring(),
dopay(),doapply(),dosave(),dowield(),ddoinv(),dozap(),ddocall(),dowhatis(),
doengrave(),dotele(),dohelp(),doeat(),doddrop(),do_mname(),doidtrap(),
doprwep(),doprarm(),doprring(),doprgold(),dodiscovered(),dotypeinv(),dolook(),
doset(),doup(), dodown(), done1(), donull(), dothrow(), doextcmd(), dodip(),
dopray(), doextlist();
#ifdef NEWCLASS
int dosit(), doturn();
#endif
#ifdef SPELLS
int docast(), dovspell(), doxcribe();
#endif
#ifdef SHELL
int dosh();
#endif
#ifdef SUSPEND
int dosuspend();
#endif
#ifdef KAA
int doremove(), dobreathe();
# ifdef KOPS
int dowipe();
# endif
#endif

int rndobjsym(), rndmonsym();
char *hcolor(), *rndmonnam(), *defmonnam();

extern char *occtxt;
extern int (*occupation)();

#ifdef DGKMOD
int dotogglepickup(), doMSCversion();
# ifdef DEBUG
int dodebug();
# endif

static int (*timed_occ_fn)();

/* Count down by decrementing multi */
timed_occupation() {
	(*timed_occ_fn)();
	if (multi > 0)
		multi--;
	return (multi > 0);
}

/* If a time is given, use it to timeout this function, otherwise the
 * function times out by its own means.
 */
void
set_occupation(fn, txt, time)
int (*fn)();
char *txt;
{
	if (time) {
		occupation = timed_occupation;
		timed_occ_fn = fn;
	} else
		occupation = fn;
	occtxt = txt;
	occtime = 0;
}
#endif /* DGKMOD */

#ifdef REDO
/* Provide a means to redo the last command.  The flag `in_doagain' is set
 * to true while redoing the command.  This flag is tested in commands that
 * require additional input (like `throw' which requires a thing and a
 * direction), and the input prompt is not shown.  Also, while in_doagain is
 * TRUE, no keystrokes can be saved into the saveq.
 */
#define BSIZE 20
static char pushq[BSIZE], saveq[BSIZE];
static int phead, ptail, shead, stail;
extern int in_doagain;

char
popch() {
	/* If occupied, return 0, letting tgetch know a character should
	 * be read from the keyboard.  If the character read is not the
	 * ABORT character (as checked in main.c), that character will be
	 * pushed back on the pushq.
	 */
	if (occupation) return(0);
	if (in_doagain) return ((shead != stail) ? saveq[stail++] : 0);
	else		return ((phead != ptail) ? pushq[ptail++] : 0);
}

/* A ch == 0 resets the pushq */
void
pushch(ch)
char ch;
{
	if (!ch)
		phead = ptail = 0;
	if (phead < BSIZE)
		pushq[phead++] = ch;
}

/* A ch == 0 resets the saveq.  Only save keystrokes when not
 * replaying a previous command.
 */
void
savech(ch)
char ch;
{
	if (!in_doagain) {
		if (!ch)
			phead = ptail = shead = stail = 0;
		else if (shead < BSIZE)
			saveq[shead++] = ch;
	}
}
#endif /* REDO */

struct func_tab cmdlist[]={
	{'\020', doredotopl},
	{'\022', doredraw},
	{'\024', dotele},
#ifdef SUSPEND
	{'\032', dosuspend},
#endif
	{'a', doapply},
	{'A', doddoremarm},
/*	'b', 'B' : go sw */
	{'c', ddocall},
	{'C', do_mname},
	{'d', dodrop},
	{'D', doddrop},
	{'e', doeat},
	{'E', doengrave},
/* Soon to be
	{'f', dofight, "fighting"},
	{'F', doFight, "fighting"},
 */
/*	'g', 'G' : multiple go */
/*	'h', 'H' : go west */
	{'I', dotypeinv},		/* Robert Viduya */
	{'i', ddoinv},
/*	'j', 'J', 'k', 'K', 'l', 'L', 'm', 'M', 'n', 'N' : move commands */
/*	'o', doopen,	*/
	{'O', doset},
	{'p', dopay},
	{'P', dowearring},
	{'q', dodrink},
	{'Q', done1},
	{'r', doread},
	{'R', doremring},
	{'s', dosearch, "searching"},
	{'S', dosave},
	{'t', dothrow},
	{'T', doremarm},
/*	'u', 'U' : go ne */
	{'v', doversion},
	{'w', dowield},
	{'W', doweararm},
#ifdef SPELLS
	{'x', dovspell},			/* Mike Stephenson */
	{'X', doxcribe},			/* Mike Stephenson */ 
#endif
/*	'y', 'Y' : go nw */
	{'z', dozap},
/*	'Z' : UNUSED */
	{'<', doup},
	{'>', dodown},
	{'/', dowhatis},
	{'?', dohelp},
#ifdef SHELL
	{'!', dosh},
#endif
	{'.', donull, "waiting"},
	{' ', donull, "waiting"},
	{',', dopickup},
	{':', dolook},
	{'^', doidtrap},
	{'\\', dodiscovered},		/* Robert Viduya */
#ifdef DGKMOD
	{'@', dotogglepickup},
	{'V', doMSCversion},
# ifdef DEBUG_DOESNT_WORK
	{'\004', dodebug},	/* generic debug function */
# endif
#endif
	{WEAPON_SYM,  doprwep},
	{ARMOR_SYM,  doprarm},
	{RING_SYM,  doprring},
	{GOLD_SYM, doprgold},
	{'#', doextcmd},
	{0,0,0}
};

struct ext_func_tab extcmdlist[] = {
#ifdef KAA
	"breathe", "breathe like a dragon", dobreathe,
#endif
#ifdef SPELLS
	"cast", "cast a spell", docast,
#endif
	"dip", "dip an object into something", dodip,
	"pray", "pray to the gods for help", dopray,
#ifdef KAA
	"remove", "remove a cursed item", doremove,
#endif
#ifdef NEWCLASS
	"sit", "sit down", dosit,
	"turn", "turn undead", doturn,
#endif
#if defined(KOPS) && defined(KAA)
	"wipe", "wipe your face off", dowipe,
#endif
	"?", "get this list of extended commands", doextlist,
	(char *) 0, (char *) 0, donull
};

extern char *parse(), lowc(), unctrl(), quitchars[];

rhack(cmd)
register char *cmd;
{
	register struct func_tab *tlist = cmdlist;
	boolean firsttime = FALSE;
	register res;

	if(!cmd) {
		firsttime = TRUE;
		flags.nopick = 0;
		cmd = parse();
	}
#ifdef REDO
	if (*cmd == DOAGAIN && !in_doagain && saveq[0]) {
		in_doagain = TRUE;
		stail = 0;
		rhack((char *) 0);	/* read and execute command */
		in_doagain = FALSE;
		return;
	}

	/* Special case of *cmd == ' ' handled better below */
	if(!*cmd || *cmd == (char)0377) {
#else
	if(!*cmd || *cmd == (char)0377 || (flags.no_rest_on_space && *cmd == ' ')){
#endif
		bell();
		flags.move = 0;
		return(0);	/* probably we just had an interrupt */
	}
	if(movecmd(*cmd)) {
	walk:
		if(multi) flags.mv = 1;
		domove();
		return;
	}
	if(movecmd(lowc(*cmd))) {
		flags.run = 1;
	rush:
		if(firsttime){
			if(!multi) multi = COLNO;
			u.last_str_turn = 0;
		}
		flags.mv = 1;
#ifdef QUEST
		if(flags.run >= 4) finddir();
		if(firsttime){
			u.ux0 = u.ux + u.dx;
			u.uy0 = u.uy + u.dy;
		}
#endif
		domove();
		return;
	}
	if((*cmd == 'g' && movecmd(cmd[1])) || movecmd(unctrl(*cmd))) {
		flags.run = 2;
		goto rush;
	}
	if(*cmd == 'G' && movecmd(lowc(cmd[1]))) {
		flags.run = 3;
		goto rush;
	}
	if(*cmd == 'm' && movecmd(cmd[1])) {
		flags.run = 0;
		flags.nopick = 1;
		goto walk;
	}
	if(*cmd == 'M' && movecmd(lowc(cmd[1]))) {
		flags.run = 1;
		flags.nopick = 1;
		goto rush;
	}
#ifdef QUEST
	if(*cmd == cmd[1] && (*cmd == 'g' || *cmd == 'G')) {
		flags.run = 4;
		if(*cmd == 'G') flags.run += 2;
		if(cmd[2] == '-') flags.run += 1;
		goto rush;
	}
#endif
	while(tlist->f_char) {
		if(*cmd == tlist->f_char){
#ifdef DGKMOD
			/* Special case of *cmd == ' ' handled here */
			if (*cmd == ' ' && flags.no_rest_on_space)
				break;

			/* Now control-A can stop lengthy commands */
			if (tlist->f_text && !occupation && multi)
				set_occupation(tlist->f_funct, tlist->f_text,
					multi);
#endif
			res = (*(tlist->f_funct))();
			if(!res) {
				flags.move = 0;
				multi = 0;
			}
			return;
		}
		tlist++;
	}
	{ char expcmd[10];
	  register char *cp = expcmd;
	  while(*cmd && cp-expcmd < sizeof(expcmd)-2) {
		if(*cmd >= 040 && *cmd < 0177)
			*cp++ = *cmd++;
		else {
			*cp++ = '^';
			*cp++ = *cmd++ ^ 0100;
		}
	  }
	  *cp++ = 0;
	  pline("Unknown command '%s'.", expcmd);
	}
	multi = flags.move = 0;
}

doextcmd()	/* here after # - now read a full-word command */
{
	char buf[BUFSZ];
	register struct ext_func_tab *efp = extcmdlist;

	pline("# ");
#ifdef COM_COMPL
	get_ext_cmd(buf);
#else
	getlin(buf);
#endif
	clrlin();
	if(buf[0] == '\033')
		return(0);
	while(efp->ef_txt) {
		if(!strcmp(efp->ef_txt, buf))
			return((*(efp->ef_funct))());
		efp++;
	}
	pline("%s: unknown command.", buf);
	return(0);
}

doextlist()	/* here after #? - now list all full-word commands */
{
	register struct ext_func_tab *efp = extcmdlist;
	char     buf[BUFSZ];

	set_pager(0);
	if(page_line("") ||
	   page_line("        Extended Command Set:") ||
	   page_line(""))					 goto quit;

	while(efp->ef_txt) {

		(void)sprintf(buf, "    %-8s  - %s.", efp->ef_txt, efp->ef_desc);
		if(page_line(buf)) goto quit;
		efp++;
	}
	set_pager(1);
	return(0);
quit:
	set_pager(2);
	return(0);
}

char
lowc(sym)
char sym;
{
    return( (sym >= 'A' && sym <= 'Z') ? sym+'a'-'A' : sym );
}

char
unctrl(sym)
char sym;
{
    return( (sym >= ('A' & 037) && sym <= ('Z' & 037)) ? sym + 0140 : sym );
}

/* 'rogue'-like direction commands */
char sdir[] = "hykulnjb><";
schar xdir[10] = { -1,-1, 0, 1, 1, 1, 0,-1, 0, 0 };
schar ydir[10] = {  0,-1,-1,-1, 0, 1, 1, 1, 0, 0 };
schar zdir[10] = {  0, 0, 0, 0, 0, 0, 0, 0, 1,-1 };

movecmd(sym)	/* also sets u.dz, but returns false for <> */
char sym;
{
	register char *dp;

	u.dz = 0;
	if(!(dp = index(sdir, sym))) return(0);
	u.dx = xdir[dp-sdir];
	u.dy = ydir[dp-sdir];
	u.dz = zdir[dp-sdir];
	return(!u.dz);
}

getdir(s)
boolean s;
{
	char dirsym;

#ifdef REDO
	if (!in_doagain)
#endif
	    if(s) pline("In what direction?");
	dirsym = readchar();
#ifdef REDO
	savech(dirsym);
#endif
#ifdef KAA
	if(dirsym == '.' || dirsym == 's')
		u.dx = u.dy = u.dz = 0;
	else
#endif
	if(!movecmd(dirsym) && !u.dz) {
		if(!index(quitchars, dirsym))
			pline("What a strange direction!");
		return(0);
	}
	if(Confusion && !u.dz) confdir();
	return(1);
}

confdir()
{
	register x = rn2(8);
	u.dx = xdir[x];
	u.dy = ydir[x];
}

#ifdef QUEST
finddir(){
register int i, ui = u.di;
	for(i = 0; i <= 8; i++){
		if(flags.run & 1) ui++; else ui += 7;
		ui %= 8;
		if(i == 8){
			pline("Not near a wall.");
			flags.move = multi = 0;
			return(0);
		}
		if(!isroom(u.ux+xdir[ui], u.uy+ydir[ui]))
			break;
	}
	for(i = 0; i <= 8; i++){
		if(flags.run & 1) ui += 7; else ui++;
		ui %= 8;
		if(i == 8){
			pline("Not near a room.");
			flags.move = multi = 0;
			return(0);
		}
		if(isroom(u.ux+xdir[ui], u.uy+ydir[ui]))
			break;
	}
	u.di = ui;
	u.dx = xdir[ui];
	u.dy = ydir[ui];
}

isroom(x,y)  register x,y; {		/* what about POOL? */
	return(isok(x,y) && (levl[x][y].typ == ROOM ||
				(levl[x][y].typ >= LDOOR && flags.run >= 6)));
}
#endif /* QUEST /**/

isok(x,y) register x,y; {
	/* x corresponds to curx, so x==1 is the first column. Ach. %% */
	return(x >= 1 && x <= COLNO-1 && y >= 0 && y <= ROWNO-1);
}
