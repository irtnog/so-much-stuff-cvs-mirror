/*	SCCS Id: @(#)demon.c	3.0	88/11/14
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include	"hack.h"

void
dsummon(ptr)		/* summon demon */
	register struct permonst *ptr;
{
	register int dtype, cnt = 0;

	if(is_dprince(ptr) || (ptr == &mons[PM_WIZARD_OF_YENDOR])) {

	    dtype = (!rn2(20)) ? dprince() : (!rn2(4)) ? dlord() : ndemon();
	    cnt = (!rn2(4) && !is_dprince(&mons[dtype])) ? 2 : 1;

	} else if(is_dlord(ptr)) {

	    dtype = (!rn2(50)) ? dprince() : (!rn2(20)) ? dlord() : ndemon();
	    cnt = (!rn2(4) && is_ndemon(&mons[dtype])) ? 2 : 1;

	} else if(is_ndemon(ptr)) {

	    dtype = (!rn2(20)) ? dlord() : ndemon();
	    cnt = 1;
	}

	if(!dtype) return;

	while(cnt > 0) {

	    (void)makemon(&mons[dtype], u.ux, u.uy);
	    cnt--;
	}
	return;
}

#ifdef HARD
#define	Athome	(Inhell && !mtmp->cham)

int
demon_talk(mtmp)		/* returns 1 if he won't attack. */
register struct monst *mtmp;
{
	char	*x_monnam(), *Xmonnam();
	long	demand, offer;

#ifdef NAMED_ITEMS
	if(uwep && is_artifact(uwep) && !strcmp(ONAME(uwep), "Excalibur")) {

	    pline("%s looks very angry.", Xmonnam(mtmp));
	    return mtmp->mpeaceful = mtmp->mtame = 0;
	}
#endif /* NAMED_ITEMS */

	if(is_ndemon(mtmp->data)) {  /* not for regular '&'s */

	    pline("%s mutters something about awful working conditions.",
		  Xmonnam(mtmp));
	    return(0);
	}

	/* Slight advantage given. */
	if(is_dprince(mtmp->data) && mtmp->minvis) {
	    mtmp->minvis = 0;
	    if (!Blind) pline("%s appears before you.", Xmonnam(mtmp));
	    pmon(mtmp);
	}
	if(u.usym == S_DEMON) {	/* Won't blackmail their own. */

	    pline("%s says, \"Good hunting %s.\" and vanishes.",
		  Xmonnam(mtmp), flags.female ? "Sister" : "Brother");
	    rloc(mtmp);
	    return(1);
	}
	demand = (u.ugold * (rnd(80) + 20 * Athome)) / 100;
	if(!demand)  		/* you have no gold */
	    return mtmp->mpeaceful = 0;
	else {

	    pline("%s demands %ld Zorkmids for safe passage.",
		  Xmonnam(mtmp), demand);

	    if((offer = bribe(mtmp)) >= demand) {
		pline("%s vanishes laughing about cowardly mortals.",
		      Xmonnam(mtmp));
	    } else {
		if((long)rnd(40) > (demand - offer)) {
		    pline("%s scowls at you menacingly, then vanishes.",
			  Xmonnam(mtmp));
		} else {
		    pline("%s gets angry...", Xmonnam(mtmp));
		    return mtmp->mpeaceful = 0;
		}
	    }
	}
	mongone(mtmp);
	return(1);
}
#endif

#if defined(HARD) || (defined(ALTARS) && defined(SOUNDS))
long
bribe(mtmp)

	struct monst *mtmp;
{
	char buf[80];
	long offer;

	pline("How much will you offer? ");
	getlin(buf);
	(void) sscanf(buf, "%ld", &offer);

/*Michael Paddon -- fix for negative offer to monster*/	/*JAR880815 - */
 	if(offer < 0L) {
 		You("try to shortchange %s, but fumble.", 
 			x_monnam(mtmp, 0));
 		offer = 0L;
 	} else if(offer >= u.ugold) {
		You("give %s all your gold.", x_monnam(mtmp, 0));
		offer = u.ugold;
	} else You("give %s %ld Zorkmids.", x_monnam(mtmp, 0), offer);

	u.ugold -= offer;
	return(offer);
}
#endif

int
dprince() {
#ifdef HARD
	int	tryct;
	struct	permonst *ptr;

	for(tryct = 0; tryct < 20; tryct++)
	    if(is_dprince((ptr = mkclass(S_DEMON))))
		return(monsndx(ptr));

#endif
	return(dlord());
}

int
dlord() {
#ifdef HARD
	int	tryct;
	struct	permonst *ptr;

	for(tryct = 0; tryct < 20; tryct++)
	    if(is_dlord((ptr = mkclass(S_DEMON))))
		return(monsndx(ptr));

#endif
	return(ndemon());
}

int
ndemon() {
#ifndef HARD
	return(PM_DEMON);
#else
	int	tryct;
	struct	permonst *ptr;

	for(tryct = 0; tryct < 20; tryct++)
	    if(is_ndemon((ptr = mkclass(S_DEMON))))
		return(monsndx(ptr));

	return(0);
#endif
}
