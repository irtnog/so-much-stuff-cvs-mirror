/*	SCCS Id: @(#)pray.c	3.0	89/01/10
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifdef THEOLOGY

#define ALIGNLIM 	(5L + (moves/200L))

struct ghods {

	char	classlet;
	char	*law, *balance, *chaos;
}  gods[] = {

'A', /* Central American */	"Quetzalcotl", "Camaxtli", "Huhetotl",
'B', /* Celtic */		"Nuada", "Dagda", "Morrigan",
'C', /* Babylonian */		"Anu", "Ishtar", "Anshar",
'E', /* Elven */		"Solonor Thelandira",
				"Aerdrie Faenya", "Erevan Ilesere",
'H', /* Greek */		"Athena", "Hermes", "Poseidon",
'K', /* Celtic */		"Lugh", "Brigit", "Macannan Mac Lir",
#ifdef NAMED_ITEMS
/* It'd look funny if someone got a sword to steal souls for Arioch but Arioch
 * goes with the wrong character class...
 */
'P', /* Hyborian */		"Mitra", "Crom", "Set",
#else
'P', /* Melnibonean */		"Donblas", "Grome", "Arioch",
#endif
'R', /* Nehwon */		"Votishal", "Death", "Rat God",
'S', /* Japanese */		"Amaterasu Omikami", "Raiden", "Susanowo",
'T', /* Chinese */		"Shan Lai Ching", "Chih Sung-tzu", "Huan Ti",
'V', /* Norse */		"Tyr", "Balder", "Loki",
'W', /* Egyptian */		"Ptah", "Thoth", "Anhur",
0,0,0,0
};

#define TROUBLE_STONED 1
#define TROUBLE_STRANGLED 2
#define TROUBLE_SICK 3
#define TROUBLE_STARVING 4
#define TROUBLE_HIT 5
#define TROUBLE_STUCK_IN_WALL 6

#define TROUBLE_PUNISHED (-1)
#define TROUBLE_LYCANTHROPE (-2)
#define TROUBLE_CURSED_ITEMS (-3)
#define TROUBLE_HUNGRY (-4)
#define TROUBLE_POISONED (-5)
#define TROUBLE_HALLUCINATION (-6)
#define TROUBLE_BLIND (-7)
#define TROUBLE_WOUNDED_LEGS (-8)
#define TROUBLE_STUNNED (-9)
#define TROUBLE_CONFUSED (-10)

/* We could force rehumanize of polyselfed people, but we can't tell
   unintentional shape changes from the other kind. Oh well. */

/* Return 0 if nothing particular seems wrong, positive numbers for
   serious trouble, and negative numbers for comparative annoyances. This
   returns the worst problem. There may be others, and the gods may fix
   more than one.

This could get as bizarre as noting surrounding opponents, (or hostile dogs),
but that's really hard.
 */

#define ugod_is_angry() (u.ualign < 0)
#ifdef ALTARS
#define on_altar()	IS_ALTAR(levl[u.ux][u.uy].typ)
#define on_shrine()	((levl[u.ux][u.uy].altarmask & A_SHRINE) != 0)
#endif

static int
in_trouble()
{
	register struct obj *otmp;
	int i, j, count=0;

/* Borrowed from eat.c */

#define	SATIATED	0
#define NOT_HUNGRY	1
#define	HUNGRY		2
#define	WEAK		3
#define	FAINTING	4
#define FAINTED		5
#define STARVED		6

	if(Stoned) return(TROUBLE_STONED);
	if(Strangled) return(TROUBLE_STRANGLED);
	if(Sick) return(TROUBLE_SICK);
	if(u.uhs >= WEAK) return(TROUBLE_STARVING);
	if(u.uhp < 5 || (u.uhp*7 < u.uhpmax)) return(TROUBLE_HIT);
	for (i= -1; i<=1; i++) for(j= -1; j<=1; j++) {
		if (!i && !j) continue;
		if (!isok(u.ux+i, u.uy+j) || IS_ROCK(levl[u.ux+i][u.uy+j].typ))
			count++;
	}
	if(count==8
#ifdef POLYSELF
	    && !passes_walls(uasmon)
#endif
	    ) return(TROUBLE_STUCK_IN_WALL);
	if(Punished) return(TROUBLE_PUNISHED);
#ifdef POLYSELF
	if(u.ulycn >= 0) return(TROUBLE_LYCANTHROPE);
#endif
	for(otmp=invent; otmp; otmp=otmp->nobj)
		if((otmp->otyp==LOADSTONE || otmp->otyp==LUCKSTONE) &&
			otmp->cursed) 
		    return(TROUBLE_CURSED_ITEMS);
	if((uarmh && uarmh->cursed) ||	/* helmet */
	   (uarms && uarms->cursed) ||	/* shield */
	   (uarmg && uarmg->cursed) ||	/* gloves */
	   (uarm && uarm->cursed) ||	/* armor */
	   (uarmc && uarmc->cursed) ||	/* cloak */
	   (uarmf && uarmf->cursed) ||	/* boots */
#ifdef SHIRT
	   (uarmu && uarmu->cursed) ||  /* shirt */
#endif
	   (uwep && (uwep->olet == WEAPON_SYM || uwep->otyp==PICK_AXE
		|| uwep->otyp==TIN_OPENER || uwep->otyp==HEAVY_IRON_BALL) &&
		(uwep->cursed)) ||
	   (uleft && uleft->cursed) ||
	   (uright && uright->cursed) ||
	   (uamul && uamul->cursed) ||
	   (ublindf && ublindf->cursed))

	   return(TROUBLE_CURSED_ITEMS);

	if(u.uhs >= HUNGRY) return(TROUBLE_HUNGRY);
	for(i=0; i<A_MAX; i++)
	    if(ABASE(i) < AMAX(i)) return(TROUBLE_POISONED);
	if(Hallucination) return(TROUBLE_HALLUCINATION);
	if(Blinded > 1) return(TROUBLE_BLIND);
	if(Wounded_legs) return (TROUBLE_WOUNDED_LEGS);
	if(HStun) return (TROUBLE_STUNNED);
	if(HConfusion) return (TROUBLE_CONFUSED);

	return(0);
}

