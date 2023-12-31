/*	SCCS Id: @(#)makemon.c	1.4	87/08/08
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* makemon.c - version 1.0.2 */

#include	"hack.h"
extern char fut_geno[];
extern char *index();
extern struct obj *mkobj_at(), *mksobj(), *mkobj();
struct monst zeromonst;
extern boolean in_mklev;

#ifdef HARD		/* used in hell for bigger, badder demons! */

struct permonst d_lord   = { "demon lord",	'&',12,13,-5,50,1,5,0 },
		d_prince = { "demon prince",	'&',14,14,-6,70,1,6,0 };
#endif

/*
 * called with [x,y] = coordinates;
 *	[0,0] means anyplace
 *	[u.ux,u.uy] means: call mnexto (if !in_mklev)
 *
 *	In case we make an Orc or killer bee, we make an entire horde
 *	(swarm); note that in this case we return only one of them
 *	(the one at [x,y]).
 */
struct monst *
makemon(ptr,x,y)
register struct permonst *ptr;
{
	register struct monst *mtmp;
	register deep, nleft;
	boolean anything = (!ptr);

	if(x != 0 || y != 0) if(m_at(x,y)) return((struct monst *) 0);
	if(ptr){
		/* If this monster is genocided, just return */
		if(index(fut_geno, ptr->mlet)) return((struct monst *) 0);
	} else {
		/* Make a random (common) monster */
		nleft = CMNUM - strlen(fut_geno);
		if(index(fut_geno, 'm')) nleft++;  /* make only 1 minotaur */
		if(index(fut_geno, '@')) nleft++;
		if(nleft <= 0) return(0); 	  /* no more monsters! */
		/* determine the strongest monster to make */
#ifdef ROCKMOLE
		deep = rn2(nleft*dlevel/24 + 6);
#else
		deep = rn2(nleft*dlevel/24 + 7);
#endif
		if(deep < dlevel - 4) deep = rn2(nleft*dlevel/24 + 12);
		/* fudge deep into the upper half of the valid range
		   (if needed) */
		if(deep >= nleft) deep = rn1(nleft - nleft/2, nleft/2);
		for(nleft = 0; nleft < CMNUM; nleft++){
			ptr = &mons[nleft];
			if(index(fut_geno, ptr->mlet)) continue;
#ifdef KOPS
			if(ptr->mlet == 'K') {
				deep--;
				continue;
			}
#endif
			if(--deep <= 0) goto gotmon;
		}
		/* This can happen if you are deep in the dungeon and
		   "weak" monsters have been genocided. */
		return((struct monst *)0);
	}
gotmon:
	mtmp = newmonst(ptr->pxlth);
	*mtmp = zeromonst;	/* clear all entries in structure */
	for(nleft = 0; nleft < ptr->pxlth; nleft++)
		((char *) &(mtmp->mextra[0]))[nleft] = 0;
	mtmp->nmon = fmon;
	fmon = mtmp;
	mtmp->m_id = flags.ident++;
	mtmp->data = ptr;
	mtmp->mxlth = ptr->pxlth;
	if(ptr->mlet == 'D') mtmp->mhpmax = mtmp->mhp = 80;
	else if(!ptr->mlevel) mtmp->mhpmax = mtmp->mhp = rnd(4);
	else mtmp->mhpmax = mtmp->mhp = d(ptr->mlevel, 8);
	mtmp->mx = x;
	mtmp->my = y;
	mtmp->mcansee = 1;
	if(ptr->mlet == 'M'){
		mtmp->mimic = 1;
		mtmp->mappearance = ']';
	}
	if(!in_mklev) {
		if(x == u.ux && y == u.uy && ptr->mlet != ' ')
			mnexto(mtmp);
		if(x == 0 && y == 0)
			rloc(mtmp);
	}
	if(ptr->mlet == 's' || ptr->mlet == 'S') {
		mtmp->mhide = mtmp->mundetected = 1;
		if(in_mklev)
		if(mtmp->mx && mtmp->my)
			(void) mkobj_at(0, mtmp->mx, mtmp->my);
	}
	if(ptr->mlet == ':') {
#ifdef DGKMOD
		/* If you're protected with a ring, don't create
		 * any shape-changing chameleons -dgk
		 */
		if (Protection_from_shape_changers)
			mtmp->cham = 0;
		else {
			mtmp->cham = 1;
			(void) newcham(mtmp,
				&mons[dlevel+14+rn2(CMNUM-14-dlevel)]);
		}
#else
		mtmp->cham = 1;
		(void) newcham(mtmp, &mons[dlevel+14+rn2(CMNUM-14-dlevel)]);
#endif
	}
	if(ptr->mlet == 'I' || ptr->mlet == ';')
		mtmp->minvis = 1;
	if(ptr->mlet == 'L' || ptr->mlet == 'N'
	    || (in_mklev && index("&w;", ptr->mlet) && rn2(5))
	) mtmp->msleep = 1;
#ifdef HARD
	if(ptr->mlet == '&' && (Inhell || u.udemigod)) {

		if(!rn2(5 + !Inhell)) {
		    if (rn2(3 + Inhell)) mtmp->data = &d_lord;
		    else  {
				mtmp->data = &d_prince;
				mtmp->mpeaceful = 1;
				mtmp->minvis = 1;
		    }
		}
	}
#endif /* HARD /**/
#ifndef NOWORM
	if(ptr->mlet == 'w' && getwn(mtmp))  initworm(mtmp);
#endif

	if(anything) if(ptr->mlet == 'O' || ptr->mlet == 'k') {
		coord mm;
		register int cnt = rnd(10);
		mm.x = x;
		mm.y = y;
		while(cnt--) {
			enexto(&mm, mm.x, mm.y);
			(void) makemon(ptr, mm.x, mm.y);
		}
	}
#ifdef DGKMOD
	m_initinv(mtmp);
#endif
	return(mtmp);
}

