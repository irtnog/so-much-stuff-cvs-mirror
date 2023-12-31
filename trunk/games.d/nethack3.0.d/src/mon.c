/*	SCCS Id: @(#)mon.c	3.0	88/10/31
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "mfndpos.h"
#ifdef NAMED_ITEMS
#  include "artifact.h"
#endif

#ifdef HARD
static boolean restrap();
#endif

long lastwarntime;
uchar lastwarnlev;
static const char *warnings[] = {
	"white", "pink", "red", "ruby", "purple", "black" };
struct monst *fdmon;	/* chain of dead monsters, need not to be saved */

/* Creates a monster corpse, a "special" corpse, or nothing if it doesn't
 * leave corpses.  Monsters which leave "special" corpses should have
 * G_NOCORPSE set in order to prevent wishing for one, finding tins of one,
 * etc....
 */
static struct obj *
make_corpse(mtmp)
register struct monst *mtmp;
{
	register struct permonst *mdat = mtmp->data;
#ifdef GOLEMS
	int pieces;
#endif
	struct obj *obj = 0;
	int x = mtmp->mx, y = mtmp->my;

	switch(monsndx(mdat)) {
	    case PM_KOBOLD_MUMMY:
		obj = mksobj_at(MUMMY_WRAPPING, x, y); /* and fall through */
	    case PM_KOBOLD_ZOMBIE:
		obj = mksobj_at(CORPSE, x, y);
		obj->corpsenm = PM_KOBOLD;
		break;
	    case PM_GNOME_MUMMY:
		obj = mksobj_at(MUMMY_WRAPPING, x, y); /* and fall through */
	    case PM_GNOME_ZOMBIE:
		obj = mksobj_at(CORPSE, x, y);
		obj->corpsenm = PM_GNOME;
		break;
	    case PM_ORC_MUMMY:
		obj = mksobj_at(MUMMY_WRAPPING, x, y); /* and fall through */
	    case PM_ORC_ZOMBIE:
		obj = mksobj_at(CORPSE, x, y);
		obj->corpsenm = PM_ORC;
		break;
	    case PM_ELF_MUMMY:
		obj = mksobj_at(MUMMY_WRAPPING, x, y); /* and fall through */
	    case PM_ELF_ZOMBIE:
		obj = mksobj_at(CORPSE, x, y);
		obj->corpsenm = PM_ELF;
		break;
	    case PM_HUMAN_MUMMY:
		obj = mksobj_at(MUMMY_WRAPPING, x, y); /* and fall through */
	    case PM_HUMAN_ZOMBIE:
		obj = mksobj_at(CORPSE, x, y);
		obj->corpsenm = PM_HUMAN;
		break;
	    case PM_GIANT_MUMMY:
		obj = mksobj_at(MUMMY_WRAPPING, x, y); /* and fall through */
	    case PM_GIANT_ZOMBIE:
		obj = mksobj_at(CORPSE, x, y);
		obj->corpsenm = PM_GIANT;
		break;
	    case PM_ETTIN_MUMMY:
		obj = mksobj_at(MUMMY_WRAPPING, x, y); /* and fall through */
	    case PM_ETTIN_ZOMBIE:
		obj = mksobj_at(CORPSE, x, y);
		obj->corpsenm = PM_ETTIN;
		break;
#ifdef GOLEMS
	    case PM_IRON_GOLEM:
		pieces = d(2,6);
		while (pieces--)
			obj = mksobj_at(IRON_CHAIN, x, y);
		break;
	    case PM_CLAY_GOLEM:
		obj = mksobj_at(ROCK, x, y);
		obj->quan = rn2(20) + 100;
		obj->owt = weight(obj);
		break;
	    case PM_STONE_GOLEM:
		obj = mkstatue(mdat, x, y);
		break;
	    case PM_WOOD_GOLEM:
		pieces = d(2,4);
		while(pieces--)
			obj = mksobj_at(QUARTERSTAFF, x, y);
		break;
	    case PM_LEATHER_GOLEM:
		pieces = d(2,4);
		while(pieces--)
			obj = mksobj_at(LEATHER_ARMOR, x, y);
		break;
#endif
	    default:
		if (mdat->geno & G_NOCORPSE)
			return (struct obj *)0;
		else obj = mkcorpse_at(mdat, x, y);
		break;
	}
	/* All special cases should precede the G_NOCORPSE check */

	/* Note: oname() cannot be used generically for non-inventory objects
	 * unless you fix the link from the previous object in the chain.
	 * (Here we know it's the first one, so there was no link.)
	 */
	if (mtmp->mnamelth) {
		obj = oname(obj, NAME(mtmp), 0);
		fobj = obj;
	}
	stackobj(fobj);
	newsym(x, y);
	return obj;
}


static void
dmonsfree(){
register struct monst *mtmp;
	while(mtmp = fdmon){
		fdmon = mtmp->nmon;
		free((genericptr_t) mtmp);
	}
}

