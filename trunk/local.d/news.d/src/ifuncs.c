/*
 * ifuncs - functions used by inews.
 */

#ifndef lint
static char	*SccsId = "@(#)ifuncs.c	2.33	9/3/84";
#endif !lint

#include "iparams.h"
#include <errno.h>

/*
 * Transmit this article to all interested systems.
 */

#ifdef u370
static struct srec srec;
#endif

static struct hbuf h, hh;

extern log(char *fmt, ...);

broadcast()
{
	register char *nptr, *hptr;
	register FILE *fp;
#ifndef u370
	struct srec srec;
#endif
	char sentbuf[BUFLEN];
	int nsent = 0;

	/* h is a local copy of the header we can scribble on */
	fp = xfopen(ARTICLE, "r");
	if (hread(&h, fp, TRUE) == NULL)
		xerror("Cannot reread article");
	fclose(fp);

	strcpy(sentbuf, h.ident);
	strcat(sentbuf, " sent to ");
	nsent = 0;
	/* break path into list of systems. */
	hptr = nptr = h.path;
	while (*hptr != '\0') {
		if (index(NETCHRS, *hptr)) {
			*hptr++ = '\0';
			nptr = hptr;
		} else
			hptr++;
	}
	*nptr = '\0';

	/* loop once per system. */
	lock();
	s_openr();
	while (s_read(&srec)) {
		if (strncmp(srec.s_name, FULLSYSNAME, SNLN) == 0)
			continue;
		hptr = h.path;
		while (*hptr != '\0') {
			if (strncmp(srec.s_name, hptr, SNLN) == 0)
				goto contin;
			while (*hptr++ != '\0')
				;
		}
		if (ngmatch(h.nbuf, srec.s_nbuf)) {
			if (h.distribution[0] == '\0' ||
				ngmatch(h.distribution, srec.s_nbuf)) {
				if (transmit(&srec, xfopen(ARTICLE, "r"), 1)) {
					if (nsent)
						strcat(sentbuf, ", ");
					strcat(sentbuf, srec.s_name);
					nsent++;
				}
			}
		}
	contin:;
	}
	if (nsent)
		log(sentbuf);
	s_close();
	unlock();
}

/*
 * Transmit file to system.
 */