static void
fix_worst_trouble(trouble)
register int trouble;
{
	int i;
	u.ublesscnt += rnz(100);
	switch (trouble) {
	    case TROUBLE_STONED:
		    You("feel more limber.");
		    Stoned = 0;
		    break;
	    case TROUBLE_STRANGLED:
		    You("can breathe again.");
		    Strangled = 0;
		    break;
	    case TROUBLE_HUNGRY:
	    case TROUBLE_STARVING:
		    Your("stomach feels content.");
		    init_uhunger ();
		    losestr(-1);
		    flags.botl = 1;
		    break;
	    case TROUBLE_SICK:
		    You("feel better.");
		    make_sick(0L,FALSE);
		    break;
	    case TROUBLE_HIT:
		    if (!Blind) {
			const char *tmp = Hallucination ? hcolor() : golden;
			pline("A%s %s glow surrounds you.",
			      index(vowels,*tmp) ? "n" : "", tmp);
		    } else You("feel much better.");
		    u.uhp = u.uhpmax += 5;
		    flags.botl = 1;
		    break;
	    case TROUBLE_STUCK_IN_WALL:
		    Your("surroundings change.");
		    tele();
		    break;
	    case TROUBLE_PUNISHED:
		    Your("chain disappears.");
		    unpunish();
		    break;
#ifdef POLYSELF
	    case TROUBLE_LYCANTHROPE:
		    You("feel purified.");
		    if(uasmon == &mons[u.ulycn] && !Polymorph_control)
			rehumanize();
		    u.ulycn = -1;       /* now remove the curse */
		    break;
#endif
	    case TROUBLE_CURSED_ITEMS:
		    {	struct obj *otmp;
			char * what;
			otmp = (struct obj *)0;
			what = NULL;
			if (uarmh && uarmh->cursed) 	/* helmet */
			    otmp = uarmh;
			else if (uarms && uarms->cursed) /* shield */
			    otmp = uarms;
			else if (uarmg && uarmg->cursed) /* gloves */
			    otmp = uarmg;
			else if (uarm && uarm->cursed) /* armor */
			    otmp = uarm;
			else if (uarmc && uarmc->cursed) /* cloak */
			    otmp = uarmc;
			else if (uarmf && uarmf->cursed) /* boots */
			    otmp = uarmf;
#ifdef SHIRT
			else if (uarmu && uarmu->cursed) /* shirt */
			    otmp = uarmu;
#endif
			else if (uleft && uleft->cursed) {
			    otmp = uleft;
			    what = "left ring softly glows";
			} else if (uright && uright->cursed) {
			    otmp = uright;
			    what = "right ring softly glows";
			} else if (uamul && uamul->cursed) /* amulet */
			    otmp = uamul;
			else if (ublindf && ublindf->cursed) /* blindfold */
			    otmp = ublindf;
			else if (welded(uwep)) otmp = uwep;
			else {
			    for(otmp=invent; otmp; otmp=otmp->nobj)
				if ((otmp->otyp==LOADSTONE ||
				     otmp->otyp==LUCKSTONE) && otmp->cursed)
					break;
			}

			otmp->cursed = 0;
			otmp->bknown = 1;
			if (!Blind)
			    Your("%s %s.",
				   what ? what : aobjnam (otmp, "softly glow"),
				   Hallucination ? hcolor() : amber);
			break;
		    }
	    case TROUBLE_HALLUCINATION:
		    pline ("Looks like you are back in Kansas.");
		    make_hallucinated(0L,FALSE);
		    break;
	    case TROUBLE_BLIND:
		    pline ("Your %s feel better.", makeplural(body_part(EYE)));
		    make_blinded(0L,FALSE);
		    break;
	    case TROUBLE_POISONED:
		    if (Hallucination)
			pline("There's a tiger in your tank.");
		    else
			You("feel in good health again.");
		    for(i=0; i<A_MAX; i++) {
			if(ABASE(i) < AMAX(i)) {
				ABASE(i) = AMAX(i);
				flags.botl = 1;
			}
		    }
		    break;
	    case TROUBLE_WOUNDED_LEGS:
		    heal_legs();
		    break;
	    case TROUBLE_STUNNED:
		    make_stunned(0L,TRUE);
		    break;
	    case TROUBLE_CONFUSED:
		    make_confused(0L,TRUE);
		    break;
	}
}