void
movemon()
{
	register struct monst *mtmp;
	register int fr;

	warnlevel = 0;

	while(1) {
		/*  Find a monster that we have not treated yet.
		 *  Note that mtmp or mtmp->nmon might get killed
		 *  while mtmp moves, so we cannot just walk down the
		 *  chain (even new monsters might get created!)
		 */
		/* Do tame monsters first.  Necessary so that when the tame
		 * monster attacks something, the something gets a chance to
		 * attack the tame monster back (which it's permitted to do
		 * only if it hasn't made its move yet).
		 */
		for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
			if(mtmp->mlstmv < moves && mtmp->mtame) goto next_mon;
		for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
			if(mtmp->mlstmv < moves && !mtmp->mtame) goto next_mon;
		/* treated all monsters */
		break;

	next_mon:
		mtmp->mlstmv = moves;

		/* most monsters drown in pools */
		{ boolean inpool,infountain,iseel,isgremlin;

		  inpool = is_pool(mtmp->mx,mtmp->my);
		  iseel = mtmp->data->mlet == S_EEL;
		  isgremlin = mtmp->data->mlet == S_GREMLIN;
		  infountain = IS_FOUNTAIN(levl[mtmp->mx][mtmp->my].typ);
		/* Gremlin multiplying won't go on forever since the hit points
		 * keep going down, and when it gets to 1 hit point the clone
		 * function will fail.
		 */
		  if((inpool || infountain) && isgremlin && rn2(3)) {
			struct monst *mtmp2 = clone_mon(mtmp);

			if (mtmp2) {
			    mtmp2->mhpmax = (mtmp->mhpmax /= 2);
			    if(cansee(mtmp->mx,mtmp->my))
				pline("%s multiplies.", Monnam(mtmp));
			}
#ifdef FOUNTAINS
			if (infountain) dryup();
#endif
		  } else
		  if(inpool && !is_flyer(mtmp->data) && !is_swimmer(mtmp->data)) {
			if(cansee(mtmp->mx,mtmp->my))
			    pline("%s drowns.", Monnam(mtmp));
			mondead(mtmp);
			continue;
		  } else
		/* but eels have a difficult time outside */
		  if(iseel && !inpool) {
			if(mtmp->mhp > 1) mtmp->mhp--;
			mtmp->mflee = 1;
			mtmp->mfleetim += 2;
		  }
		}
		if(mtmp->mblinded && !--mtmp->mblinded)
			mtmp->mcansee = 1;
		if(mtmp->mfleetim && !--mtmp->mfleetim)
			mtmp->mflee = 0;
#ifdef HARD
		/* unwatched mimics and piercers may hide again  [MRS] */
		if(is_hider(mtmp->data) && restrap(mtmp))   continue;
#endif
		if(mtmp->mimic) continue;
		if(mtmp->mspeed != MSLOW || !(moves%2)){
			/* continue if the monster died fighting */
			fr = -1;
/* TODO:	Handle the case of the agressor dying? */
			if(Conflict && cansee(mtmp->mx,mtmp->my)
				&& !mtmp->iswiz
				&& (fr = fightm(mtmp)) == 2)
				continue;
  			if(fr<0 && dochugw(mtmp))
				continue;
		}
		if(mtmp->mspeed == MFAST && dochugw(mtmp))
			continue;
	}
#ifdef NAMED_ITEMS
	if (warnlevel == 100) {
		Your("%s %s!", aobjnam(uwep, "glow"),
			Hallucination ? hcolor() : light_blue);
		warnlevel = 0;
	}
#endif
	warnlevel -= u.ulevel;
	if(warnlevel >= (unsigned)SIZE(warnings))
		warnlevel = SIZE(warnings)-1;
	if(!Blind && warnlevel >= 0)
	if(warnlevel > lastwarnlev || moves > lastwarntime + 5){
	    register char *rr;
	
	    switch((int) (Warning & (LEFT_RING | RIGHT_RING))){
	    case LEFT_RING:
		rr = "Your left ring glows";
		break;
	    case RIGHT_RING:
		rr = "Your right ring glows";
		break;
	    case LEFT_RING | RIGHT_RING:
		rr = "Both your rings glow";
		break;
	    default:
		{ char buf[33];
		Sprintf(buf, "Your %s glow", makeplural(body_part(FINGERTIP)));
		rr = buf;
		}
		break;
	    }
	    pline("%s %s!", rr, Hallucination ? hcolor() : warnings[warnlevel]);
	    lastwarntime = moves;
	    lastwarnlev = warnlevel;
	}

	dmonsfree();	/* remove all dead monsters */
}

void
meatgold(mtmp)
	register struct monst *mtmp;
{
	register struct gold *gold;
	register struct obj *otmp;

	/* Eats gold if it is there */
	while(gold = g_at(mtmp->mx, mtmp->my)){
		if (cansee(mtmp->mx, mtmp->my) && flags.verbose)
			pline("%s eats some gold!", Monnam(mtmp));
		mtmp->meating = (int)((gold->amount + 500L)/1000L);
		freegold(gold);
		/* Left behind a pile? */
		if(rnd(25) < 3) (void) mksobj_at(ROCK, mtmp->mx, mtmp->my);
		newsym(mtmp->mx, mtmp->my);
	}
	/* Eats topmost metal object if it is there */
	for (otmp = fobj; otmp; otmp = otmp->nobj)
	    if (otmp->ox == mtmp->mx && otmp->oy == mtmp->my &&
		objects[otmp->otyp].oc_material == METAL) {
		    if (cansee(mtmp->mx,mtmp->my) && flags.verbose)
			pline("%s eats %s!", Monnam(mtmp),
				distant_name(otmp,doname));
		    else if (flags.soundok && flags.verbose)
			You("hear a crunching sound.");
		    mtmp->meating = otmp->owt/2 - 1;
		    /* Heal up to the object's weight in hp */
		    if (mtmp->mhp < mtmp->mhpmax) {
			mtmp->mhp += objects[otmp->otyp].oc_weight;
			if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
		    }
		    freeobj(otmp);
		    /* Left behind a pile? */
		    if(rnd(25) < 3) (void) mksobj_at(ROCK, mtmp->mx, mtmp->my);
		    newsym(mtmp->mx, mtmp->my);
		    break;
	    }
	set_omask(mtmp->mx, mtmp->my);
}

