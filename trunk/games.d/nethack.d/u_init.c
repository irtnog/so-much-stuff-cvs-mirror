/*	SCCS Id: @(#)u_init.c	1.4	87/08/08
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* u_init.c - version 1.0.3   */

#include <stdio.h>
#include <signal.h>
#include "hack.h"
#ifdef GENIX
#define	void	int
#endif

#define Strcpy	(void) strcpy
#define	Strcat	(void) strcat
#define	UNDEF_TYP	0
#define	UNDEF_SPE	'\177'
extern struct obj *addinv();
extern char *eos();
extern char plname[];

struct you zerou;
char pl_character[PL_CSIZ];
char *(roles[]) = {	/* must all have distinct first letter */
			/* roles[4] & [7] may be changed for females */
	"Archeologist", "Tourist", "Fighter", "Knight", "Cave-man",
#ifdef NEWCLASS
	"Samurai", "Ninja", "Priest",
#endif
#ifdef KAA
	"Valkyrie", "Elf", "Healer",
#endif
	"Wizard"
};
#define	NR_OF_ROLES	SIZE(roles)
char rolesyms[NR_OF_ROLES + 1];		/* filled by u_init() */

struct trobj {
	uchar trotyp;
	schar trspe;
	char trolet;
	Bitfield(trquan,6);
	Bitfield(trknown,1);
};

#ifdef WIZARD
struct trobj Extra_objs[] = {
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};
#endif

struct trobj Cave_man[] = {
#ifdef KAA
	{ CLUB, 1, WEAPON_SYM, 1, 1 },
#else
	{ MACE, 1, WEAPON_SYM, 1, 1 },
#endif
	{ BOW, 1, WEAPON_SYM, 1, 1 },
	{ ARROW, 0, WEAPON_SYM, 25, 1 },	/* quan is variable */
	{ LEATHER_ARMOR, 0, ARMOR_SYM, 1, 1 },
	{ 0, 0, 0, 0, 0}
};

struct trobj Fighter[] = {
	{ TWO_HANDED_SWORD, 0, WEAPON_SYM, 1, 1 },
	{ RING_MAIL, 0, ARMOR_SYM, 1, 1 },
	{ 0, 0, 0, 0, 0 }
};

struct trobj Knight[] = {
	{ LONG_SWORD, 0, WEAPON_SYM, 1, 1 },
	{ SPEAR, 2, WEAPON_SYM, 1, 1 },
	{ RING_MAIL, 1, ARMOR_SYM, 1, 1 },
	{ HELMET, 0, ARMOR_SYM, 1, 1 },
	{ SHIELD, 0, ARMOR_SYM, 1, 1 },
	{ PAIR_OF_GLOVES, 0, ARMOR_SYM, 1, 1 },
	{ 0, 0, 0, 0, 0 }
};

#ifdef KAA
struct trobj Elf[] = {
	{ SHORT_SWORD, 0, WEAPON_SYM, 1, 1 },
	{ BOW, 0, WEAPON_SYM, 1, 1 },
	{ ARROW, 0, WEAPON_SYM, 25, 1 },
	{ UNDEF_TYP, 0, ARMOR_SYM, 1, 1 },
	{ 0, 0, 0, 0, 0 }
};

struct trobj Valkyrie[] = {
	{ LONG_SWORD, 1, WEAPON_SYM, 1, 1 },
	{ SHIELD, 3, ARMOR_SYM, 1, 1 },
	{ FOOD_RATION, 0, FOOD_SYM, 1, 1 },
	{ 0, 0, 0, 0, 0 }
};

struct trobj Healer[] = {
	{ STETHOSCOPE, 0, TOOL_SYM, 1, 0 },
	{ POT_HEALING, 0, POTION_SYM, 4, 0 },
	{ POT_EXTRA_HEALING, 0, POTION_SYM, 4, 0 },
	{ APPLE, 0, FOOD_SYM, 5, 0 },
	{ 0, 0, 0, 0, 0}
};
#endif /* KAA /**/