static void
angrygods() {
	register int	tmp;

	u.ublessed = 0;

	/* changed from tmp = u.ugangr + abs (u.uluck) -- rph */
	tmp =  3*u.ugangr +
	       (u.uluck > 0 || u.ualign > 3 ? -u.uluck/3 : -u.uluck);
	if (tmp < 0) tmp = 0; /* possible if bad alignment but good luck */
	tmp =  (tmp > 15 ? 15 : tmp);  /* lets be a little reasonable */
	switch (tmp ? rn2(tmp): 0) {

	    case 0:
	    case 1:	if (Hallucination)
			    You("feel %sholy dread.",
				  u.ualigntyp == U_CHAOTIC ? "an un" : "a ");
			else You("feel that %s is %s.",
#  ifdef ALTARS
				   on_altar() ? a_gname() : u_gname(),
#  else
				   u_gname(),
#  endif
				   u.ualigntyp == U_NEUTRAL ? "offended" : "angry");
			break;
	    case 2:
	    case 3:
# ifdef POLYSELF
			pline("A voice booms out:  \"Thou %s, %s.\"",
			      ugod_is_angry() ? "hast strayed from the path" :
					        "art arrogant",
			      u.usym == S_HUMAN ? "mortal" : "creature");
# else
			pline("A voice booms out:  \"Thou %s, mortal.\"",
			      ugod_is_angry() ? "hast strayed from the path" :
					        "art arrogant");
# endif
			pline("\"Thou must relearn thy lessons!\"");
			adjattrib(A_WIS, -1, FALSE);
			if (u.ulevel > 1) {
			    losexp();
			    if(u.uhp < 1) u.uhp = 1;
			    if(u.uhpmax < 1) u.uhpmax = 1;
			} else  {
			    u.uexp = 0;
			    flags.botl = 1;
			}
			break;
	    case 6:	if (!Punished) {
			    punish((struct obj *)0);
			    break;
			} /* else fall thru */
	    case 4:
	    case 5:	if (!Blind) {
			    const char *temp = Hallucination ? hcolor() : black;
			    pline("A%s %s glow surrounds you.",
				  index(vowels,*temp) ? "n" : "", temp);
			}
			rndcurse();
			break;
	    case 7:
	    case 8:	pline("A voice booms out:  \"Thou durst call upon me?\"");
# ifdef POLYSELF
			pline("\"Then die, %s!\"",
			      u.usym == S_HUMAN ? "mortal" : "creature");
# else
			pline("\"Then die, mortal!\"");
# endif
			(void) makemon(&mons[ndemon()], u.ux, u.uy);
			break;

	    default:	pline("Suddenly, a bolt of lightning strikes you!");
			if (Reflecting) {
			    shieldeff(u.ux, u.uy);
			    if (Blind)
				pline("For some reason you're unaffected.");
			    else {
				if (Reflecting & W_AMUL) {
				    pline("It reflects from your medallion.");
#if defined(EXPLORE_MODE) || defined(WIZARD)
				    makeknown(AMULET_OF_REFLECTION);
#endif
				} else {
				    pline("It reflects from your shield.");
	/* No ifdef here since they could survive with an amulet of
	 * life saving which wasn't possible above.
	 */
				    makeknown(SHIELD_OF_REFLECTION);
				}
			    }
	You("hear a cosmic sigh, and sense a decision being made.");
	pline("A wide-angle disintegration beam hits you!");
	goto ohno;
			} else if (Shock_resistance) {
				shieldeff(u.ux, u.uy);
				pline("It seems not to affect you.");
			pline("However, the ensuing disintegration beam does.");
ohno:
				if (Disint_resistance) {
	You("bask in the disintegration beam for a minute...");
	pline("A voice rings out:  \"I believe it not!\"");
					break;
				}
			}
			You("fry to a crisp.");
			killer = "holy wrath";
			done("died");
			break;
	}
	u.ublesscnt = rnz(300);
	return;
}

static void
pleased() {
	const char	*tmp;
	int trouble = in_trouble ();	/* what's your worst difficulty? */
	int pat_on_head = 0;

	if (u.ugangr > 0) u.ugangr--;
	You("feel that %s is pleased.",
#ifndef ALTARS
		   u_gname());
#else
		   on_altar() ? a_gname() : u_gname());

	/* not your deity */
	if (on_altar() &&
	    (levl[u.ux][u.uy].altarmask & ~A_SHRINE) != u.ualigntyp + 1) {
		adjalign(-1);
		return;
	} else if (u.ualign < 2) adjalign(1);