void
meatobj(mtmp)		/* for gelatinous cubes */
	register struct monst *mtmp;
{
	register struct obj *otmp, *otmp2;

	/* Eats organic, glass, or wood objects if there */
	/* Engulfs anything else, metal and rock */
	for (otmp = fobj; otmp; otmp = otmp2) {
	    otmp2 = otmp->nobj;
	    if (otmp->ox == mtmp->mx && otmp->oy == mtmp->my) {
		if(!objects[otmp->otyp].oc_material <= WOOD) {
		    if (cansee(mtmp->mx,mtmp->my) && flags.verbose)
			pline("%s eats %s!", Monnam(mtmp),
				distant_name(otmp, doname));
		    else if (flags.soundok && flags.verbose)
			You("hear a slurping sound.");
                    /* Heal up to the object's weight in hp */
		    if (mtmp->mhp < mtmp->mhpmax) {
			mtmp->mhp += objects[otmp->otyp].oc_weight;
			if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
		    }
		    delobj(otmp);		/* munch */
		} else if (otmp->olet != ROCK_SYM && otmp->olet != BALL_SYM) {
		    if (cansee(mtmp->mx, mtmp->my) && flags.verbose)
			pline("%s engulfs %s.", Monnam(mtmp),
				distant_name(otmp,doname));
		    freeobj(otmp);
		    mpickobj(mtmp, otmp);	/* slurp */
		}
	    }
	    /* Engulf & devour is instant, so don't set meating */
	    newsym(mtmp->mx, mtmp->my);
	}
	set_omask(mtmp->mx, mtmp->my);
}

void
mpickgold(mtmp)
	register struct monst *mtmp;
{
	register struct gold *gold;

	while(gold = g_at(mtmp->mx, mtmp->my)){
		mtmp->mgold += gold->amount;
		if (cansee(mtmp->mx, mtmp->my) && flags.verbose)
			pline("%s picks up some gold.", Monnam(mtmp));
		freegold(gold);
		if(levl[mtmp->mx][mtmp->my].scrsym == GOLD_SYM)
			newsym(mtmp->mx, mtmp->my);
	}
	set_omask(mtmp->mx, mtmp->my);
}

/* Now includes giants which pick up enormous rocks.  KAA */
void
mpickgems(mtmp)
	register struct monst *mtmp;
{
	register struct obj *otmp;

	for(otmp = fobj; otmp; otmp = otmp->nobj)
	  if(throws_rocks(mtmp->data) ? otmp->otyp == BOULDER :
			(otmp->olet == GEM_SYM && otmp->otyp < LAST_GEM+5))
	    if(otmp->ox == mtmp->mx && otmp->oy == mtmp->my)
	      if(mtmp->data->mlet != S_UNICORN
		 || objects[otmp->otyp].g_val != 0){
		if (cansee(mtmp->mx,mtmp->my) && flags.verbose)
			pline("%s picks up %s.", Monnam(mtmp),
				distant_name(otmp, doname));
		freeobj(otmp);
		mpickobj(mtmp, otmp);
		newsym(mtmp->mx, mtmp->my);
		return;	/* pick only one object */
	      }
	set_omask(mtmp->mx, mtmp->my);
}

int
curr_mon_load(mtmp)
register struct monst *mtmp;
{
	register int curload = 0;
	register struct obj *obj;

	for(obj = mtmp->minvent; obj; obj = obj->nobj) {
		if(obj->otyp != BOULDER || !throws_rocks(mtmp->data))
			curload += weight(obj);
	}

	return curload;
}

int
max_mon_load(mtmp)
register struct monst *mtmp;
{
	register int maxload;

	/* Base monster carrying capacity is equal to human maximum
	 * carrying capacity, or half human maximum if not strong.
	 * (for a polymorphed player, the value used would be the
	 * non-polymorphed carrying capacity instead of max/half max).
	 * This is then modified by the ratio between the monster weights
	 * and human weights (weight of a human=45).  Limits for corpseless
	 * monsters are arbitrary.
	 */
	maxload = (mtmp->data->cwt ? mtmp->data->cwt : mtmp->data->mlevel*6)
		* MAX_CARR_CAP / 45;
	if (!strongmonst(mtmp->data)) maxload /= 2;

	return maxload;
}

/* for restricting monsters' object-pickup */
boolean
can_carry(mtmp,otmp)
struct monst *mtmp;
struct obj *otmp;
{
	register int newload = weight(otmp);

	if (mtmp->isshk) return(TRUE); /* no limit */
	if (mtmp->mpeaceful && !mtmp->mtame) return(FALSE);
	/* otherwise players might find themselves obligated to violate
	 * their alignment if the monster takes something they need
	 */
	
	/* special--boulder throwers carry unlimited amounts of boulders */
	if (throws_rocks(mtmp->data) && otmp->otyp == BOULDER)
		return(TRUE);
	
	/* nymphs deal in stolen merchandise, but not boulders or statues */
	if (mtmp->data->mlet == S_NYMPH)
		return !(otmp->olet == ROCK_SYM);

	if(curr_mon_load(mtmp) + newload > max_mon_load(mtmp)) return(FALSE);

	return(TRUE);
}

