/*
 *	attrib.c	- attribute modification routines.
 *
 *	Copyright 1988, M. Stephenson
 */
/* NetHack may be freely redistributed.  See license for details. */

#include	"hack.h"

const char	*plusattr[] = {	/* part of the output on gain of attribute */

	"strong", "smart", "wise", "agile", "tough", "charismatic"
};

const char	*minusattr[] = { /* part of the output on loss of attribute */

	"weak", "stupid", "foolish", "clumsy", "vulnerable", "ugly"
};

struct attribs	attrmax = {	/* max values for the attributes */

	118, 18, 18, 18, 18, 18
},
		attrmin = {	/* min values for the attributes */

	3, 3, 3, 3, 3, 3
};

const struct innate {

	schar	ulevel;
	long	*ability;
	char	*gainstr, *losestr;
}	a_abil[] = { {	 0, &(Stealth), "", "" },
		     {   0, &(Fast), "", "" },
		     {  10, &(Searching), "perceptive", "" },
		     {	 0, 0, 0, 0 } },

	b_abil[] = { {	 0, &(HPoison_resistance), "", "" },
		     {   7, &(Fast), "quick", "slow" },
		     {  15, &(Stealth), "stealthy", "" },
		     {	 0, 0, 0, 0 } },

	c_abil[] = { {	 7, &(Fast), "quick", "slow" },
		     {	15, &(Warning), "sensitive", "" },
		     {	 0, 0, 0, 0 } },

	e_abil[] = { {   0, &(Fast), "", "" },
		     {	 0, &(HSee_invisible), "", "" },
		     {	 0, &(Searching), "", "" },
		     {	 0, &(HSleep_resistance), "", "" },
		     {	 0, 0, 0, 0 } },

	h_abil[] = { {	 0, &(HPoison_resistance), "", "" },
		     {	15, &(Warning), "sensitive", "" },
		     {	 0, 0, 0, 0 } },

	k_abil[] = { {	 7, &(Fast), "quick", "slow" },
		     {	 0, 0, 0, 0 } },

	p_abil[] = { {	15, &(Warning), "sensitive", "" },
		     {  20, &(HFire_resistance), "cool", "warmer" },
		     {	 0, 0, 0, 0 } },

	r_abil[] = { {	 0, &(Stealth), "", ""  },
		     {  10, &(Searching), "perceptive", "" },
		     {	 0, 0, 0, 0 } },

	s_abil[] = { {	 0, &(Fast), "", "" },
		     {  15, &(Stealth), "stealthy", "" },
		     {	 0, 0, 0, 0 } },

	t_abil[] = { {	10, &(Searching), "perceptive", "" },
		     {	20, &(HPoison_resistance), "hardy", "" },
		     {	 0, 0, 0, 0 } },

	v_abil[] = { {	 0, &(HCold_resistance), "", "" },
		     {	 0, &(Stealth), "", "" },
		     {   7, &(Fast), "quick", "slow" },
		     {	 0, 0, 0, 0 } },

	w_abil[] = { {	15, &(Warning), "sensitive", "" },
		     {  17, &(HTeleport_control), "controlled","uncontrolled" },
		     {	 0, 0, 0, 0 } };

const struct clattr {

	struct	attribs	base, dist;
 	schar	align, aligntyp;
	schar	shp, hd, xlev, ndx;
/* According to AD&D, HD for some classes (i.e. Wizard) should be smaller
 * (4-sided for wizards).  But this is not AD&D, and using the AD&D
 * rule here produces an unplayable character.  This I have used a minimum
 * of an 10-sided hit die for everything.  Another AD&D change: wizards get
 * a minimum strength of 6 since without one you can't teleport or cast
 * spells. --KAA
 */
	const struct	innate *abil;
}	a_attr = { {	 6,  9,  9,  6,  6,  6 },  /* Archeologist */
		   {	20, 20, 20, 10, 20, 10 },
		    10,  1, 13, 10, 14,  2, a_abil },

	b_attr = { {	15,  6,  6, 14, 15,  5 },  /* Barbarian */
		   {	30,  6,  7, 20, 30,  7 },
		    10, -1, 16, 12, 10,  3, b_abil },

	c_attr = { {	 9,  6,  6,  6,  7,  5 },  /* Caveman (fighter) */
		   {	30,  6,  7, 20, 30,  7 },
		     0,  1, 16, 10, 10,  3, c_abil },