#endif


	/* depending on your luck, the gods will:
	   - fix your worst problem if it's major.
	   - fix all your major problems.
	   - fix your worst problem if it's minor.
	   - fix all of your problems.
	   - do you a gratuitous favor.

	   if you make it to the the last category, you roll randomly again
	   to see what they do for you.

	   If your luck is at least 0, then you are guaranteed rescued
	   from your worst major problem. */

	if (!trouble) pat_on_head = 1;
	else {
#ifdef ALTARS
	    int action = rn1(on_altar() ? 3 + on_shrine() : 2, u.uluck+1);

	    if (!on_altar()) action = max(action,2);
#else
	    int action = rn1(4,u.uluck+1);
#endif

	    switch(min(action,5)) {
		case 5: pat_on_head = 1;
		case 4: do fix_worst_trouble(trouble);
			while(trouble = in_trouble());
			break;

		case 3: fix_worst_trouble(trouble);
		case 2: while((trouble = in_trouble()) > 0)
			    fix_worst_trouble(trouble);
			break;

		case 1: if (trouble > 0) fix_worst_trouble(trouble);
	    }
	}

    if(pat_on_head)
	switch(rn2((u.uluck + 6)>>1))  {

	    case 0:	break;
	    case 1:
			if(uwep && (uwep->olet == WEAPON_SYM || uwep->otyp
				== PICK_AXE) && (!uwep->blessed)) {
			    if (uwep->cursed) {
				uwep->cursed = 0;
				uwep->bknown = 1;
				if (!Blind)
				    Your("%s %s.",
					  aobjnam(uwep, "softly glow"),
					  Hallucination ? hcolor() : amber);
			    } else if(uwep->otyp < BOW) {
				uwep->blessed = uwep->bknown = 1;
				if (!Blind) {
				    tmp = Hallucination ? hcolor() : light_blue;
				    Your("%s with a%s %s aura.",
					  aobjnam(uwep, "softly glow"),
					  index(vowels,*tmp) ? "n" : "", tmp);
				}
			    }
			}
			break;
	    case 3:
#if defined(STRONGHOLD) && defined(MUSIC)
			/* takes 2 hints to get the music to enter the Stronghold */
			if (flags.soundok) {
			    if(music_heard < 1) {
				pline("A voice booms out:  \"Hark, mortal!\"");
				verbalize("To enter the castle, thou must play the right tune!");
				music_heard++;
				break;
			    } else if (music_heard < 2) {
				You("hear a divine music...");
				pline("It sounds like:  \"%s\".", tune);
				music_heard++;
				break;
			    }
			}
			/* Otherwise, falls into next case */
#endif
	    case 2:	if (!Blind) {
			    tmp = Hallucination ? hcolor() : golden;
			    You("are surrounded by a%s %s glow.",
				  index(vowels,*tmp) ? "n" : "", tmp);
			}
			u.uhp = u.uhpmax += 5;
			ABASE(A_STR) = AMAX(A_STR);
			if (u.uhunger < 900)	init_uhunger();
			if (u.uluck < 0)	u.uluck = 0;
			make_blinded(0L,TRUE);
			flags.botl = 1;
			break;
	    case 4:
		{	register struct obj *otmp;

			tmp = Hallucination ? hcolor() : light_blue;
			if (Blind)
				You("feel the power of %s.", u_gname());
			else You("are surrounded by a%s %s aura.",
					index(vowels,*tmp) ? "n" : "", tmp);
			for(otmp=invent; otmp; otmp=otmp->nobj) {
				if (otmp->cursed) {
				    otmp->cursed = 0;
				    if (!Blind)
					Your("%s %s.",
					      aobjnam(otmp, "softly glow"),
					      Hallucination ? hcolor() : amber);
				}
			}
			break;
		}
	    case 5:	pline("A voice booms out:  \"Thou hast pleased me with thy progress,\"");
			pline("\"and thus I grant thee the gift of ");
			if (!(HTelepat & INTRINSIC))  {
				HTelepat |= INTRINSIC;
				addtopl("Telepathy!\"");
			} else if (!(Fast & INTRINSIC))  {
				Fast |= INTRINSIC;
				addtopl("Speed!\"");
			} else if (!(Stealth & INTRINSIC))  {
				Stealth |= INTRINSIC;
				addtopl("Stealth!\"");
			} else {
			    if (!(Protection & INTRINSIC))  {
				Protection |= INTRINSIC;
				if (!u.ublessed)  u.ublessed = rnd(3) + 1;
			    } else u.ublessed++;
			    addtopl("our protection!\"");
			}
			pline ("\"Use it wisely in my name!\"");
			break;

	    case 7:
#ifdef ELBERETH
			if (u.ualign > 3 && !u.uhand_of_elbereth) {
			    u.uhand_of_elbereth = TRUE;
			    HSee_invisible |= INTRINSIC;
			    HFire_resistance |= INTRINSIC;
			    HCold_resistance |= INTRINSIC;
			    HPoison_resistance |= INTRINSIC;
			    if (u.ualigntyp != U_CHAOTIC) {
			        pline("A voice booms out:  \"I crown thee...\"");
				pline("\"The Hand of Elbereth!\"");
#ifdef NAMED_ITEMS
				if(uwep && (uwep->otyp == LONG_SWORD)) {
					bless(uwep);
					uwep->bknown = 1;
					uwep->rustfree = 1;
					(void)oname(uwep, "Excalibur", 1);
				}
#endif
			    } else {
				register struct obj *obj;
				pline("A voice booms out:  \"Thou art chosen to steal souls for Arioch!\"");
				/* This does the same damage as Excalibur.
				 * Disadvantages: doesn't do bonuses to undead;
				 * doesn't aid searching.
				 * Advantages: part of that bonus is a level
				 * drain.
				 * Disadvantage: player cannot start with a
				 * +5 weapon and turn it into a Stormbringer.
				 * Advantage: they don't need to already have a
				 * sword of the right type to get it...
				 */
				if (Blind)
				    pline("Something appears at your %s.",
					makeplural(body_part(FOOT)));
				else
				    pline("A %s sword appears at your %s!",
					Hallucination ? hcolor() : "black",
					makeplural(body_part(FOOT)));
				obj = mksobj(BROADSWORD, FALSE);
				obj = oname(obj, "Stormbringer", 0);
				obj->rustfree = 1;
				obj->cursed = 0;
				obj->blessed = 1;
			/* Why bless it?  Why not.  After all, chaotic gods
			 * will bless regular weapons.  And blessed really
			 * means given sanctified to a deity, which is certainly
			 * sensible even for Stormbringer and a chaotic deity...
			 */
				obj->spe = 1;
				dropy(obj);
			    }
			    break;
			}
#endif

	    case 6:	pline ("An object appears at your %s!",
				makeplural(body_part(FOOT)));
#ifdef SPELLS
			(void) mkobj_at(SPBOOK_SYM, u.ux, u.uy);
#else
			(void) mkobj_at(SCROLL_SYM, u.ux, u.uy);
#endif
			break;

	    default:	impossible("Confused deity!");
			break;
	}
	u.ublesscnt = rnz(350);