void
mpickstuff(mtmp, str)
	register struct monst *mtmp;
	register const char *str;
{
	register struct obj *otmp;

/*	prevent shopkeepers from leaving the door of their shop */
	if(mtmp->isshk && inhishop(mtmp)) return;

	for(otmp = fobj; otmp; otmp = otmp->nobj)
	  if(index(str, otmp->olet))
	    if(otmp->ox == mtmp->mx && otmp->oy == mtmp->my) {
		if(!can_carry(mtmp,otmp)) return;
		if (cansee(mtmp->mx,mtmp->my) && flags.verbose)
			pline("%s picks up %s.", Monnam(mtmp), doname(otmp));
		freeobj(otmp);
		mpickobj(mtmp, otmp);
		if(index(str, (char) levl[mtmp->mx][mtmp->my].scrsym))
			newsym(mtmp->mx, mtmp->my);
		set_omask(mtmp->mx, mtmp->my);
		return;			/* pick only one object */
	    }
}

/* return number of acceptable neighbour positions */
int
mfndpos(mon, poss, info, flag)
	register struct monst *mon;
	coord *poss;	/* coord poss[9] */
	long *info;	/* long info[9] */
	long flag;
{
	register int x,y,nx,ny,cnt = 0,ntyp;
	int nowtyp;
	boolean wantpool,poolok;

	x = mon->mx;
	y = mon->my;
	nowtyp = levl[x][y].typ;

	wantpool = mon->data->mlet == S_EEL;
	poolok = is_flyer(mon->data) || (is_swimmer(mon->data) && !wantpool);
nexttry:	/* eels prefer the water, but if there is no water nearby,
		   they will crawl over land */
	if(mon->mconf) {
		flag |= ALLOW_ALL;
		flag &= ~NOTONL;
	}
	if(!mon->mcansee)
		flag |= ALLOW_SSM;
	for(nx = x-1; nx <= x+1; nx++) for(ny = y-1; ny <= y+1; ny++) {
	    if((nx == x && ny == y) || !isok(nx,ny)) continue;
	    if(IS_ROCK(ntyp = levl[nx][ny].typ) && !(flag & ALLOW_WALL) &&
		!((flag & ALLOW_DIG) && may_dig(nx,ny))) continue;
	    if(IS_DOOR(ntyp) && !amorphous(mon->data) &&
	       ((levl[nx][ny].doormask & D_LOCKED &&
		   !is_giant(mon->data) && !mon->isshk) ||
		(levl[nx][ny].doormask & D_CLOSED &&
		   (verysmall(mon->data) ||
		    (!is_giant(mon->data) && nohands(mon->data))))
	       ) && !(flag & (ALLOW_WALL|ALLOW_DIG))) continue;
	    if(nx != x && ny != y && (IS_DOOR(nowtyp) || IS_DOOR(ntyp)))
		continue;
	    if(is_pool(nx,ny) == wantpool || poolok) {
		/* Displacement also displaces the Elbereth/scare monster,
		 * as long as you are visible.
		 */
		int dispx = (Displaced && (!Invis || perceives(mon->data)) &&
			(mon->mux==nx)) ? u.ux : nx;
		int dispy = (Displaced && (!Invis || perceives(mon->data)) &&
			(mon->muy==ny)) ? u.uy : ny;

		info[cnt] = 0;
		if(sobj_at(SCR_SCARE_MONSTER, dispx, dispy)
#ifdef ELBERETH
		   || sengr_at("Elbereth", dispx, dispy)
#endif
		  ) {
			if(!(flag & ALLOW_SSM)) continue;
			info[cnt] |= ALLOW_SSM;
		}
		if((nx == u.ux && ny == u.uy) ||
		   (nx == mon->mux && ny == mon->muy)) {
			if(!(flag & ALLOW_U)) continue;
			info[cnt] |= ALLOW_U;
		} else {
			if(levl[nx][ny].mmask) {
				if(!(flag & ALLOW_M)) continue;
				info[cnt] |= ALLOW_M;
				if((m_at(nx,ny))->mtame) {
					if(!(flag & ALLOW_TM)) continue;
					info[cnt] |= ALLOW_TM;
				}
			}
#if defined(ALTARS) && defined(THEOLOGY)
			/* Note: ALLOW_SANCT only prevents movement, not */
			/* attack, into a temple. */
			if(!in_temple(x, y) && in_temple(nx, ny) &&
					u_in_sanctuary(in_temple(nx, ny))) {
				if(!(flag & ALLOW_SANCT)) continue;
				info[cnt] |= ALLOW_SANCT;
			}
#endif
		}
		if(sobj_at(CLOVE_OF_GARLIC, nx, ny)) {
			if(flag & NOGARLIC) continue;
			info[cnt] |= NOGARLIC;
		}
		if(sobj_at(BOULDER, nx, ny)) {
			if(!(flag & ALLOW_ROCK)) continue;
			info[cnt] |= ALLOW_ROCK;
		}
		if((!Invis || perceives(mon->data)) && online(nx,ny)){
			if(flag & NOTONL) continue;
			info[cnt] |= NOTONL;
		}
		/* we cannot avoid traps of an unknown kind */
		{ register struct trap *ttmp = t_at(nx, ny);
		  register long tt;
			if(ttmp) {
/*				tt = 1L << ttmp->ttyp;*/
/* why don't we just have code look like what it's supposed to do? then it
/* might start working for every case. try this instead: -sac */
				tt = (ttmp->ttyp < TRAPNUM && ttmp->ttyp);
				/* below if added by GAN 02/06/87 to avoid
				 * traps out of range
				 */
				if(!(tt & ALLOW_TRAPS))  {
impossible("A monster looked at a very strange trap of type %d.", ttmp->ttyp);
					continue;
				}
				if(mon->mtrapseen & tt) {

					if(!(flag & tt)) continue;
					info[cnt] |= tt;
				}
			}
		}
		poss[cnt].x = nx;
		poss[cnt].y = ny;
		cnt++;
	    }
	}
	if(!cnt && wantpool && !is_pool(x,y)) {
		wantpool = FALSE;
		goto nexttry;
	}
	return(cnt);
}

