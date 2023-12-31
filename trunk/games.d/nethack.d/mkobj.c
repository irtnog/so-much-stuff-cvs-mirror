/*	SCCS Id: @(#)mkobj.c	1.4	87/08/08
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* mkobj.c - version 1.0.3 */

#include "hack.h"
#ifdef SPELLS
char mkobjstr[] = "))[[!!!!????+%%%%/=**))[[!!!!????+%%%%/=**(%";
#else
char mkobjstr[] = "))[[!!!!????%%%%/=**))[[!!!!????%%%%/=**(%";
#endif

struct obj *mkobj(), *mksobj();

struct obj *
mkobj_at(let,x,y)
register let,x,y;
{
	register struct obj *otmp = mkobj(let);
	otmp->ox = x;
	otmp->oy = y;
	otmp->nobj = fobj;
	fobj = otmp;
	return(otmp);
}

mksobj_at(otyp,x,y)
register otyp,x,y;
{
	register struct obj *otmp = mksobj(otyp);
	otmp->ox = x;
	otmp->oy = y;
	otmp->nobj = fobj;
	fobj = otmp;
}

struct obj *
mkobj(let) {
int realtype;
	switch (let) {
		case 0: {
			realtype=probtype(mkobjstr[rn2(sizeof(mkobjstr)-1)]);
			break;
		}
		case '9': { realtype = DEAD_GIANT; break; }
		case '&': { realtype = DEAD_DEMON; break; }
		default: realtype = letter(let) ?
				CORPSE + ((let>'Z') ? (let-'a'+'Z'-'@'+1) : (let-'@'))
			:	probtype(let);
	}
	return(mksobj(realtype));
}
	

struct obj zeroobj;

struct obj *
mksobj(otyp)
register otyp;
{
	register struct obj *otmp;
	char let = objects[otyp].oc_olet;

	otmp = newobj(0);
	*otmp = zeroobj;
	otmp->age = moves;
	otmp->o_id = flags.ident++;
	otmp->quan = 1;
	otmp->olet = let;
	otmp->otyp = otyp;
	otmp->dknown = index(
#ifdef KAA
#ifdef SPELLS
	"/=!?*+)",
#else
	"/=!?*)",
#endif
#else
#ifdef SPELLS
	"/=!?*+",
#else
	"/=!?*",
#endif
#endif
		    let) ? 0 : 1;
	switch(let) {
	case WEAPON_SYM:
		otmp->quan = (otmp->otyp <= ROCK) ? rn1(6,6) : 1;
		if(!rn2(11)) otmp->spe = rne(2);
		else if(!rn2(10)) {
			otmp->cursed = 1;
			otmp->spe = -rne(2);
		}
		break;
	case FOOD_SYM:
		if(otmp->otyp >= CORPSE) break;
#ifdef NOT_YET_IMPLEMENTED
		/* if tins are to be identified, need to adapt doname() etc */
		if(otmp->otyp == TIN)
			otmp->spe = rnd(...);
#endif
		/* fall into next case */
	case GEM_SYM:
		otmp->quan = rn2(6) ? 1 : 2;
	case TOOL_SYM:
	case CHAIN_SYM:
	case BALL_SYM:
	case ROCK_SYM:
	case POTION_SYM:
	case SCROLL_SYM:
	case AMULET_SYM:
		break;
#ifdef SPELLS
	case SPBOOK_SYM:
		if(!rn2(17)) otmp->cursed = 1;
		break;
#endif
	case ARMOR_SYM:
		if(!rn2(8)) otmp->cursed = 1;
		if(!rn2(10)) otmp->spe = rne(2);
		else if(!rn2(9)) {
			otmp->spe = -rne(2);
			otmp->cursed = 1;
		}
		break;
	case WAND_SYM:
		if(otmp->otyp == WAN_WISHING) otmp->spe = 3; else
		otmp->spe = rn1(5,
			(objects[otmp->otyp].bits & NODIR) ? 11 : 4);
		break;
	case RING_SYM:
		if(objects[otmp->otyp].bits & SPEC) {
			if(!rn2(3)) {
				otmp->cursed = 1;
				otmp->spe = -rne(3);
			} else otmp->spe = rne(3);
		} else if(otmp->otyp == RIN_TELEPORTATION ||
			  otmp->otyp == RIN_AGGRAVATE_MONSTER ||
			  otmp->otyp == RIN_HUNGER || !rn2(9))
			otmp->cursed = 1;
		break;
	default:
		panic("impossible mkobj %d", otmp->otyp);
	}
	otmp->owt = weight(otmp);
	return(otmp);
}

letter(c) {
	return(('@' <= c && c <= 'Z') || ('a' <= c && c <= 'z'));
}

weight(obj)
register struct obj *obj;
{
register int wt = objects[obj->otyp].oc_weight;
	return(wt ? wt*obj->quan : (obj->quan + 1)/(unsigned)2);
}

mkgold(num,x,y)
register long num;
{
	register struct gold *gold;
	register long amount = (num ? num : 1 + (rnd(dlevel+2) * rnd(30)));

	if(gold = g_at(x,y))
		gold->amount += amount;
	else {
		gold = newgold();
		gold->ngold = fgold;
		gold->gx = x;
		gold->gy = y;
		gold->amount = amount;
		fgold = gold;
		/* do sth with display? */
	}
}