#ifdef HARD
# ifndef ELBERETH
	u.ublesscnt += (u.udemigod * rnz(1000));
# else
	u.ublesscnt += ((u.udemigod + u.uhand_of_elbereth) * rnz(1000));
# endif
#endif
	return;
}

static void
gods_upset()
{
#ifdef HARD
	u.ugangr++;
	angrygods();
#else
	if (u.ugangr++)	angrygods();
	else {			/* exactly one warning */
#ifdef ALTARS
		pline("The voice of %s booms out:  \"Thou hast angered me.\"",
				a_gname());
#else
		pline("A voice booms out:  \"Thou hast angered me.\"");
#endif
		pline("\"Disturb me again at thine own risk!\"");
	}
#endif
}

#ifdef ENDGAME
static const char sacrifice_types[] = { FOOD_SYM, AMULET_SYM, 0 };
#endif

static void
consume_offering(otmp)
register struct obj *otmp;
{
	if (Hallucination)
    pline ("Your sacrifice sprouts wings and a propeller and roars away!");
	else pline ("Your sacrifice is consumed in a %s!",
		    u.ualigntyp == U_LAWFUL ? "flash of light" : "burst of flame");
	useup(otmp);
}

int
dosacrifice()
{
	register struct obj *otmp;
	int value = 0;

#ifdef ALTARS
	if (!on_altar()) {
		You("are not standing on an altar.");
		return 0;
	}
#endif /* ALTARS /**/

#ifdef ENDGAME
	if (dlevel == ENDLEVEL) {
		if (!(otmp = getobj(sacrifice_types, "sacrifice"))) return 0;
	} else
		if (!(otmp = floorfood("sacrifice", 0))) return 0;
#else
	if (!(otmp = floorfood("sacrifice", 0))) return 0;
#endif
/*
   Was based on nutritional value and aging behavior (< 50 moves).
   Sacrificing a food ration got you max luck instantly, making the
   gods as easy to please as an angry dog!

   Now only accepts corpses, based on their level (presumably, how hard
   they were to kill).  Human sacrifice, as well as sacrificing unicorns
   of your alignment, is strongly discouraged.  (We can't tell whether a dog
   corpse was tame, so you can still sacrifice it.)
 */

#define MAXVALUE 17	/* Highest corpse value */

	if (otmp->otyp == CORPSE) {
	    register struct permonst *mtmp = &mons[otmp->corpsenm];

	    if (otmp->corpsenm == PM_ACID_BLOB || (moves <= otmp->age + 50))
		value = mtmp->mlevel + 1; /* only fresh kills */

	    if (is_human(mtmp)) { /* Human sacrifice! */
#ifdef POLYSELF
		if (is_demon(uasmon))
		    You("find the idea very satisfying.");
		else
#endif
			if (u.ualigntyp != U_CHAOTIC)
		    pline("You'll regret this infamous offense!");
#ifdef ALTARS
		if (levl[u.ux][u.uy].altarmask & ~A_SHRINE) {
			/* curse the lawful/neutral altar */
			pline("The altar is stained with human blood.");
			levl[u.ux][u.uy].altarmask = A_CHAOS;
			angry_priest();
		} else {
			register struct monst *dmon;
			const char *color = Hallucination ? hcolor() : black;
    /* Human sacrifice on a chaotic altar is equivalent to demon summoning */
    pline("The blood floods over the altar, which vanishes in a%s %s cloud!",
				index(vowels, *color) ? "n" : "", color);
			levl[u.ux][u.uy].typ = ROOM;
			if(Invisible) newsym(u.ux, u.uy);
			if(dmon = makemon(&mons[dlord()], u.ux, u.uy)) {
				You("have summoned a demon lord!");
				if (u.ualigntyp == U_CHAOTIC) 
					dmon->mpeaceful = 1;
				You("are terrified, and unable to move.");
				nomul(-3);
			} else pline("The cloud dissipates.");
		}
#endif
		if (u.ualigntyp != U_CHAOTIC) {
			adjalign(-5);
			u.ugangr += 3;
			adjattrib(A_WIS, -1, TRUE);
			if (!Inhell) angrygods();
			change_luck(-5);
		} else adjalign(5);
		useup(otmp);
		return(1);
	    } else if (is_undead(mtmp)) { /* Not demons--no demon corpses */
		if (u.ualigntyp != U_CHAOTIC)
			value += 1;
	    } else if (mtmp->mlet == S_UNICORN) {
		int unicalign;

		if (mtmp == &mons[PM_BLACK_UNICORN]) unicalign = -1;
		else if (mtmp == &mons[PM_GREY_UNICORN]) unicalign = 0;
		else if (mtmp == &mons[PM_WHITE_UNICORN]) unicalign = 1;
		if (unicalign == u.ualigntyp) {
		    pline("Such an action is an insult to %s!", (unicalign== -1)
				? "chaos" : unicalign ? "law" : "neutrality");
		    adjattrib(A_WIS, -1, TRUE);
		    value = -5;
		} else if ((unicalign == -u.ualigntyp) ||
						(!u.ualigntyp && unicalign)) {
		    if (u.ualign < ALIGNLIM)
			You("feel stridently %s!", (u.ualigntyp== U_CHAOTIC) ?
			    "chaotic" : u.ualigntyp ? "lawful" : "neutral");
		    else You("feel you are thoroughly on the right path.");
		    u.ualign = ALIGNLIM;
		    value += 3;
		}
	    }
	}
#ifdef ENDGAME
	if (otmp->otyp == AMULET_OF_YENDOR) {
	    if (dlevel != ENDLEVEL) {
		if (otmp->spe == 0) {
		    if (Hallucination)
			You("feel homesick.");
		    else
			You("feel an urge to return to the surface.");
		    return (1);
		}
	    } else
		if (otmp->spe < 0) {  /* fake! */
		    if (flags.soundok)
			You("hear a nearby thunderclap.");
		    if (!otmp->known) {
			You("realize you have made a %s.",
			      Hallucination ? "boo-boo" : "mistake");
			otmp->known = 1;
			return (1);
		    } else {
			/* don't you dare try to fool the gods */
			change_luck(-3);
			u.ugangr += 3;
			value = -3;
		    }
		}
		else {
		    /* The final Test.	Did you win? */
		    if(uamul && uamul->otyp == otmp->otyp) Amulet_off();
		    useup(otmp);    /* well, it's gone now */
		    You("offer the Amulet to %s...", a_gname());
		    if (u.ualigntyp !=
			    (levl[u.ux][u.uy].altarmask & ~A_SHRINE) - 1) {
			/* And the opposing team picks him up and
			       carries him off on their shoulders */
		       pline("%s accepts your gift, and gains dominion over %s...",
				  a_gname(), u_gname());
			pline("%s is enraged...", u_gname());
			pline("Fortunately, %s permits you to live...", a_gname());
			pline("A cloud of %s smoke surrounds you...",
				Hallucination ? hcolor() : "orange");
			done("escaped");
		    } else {	    /* super big win */
	    pline("An invisible choir sings, and you are bathed in radiance...");
	    pline("\"Congratulations, mortal!  In return for thy service,");
			pline("\"I grant thee the gift of Immortality!\"");
			You("ascend to the status of Demigod...");
			done("ascended");
		    }
		}
	}
#endif /* ENDGAME */

#ifndef ALTARS	/* No altars in shops */
	if (otmp->unpaid && u.ualigntyp != U_CHAOTIC) {
	    You("realize sacrificing what is not yours is a very chaotic act.");
	    value = -3;
	}
#endif

	if (value == 0
#ifndef ALTARS
		       || Inhell
#endif
	   ) {
	    pline(nothing_happens);
	    return (1);
	}

	if (value < 0)  /* I don't think the gods are gonna like this... */
	    gods_upset();
	else {

	    int saved_anger = u.ugangr;
	    int saved_cnt = u.ublesscnt;
	    int saved_luck = u.uluck;
	    boolean consumed = FALSE;
#ifdef ALTARS
	    /* Sacrificing at an altar of a different alignment */
	    if (u.ualigntyp != (levl[u.ux][u.uy].altarmask & ~A_SHRINE) - 1) {
		/* Is this a conversion ? */
		if(ugod_is_angry()) {
		    if(u.ualignbase[0] == u.ualignbase[1]) {
			consume_offering(otmp);
			You("have a strong feeling that %s is angry...",
					u_gname());
			pline("%s accepts your allegiance.",a_gname());
			You("have a sudden sense of a new direction.");
			/* The player wears a helm of opposite alignment? */
			if (uarmh && uarmh->otyp == HELM_OF_OPPOSITE_ALIGNMENT)
			    u.ualignbase[0] =
				(levl[u.ux][u.uy].altarmask & ~A_SHRINE) - 1;
			else
			    u.ualigntyp = u.ualignbase[0] =
				(levl[u.ux][u.uy].altarmask & ~A_SHRINE) - 1;
			flags.botl = 1;
			/* Beware, Conversion is costly */
			change_luck(-3);
			u.ublesscnt += 300;
			adjalign((int)(u.ualignbase[1] * (ALIGNLIM / 2)));
		    } else {
			pline("%s rejects your sacrifice!",a_gname());
			pline("The voice of %s booms:  \"Suffer, infidel!\"",
					u_gname());
			adjalign(-5);
			u.ugangr += 3;
			adjattrib(A_WIS, -2, TRUE);
			if (!Inhell) angrygods();
			change_luck(-5);
		    }
		    return(1);
 		} else {
		    consume_offering(otmp);
		    consumed = TRUE;
		    You("sense a conflict between %s and %s.",
					u_gname(), a_gname());
		    if (rn2(8 + (int)u.ulevel) > 5) {
			You("feel the power of %s increase.",
					u_gname());
			change_luck(1);
			levl[u.ux][u.uy].altarmask &= A_SHRINE;
			/* the following accommodates stupid compilers */
			levl[u.ux][u.uy].altarmask =
				levl[u.ux][u.uy].altarmask | (u.ualigntyp + 1);
			if (!Blind)
			    pline("The newly consecrated altar glows %s.",
				Hallucination ? hcolor() :
				u.ualigntyp == U_LAWFUL ? white :
				u.ualigntyp ? black : "gray");
		    } else {
			pline("Unluckily, you feel the power of %s decrease.",
					u_gname());
			change_luck(-1);
		    }
		    return(1);
		}
	    }
#endif
	    if(!consumed) consume_offering(otmp);
	    /* OK, you get brownie points. */
	    if(u.ugangr) {
		u.ugangr -= ((value * (u.ualigntyp == U_CHAOTIC ? 2 : 3)) / MAXVALUE);
		if(u.ugangr < 0) u.ugangr = 0;
		if(u.ugangr != saved_anger) {
		    if (u.ugangr) {
			if(Hallucination)
			    pline("The gods seem %s.", hcolor());
			else pline("The gods seem slightly mollified.");

			if ((int)u.uluck < 0) change_luck(1);
		    } else {
			if (Hallucination)
			    pline("The gods seem cosmic (not a new fact).");
			else pline ("The gods seem mollified.");

			if ((int)u.uluck < 0) u.uluck = 0;
		    }
		} else {  /* not satisfied yet */
			if (Hallucination)
			    pline("The gods seem tall.");
			else You("have a feeling of inadequacy.");
		}
	    }
	    else if (u.ublesscnt > 0) {
		u.ublesscnt -= ((value * (u.ualigntyp == U_CHAOTIC ? 500 : 300))
				/ MAXVALUE);
		if(u.ublesscnt < 0) u.ublesscnt = 0;
		if(u.ublesscnt != saved_cnt) {

		    if (u.ublesscnt) {
			if (Hallucination)
		    You("realize that the gods are not like you and I.");
			else
		    	    You("have a hopeful feeling.");
			if ((int)u.uluck < 0) change_luck(1);
		    } else {
			if (Hallucination)
		    pline("Overall, there is a smell of fried onions.");
			else
			    You("have a feeling of reconciliation.");
			if ((int)u.uluck < 0) u.uluck = 0;
		    }
		}
	    } else {
		/* you were already in pretty good standing */
#if defined(ALTARS) && defined(NAMED_ITEMS)
		/* The player can gain an artifact */
		if(!rn2(10)) {
			otmp = mk_aligned_artifact((int)(levl[u.ux][u.uy].altarmask & ~A_SHRINE) - 1);
			if(otmp) {
			    dropy(otmp);
			    pline("An object appears at your %s!",
				  makeplural(body_part(FOOT)));
			    return(1);
			}
		}
#endif
		change_luck((value * LUCKMAX) / (MAXVALUE * 2));
		if (u.uluck != saved_luck) {
		    You(Hallucination ?
		"see crabgrass at your %s.  A funny thing in a dungeon." :
		"glimpse a four-leaf clover at your %s.",
			makeplural(body_part(FOOT)));
		}
	    }
	}
	return(1);
}