/*
	e_attr = { {	13, 13, 14,  6, 14,  6 },
 */
	e_attr = { {	12, 12, 12,  8, 12,  6 },  /* Elf (ranger) */
		   {	30, 10, 10, 20, 20, 10 },
		    10,  1, 15, 10, 11,  2, e_abil },

	h_attr = { {	 6,  6, 12,  6, 10, 15 },  /* Healer (druid) */
		   {	15, 20, 20, 15, 25, 10 },
		    10,  1, 13, 10, 20,  2, h_abil },

	k_attr = { {	12,  6, 13,  6,  9, 17 },  /* Knight (paladin) */
		   {	20, 15, 15, 10, 20, 10 },
		    10,  1, 16, 10, 10,  3, k_abil },

	p_attr = { {	 6,  6,  9,  6,  6,  6 },  /* Priest (cleric) */
		   {	15, 10, 30, 15, 20, 10 },
		     0,  0, 14, 10, 10,  2, p_abil },

	r_attr = { {	 6,  6,  6,  9,  6,  5 },  /* Rogue (thief) */
		   {	20, 10, 10, 30, 20, 10 },
		    10, -1, 12, 10, 11,  2, r_abil },

	s_attr = { {	 9,  6,  6,  9, 17,  5 },  /* Samurai (fighter/thief) */
		   {	30, 10, 10, 30, 14, 10 },
		    10,  1, 15, 10, 11,  2, s_abil },

	t_attr = { {	 6,  9,  5,  6,  6,  9 },  /* Tourist */
		   {	15, 10, 10, 15, 30, 20 },
		     0,  0, 10, 10, 14,  1, t_abil },

	v_attr = { {	 9,  6,  6,  6,  9,  6 },  /* Valkyrie (fighter) */
		   {	30,  6,  7, 20, 30,  7 },
		     0, -1, 16, 10, 10,  3, v_abil },

	w_attr = { {	 6,  9,  6,  6,  6,  6 },  /* Wizard (magic-user) */
		   {	10, 30, 10, 20, 20, 10 },
		     0,  0, 12, 10, 12,  1, w_abil },

	X_attr = { {	 3,  3,  3,  3,  3,  3 },
		   {	20, 15, 15, 15, 20, 15 },
		     0,  0, 12, 10, 14,  1,  0 };

#ifdef __STDC__
void
adjattrib(int ndx,int incr,boolean silent)
#else
void
adjattrib(ndx, incr, silent)
	int	ndx, incr;
	boolean	silent;
#endif
{
	if(!incr) return;

	if(incr > 0) {
	    if((AMAX(ndx) >= attrmax.a[ndx]) && (ACURR(ndx) == AMAX(ndx))) {

		if(!silent && flags.verbose)
		    pline("You're already as %s as you can get.",
			  plusattr[ndx]);
		ABASE(ndx) = AMAX(ndx) = attrmax.a[ndx]; /* just in case */
		return;
	    }

	    ABASE(ndx) += incr;
	    if(ABASE(ndx) > AMAX(ndx)) {
		incr = ABASE(ndx) - AMAX(ndx);
		AMAX(ndx) += incr;
		if(AMAX(ndx) > attrmax.a[ndx])
		    AMAX(ndx) = attrmax.a[ndx];
		ABASE(ndx) = AMAX(ndx);
	    }
	} else {
	    if((AMAX(ndx) <= attrmin.a[ndx]) && (ABASE(ndx) == AMAX(ndx))) {
		if(!silent && flags.verbose)
		    pline("You're already as %s as you can get.",
			  minusattr[ndx]);
		ABASE(ndx) = AMAX(ndx) = attrmin.a[ndx]; /* just in case */
		return;
	    }

	    ABASE(ndx) += incr;
	    if(ABASE(ndx) < attrmin.a[ndx]) {
		incr = ABASE(ndx) - attrmin.a[ndx];
		ABASE(ndx) = attrmin.a[ndx];
		AMAX(ndx) += incr;
		if(AMAX(ndx) < attrmin.a[ndx])
		    AMAX(ndx) = attrmin.a[ndx];
	    }
	}
	if(!silent)
	    You("feel %s%s!",
		  (incr > 1) ? "very ": "",
		  (incr > 0) ? plusattr[ndx] : minusattr[ndx]);
	flags.botl = 1;
	return;
}