#ifdef DGKMOD
/* Give some monsters an initial inventory to use */
m_initinv(mtmp)
struct monst *mtmp;
{
	struct obj *otmp;

	switch (mtmp->data->mlet) {
# ifdef KAA
	case '9':
		if (rn2(2)) {
			otmp = mksobj(ENORMOUS_ROCK);
			mpickobj(mtmp, otmp);
		}
# endif
# ifdef KOPS
	case 'K':		/* create Keystone Kops with cream pies to
				 * throw. As suggested by KAA.	   [MRS]
				 */
		if (!rn2(4)) {
			otmp = mksobj(CREAM_PIE);
			otmp->quan = 2 + rnd(2);
			mpickobj(mtmp, otmp);
		}
		break;
	case 'O':
# else
	case 'K':
# endif
		if (!rn2(4)) {
			otmp = mksobj(DART);
			otmp->quan = 2 + rnd(12);
			mpickobj(mtmp, otmp);
		}
		break;
	case 'C':
		if (rn2(2)) {
			otmp = mksobj(CROSSBOW);
			otmp->cursed = rn2(2);
			mpickobj(mtmp, otmp);
			otmp = mksobj(CROSSBOW_BOLT);
			otmp->quan = 2 + rnd(12);
			mpickobj(mtmp, otmp);
		}
		break;
	default:
		break;
	}
}
#endif

enexto(cc, xx,yy)
coord	*cc;
register xchar xx,yy;
{
	register xchar x,y;
	coord foo[15], *tfoo;
	int range;

	tfoo = foo;
	range = 1;
	do {	/* full kludge action. */
		for(x = xx-range; x <= xx+range; x++)
			if(goodpos(x, yy-range)) {
				tfoo->x = x;
				(tfoo++)->y = yy-range;
				if(tfoo == &foo[15]) goto foofull;
			}
		for(x = xx-range; x <= xx+range; x++)
			if(goodpos(x,yy+range)) {
				tfoo->x = x;
				(tfoo++)->y = yy+range;
				if(tfoo == &foo[15]) goto foofull;
			}
		for(y = yy+1-range; y < yy+range; y++)
			if(goodpos(xx-range,y)) {
				tfoo->x = xx-range;
				(tfoo++)->y = y;
				if(tfoo == &foo[15]) goto foofull;
			}
		for(y = yy+1-range; y < yy+range; y++)
			if(goodpos(xx+range,y)) {
				tfoo->x = xx+range;
				(tfoo++)->y = y;
				if(tfoo == &foo[15]) goto foofull;
			}
		range++;
	} while(tfoo == foo);
foofull:
	range = rn2(tfoo-foo);
	cc->x = foo[range].x;
	cc->y = foo[range].y;
	return(0);
}

goodpos(x,y)	/* used only in mnexto and rloc */
{
	return(
	! (x < 1 || x > COLNO-2 || y < 1 || y > ROWNO-2 ||
	   m_at(x,y) || !ACCESSIBLE(levl[x][y].typ)
	   || (x == u.ux && y == u.uy)
	   || sobj_at(ENORMOUS_ROCK, x, y)
	));
}

rloc(mtmp)
struct monst *mtmp;
{
	register tx,ty;
	register char ch = mtmp->data->mlet;

#ifndef NOWORM
	if(ch == 'w' && mtmp->mx) return;	/* do not relocate worms */
#endif
	do {
		tx = rn1(COLNO-3,2);
		ty = rn2(ROWNO);
	} while(!goodpos(tx,ty));
	mtmp->mx = tx;
	mtmp->my = ty;
	if(u.ustuck == mtmp){
		if(u.uswallow) {
			u.ux = tx;
			u.uy = ty;
			docrt();
		} else	u.ustuck = 0;
	}
	pmon(mtmp);
}

struct monst *
mkmon_at(let,x,y)
char let;
register int x,y;
{
	register int ct;
	register struct permonst *ptr;

	for(ct = 0; ct < CMNUM; ct++) {
		ptr = &mons[ct];
		if(ptr->mlet == let)
			return(makemon(ptr,x,y));
	}
	return(0);
}