int
dopray() {		/* M. Stephenson (1.0.3b) */
	int trouble = in_trouble();
#ifdef ALTARS
   	int aligntyp =
		on_altar() ?
			(int)(levl[u.ux][u.uy].altarmask & ~A_SHRINE) - 1 :
			u.ualigntyp;
   	int align;

	if (u.ualigntyp && u.ualigntyp == -aligntyp) align = -u.ualign;
		/* Opposite alignment altar */
	else if (u.ualigntyp != aligntyp) align = u.ualign / 2;
		/* Different (but non-opposite) alignment altar */
	else align = u.ualign;
#else
  	int aligntyp = u.ualigntyp;
  	int align = u.ualign;
#endif

#ifdef POLYSELF
	if (is_undead(uasmon)) {
		if (aligntyp == 1 || (aligntyp == 0 && !rn2(10))) {
			pline(aligntyp == 1 ?
			      "\"Vile creature, thou durst call upon me?\"" :
			      "\"Walk no more, perversion of nature!\"");
			You("feel like you are falling apart.");
			rehumanize();
			losehp(rnd(20), "residual undead turning effect");
			return(1);
		}
	}
	if (is_demon(uasmon) && (aligntyp != -1)) {
	    pline("The very idea of praying to a %s god is repugnant to you.",
			u.ualigntyp ? "lawful" : "neutral");
	    return(0);
	}
#endif
	if (Inhell && u.ualigntyp != U_CHAOTIC) {
		pline("Since you are in hell, %s won't help you.",
#  ifdef ALTARS
			on_altar() ?  a_gname() :
#  endif
						u_gname());
		aggravate();
		return(0);
	}

#ifdef WIZARD
	if (wizard) {
		pline("Force the gods to be pleased? ");
		if (yn() == 'y') {
			u.ublesscnt = 0;
			if (u.uluck < 0) u.uluck = 0;
			u.ugangr = 0;
			if (align < 1) align = 1;
		}
	}
#endif
	if ((!trouble && (u.ublesscnt > 0)) ||
	    ((trouble < 0) && (u.ublesscnt > 100)) /* minor difficulties */ ||
	    ((trouble > 0) && (u.ublesscnt > 200)) /* big trouble */
	    ) {
		u.ublesscnt += rnz(250);
		change_luck(-3);
		gods_upset();
	} else if ((int)u.uluck < 0 || u.ugangr || align < 0)
		angrygods();			/* naughty */
	else	if (align >= 0) pleased();	/* nice */
	nomovemsg = "You finish your prayer.";
	nomul(-3);
	return(1);
}
#endif /* THEOLOGY */