#define PROC 0004
transmit(sp, ifp, maynotify)
register struct srec *sp;
register FILE *ifp;
int maynotify;
{
	register FILE *ofp;
	register int c;
	register char *ptr;
	char TRANS[BUFLEN];
	char *argv[20];
	register int pid;
	extern char firstbufname[];

/* A:	afmt: the other machine runs an A news, so we xmit in A format */
	int afmt = (index(sp->s_flags, 'A') != NULL);
/* B:	use B format (this is the default - don't use this letter elsewise). */
/* F:	append name to file */
	int appfile = (index(sp->s_flags, 'F') != NULL);
/* L:	local: don't send the article unless it was generated locally */
	int local = (index(sp->s_flags, 'L') != NULL);
/* N:	notify: don't send the article, just tell him we have it */
	int notify = maynotify && (index(sp->s_flags, 'N') != NULL);
/* S:	noshell: don't fork a shell to execute the xmit command */
	int noshell = (index(sp->s_flags, 'S') != NULL);
/* U:	useexist: use the -c option to uux to use the existing copy */
	int useexist = (index(sp->s_flags, 'U') != NULL);

	if (local && mode == PROC) {
		fclose(ifp);
		return FALSE;
	}
#ifdef DEBUG
	printf("Transmitting to '%s'\n", sp->s_name);
#endif
	if (!appfile && !useexist) {
		if (hread(&hh, ifp, TRUE) == NULL) {
			logerr("Bad header, not transmitting %s re %s to %s",
				hh.ident, hh.title, sp->s_name);
			fclose(ifp);
			return FALSE;
		}
		if (hh.nbuf[0] == '\0') {
			fprintf(stderr, "Article not subscribed to by %s\n", sp->s_name);
			fclose(ifp);
			return FALSE;
		}
		sprintf(TRANS, "%s/trXXXXXX", SPOOL);
	}

	if (notify) {
		char oldid[50];
		sprintf(hh.title, "ihave %s %s", hh.ident, FULLSYSNAME);
		sprintf(hh.nbuf, "to.%s.ctl", sp->s_name);
		strcpy(oldid, hh.ident);
		getident(&hh);
		log("tell %s about %s, notif. id %s",
			sp->s_name, oldid, hh.ident);
	}

	if (appfile) {
		if (firstbufname[0] == '\0') {
			extern char histline[];
			localize("junk");
			savehist(histline);
			xerror("No file name to xmit from");
		}
#ifdef IHCC
		sprintf(TRANS, "%s/%s/%s", logdir(HOME), BATCHDIR, sp->s_xmit);
		ofp = fopen(TRANS, "a");
#else
		ofp = fopen(sp->s_xmit, "a");
#endif
		if (ofp == NULL)
			xerror("Cannot append to %s", sp->s_xmit);
		fprintf(ofp, "%s\n", firstbufname);
		fclose(ofp);
		fclose(ifp);
		return TRUE;
	}
	else
#ifdef UXMIT
	if (useexist) {
		if (firstbufname[0] == '\0')
			xerror("No file name to xmit from");
		if (*sp->s_xmit == '\0')
			sprintf(bfr, UXMIT, sp->s_name, firstbufname);
		else
			sprintf(bfr, sp->s_xmit, firstbufname);
		fclose(ifp);
	} else
#endif
	{
		ofp = xfopen(mktemp(TRANS), "w");
		if (afmt)
			ohwrite(&hh, ofp);
		else
			hwrite(&hh, ofp);
		if (!notify)
			while ((c = getc(ifp)) != EOF)
				putc(c, ofp);
		fclose(ifp);
		fclose(ofp);
		if (*sp->s_xmit == '\0')
			sprintf(bfr, DFTXMIT, sp->s_name, TRANS);
		else
			sprintf(bfr, "(%s) < %s", sp->s_xmit, TRANS);
	}

	/* At this point, the command to be executed is in bfr. */
	if (noshell) {
		if (pid = fork())
			fwait(pid);
		else {
			close(0);
			open(TRANS, 0);
			if (*sp->s_xmit == '\0')
				ptr = bfr;
			else
				ptr = sp->s_xmit;
			for (pid = 0; pid < 19; pid++) {
				while (isspace(*ptr))
					*ptr++ = 0;
				argv[pid] = ptr;
				while (!isspace(*++ptr) && *ptr)
					;
				if (!*ptr)
					break;
			}
			argv[++pid] = 0;
			execv(argv[0], argv);
			xerror("Can't execv %s", argv[0]);
		}
	} else
		system(bfr);
	if (!appfile && !useexist)
		unlink(TRANS);
	fclose(ifp);
	return TRUE;
}

typedef struct {
	char *dptr;
	int dsize;
} datum;

/*
 * Return TRUE if we have seen this file before, else FALSE.
 */
history(hp)
struct hbuf *hp;
{
#ifdef DBM
	datum lhs, rhs;
	datum fetch();
#else !DBM
	register FILE *hfp;
#endif !DBM
	register char *p;
	char lcident[BUFLEN];

#ifdef DEBUG
	fprintf(stderr,"history(%s)\n", hp->ident);
#endif
	/*
	 * Make the article ID case insensitive.
	 */
	strcpy(lcident, hp->ident);
	p = lcident;
	while (*++p)
		if (isupper(*p))
			*p = tolower(*p);

	idlock(lcident);
#ifdef DBM
	dbminit(ARTFILE);
	lhs.dptr = lcident;
	lhs.dsize = strlen(lhs.dptr) + 1;
	rhs = fetch(lhs);
	if (rhs.dptr)
		return(TRUE);
#else
	hfp = xfopen(ARTFILE, "r");
	while (fgets(bfr, BUFLEN, hfp) != NULL) {
		p = index(bfr, '\t');
		if (p == NULL)
			p = index(bfr, '\n');
		if (p != NULL)	/* can happen if nulls in file */
			*p = 0;
		p = bfr;
		while (*++p)
			if (isupper(*p))
				*p = tolower(*p);

		if (strcmp(bfr, lcident)==0 ||
				hp->oident[0] && strcmp(bfr, hp->oident)==0) {
			fclose(hfp);
			idunlock();
#ifdef DEBUG
			fprintf(stderr,"history returns true\n");
#endif
			return(TRUE);
		}
	}
	fclose(hfp);
#endif
	addhist(hp->ident);
	addhist("\t");
#ifdef DEBUG
	fprintf(stderr,"history returns false\n");
#endif
	return(FALSE);
}

