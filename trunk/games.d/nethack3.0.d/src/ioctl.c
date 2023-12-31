/*	SCCS Id: @(#)ioctl.c	2.0	87/09/18
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* This cannot be part of hack.tty.c (as it was earlier) since on some
   systems (e.g. MUNIX) the include files <termio.h> and <sgtty.h>
   define the same constants, and the C preprocessor complains. */

/* block some unused #defines to avoid overloading some cpp's */
#define MONATTK_H
#include "hack.h"

#if defined(BSD) || defined(ULTRIX)
#include	<sgtty.h>
struct ltchars ltchars, ltchars0;
#else
#include	<termio.h>	/* also includes part of <sgtty.h> */
struct termio termio;
#endif

void
getioctls() {
#if defined(BSD) || defined(ULTRIX)
	(void) ioctl(fileno(stdin), (int) TIOCGLTC, (char *) &ltchars);
	(void) ioctl(fileno(stdin), (int) TIOCSLTC, (char *) &ltchars0);
#else
	(void) ioctl(fileno(stdin), (int) TCGETA, &termio);
#endif
}

void
setioctls() {
#if defined(BSD) || defined(ULTRIX)
	(void) ioctl(fileno(stdin), (int) TIOCSLTC, (char *) &ltchars);
#else
	/* Now modified to run under Sys V R3.	- may have to be #ifdef'ed */
	(void) ioctl(fileno(stdin), (int) TCSETAW, &termio);
#endif
}

#ifdef SUSPEND		/* implies BSD */
int
dosuspend() {
#include	<signal.h>
#ifdef SIGTSTP
	if(signal(SIGTSTP, SIG_IGN) == SIG_DFL) {
		settty(NULL);
		(void) signal(SIGTSTP, SIG_DFL);
		(void) kill(0, SIGTSTP);
		gettty();
		setftty();
		docrt();
	} else {
		pline("I don't think your shell has job control.");
	}
#else
	pline("Sorry, it seems we have no SIGTSTP here.  Try ! or S.");
#endif
	return(0);
}
#endif /* SUSPEND /**/