int
doturn()
{	/* Knights & Priest(esse)s only please */

	register struct monst *mtmp;
	register int	xlev = 6;

	if((pl_character[0] != 'P') &&
	   (pl_character[0] != 'K')) {

		You("don't know how to turn undead!");
		return(0);
	}
#if defined(POLYSELF) || defined(THEOLOGY)
	if (
#  ifdef POLYSELF
		(u.ualigntyp != U_CHAOTIC && (is_demon(uasmon) || is_undead(uasmon)))
#  endif
#  if defined(POLYSELF) && defined(THEOLOGY)
		||
#  endif
#  ifdef THEOLOGY
		u.ugangr > 6 /* "Die, mortal!" */
#  endif
		) {

		pline("For some reason, the gods seem not to listen to you.");
		aggravate();
		return(0);
	}
#endif
	if (Inhell && u.ualigntyp != U_CHAOTIC) {
#ifdef THEOLOGY
		pline("Since you are in hell, %s won't help you.", u_gname());
#else
		pline("Since you are in hell, the gods won't help you.");
#endif
		aggravate();
		return(0);
	}
#ifdef THEOLOGY
	pline("Calling upon %s, you chant an arcane formula.", u_gname());
#else
	pline("Calling upon the gods, you chant an arcane formula.");
#endif
	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
	    if(cansee(mtmp->mx,mtmp->my)) {
		if(!mtmp->mtame && !mtmp->mpeaceful && (is_undead(mtmp->data) ||
		   (is_demon(mtmp->data) && (u.ulevel > (MAXULEV/2))))) {

		    if(Confusion) {
			pline("Unfortunately, your voice falters.");
			mtmp->mflee = mtmp->mfroz = mtmp->msleep = 0;
		    } else if (! resist(mtmp, '\0', 0, TELL))
			switch (mtmp->data->mlet) {
			    /* this is intentional, lichs are tougher
			       than zombies. */
			    case S_LICH:    xlev += 2;
			    case S_GHOST:   xlev += 2;
			    case S_VAMPIRE: xlev += 2;
			    case S_WRAITH:  xlev += 2;
			    case S_MUMMY:   xlev += 2;
			    case S_ZOMBIE:
				mtmp->mflee = 1; /* at least */
				if(u.ulevel >= xlev)  {
				    if(!resist(mtmp, '\0', 0, NOTELL)) {
					if(u.ualigntyp == U_CHAOTIC) {
					    mtmp->mpeaceful = 1; /* make them friendly */
					} else { /* damn them */
					    You("destroy %s!", mon_nam(mtmp));
					    mondied(mtmp);
					}
				    }
				}
				break;
			    default:    mtmp->mflee = 1;
					break;
			}
		   }
	    }
	    nomul(-5);
	    return(1);
}

