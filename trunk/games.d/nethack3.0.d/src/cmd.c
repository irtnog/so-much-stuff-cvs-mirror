/*	SCCS Id: @(#)cmd.c	3.0	88/10/24
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include	"hack.h"
#include	"func_tab.h"

#ifdef DUMB	/* stuff commented out in extern.h, but needed here */
extern int doapply(); /**/
extern int dorub(); /**/
extern int dojump(); /**/
extern int doextlist(); /**/
extern int dodrop(); /**/
extern int doddrop(); /**/
extern int dodown(); /**/
extern int doup(); /**/
extern int donull(); /**/
extern int dowipe(); /**/
extern int do_mname(); /**/
extern int ddocall(); /**/
extern int dotakeoff(); /**/
extern int doremring(); /**/
extern int dowear(); /**/
extern int doputon(); /**/
extern int doddoremarm(); /**/
extern int dokick(); /**/
extern int dothrow(); /**/
extern int doeat(); /**/
extern int done2(); /**/
extern int doengrave(); /**/
extern int dopickup(); /**/
extern int ddoinv(); /**/
extern int dotypeinv(); /**/
extern int dolook(); /**/
extern int doprgold(); /**/
extern int doprwep(); /**/
extern int doprarm(); /**/
extern int doprring(); /**/
extern int dopramulet(); /**/
extern int doprtool(); /**/
extern int dosuspend(); /**/
extern int doforce(); /**/
extern int doopen(); /**/
extern int doclose(); /**/
extern int dosh(); /**/
extern int dodiscovered(); /**/
extern int doset(); /**/
extern int dotogglepickup(); /**/
extern int dowhatis(); /**/
extern int dowhatdoes(); /**/
extern int dohelp(); /**/
extern int dohistory(); /**/
extern int dosh(); /**/
extern int doloot(); /**/
extern int dodrink(); /**/
extern int dodip(); /**/
extern int dosacrifice(); /**/
extern int dopray(); /**/
extern int doturn(); /**/
extern int doredraw(); /**/
extern int doread(); /**/
extern int dosave(); /**/
extern int dosave0(); /**/
extern int dosearch(); /**/
extern int dosearch0 P((int)); /**/
extern int doidtrap(); /**/
extern int dopay(); /**/
extern int dosit(); /**/
extern int dotalk(); /**/
extern int docast(); /**/
extern int dovspell(); /**/
extern int doredotopl(); /**/
extern int dotele(); /**/
extern int dountrap(); /**/
extern int doversion(); /**/
extern int dowield(); /**/
extern int dozap(); /**/
#endif /* DUMB */

static int (*timed_occ_fn)();
#ifdef POLYSELF
static int domonability();
#endif

/* Count down by decrementing multi */
static int
timed_occupation() {
	(*timed_occ_fn)();
	if (multi > 0)
		multi--;
	return multi > 0;
}

/* If a time is given, use it to timeout this function, otherwise the
 * function times out by its own means.
 */
void
set_occupation(fn, txt, xtime)
int (*fn)();
char *txt;
int xtime;
{
	if (xtime) {
		occupation = timed_occupation;
		timed_occ_fn = fn;
	} else
		occupation = fn;
	occtxt = txt;
	occtime = 0;
	return;
}

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

static char
popch() {
	/* If occupied, return 0, letting tgetch know a character should
	 * be read from the keyboard.  If the character read is not the
	 * ABORT character (as checked in pcmain.c), that character will be
	 * pushed back on the pushq.
	 */
	if (occupation) return 0;
	if (in_doagain) return (shead != stail) ? saveq[stail++] : 0;
	else		return (phead != ptail) ? pushq[ptail++] : 0;
}

char
pgetchar() {		/* curtesy of aeb@cwi.nl */
	register int ch;

	if(!(ch = popch()))
		ch = tgetch();
	return(ch);
}

/* A ch == 0 resets the pushq */
#ifdef __STDC__
void
pushch(char ch)
#else
void
pushch(ch)
char ch;
#endif
{
	if (!ch)
		phead = ptail = 0;
	if (phead < BSIZE)
		pushq[phead++] = ch;
	return;
}

/* A ch == 0 resets the saveq.  Only save keystrokes when not
 * replaying a previous command.
 */