void
gainstr(otmp, incr)
	register struct obj *otmp;
	register int incr;
{
	int num = 1;

	if(incr) num = incr;
	else {
	    if(ABASE(A_STR) < 18) num = (rn2(4) ? 1 : rnd(6) );
	    else if (ABASE(A_STR) < 103) num = rnd(10);
	}
	adjattrib(A_STR, (otmp && otmp->cursed) ? -num : num, TRUE);
}

void
losestr(num)	/* may kill you; cause may be poison or monster like 'a' */
	register int num;
{
	int ustr = ABASE(A_STR) - num;

	while(ustr < 3) {
		ustr++;
		num--;
		u.uhp -= 6;
		u.uhpmax -= 6;
	}
	adjattrib(A_STR, -num, TRUE);
}

#ifdef __STDC__
void
change_luck(schar n)
#else
void
change_luck(n)
register schar n;
#endif
{
	u.uluck += n;
	if (u.uluck < 0 && u.uluck < LUCKMIN)	u.uluck = LUCKMIN;
	if (u.uluck > 0 && u.uluck > LUCKMAX)	u.uluck = LUCKMAX;
}

#ifdef __STDC__
int
stone_luck(boolean parameter)
#else
int
stone_luck(parameter)
boolean parameter; /* So I can't think up of a good name.  So sue me. --KAA */
#endif
{
	register struct obj *otmp;
	register int bonchance = 0;

	for(otmp = invent; otmp; otmp=otmp->nobj)
	    if(otmp->otyp == LUCKSTONE) {
		if (otmp->cursed) bonchance -= otmp->quan;
		else if (otmp->blessed) bonchance += otmp->quan;
		else if (parameter) bonchance += otmp->quan;
	    }

	return sgn(bonchance);
}

void
restore_attrib() {

	int	i;

	for(i = 0; i < A_MAX; i++) {	/* all temporary losses/gains */

	   if(ATEMP(i) && ATIME(i)) {
		if(!(--(ATIME(i)))) { /* countdown for change */
		    ATEMP(i) += ATEMP(i) > 0 ? -1 : 1;

		    if(ATEMP(i)) /* reset timer */
			ATIME(i) = 100 / ACURR(A_CON);
		}
	    }
	}
}

static const struct	clattr *
clx()  {

	register const struct	clattr	*attr;

	switch	(pl_character[0]) {

	    case 'A':	attr = &a_attr;
			break;
	    case 'B':	attr = &b_attr;
			break;
	    case 'C':	attr = &c_attr;
			break;
	    case 'E':	attr = &e_attr;
			break;
	    case 'H':	attr = &h_attr;
			break;
	    case 'K':	attr = &k_attr;
			break;
	    case 'P':	attr = &p_attr;
			break;
	    case 'R':	attr = &r_attr;
			break;
	    case 'S':	attr = &s_attr;
			break;
	    case 'T':	attr = &t_attr;
			break;
	    case 'V':	attr = &v_attr;
			break;
	    case 'W':	attr = &w_attr;
			break;
	    default:	/* unknown type */
			attr = &X_attr;
			break;
	}
	return(attr);
}

static void
init_align() {	/* called from newhp if u.ulevel is 0 */

	register const struct	clattr	*attr = clx();

	u.ualign = (int)attr->align;
	u.ualigntyp = attr->aligntyp;
}