char histline[256];	/* Assumed initially zero */

addhist(msg)
char *msg;
{
	strcat(histline, msg);
}

savehist(hline)
char *hline;
{
	register FILE *hfp;
	datum lhs, rhs;
	long fpos;
	register char *p;

	hfp = xfopen(ARTFILE, "a");
	fpos = ftell(hfp);
	fprintf(hfp, "%s\n", hline);
	fclose(hfp);
#ifdef DBM
	/* We assume that history has already been called, calling dbminit. */
	p = index(hline, '\t');
	if (p)
		*p = 0;
	p = hline;
	while(*++p)
		if(isupper(*p))
			*p = tolower(*p);
	lhs.dptr = hline;
	lhs.dsize = strlen(lhs.dptr) + 1;
	rhs.dptr = (char *) &fpos;
	rhs.dsize = sizeof fpos;
	store(lhs, rhs);
#endif
	hline[0] = 0;
	idunlock();
}

/*
 * Save partial news.
 */
newssave(fd, dummy)
FILE *fd;
char *dummy;
{
	register FILE *tofd, *fromfd;
	char sfname[BUFLEN];
	register int c;
	time_t tim;

#ifdef lint
	c = *dummy;
#endif lint

	if (fd == NULL)
		fromfd = xfopen(INFILE, "r");
	else
		fromfd = fd;
	umask(savmask);
	setgid(gid);
	setuid(uid);

	sprintf(sfname, "%s/%s", userhome, PARTIAL);
	if ((tofd = fopen(sfname, "a")) == NULL)
		xerror("Cannot save partial news");
	time(&tim);
	fprintf(tofd, "----- News saved at %s\n", arpadate(&tim));
	while ((c = getc(fromfd)) != EOF)
		putc(c, tofd);
	fclose(fromfd);
	fclose(tofd);
	printf("News saved in %s\n", sfname);
	xxit(0);
}

/*
 * Handle dates in header.
 */

dates(hp)
struct hbuf *hp;
{
	time_t edt;

	time(&hp->rectime);
	strcpy(hp->recdate, arpadate(&hp->rectime));
	nstrip(hp->recdate);
	if (*hp->subdate) {
		if (cgtdate(hp->subdate) < 0) {
			xerror("Cannot parse submittal date '%s'", hp->subdate);
		}
	} else
		strcpy(hp->subdate, hp->recdate);
	if (*hp->expdate) {
		if ((edt = cgtdate(hp->expdate)) < 0)
			xerror("Cannot parse expiration date '%s'",hp->expdate);
		nstrip(strcpy(hp->expdate, arpadate(&edt)));
	} else {
		defexp = TRUE;
		/*
		 * Default is now applied in expire.c
		hp->exptime = hp->rectime + DFLTEXP;
		nstrip(strcpy(hp->expdate, arpadate(&hp->exptime)));
		*/
	}
}