#ifdef __STDC__
void
savech(char ch)
#else
void
savech(ch)
char ch;
#endif
{
	if (!in_doagain) {
		if (!ch)
			phead = ptail = shead = stail = 0;
		else if (shead < BSIZE)
			saveq[shead++] = ch;
	}
	return;
}
#endif /* REDO */

static int
doextcmd()	/* here after # - now read a full-word command */
{
	char buf[BUFSZ];
	register const struct ext_func_tab *efp = extcmdlist;
again:
	pline("# ");
#ifdef COM_COMPL
	get_ext_cmd(buf);
#else
	getlin(buf);
#endif
	clrlin();
	if(buf[0] == '\0' || buf[0] == '\033')
		return 0;
	if(buf[0] == '?') {
		(void) doextlist();
		goto again;
	}
	while(efp->ef_txt) {
		if(!strcmp(efp->ef_txt, buf))
			return (*(efp->ef_funct))();
		efp++;
	}
	pline("%s: unknown extended command.", buf);
	return 0;
}

int
doextlist()	/* here after #? - now list all full-word commands */
{
	register const struct ext_func_tab *efp = extcmdlist;
	char     buf[BUFSZ];

	set_pager(0);
	if(page_line("") ||
	   page_line("            Extended Commands List") ||
	   page_line("") ||
	   page_line("    Press '#', then type (first letter only):") ||
	   page_line(""))					 goto quit;

	while(efp->ef_txt) {

		Sprintf(buf, "    %-8s  - %s.", efp->ef_txt, efp->ef_desc);
		if(page_line(buf)) goto quit;
		efp++;
	}
	set_pager(1);
	return 0;
quit:
	set_pager(2);
	return 0;
}

#ifdef POLYSELF
static int
domonability()
{
	if (can_breathe(uasmon)) return dobreathe();
	else if (attacktype(uasmon, AT_SPIT)) return dospit();
	else if (u.usym == S_NYMPH) return doremove();
	else if (u.usym == S_UMBER) return doconfuse();
	else if (is_were(uasmon)) return dosummon();
	else if (webmaker(uasmon)) return dospinweb();
	else if (is_hider(uasmon)) return dohide();
	else if (u.umonnum >= 0)
		pline("Any special ability you may have is purely reflexive.");
	else You("don't have a special ability!");
	return 0;
}
#endif

#ifdef WIZARD
static int
wiz_wish()	/* Unlimited wishes for wizard mode by Paul Polderman */
{
	if (wizard)	makewish();
	else		pline("Unavailable command '^W'.");
	return 0;
}

static int
wiz_identify()
{
	struct obj *obj;

	if (!wizard)
		pline("Unavailable command '^I'.");
	else {
		for (obj = invent; obj; obj = obj->nobj)
			if (!objects[obj->otyp].oc_name_known || !obj->known
						|| !obj->dknown || !obj->bknown)
				(void) identify(obj);
	}
	return 0;
}

static int
wiz_map()
{
	if (wizard)	do_mapping();
	else		pline("Unavailable command '^F'.");
	return 0;
}

static int
wiz_genesis()
{
	if (wizard)	(void) create_particular();
	else		pline("Unavailable command '^G'.");
	return 0;
}

static int
wiz_where()
{
	if (wizard) {
		pline("Medusa:%d  Wiz:%d  Big:%d", medusa_level, wiz_level, bigroom_level);
#ifdef STRONGHOLD
#  ifdef MUSIC
		pline("Castle:%d (tune %s)  Tower:%d-%d", 
		      stronghold_level, tune, tower_level, tower_level+2);
#  else
		pline("Castle:%d  Tower:%d-%d",
		      stronghold_level, tower_level, tower_level+2);
#  endif
#endif
#ifdef REINCARNATION
		pline("Rogue:%d", rogue_level);
#endif
#ifdef ORACLE
		pline("Oracle:%d", oracle_level);
#endif
	}
	else	pline("Unavailable command '^O'.");
	return 0;
}

static int
wiz_detect()
{
	if(wizard)  (void) findit();
	else	    pline("Unavailable command '^E'.");
	return 0;
}

static int
wiz_level_tele()
{
	if (wizard)	level_tele();
	else		pline("Unavailable command '^V'.");
	return 0;
}

#endif /* WIZARD */