#ifdef ALTARS
char *
a_gname()
{
	return(a_gname_at(u.ux, u.uy));
}

#ifdef __STDC__
char *
a_gname_at(xchar x,xchar y)     /* returns the name of an altar's deity */
#else
char *
a_gname_at(x,y)     /* returns the name of an altar's deity */
xchar x, y;
#endif
{
	register int align;

	if(!IS_ALTAR(levl[x][y].typ)) return((char *)0);

	align = levl[x][y].altarmask & ~A_SHRINE;
# ifdef THEOLOGY
       {struct ghods *aghod;

	for(aghod=gods; aghod->classlet; aghod++)
	    if(aghod->classlet == pl_character[0])
		switch(align) {
		case A_CHAOS:	return(aghod->chaos);
		case A_NEUTRAL:	return(aghod->balance);
		case A_LAW:	return(aghod->law);
		default: impossible("unknown altar alignment.");
			 return("Balance");
		}
	impossible("Altar to unknown character's god?");
	return("someone");
       }
# else
	switch(align) {
	case A_CHAOS:	return("Chaos");
	case A_NEUTRAL:	return("Balance");
	case A_LAW:	return("Law");
	default: impossible("unknown altar alignment.");
		 return("Balance");
	}
# endif /* THEOLOGY */
}

# ifdef THEOLOGY
void
altar_wrath(x, y)
register int x, y;
{
    	if(!strcmp(a_gname_at(x,y), u_gname())) {
	    pline("%s's voice booms:  \"How darest thou desecrate my altar!\"", 
					a_gname_at(x,y));
	    adjattrib(A_WIS, -1, FALSE);
	} else {
	    pline("A voice whispers in your ear:  \"Thou shalt pay, infidel!\"");
	    change_luck(-1);
	}
}
# endif /* THEOLOGY */
#endif /* ALTARS */

#ifdef THEOLOGY
char *
u_gname() {  /* returns the name of the player's deity */
	register struct ghods *aghod;

	for(aghod=gods; aghod->classlet; aghod++)
	    if(aghod->classlet == pl_character[0])
		switch(u.ualigntyp) {
		case  1: return(aghod->law);
		case  0: return(aghod->balance);
		case -1: return(aghod->chaos);
		default: impossible("unknown character alignment.");
			 return("Balance");
		}
	impossible("atheist player?");
	return("someone");
}
#endif /* THEOLOGY */