void
init_attr(np)
	register int	np;
{
	register int	i, x, tryct;
	register const struct	clattr	*attr = clx();

	for(i = 0; i < A_MAX; i++) {

	    ABASE(i) = AMAX(i) = attr->base.a[i];
	    ATEMP(i) = ATIME(i) = 0;
	    np -= attr->base.a[i];
	}

	tryct = 0;
	while(np > 0 && tryct < 100) {

	    x = rn2(100);
	    for(i = 0; (i < A_MAX) && ((x -= attr->dist.a[i]) > 0); i++);
	    if(i >= A_MAX) continue; /* impossible */

	    if(ABASE(i) >= attrmax.a[i]) {

		tryct++;
		continue;
	    }
	    tryct = 0;
	    ABASE(i)++;
	    AMAX(i)++;
	    np--;
	}

	tryct = 0;
	while(np < 0 && tryct < 100) {		/* for redistribution */

	    x = rn2(100);
	    for(i = 0; (i < A_MAX) && ((x -= attr->dist.a[i]) > 0); i++);
	    if(i >= A_MAX) continue; /* impossible */

	    if(ABASE(i) <= attrmin.a[i]) {

		tryct++;
		continue;
	    }
	    tryct = 0;
	    ABASE(i)--;
	    AMAX(i)--;
	    np++;
	}
}

#ifdef POLYSELF
void
redist_attr() {

	register int i, tmp;

	for(i = 0; i < A_MAX; i++) {
	    if (i==A_INT || i==A_WIS) continue;
		/* Polymorphing doesn't change your mind */
	    tmp = AMAX(i);
	    AMAX(i) += (rn2(5)-2);
	    if (AMAX(i) > attrmax.a[i]) AMAX(i) = attrmax.a[i];
	    if (AMAX(i) < attrmin.a[i]) AMAX(i) = attrmin.a[i];
	    ABASE(i) = ABASE(i) * AMAX(i) / tmp;
	    /* ABASE(i) > attrmax.a[i] is impossible */
	    if (ABASE(i) < attrmin.a[i]) ABASE(i) = attrmin.a[i];
	}
}
#endif

void
adjabil(flag)

	int	flag;		/* +ve/-ve  = gain/lose */
{
	register const struct	clattr	*attr = clx();
	register const struct innate	*abil = attr->abil;

	if(abil) {

	    for(; abil->ability; abil++) {
		if ((flag>0 && u.ulevel >= abil->ulevel) ||
					(flag<0 && u.ulevel < abil->ulevel)) {
		    if(flag > 0) {
			if(!(*(abil->ability) & INTRINSIC)) {
			    *(abil->ability) |= INTRINSIC;
			    if(strlen(abil->gainstr))
				You("feel %s!", abil->gainstr);
			}
		    } else {
			if((*(abil->ability) & INTRINSIC)) {
			    *(abil->ability) &= ~INTRINSIC;
			    if(strlen(abil->losestr))
				You("feel %s!", abil->losestr);
			    else if(strlen(abil->gainstr))
				You("feel less %s!", abil->gainstr);
			}
		    }
		} 
	    }
	}
}

int
newhp() {
	register const struct	clattr	*attr = clx();
	int	hp, conplus;

	if(u.ulevel == 0) {

		hp = attr->shp;
		init_align();	/* initialize alignment stuff */
		return hp;
	} else {

	    if(u.ulevel < attr->xlev)
		hp = rnd(attr->hd);
	    else
		hp = attr->ndx;
	}

	switch(ACURR(A_CON)) {
		case	3:	conplus = -2; break;
		case	4:
		case	5:
		case	6:	conplus = -1; break;
		case	15:
		case	16:	conplus = 1; break;
		case	17:	conplus = 2; break;
		case	18:	conplus = 3; break;
		default:	conplus = 0;
	}
	hp += conplus;
	return((hp <= 0) ? 1 : hp);
}

schar
acurr(x) { 
	register int tmp = (u.abon.a[x] + u.atemp.a[x] + u.acurr.a[x]);

	if (x == A_STR) {
		if (uarmg && uarmg->otyp == GAUNTLETS_OF_POWER) return(125);
		else return((tmp >= 125) ? 125 : (tmp <= 3) ? 3 : tmp);
	} 
	else return((tmp >= 25) ? 25 : (tmp <= 3) ? 3 : tmp);
}

/* avoid possible problems with alignment overflow, and provide a centralized
 * location for any future alignment limits
 */
void
adjalign(n)
register int n;
{
	register int newalign = u.ualign + n;

	if(n < 0) {
		if(newalign < u.ualign)
			u.ualign = newalign;
	} else
		if(newalign > u.ualign)
			u.ualign = newalign;
}