void
enlightenment() {

	cornline(0, "Current Attributes:");

	if (u.ualign == 0) cornline(1, "You are nominally aligned.");
	else if (u.ualign > 3) cornline(1, "You are stridently aligned.");
	else if (u.ualign > 0) cornline(1, "You are haltingly aligned.");
	else cornline(1, "You have strayed.");

	if (Adornment) cornline(1, "You are adorned.");
	if (Teleportation) cornline(1, "You can teleport.");
	if (Regeneration) cornline(1, "You regenerate.");
	if (Searching) cornline(1, "You have automatic searching.");
	if (See_invisible) cornline(1, "You see invisible.");
	if (Stealth) cornline(1, "You are stealthy.");
	if (Levitation) cornline(1, "You are levitating.");
	if (Hunger) cornline(1, "You have hunger.");
	if (Aggravate_monster) cornline(1, "You aggravate monsters.");
	if (Poison_resistance) cornline(1, "You are poison resistant.");
	if (Fire_resistance) cornline(1, "You are fire resistant.");
	if (Cold_resistance) cornline(1, "You are cold resistant.");
	if (Shock_resistance) cornline(1, "You are shock resistant.");
	if (Sleep_resistance) cornline(1, "You are sleep resistant.");
	if (Disint_resistance) cornline(1, "You are disintegration-resistant.");
	if (Protection_from_shape_changers)
		cornline(1, "You are protected from shape changers.");
	if (Conflict) cornline(1, "You cause conflict.");
	if (Protection) cornline(1, "You are protected.");
	if (Warning) cornline(1, "You are warned.");
	if (Teleport_control) cornline(1, "You have teleport control.");
	if (Polymorph) cornline(1, "You are polymorphing.");
	if (Polymorph_control) cornline(1, "You have polymorph control.");
	if (Telepat) cornline(1, "You are telepathic.");
	if (Fast) cornline(1, "You are fast.");
	/* if (Stunned) cornline(1, "You are stunned."); */
	/* if (Confusion) cornline(1, "You are confused."); */
	/* if (Sick) cornline(1, "You are sick."); */
	/* if (Blinded) cornline(1, "You are blinded."); */
	if (Invisible) cornline(1, "You are invisible.");
	else if (Invis) cornline(1, "You are invisible to others.");
	if (Wounded_legs) {
		char buf[41];

		Sprintf(buf, "You have wounded %s.",
						makeplural(body_part(LEG)));
		cornline(1, buf);
	}
	if (Stoned) cornline(1, "You are turning to stone.");
	/* if (Hallucination) cornline(1, "You are hallucinating."); */
	if (Glib) {
		char buf[41];

		Sprintf(buf, "You have slippery %s.",
						makeplural(body_part(FINGER)));
		cornline(1, buf);
	}
	if (Reflecting) cornline(1, "You have reflection.");
	if (Strangled) cornline(1, "You are being strangled.");
	if (Lifesaved) cornline(1, "Your life will be saved.");
	if (Fumbling) cornline(1, "You fumble.");
	if (Jumping) cornline(1, "You can jump.");
	if (Wwalking) cornline(1, "You can walk on water.");
	if (Antimagic) cornline(1, "You are magic-protected.");
	if (Displaced) cornline(1, "You are displaced.");
	if (Clairvoyant) cornline(1, "You are clairvoyant.");
	if (stone_luck(TRUE) > 0) cornline(1, "You have extra luck.");
	if (stone_luck(TRUE) < 0) cornline(1, "You have reduced luck.");
	if (carrying(LUCKSTONE)) {
		if (stone_luck(FALSE) <= 0)
			cornline(1, "Bad luck does not time out for you.");
		if (stone_luck(FALSE) >= 0)
			cornline(1, "Good luck does not time out for you.");
	}

	cornline(2, "");
	return;
}

#if defined(WIZARD) || defined(EXPLORE_MODE)
static int
wiz_attributes()
{
	if (wizard || discover)
		enlightenment();
	else
		pline("Unavailable command '^X'.");
	return 0;
}
#endif /* WIZARD || EXPLORE_MODE */