struct trobj Archeologist[] = {
	{ STUDDED_LEATHER_ARMOR, 0, ARMOR_SYM, 1, 1 },
	{ UNDEF_TYP, 0, POTION_SYM, 2, 0 },
	{ FOOD_RATION, 0, FOOD_SYM, 3, 1 },
	{ PICK_AXE, UNDEF_SPE, TOOL_SYM, 1, 0 },
	{ ICE_BOX, 0, TOOL_SYM, 1, 0 },
	{ 0, 0, 0, 0, 0}
};

struct trobj Tinopener[] = {
	{ CAN_OPENER, 0, TOOL_SYM, 1, 1 },
	{ 0, 0, 0, 0, 0 }
};

#ifdef MARKER
struct trobj Magicmarker[] = {
	{ MAGIC_MARKER, 50, TOOL_SYM, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
#endif

#ifdef WALKIES
struct trobj Leash[] = {
	{ LEASH, 0, TOOL_SYM, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
#endif

struct trobj Tourist[] = {
	{ UNDEF_TYP, 0, FOOD_SYM, 10, 1 },
	{ POT_EXTRA_HEALING, 0, POTION_SYM, 2, 0 },
	{ EXPENSIVE_CAMERA, 0, TOOL_SYM, 1, 1 },
	{ DART, 2, WEAPON_SYM, 25, 1 },	/* quan is variable */
	{ 0, 0, 0, 0, 0 }
};

struct trobj Wizard[] = {
	{ ELVEN_CLOAK, 0, ARMOR_SYM, 1, 1 },
	{ UNDEF_TYP, UNDEF_SPE, WAND_SYM, 2, 0 },
	{ UNDEF_TYP, UNDEF_SPE, RING_SYM, 2, 0 },
	{ UNDEF_TYP, UNDEF_SPE, POTION_SYM, 2, 0 },
	{ UNDEF_TYP, UNDEF_SPE, SCROLL_SYM, 3, 0 },
#ifdef SPELLS
	{ UNDEF_TYP, UNDEF_SPE, SPBOOK_SYM, 1, 0 },
#endif
	{ 0, 0, 0, 0, 0 }
};

#ifdef NEWCLASS
struct	trobj	Samurai[] = {
	{ KATANA, 0, WEAPON_SYM, 1, 1 },
	{ BOW,    1, WEAPON_SYM, 1, 1 },
	{ ARROW,  0, WEAPON_SYM, 25, 1 },	/* quan is variable */
	{ SPLINT_MAIL, 0, ARMOR_SYM, 1, 1},
	{ 0, 0, 0, 0, 0 }
};

struct	trobj	Ninja[] = {
	{ KATANA, 0, WEAPON_SYM, 1, 1 },
	{ SHURIKEN, 0, WEAPON_SYM, 25, 1 },	/* quan is variable */
	{ LEATHER_ARMOR, 1, ARMOR_SYM, 1, 1},
	{ 0, 0, 0, 0, 0 }
};

struct	trobj	Priest[] = {
	{ CHAIN_MAIL, 0, ARMOR_SYM, 1, 1 },
	{ SHIELD, 0, ARMOR_SYM, 1, 1 },
	{ MACE, 1, WEAPON_SYM, 1, 1 },
#ifdef SPELLS
	{ UNDEF_TYP, UNDEF_SPE, SPBOOK_SYM, 2, 0 },
#endif
	{ 0, 0, 0, 0, 0 }
};
#endif /* NEWCLASS /**/

u_init(){
register int i;
char exper = 'y', pc;
extern char readchar();
	if(flags.female)  {	/* should have been set in HACKOPTIONS */
		roles[4] = "Cave-woman";
#ifdef NEWCLASS
		roles[7] = "Priestess";
#endif
	}
	for(i = 0; i < NR_OF_ROLES; i++)
		rolesyms[i] = roles[i][0];
	rolesyms[i] = 0;

	if(pc = pl_character[0]) {
		if('a' <= pc && pc <= 'z') pc += 'A'-'a';
		if((i = role_index(pc)) >= 0)
			goto got_suffix;	/* implies experienced */
		printf("\nUnknown role: %c\n", pc);
		pl_character[0] = pc = 0;
	}

	printf("\nShall I pick a character for you (yes, no, or quit) ? [ynq] ");

	while(!index("yYnNqQ", (exper = readchar())))	bell();

	printf("%c\n", exper);		/* echo */

	if (index("qQ", exper)) {
		clearlocks();
		settty((char *) 0);
		exit(0);
	}

	if(index("Yy", exper)) {
		exper = 0;
		goto beginner;
	}

	printf("\n Tell me what kind of character you are:\n");
	printf(" Are you");
	for(i = 0; i < NR_OF_ROLES; i++) {
		printf(" %s %s", index("AEIOU",roles[i][0]) ? "an" : "a", roles[i]);
		if((((i + 1) % 4) == 0) && (i != NR_OF_ROLES -1)) printf(",\n\t");
		else if(i < NR_OF_ROLES - 2)	printf(",");
		if(i == NR_OF_ROLES - 2)	printf(" or");
	}
	printf("? [%s or q(quit)] ", rolesyms);

	while(pc = readchar()) {
		if (pc == 'q' || pc == 'Q') {

			clearlocks();
			settty((char *) 0);
			exit(0);
		}
		if('a' <= pc && pc <= 'z') pc += 'A'-'a';
		if((i = role_index(pc)) >= 0) {
			printf("%c\n", pc);	/* echo */
			(void) fflush(stdout);	/* should be seen */
			break;
		}
		if(pc == '\n') break;
		bell();
	}
	if(pc == '\n')	pc = 0;

beginner:
	if(!pc) {
		i = rn2(NR_OF_ROLES);
		pc = rolesyms[i];
		printf("\nThis game you will be %s %s%s.\n",
			(exper || index("AEIOU", roles[i][0])) ? "an" : "a",
			exper ? "experienced " : "", roles[i]);
		getret();
		/* give him some feedback in case mklev takes much time */
		(void) putchar('\n');
		(void) fflush(stdout);
	}
	if(exper) {
		roles[i][0] = pc;
	}

got_suffix:

	(void) strncpy(pl_character, roles[i], PL_CSIZ-1);
	pl_character[PL_CSIZ-1] = 0;
	flags.beginner = 1;
	u = zerou;
	u.usym = '@';
	u.ulevel = 1;
#ifdef SPELLS
	u.uen = u.uenmax = 1;
#endif
#ifdef PRAYERS
	u.ublesscnt = 300;			/* no prayers just yet */
	u.ublessed = 0;				/* not worthy yet */
	u.ugangr   = 0;				/* gods not angry */
#endif
#ifdef KAA
	u.mh = u.mhmax = u.umonnum = u.mtimedone = 0;
#endif
	init_uhunger();
#ifdef QUEST
	u.uhorizon = 6;
#endif
	uarm = uarm2 = uarmh = uarms = uarmg = uwep = uball = uchain =
	uleft = uright = 0;
#ifdef SPELLS
	for (i = 0; i <= MAXSPELL; i++) spl_book[i].sp_id = NO_SPELL;
#endif
	switch(pc) {
	case 'c':
	case 'C':
		Cave_man[2].trquan = 12 + rnd(9)*rnd(9);
		u.uhp = u.uhpmax = 16;
		u.ustr = u.ustrmax = 18;
		ini_inv(Cave_man);
		break;
	case 't':
	case 'T':
#ifdef KAA
		objects[POT_EXTRA_HEALING].oc_name_known=1;
#endif
		Tourist[3].trquan = 20 + rnd(20);
		u.ugold = u.ugold0 = rnd(1000);
		u.uhp = u.uhpmax = 10;
		u.ustr = u.ustrmax = 8;
		ini_inv(Tourist);
		if(!rn2(25)) ini_inv(Tinopener);
#ifdef MARKER
		else if(!rn2(25)) ini_inv(Magicmarker);
#endif
#ifdef WALKIES
		else if(!rn2(25)) ini_inv(Leash);
#endif
		break;
	case 'w':
	case 'W':
		for(i=1; i<=4; i++) if(!rn2(5))
			Wizard[i].trquan += rn2(3) - 1;
		u.uhp = u.uhpmax = 15;
		u.ustr = u.ustrmax = 16;
#ifdef SPELLS
		u.uen = u.uenmax += rn2(4);
#endif
		ini_inv(Wizard);
#ifdef MARKER
		if(!rn2(5)) ini_inv(Magicmarker);
#endif
		break;
	case 'a':
	case 'A':
		Fast = INTRINSIC;
		Stealth = INTRINSIC;
		u.uhp = u.uhpmax = 12;
		u.ustr = u.ustrmax = 10;
		ini_inv(Archeologist);
		if(!rn2(10)) ini_inv(Tinopener);
#ifdef MARKER
		else if(!rn2(10)) ini_inv(Magicmarker);
#endif
		break;
#ifdef KAA
	case 'e':
	case 'E':
		Elf[2].trquan = 15+rnd(20);
		Elf[3].trotyp = (rn2(2) ? ELVEN_CHAIN_MAIL : ELVEN_CLOAK);
		Fast = INTRINSIC;
		HSee_invisible = INTRINSIC;
		u.uhp = u.uhpmax = 16;
		u.ustr = u.ustrmax = 16;
		ini_inv(Elf);
		break;
	case 'v':
	case 'V':
		Stealth = INTRINSIC;
		HCold_resistance = INTRINSIC;
		flags.female = TRUE;
		u.uhp = u.uhpmax = 16;
		u.ustr = u.ustrmax = 17;
		ini_inv(Valkyrie);
		break;
	case 'h':
	case 'H':
		objects[POT_HEALING].oc_name_known=1;
		objects[POT_EXTRA_HEALING].oc_name_known=1;
		HPoison_resistance = INTRINSIC;
		u.uhp = u.uhpmax = 16;
		u.ustr = u.ustrmax = 15;
		ini_inv(Healer);
		break;
#endif
	case 'k':
	case 'K':
		u.uhp = u.uhpmax = 12;
		u.ustr = u.ustrmax = 10;
		ini_inv(Knight);
		break;
	case 'f':
	case 'F':
		u.uhp = u.uhpmax = 14;
		u.ustr = u.ustrmax = 17;
		ini_inv(Fighter);
		break;
#ifdef NEWCLASS
	case 's':
	case 'S':
		Fast = INTRINSIC;
		u.uhp = u.uhpmax = 16;
		u.ustr = u.ustrmax = 16;
		Samurai[2].trquan = 12 + rnd(9)*rnd(9);
		ini_inv(Samurai);
		break;
	case 'n':
	case 'N':
		Fast = INTRINSIC;
		Stealth = INTRINSIC;
		u.uhp = u.uhpmax = 15;
		u.ustr = u.ustrmax = 10;
		Ninja[1].trquan = 12 + rnd(9)*rnd(9);
		ini_inv(Ninja);
		break;
	case 'p':
	case 'P':
		u.uhp = u.uhpmax = 13;
		u.ustr = u.ustrmax = 15;
# ifdef SPELLS
		u.uen = u.uenmax += rn2(4);
# endif
		ini_inv(Priest);
# ifdef KAA
		uwep->dknown = 1;	/* bless his primary weapon */
# endif
# ifdef MARKER
		if(!rn2(10)) ini_inv(Magicmarker);
# endif
		break;
#endif /* NEWCLASS /**/
	default:	/* impossible */
		u.uhp = u.uhpmax = 12;
		u.ustr = u.ustrmax = 16;
	}
	find_ac();
	if(!rn2(20)) {
		register int d = rn2(7) - 2;	/* biased variation */
		u.ustr += d;
		u.ustrmax += d;
	}

#ifdef WIZARD
	if(wizard) wiz_inv();
#endif

	/* make sure he can carry all he has - especially for T's */
	while(inv_weight() > 0 && u.ustr < 118)
		u.ustr++, u.ustrmax++;
}

ini_inv(trop) register struct trobj *trop; {
register struct obj *obj;
extern struct obj *mkobj();
	while(trop->trolet) {
		obj = mkobj(trop->trolet);
		obj->known = trop->trknown;
		/* not obj->dknown = 1; - let him look at it at least once */
		obj->cursed = 0;
		if(obj->olet == WEAPON_SYM){
			obj->quan = trop->trquan;
			trop->trquan = 1;
		}
		if(trop->trspe != UNDEF_SPE)
			obj->spe = trop->trspe;
		if(trop->trotyp != UNDEF_TYP)
			obj->otyp = trop->trotyp;
		else
			if(obj->otyp == WAN_WISHING)	/* gitpyr!robert */
				obj->otyp = WAN_DEATH;
		obj->owt = weight(obj);	/* defined after setting otyp+quan */
		obj = addinv(obj);
		if(obj->olet == ARMOR_SYM){
			switch(obj->otyp){
			case SHIELD:
				if(!uarms) setworn(obj, W_ARMS);
				break;
			case HELMET:
				if(!uarmh) setworn(obj, W_ARMH);
				break;
			case PAIR_OF_GLOVES:
				if(!uarmg) setworn(obj, W_ARMG);
				break;
			case ELVEN_CLOAK:
				if(!uarm2)
					setworn(obj, W_ARM);
				break;
			default:
				if(!uarm) setworn(obj, W_ARM);
			}
		}
		/* below changed by GAN 01/09/87 to allow wielding of
		 * pick-axe or can-opener if there is no weapon
		 */
		if(obj->olet == WEAPON_SYM || obj->otyp == PICK_AXE ||
		   obj->otyp == CAN_OPENER)
			if(!uwep) setuwep(obj);
#ifndef PYRAMID_BUG
		if(--trop->trquan) continue;	/* make a similar object */
#else
		if(trop->trquan) {		/* check if zero first */
			--trop->trquan;
			if(trop->trquan)
				continue;	/* make a similar object */
		}
#endif
		trop++;
	}
}

#ifdef WIZARD
wiz_inv(){
register struct trobj *trop = &Extra_objs[0];
extern char *getenv();
register char *ep = getenv("INVENT");
register int type;
	while(ep && *ep) {
		type = atoi(ep);
		ep = index(ep, ',');
		if(ep) while(*ep == ',' || *ep == ' ') ep++;
		if(type <= 0 || type > NROFOBJECTS) continue;
		trop->trotyp = type;
		trop->trolet = objects[type].oc_olet;
		trop->trspe = 4;
		trop->trknown = 1;
		trop->trquan = 1;
		ini_inv(trop);
	}
	/* give him a wand of wishing by default */
	trop->trotyp = WAN_WISHING;
	trop->trolet = WAND_SYM;
	trop->trspe = 20;
	trop->trknown = 1;
	trop->trquan = 1;
	ini_inv(trop);
}
#endif /* WIZARD /**/

plnamesuffix() {
register char *p;
	if(p = rindex(plname, '-')) {
		*p = 0;
		pl_character[0] = p[1];
		pl_character[1] = 0;
		if(!plname[0]) {
			askname();
			plnamesuffix();
		}
	}
}

role_index(pc)
char pc;
{		/* must be called only from u_init() */
		/* so that rolesyms[] is defined */
	register char *cp;

	if(cp = index(rolesyms, pc))
		return(cp - rolesyms);
	return(-1);
}