int
dist(x, y)
register int x,y;
{
	register int dx = x - u.ux, dy = y - u.uy;
	return dx*dx + dy*dy;
}

static const char *poiseff[] = {

	" feel very weak", "r brain is on fire",
	" can't think straight", "r muscles won't obey you",
	" feel very sick", " break out in hives"
};

void
poisontell(typ)

	int	typ;
{
	pline("You%s.", poiseff[typ]);
}

void
poisoned(string, typ, pname)
register char *string, *pname;
register int  typ;
{
	register int i, plural;
	boolean thrown_weapon = !strncmp(string, "poison", 6);
		/* admittedly a kludge... */

	if(strcmp(string, "blast") && !thrown_weapon) {
	    /* 'blast' has already given a 'poison gas' message */
	    /* so have "poison arrow", "poison dart", etc... */
	    plural = (string[strlen(string) - 1] == 's')? 1 : 0;
	    if(Blind)
		pline("%s poisoned.", (plural) ? "They were" : "It was");
	    else
		pline("The %s %s poisoned!", string, (plural) ? "were" : "was");
	}

	if(Poison_resistance) {
		if(!strcmp(string, "blast")) shieldeff(u.ux, u.uy);
		pline("The poison doesn't seem to affect you.");
		return;
	}
	i = rn2(10 + 20*thrown_weapon);
	if(i == 0 && typ != A_CHA) {
		u.uhp = -1;
		pline("The poison was deadly...");
	} else if(i <= 5) {
		pline("You%s!", poiseff[typ]);
		adjattrib(typ, thrown_weapon ? -1 : -rn1(3,3), TRUE);
	} else {
		losehp(thrown_weapon ? rnd(6) : rn1(10,6), pname);
	}
	if(u.uhp < 1) {
		killer = pname;
		done("died");
	}
}

static void
m_detach(mtmp)
register struct monst *mtmp;
{
#ifdef WALKIES
	if(mtmp->mleashed) m_unleash(mtmp);
#endif
	relobj(mtmp,1);
	unpmon(mtmp);
	relmon(mtmp);
	unstuck(mtmp);
}

void
mondead(mtmp)
register struct monst *mtmp;
{
	m_detach(mtmp);	
#ifdef KOPS
	if(mtmp->data->mlet == S_KOP && allow_kops) {
	    /* Dead Kops may come back. */
	    switch(rnd(5)) {
		case 1:	     /* returns near the stairs */
			(void) mkmon_at(mtmp->data->mname,xdnstair,ydnstair);
			break;
		case 2:	     /* randomly */
			(void) mkmon_at(mtmp->data->mname,0,0);
			break;
		default:
			break;
	    }
	}
#endif
	if(mtmp->isshk) shkdead(mtmp);
	if(mtmp->isgd) gddead();
#ifdef WORM
	if(mtmp->wormno) wormdead(mtmp);
#endif
#ifdef HARD
	if(mtmp->iswiz) wizdead(mtmp);
#endif
#ifdef MEDUSA
	if(mtmp->data == &mons[PM_MEDUSA]) u.ukilled_medusa = TRUE;
#endif
	monfree(mtmp);
}

/* called when monster is moved to larger structure */
void
replmon(mtmp, mtmp2)
register struct monst *mtmp, *mtmp2;
{
	relmon(mtmp);
	monfree(mtmp);
	levl[mtmp2->mx][mtmp2->my].mmask = 1;
	mtmp2->nmon = fmon;
	fmon = mtmp2;
	if(u.ustuck == mtmp) u.ustuck = mtmp2;
	if(mtmp2->isshk) replshk(mtmp,mtmp2);
	if(mtmp2->isgd) replgd(mtmp,mtmp2);
}

void
relmon(mon)
register struct monst *mon;
{
	register struct monst *mtmp;

	if (fmon == ((struct monst *) 0))
            panic ("relmon: no fmon available.");

	levl[mon->mx][mon->my].mmask = 0;

	if(mon == fmon) fmon = fmon->nmon;
	else {
		for(mtmp = fmon; mtmp && mtmp->nmon != mon; mtmp = mtmp->nmon) ;
		if(mtmp)    mtmp->nmon = mon->nmon;
		else	    panic("relmon: mon not in list.");
	}
}

/* we do not free monsters immediately, in order to have their name
   available shortly after their demise */
void
monfree(mtmp) register struct monst *mtmp; {
	mtmp->nmon = fdmon;
	fdmon = mtmp;
	levl[mtmp->mx][mtmp->my].mmask = 0;
}

void
unstuck(mtmp)
register struct monst *mtmp;
{
	if(u.ustuck == mtmp) {
		if(u.uswallow){
			u.ux = mtmp->mx;
			u.uy = mtmp->my;
			u.uswallow = 0;
			setsee();
			docrt();
		}
		u.ustuck = 0;
	}
}

void
killed(mtmp)
register struct monst *mtmp;
{
	xkilled(mtmp, 1);
}

void
xkilled(mtmp, dest)
	register struct monst *mtmp;