const struct func_tab cmdlist[]={
	{'\004', /* ^D */ dokick},	/* "D" is for door!...? */
#ifdef WIZARD
	{'\005', /* ^E */ wiz_detect},
	{'\006', /* ^F */ wiz_map},
	{'\007', /* ^G */ wiz_genesis},
	{'\011', /* ^I */ wiz_identify},
	{'\017', /* ^O */ wiz_where},
#endif
	{'\020', /* ^P */ doredotopl},
	{'\022', /* ^R */ doredraw},
	{'\024', /* ^T */ dotele},
#ifdef WIZARD
	{'\026', /* ^V */ wiz_level_tele},
	{'\027', /* ^W */ wiz_wish},
#endif
#if defined(WIZARD) || defined(EXPLORE_MODE)
	{'\030', /* ^X */ wiz_attributes},
#endif
#ifdef SUSPEND
	{'\032', /* ^Z */ dosuspend},
#endif
	{'a', doapply},
	{'A', doddoremarm},
/*	'b', 'B' : go sw */
	{'c', doclose},
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
	{'h', dohelp}, /* if number_pad is set */
	{'i', ddoinv},
	{'I', dotypeinv},		/* Robert Viduya */
/*	'j', 'J', 'k', 'K', 'l', 'L', 'm', 'M', 'n', 'N' : move commands */
	{'j', dojump}, /* if number_pad is on */
	{'k', dokick}, /* if number_pad is on */
	{'l', doloot}, /* if number_pad is on */
/*	'n' prefixes a count if number_pad is on */
	{'N', ddocall}, /* if number_pad is on */
	{'o', doopen},
	{'O', doset},
	{'p', dopay},
	{'P', doputon},
	{'q', dodrink},
	{'Q', done2},
	{'r', doread},
	{'R', doremring},
	{'s', dosearch, "searching"},
	{'S', dosave},
	{'t', dothrow},
	{'T', dotakeoff},
/*	'u', 'U' : go ne */
	{'u', dountrap}, /* if number_pad is on */
	{'v', doversion},
	{'V', dohistory},
	{'w', dowield},
	{'W', dowear},
#ifdef SPELLS
	{'x', dovspell},			/* Mike Stephenson */
#endif
/*	'y', 'Y' : go nw */
	{'z', dozap},
#ifdef SPELLS
	{'Z', docast},
#endif
	{'<', doup},
	{'>', dodown},
	{'/', dowhatis},
	{'&', dowhatdoes},
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
	{'@', dotogglepickup},
	{WEAPON_SYM,  doprwep},
	{ARMOR_SYM,  doprarm},
	{RING_SYM,  doprring},
	{AMULET_SYM, dopramulet},
	{TOOL_SYM, doprtool},
	{GOLD_SYM, doprgold},
#ifdef SPELLS
	{SPBOOK_SYM, dovspell},			/* Mike Stephenson */
#endif
	{'#', doextcmd},
	{0,0,0}
};

const struct ext_func_tab extcmdlist[] = {
	"chat", "talk to someone", dotalk,	/* converse? */
	"dip", "dip an object into something", dodip,
	"force", "force the lock on a chest", doforce,
	"jump", "jump to a location", dojump,
	"loot", "loot a box on the floor", doloot,
#ifdef POLYSELF
	"monster", "use a monster's special ability", domonability,
#endif
	"name", "name an item or type of object", ddocall,
#ifdef THEOLOGY
	"offer", "offer a sacrifice to the gods", dosacrifice,
	"pray", "pray to the gods for help", dopray,
#endif
	"rub", "rub a lamp", dorub,
	"sit", "sit down", dosit,
	"turn", "turn undead", doturn,
	"untrap", "untrap a trapped object", dountrap,
	"wipe", "wipe your face off", dowipe,
	"?", "get this list of extended commands", doextlist,
	NULL, NULL, donull
};

#ifdef __STDC__
char
unctrl(char sym)
#else
char
unctrl(sym)
char sym;
#endif
{
    return (sym >= ('A' & 037) && sym <= ('Z' & 037)) ? sym + 0140 : sym;
}