char lockname[80];
idlock(str)
char *str;
{
	register int i;
	char tempname[80];
	time_t now;
	struct stat sbuf;
#ifdef	VMS
	int fd;

	sprintf(lockname, "/tmp/%s.l.1", str);
	if ((fd = creat(lockname, 0444)) < 0) {
#else	!VMS
	sprintf(tempname, "/tmp/LTMP.%d", getpid());
	sprintf(lockname, "/tmp/L%s", str);
#ifdef FOURTEENMAX
	lockname[5 /* /tmp/ */ + 14] = '\0';
#endif
	close(creat(tempname, 0666));
	while (link(tempname, lockname)) {
#endif	!VMS
		(void) time(&now);
		i = stat(lockname, &sbuf);
		if (i < 0) {
			xerror("Directory permission problem in /tmp");
		}
		if (sbuf.st_mtime + 10*60 < now) {
			unlink(lockname);
			logerr("Article %s locked up", str);
			continue;
		}
		log("waiting on lock for %s", lockname);
		sleep((unsigned)60);
	}
#ifdef VMS
	close(fd);
#else
	unlink(tempname);
#endif
	unlink(tempname);
}

idunlock()
{
	unlink(lockname);
}

/*
 * Put a unique name into header.ident.
 */
getident(hp)
struct hbuf *hp;
{
	long seqn;
	register FILE *fp;

	lock();
	fp = xfopen(SEQFILE, "r");
	fgets(bfr, BUFLEN, fp);
	fclose(fp);
	seqn = atol(bfr) + 1;
#ifdef	VMS
	unlink(SEQFILE);
#endif	VMS
	fp = xfopen(SEQFILE, "w");
	fprintf(fp, "%ld\n", seqn);
	fclose(fp);
	unlock();
	sprintf(hp->ident, "<%ld@%s%s>", seqn, FULLSYSNAME, MYDOMAIN);
}

/*
 * Check that header.nbuf contains only valid newsgroup names;
 * exit with error if not valid.
 *
 */

ngfcheck(isproc)
int isproc;
{
	register char *s1, *s2;
	register FILE *f;
	register char *os1;
	char tbuf[BUFLEN], ngcheck[AFSIZ];

	f = xfopen(ACTIVE, "r");
	s1 = ngcheck;
	while (fgets(bfr, BUFLEN, f) != NULL) {
		os1 = s1;
		for(s2 = bfr; *s2 != '\0' && *s2 != ' ';) {
			if (s1 >= &ngcheck[AFSIZ-2])
				xerror("ACTIVE file too long");
			*s1++ = *s2++;
		}
		*s1++ = '\0';
		if (isproc) /* don't check to see if can post to this group */
			continue;
		while (*s2++ != '\0' && *s2 != ' ')
			;	/* skip max article number */
		while (*s2++ != '\0' && *s2 != ' ')
			;	/* skip min article number */
		if (*s2++ != '\0' && *s2 == 'n')
			s1 = os1;	/* can't post to this group */
	}
	*s1++ = '\0';
	*s1 = '\0';
	fclose(f);

	s1 = header.nbuf;
	s2 = nbuf;
	while (*s1 == NGDELIM || *s1 == ' ')
		s1++;	/* skip leading junk */
	do {
		/* there shouldn't be blanks, but give the jerk a break */
		if (*s1 == NGDELIM || *s1 == ' ') {
			*s2++ = '\0';
			while (*++s1 == NGDELIM || *s1 == ' ')
				;	/* remove extra commas */
		} else
			*s2++ = *s1++;
	} while (*s1 != '\0');
	*s2 = '\0';

	s1 = nbuf;
	while (*s1 != '\0') {	/* for each newsgroup in header */
		s2 = ngcheck;
		while (*s2 != '\0') { /* for each newsgroup in active file */
			if (strcmp(s1, s2) == 0)
				break;
			while (*s2++ != '\0')
				;
		}
		if (*s2 == '\0') {	/* not found. remove it */
			if (!isproc) {
				logerr("Invalid news group '%s'", s1);
				newssave(stdin, (char *)NULL);
			}
			/* See if it's in our alias list */
			f = xfopen(ALIASES,"r");
			while (fscanf(f,"%s %s", tbuf, bfr) == 2
				&& strcmp(s1, tbuf))
				;
			fclose(f);
			if (strcmp(s1, tbuf) == 0) {
				logerr("Aliased newsgroup '%s' to '%s'", s1, bfr);
				os1 = s1;
				s1 = nbuf;
				s2 = tbuf;
				while (s1 < os1) /* copy left part */
					*s2++ = *s1++;
				s1 = bfr;
				while (*s1 != '\0') /* copy alias */
					*s2++ = *s1++;
				*s2++ = '\0';
				s1 = os1;
				os1 = nbuf + (s2 - tbuf);
				while (*s1++ != '\0') /* skip old group */
					;
				/* copy right part */
				tbufcpy(s2, s1);
				/* copy back to original buffer */
				tbufcpy(nbuf, tbuf);
				s1 = os1;
			} else {
				logerr("Unknown newsgroup '%s' removed", s1);
				s2 = s1;
				while (*s2++ != '\0')	/* skip the bad one */
					;
				tbufcpy(s1, s2);
			}
		} else { /* It's in our active file */
			os1 = s1;
			while (*s1++ != '\0')
				;
			/* check for local only distribution on incoming
			   newsgroups. This might occur if someone posted to
			   general,net.unix */
			if(isproc && index(os1, '.') == NULL) {
				logerr("Local group '%s' removed", os1);
				tbufcpy(os1, s1);
				s1 = os1;
			}
		}
	}
	/*  remove any duplicates */
	os1 = s1 = nbuf;
	for(;;) {
		if (*s1++ == '\0') {
			if (*s1 == '\0')
				break;
			s2 = s1;
			while (*s2 != '\0') {
				if (strcmp(os1, s2) == 0) {
					logerr("Duplicate '%s' removed",os1);
					os1 = s2;
					while (*s2++ != '\0') /* skip it */
						;
					tbufcpy(os1, s2);
				} else
					while (*s2++ != '\0')
						;
			}
			os1 = s1;
			s1[-1] = '\0';
		}
	}
	if (nbuf[0] != '\0') {
		s1 = header.nbuf;
		s2 = nbuf;
		do {
			while (*s2 != '\0')
				*s1++ = *s2++;
			*s1++ = NGDELIM;
		} while (*++s2 != '\0');
		*s1 = '\0';
		return FALSE;
	}
	return TRUE;
}

tbufcpy(s1, s2)
register char *s1, *s2;
{
	do {
		while (*s2 != '\0')
			*s1++ = *s2++;
		*s1++ = '\0';
	} while (*++s2 != '\0');
	*s1 = '\0';
}


/*
 * Figure out who posted the article (which is locally entered).
 * The results are placed in the header structure hp.
 */
gensender(hp, logname)
struct hbuf *hp;
char *logname;
{
	char *fn;
	static char buf[100];
	char buf2[100];
	char *fullname(), *getenv();
	char *p;
	int fd;

	fn = getenv("NAME");

	if (fn == NULL) {
		sprintf(buf, "%s/%s", getenv("HOME"), ".name");
		fd = open(buf, 0);
		if (fd >= 0) {
			read(fd, buf2, sizeof buf2);
			close(fd);
			if (buf2[0] >= 'A')
				fn = buf2;
			for (p=fn; *p; p++)
				if (*p < ' ')
					*p = 0;
		}
	}

	if (fn == NULL)
		fn = fullname(logname);

	sprintf(hp->path, "%s", logname);
	sprintf(hp->from, "%s@%s%s (%s)", logname, FULLSYSNAME, MYDOMAIN, fn);
}

/*
 * Trap interrupts.
 */
onsig(n)
int n;
{
	static int numsigs = 0;
	/*
	 * Most UNIX systems reset caught signals to SIG_DFL.
	 * This bad design requires that the trap be set again here.
	 * Unfortunately, if the signal recurs before the trap is set,
	 * the program will die, possibly leaving the lock in place.
	 */
	if (++numsigs > 100) {
		logerr("inews ran away looping on signal %d", n);
		xxit(1);
	}
	signal(n, onsig);
	sigtrap = n;
}

#ifdef BATCH
/*
 * If the stdin begins with "#", we assume we have been fed a batched
 * shell script which looks like this:
 *	#! rnews 1234
 *	article with 1234 chars
 *	#! rnews 4321
 *	article with 4321 chars
 *
 * In this case we just exec the unbatcher and let it unpack and call us back.
 *
 * Note that there is a potential security hole here.  If the batcher is
 * /bin/sh, someone could ship you arbitrary stuff to run as shell commands.
 * The main protection you have is that the effective uid will be news, not
 * uucp and not the super user.  (That, plus the fact that BATCH is set to
 * "unbatch" as the system is distributed.)  If you want to run a batched link
 * and you are security concious, do not use /bin/sh as the unbatcher.
 * the thing to do is to change BATCH in your localize.sh file from /bin/sh
 * to some restricted shell which can only run rnews.
 */
checkbatch()
{
	int c;

	c = getc(stdin);
	if (c != EOF)
		ungetc(c, stdin);
	clearerr(stdin);
	if (c == '#') {
		char unbatcher[BUFLEN];

		sprintf(unbatcher, "%s/%s", LIB, BATCH);
		reset_stdin();
		execl(unbatcher, "news-unpack", (char *)0);
		xerror("Unable to exec shell to unpack news.");
	}
}

/*
 * We've already done a read on stdin, and we want to seek back to the
 * beginning.  We want the real file descriptor (beyond buffers) to
 * reflect the true beginning.  Do whatever is necessary.
 */
reset_stdin()
{
	register FILE *ofd;
	register int c;
	char *ofdname;
	long lseek();

	/* First try to seek back - if so, it's a cheap way back. */
	if (lseek(0, 0L, 0) == 0L)
		return;

	/* Can't seek, so have to copy input to a file and use that. */
	ofdname = "/tmp/inewsXXXXX";
	mktemp(ofdname);
	ofd = fopen(ofdname, "w");
	while ((c=getc(stdin)) != EOF)
		putc(c, ofd);
	fclose(stdin);
	fclose(ofd);

	/* Now for a few lower level hacks to reopen stdin and make
	 * absolutely sure that the right fd's are done for the exec.
	 */
	(void) close(0);		/* make sure stdin is really closed. */
	(void) open(ofdname, 0);	/* should return zero */
	(void) unlink(ofdname);		/* to avoid cleaning it up later. */
}
#endif BATCH

/*
 *	Exit and cleanup.
 */
xxit(status)
int status;
{
	unlink(INFILE);
	unlink(ARTICLE);
	while (lockcount > 0)
		unlock();
	idunlock();
	exit(status);
}

rwaccess(fname)
char *fname;
{
	int fd;

	fd = open(fname, 2);
	if (fd < 0)
		return 0;
	close(fd);
	return 1;
}

exists(fname)
char *fname;
{
	int fd;

	fd = open(fname, 0);
	if (fd < 0)
		return 0;
	close(fd);
	return 1;
}

int	lockcount = 0;			/* no. of times we've called lock */

#ifdef	VMS

#define	SUBLOCK	"/tmp/netnews.lck.1"

/*
 * Newsystem locking.
 * These routines are different for VMS because we can not
 * effectively simulate links, and VMS supports multiple
 * version numbers of files
 */
lock()
{
	register int i;
	register int fd;

	if (lockcount++ == 0) {
		i = DEADTIME;
		while ((fd = creat(SUBLOCK, 0444)) < 0) {
			if (--i < 0) {
				unlink(SUBLOCK);
				fprintf(stderr,"%s: %s\n", Progname, msg);
				logerr("News system locked up");
			}
			if (i < -3)
				xerror("Unable to unlock news system");
			sleep((unsigned)1);
		}
		close(fd);
	}
}

unlock()
{
	if (--lockcount == 0)
		unlink(SUBLOCK);
}

#else	!VMS

/*
 * Newsystem locking.
 */

lock()
{
	register int i;
	extern int errno;

	if (lockcount++ == 0) {
		i = DEADTIME;
		while (link(SUBFILE, LOCKFILE)) {
			if (errno != EEXIST)
				break;
			if (--i < 0)
				xerror("News system locked up");
			sleep((unsigned)1);
		}
	}
}

unlock()
{
	if (--lockcount == 0)
		unlink(LOCKFILE);
}
#endif	VMS

/*
 * Generate the name of the person responsible for posting this article,
 * in order to check that two articles were posted by the same person.
 */
char *
senderof(hp)
struct hbuf *hp;
{
	char *q, *tp;
	char *tailpath();

	if (hp->sender[0])
		tp = hp->sender;
	else if (hp->from[0])
		tp = hp->from;
	else
		tp = tailpath(hp);

	/* Remove full name */
	q = index(tp, ' ');
	if (q)
		*q = '\0';

	q = malloc((unsigned)(strlen(tp) + 1));
	strcpy(q, tp);
	return q;
}