/*
 * Dest=1, normal; dest=0, don't print message; dest=2, don't drop corpse
 * either; dest=3, message but no corpse
 */
	int	dest;
{
	register int tmp, nk, x, y;
	register struct permonst *mdat = mtmp->data;
	register struct obj *otmp;
	boolean chance;

	if (dest & 1) {
	    if(!cansee(mtmp->mx,mtmp->my)) You("destroy it!");
	    else {
		You("destroy %s!",
			mtmp->mtame ? a_monnam(mtmp, "poor") : mon_nam(mtmp));
	    }
	}

	/* restore chameleon, lycanthropes to true form at death */
	/* cannot do this in make_corpse() since genociding monsters after
	 * MAXMONNO were killed does the wrong type
	 */
	if(mtmp->cham) mtmp->data = mdat = &mons[PM_CHAMELEON];
	if(mdat == &mons[PM_JACKALWERE])
		mtmp->data = mdat = &mons[PM_WEREJACKAL];
	if(mdat == &mons[PM_WOLFWERE])
		mtmp->data = mdat = &mons[PM_WEREWOLF];
	if(mdat == &mons[PM_RATWERE])
		mtmp->data = mdat = &mons[PM_WERERAT];

	if(u.umconf) {
	    if(!Blind) {
		Your("%s stop glowing %s.",
		makeplural(body_part(HAND)),
		Hallucination ? hcolor() : red);
	    }
	    u.umconf = 0;
	}

	/* if we have killed MAXMONNO monsters of a given type, and it
	 * can be done, genocide that monster.
	 */
	tmp = monsndx(mdat);
	u.nr_killed[tmp]++;
	nk = u.nr_killed[tmp];
#ifdef TOLKIEN
	if(nk > (tmp==PM_NAZGUL ? 9 : MAXMONNO) &&
				!(mons[tmp].geno & (G_NOGEN | G_GENOD))) {
#else
	if(nk > MAXMONNO && !(mons[tmp].geno & (G_NOGEN | G_GENOD))) {
#endif
#ifdef DEBUG
		pline("Automatically genocided %s.", makeplural(mons[tmp].mname));
#endif
		mons[tmp].geno |= G_GENOD;
	}
#ifdef MAIL
	/* If you kill the mail daemon, no more mail delivery.  -3. */
	else if(tmp==PM_MAIL_DAEMON) mons[tmp].geno |= G_GENOD;
#endif	

	/* punish bad behaviour */
	if(is_human(mdat) && !always_hostile(mdat) &&
	   (monsndx(mdat) < PM_ARCHEOLOGIST || monsndx(mdat) > PM_WIZARD) &&
	   u.ualigntyp != U_CHAOTIC) {
		HTelepat &= ~INTRINSIC;
		change_luck(-2);
	}
	if((mtmp->mpeaceful && !rn2(2)) || mtmp->mtame)	change_luck(-1);
	if ((mdat==&mons[PM_BLACK_UNICORN] && u.ualigntyp == U_CHAOTIC) ||
	    (mdat==&mons[PM_GREY_UNICORN] && u.ualigntyp == U_NEUTRAL) ||
	    (mdat==&mons[PM_WHITE_UNICORN] && u.ualigntyp == U_LAWFUL))
		change_luck(-5);

	/* give experience points */
	tmp = experience(mtmp, nk);
	more_experienced(tmp, 0);
	newexplevel();		/* will decide if you go up */

	/* adjust alignment points */
	if(mtmp->mtame)
		adjalign(-15);	/* bad!! */
#if defined(ALTARS) && defined(THEOLOGY)
	else if (mtmp->ispriest && !p_coaligned(mtmp))
		adjalign(2);
#endif
	else if (mtmp->mpeaceful)
		adjalign(-5);
	/* malign was already adjusted for ualigntyp and randomization */
	adjalign(mtmp->malign);

	/* dispose of monster and make cadaver */
	if(stoned) {
		monstone(mtmp);
		return;
	}

	x = mtmp->mx;   y = mtmp->my;

	mondead(mtmp);

	if((dest & 2)
#ifdef REINCARNATION
		 || dlevel == rogue_level
#endif
					) return;

#ifdef WORM
	if(mdat == &mons[PM_LONG_WORM]) {
		(void) mksobj_at(WORM_TOOTH, x, y);
		stackobj(fobj);
		newsym(x,y);
	}
#endif
#ifdef MAIL
	if(mdat == &mons[PM_MAIL_DAEMON]) {
		(void) mksobj_at(SCR_MAIL, x, y);
		stackobj(fobj);
		newsym(x,y);
	}
#endif
	if(!ACCESSIBLE(levl[x][y].typ) ||
	   (IS_DOOR(levl[x][y].typ) &&
	    levl[x][y].doormask & (D_CLOSED | D_LOCKED))) {
	    /* might be mimic in wall or dead eel*/
 	    newsym(x,y);
	} else if(x != u.ux || y != u.uy) {
		/* might be here after swallowed */
		if (!rn2(6) && !(mdat->geno & G_NOCORPSE)
#ifdef KOPS
					&& mdat->mlet != S_KOP
#endif
							) {
			int typ;

			otmp = mkobj_at(RANDOM_SYM, x, y);
			/* Don't create large objects from small monsters */
			typ = otmp->otyp;
			if (!bigmonst(mdat) && typ != FOOD_RATION
#ifdef WALKIES
			    && typ != LEASH
#endif
			    && typ != FIGURINE
			    && (otmp->owt > 3 ||
				(typ >= SPEAR && typ <= LANCE) ||
				(typ >= SCIMITAR && typ <= KATANA) ||
				(typ == MORNING_STAR || typ == QUARTERSTAFF) ||
				(typ >= BARDICHE && typ <= VOULGE) ||
				(typ>=PLATE_MAIL && typ<=DRAGON_SCALE_MAIL) ||
				(typ == LARGE_SHIELD))) {
			    delobj(otmp);
			} else newsym(x,y);
		}
		/* Whether or not it always makes a corpse is, in theory,
		 * different from whether or not the corpse is "special";
		 * if we want both, we have to specify it explicitly.
		 */
		if (bigmonst(mdat)
#ifdef GOLEMS
				   || is_golem(mdat)
#endif
		   ) chance = 1;
		else chance = !rn2((int)
			(2 + ((mdat->geno & G_FREQ)<2) + verysmall(mdat)));
		if (chance)
			(void) make_corpse(mtmp);
	}
}

#ifdef __STDC__
#include <stdarg.h>
void
kludge(char *str, char *arg, ...)
{	va_list ap;
	char buf[BUFSZ];

	va_start(ap, arg);
	if(Blind || !flags.verbose) {
		if(*str == '%') arg = "It";
		else arg = "it";
	}
	vsprintf(buf, str, ap);
	pline("%s", buf);
}
#else
/*VARARGS2*/
void
kludge(str, arg, arg2, arg3)
	register char *str,*arg,*arg2,*arg3;
{
	if(Blind || !flags.verbose) {
		if(*str == '%') pline(str,"It",arg2,arg3);
		else pline(str,"it",arg2,arg3);
	} else pline(str,arg,arg2,arg3);
}
#endif

void
rescham() {	/* force all chameleons to become normal */

	register struct monst *mtmp;

	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if(mtmp->cham) {
			mtmp->cham = 0;
			(void) newcham(mtmp, &mons[PM_CHAMELEON]);
		}
		if(is_were(mtmp->data) && mtmp->data->mlet != S_HUMAN)
			(void) new_were(mtmp);
	}
}