void
rhack(cmd)
register char *cmd;
{
	register const struct func_tab *tlist = cmdlist;
	boolean firsttime = FALSE;
	register int res;

	if(!cmd) {
		firsttime = TRUE;
		flags.nopick = 0;
		cmd = parse();
	}
	if(*cmd == (char)033) {
		flags.move = 0;
		return;
	}
#ifdef REDO
	if (*cmd == DOAGAIN && !in_doagain && saveq[0]) {
		in_doagain = TRUE;
		stail = 0;
		rhack(NULL);	/* read and execute command */
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
		return;		/* probably we just had an interrupt */
	}
	if(movecmd(*cmd)) {
	walk:
		if(multi) flags.mv = 1;
		domove();
		return;
	}
	if(!flags.num_pad && movecmd(lowc(*cmd))) {
		flags.run = 1;
	rush:
		if(firsttime){
			if(!multi) multi = COLNO;
			u.last_str_turn = 0;
		}
		flags.mv = 1;
		domove();
		return;
	}
	if(*cmd == 'g' && movecmd(cmd[1])) {
		flags.run = 2;
		goto rush;
	}
	if(((*cmd == 'G' || (flags.num_pad && *cmd == '5')) && 
	    movecmd(lowc(cmd[1]))) || movecmd(unctrl(*cmd))) {
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
	while(tlist->f_char) {
		if(*cmd == tlist->f_char){
			/* Special case of *cmd == ' ' handled here */
			if (*cmd == ' ' && flags.no_rest_on_space)
				break;

			/* Now control-A can stop lengthy commands */
			/* in the PC version only -- use ^C-N otherwise */
			if (tlist->f_text && !occupation && multi)
				set_occupation(tlist->f_funct, tlist->f_text,
					multi);
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
	return;
}

#ifdef __STDC__
char
lowc(char sym)
#else
char
lowc(sym)
char sym;
#endif
{
    return (sym >= 'A' && sym <= 'Z') ? sym+'a'-'A' : sym;
}

/* 'rogue'-like direction commands */
const char sdir[] = "hykulnjb><";
const char ndir[] = "47896321><";
const schar xdir[10] = { -1,-1, 0, 1, 1, 1, 0,-1, 0, 0 };
const schar ydir[10] = {  0,-1,-1,-1, 0, 1, 1, 1, 0, 0 };
const schar zdir[10] = {  0, 0, 0, 0, 0, 0, 0, 0, 1,-1 };

#ifdef WALKIES
#ifdef __STDC__
int
xytod(schar x,schar y)	/* convert an x,y pair into a direction code */
#else
int
xytod(x, y)	/* convert an x,y pair into a direction code */
schar x, y;
#endif
{
	register int dd;

	for(dd = 0; dd < 8; dd++)
	    if(x == xdir[dd] && y == ydir[dd]) return dd;

	return -1;
}

void
dtoxy(cc,dd)	/* convert a direction code into an x,y pair */
coord *cc;
register int dd;
{
	cc->x = xdir[dd];
	cc->y = ydir[dd];
	return;
}
#endif /* WALKIES */

#ifdef __STDC__
int
movecmd(char sym)	/* also sets u.dz, but returns false for <> */
#else
int
movecmd(sym)	/* also sets u.dz, but returns false for <> */
char sym;
#endif
{
	register const char *dp, *sdp = flags.num_pad ? ndir : sdir;

	u.dz = 0;
	if(!(dp = index(sdp, sym))) return 0;
	u.dx = xdir[dp-sdp];
	u.dy = ydir[dp-sdp];
	u.dz = zdir[dp-sdp];
	return !u.dz;
}

#ifdef __STDC__
int
getdir(boolean s)
#else
int
getdir(s)
boolean s;
#endif
{
	char dirsym;

#ifdef REDO
	if (!in_doagain)
#endif
	    if(s) pline("In what direction? ");
	dirsym = readchar();
#ifdef REDO
	savech(dirsym);
#endif
	if(dirsym == '.' || dirsym == 's')
		u.dx = u.dy = u.dz = 0;
	else if(!movecmd(dirsym) && !u.dz) {
		if(!index(quitchars, dirsym))
			pline("What a strange direction!");
		return 0;
	}
	if(!u.dz && (Stunned || (Confusion && !rn2(5)))) confdir();
	return 1;
}

void
confdir()
{
	register int x = rn2(8);
	u.dx = xdir[x];
	u.dy = ydir[x];
	return;
}

int
isok(x,y)
register int x, y;
{
	/* x corresponds to curx, so x==1 is the first column. Ach. %% */
	return x >= 1 && x <= COLNO-1 && y >= 0 && y <= ROWNO-1;
}
