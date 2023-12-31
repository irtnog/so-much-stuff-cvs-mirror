/*	SCCS Id: @(#)hack.h	3.0	88/07/19
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef HACK_H
#define HACK_H

#ifndef CONFIG_H
#include "config.h"
#endif

#define	TELL		1
#define NOTELL		0
#define ON 		1
#define OFF 		0
#define	BOLT_LIM    8	/* from this distance ranged attacks will be made */
#ifdef HARD
#define	MAX_CARR_CAP	120	/* so that boulders can be heavier */
#else
#define	MAX_CARR_CAP	500
#endif
#define	FAR	(COLNO+2)	/* position outside screen */
#ifdef NULL
#undef NULL
#endif /* NULL */
#define NULL  ((genericptr_t)0)
#define DUMMY { 0 }

#ifndef DECL_H
#include "decl.h"
#endif

#ifndef MONSYM_H
#include 	"monsym.h"
#endif
#ifndef MKROOM_H
#include	"mkroom.h"
#endif
#ifndef OBJCLASS_H
#include	"objclass.h"
#endif

extern coord bhitpos;	/* place where thrown weapon falls to the ground */

#ifndef GOLD_H
#include	"gold.h"
#endif
#ifndef TRAP_H
#include	"trap.h"
#endif
#ifndef FLAG_H
#include	"flag.h"
#endif

#ifndef RM_H
#include	"rm.h"
#endif

#ifdef STRONGHOLD
# define Inhell		(dlevel > stronghold_level && dlevel <= MAXLEVEL)
#else
# define Inhell 	(dlevel >= HELLLEVEL)
#endif

#ifdef SPELLS
#define	NO_SPELL	0
#endif

/*** some utility macros ***/
# ifndef STUPID_CPP	/* otherwise these macros are functions in hack.c */
#define yn() yn_function(ynchars, 'n')
#define ynq() yn_function(ynqchars, 'q')
#define ynaq() yn_function(ynaqchars, 'y')
#define nyaq() yn_function(nyaqchars, 'n')

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(x,y) ((x) < (y) ? (x) : (y))
#define	plur(x)	(((x) == 1) ? "" : "s")

#define	makeknown(x)	objects[x].oc_name_known = 1
# endif /* STUPID_CPP */

#ifdef MSDOS
#define getuid() 1
#define getlogin() ((char *) NULL)
#endif /* MSDOS */

#endif /* HACK_H /**/