/* Let the chameleons change again -dgk */
void
restartcham() {

	register struct monst *mtmp;

	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
		if (mtmp->data->mlet == S_CHAMELEON)
			mtmp->cham = 1;
}

int
newcham(mtmp, mdat)	/* make a chameleon look like a new monster */
			/* returns 1 if the monster actually changed */
	register struct monst *mtmp;
	register struct permonst *mdat;
{
	register int mhp, hpn, hpd;
	int tryct;

	/* mdat = 0 -> caller wants a random monster shape */
	tryct = 0;
	if(mdat == ((struct permonst *) 0)) {
		while (++tryct < 100) {
			static int num;
			mdat = &mons[num=rn2(NUMMONS)];
			if ((!is_human(mdat) || num == PM_NURSE)
				&& !type_is_pname(mdat)
				&& !is_were(mdat)
#ifdef MEDUSA
				&& num != PM_MEDUSA
#endif
#ifdef MAIL
				&& num != PM_MAIL_DAEMON
#endif
				)
				break;
		}
		if (tryct >= 100) return(0); /* Should never happen */
	}
  	if(mdat == mtmp->data) return(0);	/* still the same monster */

#ifdef WORM
	if(mtmp->wormno) wormdead(mtmp);	/* throw tail away */
#endif
	mtmp->m_lev = adj_lev(mdat);		/* new monster level */

	hpn = mtmp->mhp;
 	hpd = (mtmp->m_lev < 50) ? (mtmp->m_lev)*8 : mdat->mlevel;
 	if(!hpd) hpd = 4;
 	mhp = (mtmp->m_lev < 50) ? (mtmp->m_lev)*8 : mdat->mlevel;
 	if(!mhp) mhp = 4;

	/* new hp: same fraction of max as before */
#ifndef LINT
 	mtmp->mhp = (int)(((long)hpn*(long)mhp)/(long)hpd);
#endif
 	if(mtmp->mhp < 0) mtmp->mhp = hpn;	/* overflow */
/* Unlikely but not impossible; a 1HD creature with 1HP that changes into a
   0HD creature will require this statement */
 	if (!mtmp->mhp) mtmp->mhp = 1;

/* and the same for maximum hit points */
	hpn = mtmp->mhpmax;
#ifndef LINT
 	mtmp->mhpmax = (int)(((long)hpn*(long)mhp)/(long)hpd);
#endif
 	if(mtmp->mhpmax < 0) mtmp->mhpmax = hpn;	/* overflow */
 	if (!mtmp->mhpmax) mtmp->mhpmax = 1;

 	mtmp->data = mdat;
	mtmp->minvis = !!(mdat->mlet == S_STALKER);
	mtmp->mhide = !!hides_under(mdat);
	if (!mtmp->mhide) mtmp->mundetected = 0;
	if (u.ustuck == mtmp
#ifdef POLYSELF
			&& !sticks(uasmon)
#endif
			&& !sticks(mdat))
		u.ustuck = 0;
#ifdef WORM
	if(mdat == &mons[PM_LONG_WORM] && getwn(mtmp)) initworm(mtmp);
			/* perhaps we should clear mtmp->mtame here? */
#endif
	unpmon(mtmp);	/* necessary for 'I' and to force pmon */
	pmon(mtmp);
	return(1);
}

void
mnexto(mtmp)	/* Make monster mtmp next to you (if possible) */
	struct monst *mtmp;
{
	coord mm;
	enexto(&mm, u.ux, u.uy);
	levl[mtmp->mx][mtmp->my].mmask = 0;
	levl[mm.x][mm.y].mmask = 1;
	mtmp->mx = mm.x;
	mtmp->my = mm.y;
	pmon(mtmp);
	set_apparxy(mtmp);
}

/* Make monster near (or at) location if possible */
#ifdef __STDC__
void
mnearto(struct monst *mtmp,xchar x,xchar y,boolean gz)
#else
void
mnearto(mtmp,x,y,gz)
register struct monst *mtmp;
xchar x, y;
boolean gz;     
#endif
{
	coord mm;
	if(!gz || !goodpos(x,y)) {
		enexto(&mm, x, y);
		x = mm.x; y = mm.y;
	}
	if(x == mtmp->mx && y == mtmp->my) /* that was easy */
		return;
	levl[mtmp->mx][mtmp->my].mmask = 0;
	levl[x][y].mmask = 1;
	mtmp->mx = x;
	mtmp->my = y;
	pmon(mtmp);
	set_apparxy(mtmp);
}

void
setmangry(mtmp)
	register struct monst *mtmp;
{
	if(!mtmp->mpeaceful) return;
	if(mtmp->mtame) return;
	mtmp->mpeaceful = 0;
#if defined(ALTARS) && defined(THEOLOGY)
	if(mtmp->ispriest) {
		if(p_coaligned(mtmp)) adjalign(-5); /* very bad */
		else adjalign(2);
	} else
#endif
	adjalign(-1);		/* attacking peaceful monsters is bad */
	if(humanoid(mtmp->data) || mtmp->isshk || mtmp->isgd)
		pline("%s gets angry!", Monnam(mtmp));
#ifdef SOUNDS
	else if (flags.verbose && flags.soundok) growl(mtmp);
#endif
}

int
disturb(mtmp)		/* awaken monsters while in the same room.
			 * return a 1 if they have been woken.
			 */
	register struct monst *mtmp;
{
	/* wake up, or get out of here. */
	/* ettins are hard to surprise */
	/* Nymphs and Leprechauns do not easily wake up */
	if(cansee(mtmp->mx,mtmp->my) &&
		(!Stealth || (mtmp->data == &mons[PM_ETTIN] && rn2(10))) &&
		(!(mtmp->data->mlet == S_NYMPH
		   || mtmp->data->mlet == S_LEPRECHAUN) || !rn2(50)) &&
		(Aggravate_monster ||
		 (mtmp->data->mlet == S_DOG || mtmp->data->mlet == S_HUMAN) ||
		(!rn2(7) && !mtmp->mimic))) {
		mtmp->msleep = 0;
		return(1);
	}
	if(Hallucination) pmon(mtmp);
	return(0);
}

#ifdef HARD
static boolean
restrap(mtmp)
/* unwatched hiders may hide again,
 * if so, a 1 is returned.
 */
register struct monst *mtmp;
{
	if(mtmp->cham || mtmp->mcan || mtmp->mimic ||
	   cansee(mtmp->mx, mtmp->my) || rn2(3))
		return(FALSE);

	if(mtmp->data->mlet == S_MIMIC) {
		set_mimic_sym(mtmp);
		return(TRUE);
	} else
	    if(levl[mtmp->mx][mtmp->my].typ == ROOM)  {
		(void) maketrap(mtmp->mx, mtmp->my, MONST_TRAP);
		/* override type selection */
		ftrap->pm = monsndx(mtmp->data);
		mondead(mtmp);
		return(TRUE);
	    }

	return(FALSE);
}
#endif

/* drop (perhaps) a cadaver and remove monster */
void
mondied(mdef)
register struct monst *mdef;
{
	mondead(mdef);
	if(rn2(3)
#ifdef REINCARNATION
	   && dlevel != rogue_level
#endif
					)
		(void) make_corpse(mdef);
}

/* monster disappears, not dies */
void
mongone(mdef)
register struct monst *mdef;
{
	register struct obj *otmp, *otmp2;

	/* release monster's inventory */
	for (otmp = mdef->minvent; otmp; otmp = otmp2) {
		otmp2 = otmp->nobj;
		obfree(otmp, (struct obj *)0);
	}
	mdef->minvent = 0;
	mdef->mgold = 0;
	m_detach(mdef);
	monfree(mdef);
}

/* drop a statue or rock and remove monster */
void
monstone(mdef)
register struct monst *mdef;
{
	struct obj *otmp;

	if(!verysmall(mdef->data) ||
	   !rn2(2 + ((mdef->data->geno & G_FREQ) > 2))) {
		otmp = mk_named_object(STATUE, mdef->data, mdef->mx, mdef->my,
			NAME(mdef), (int)mdef->mnamelth);
		otmp->spe = 0; /* no book inside */
	} else
		(void) mksobj_at(ROCK, mdef->mx, mdef->my);

	stackobj(fobj);

	if(cansee(mdef->mx, mdef->my)){
		unpmon(mdef);
		atl(mdef->mx,mdef->my,Hallucination ? rndobjsym() : fobj->olet);
	}
	mondead(mdef);
}

#ifdef GOLEMS
void
golemeffects(mon, damtype, dam)
register struct monst *mon;
int damtype, dam;
{
	int heal=0, slow=0;

	if (mon->data != &mons[PM_FLESH_GOLEM]
					&& mon->data != &mons[PM_IRON_GOLEM])
		return;

	if (mon->data == &mons[PM_FLESH_GOLEM]) {
		if (damtype == AD_ELEC) heal = dam / 6;
		else if (damtype == AD_FIRE || damtype == AD_COLD) slow = 1;
	} else {
		if (damtype == AD_ELEC) slow = 1;
		else if (damtype == AD_FIRE) heal = dam;
	}
	if (slow) {
		if (mon->mspeed != MSLOW) {
			if (mon->mspeed == MFAST) mon->mspeed = 0;
			else mon->mspeed = MSLOW;
			if (cansee(mon->mx, mon->my))
				pline("%s seems to be moving slower.",
					Monnam(mon));
		}
	}
	if (heal) {
		if (mon->mhp < mon->mhpmax) {
			mon->mhp += dam;
			if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
			if (cansee(mon->mx, mon->my))
				pline("%s seems healthier.", Monnam(mon));
		}
	}
}
#endif /* GOLEMS */
