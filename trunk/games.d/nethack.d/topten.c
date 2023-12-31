/*	SCCS Id: @(#)topten.c	1.4	87/08/08
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* topten.c - version 1.0 */

#include <stdio.h>
#include "hack.h"
#ifdef GENIX
#define	void	int	/* jhn - mod to prevent compiler from bombing */
#endif

#define	Sprintf	(void) sprintf
extern char plname[], pl_character[];
extern char *itoa(), *ordin(), *eos();
extern int done_hup, done_stopprint;

#define newttentry() (struct toptenentry *) alloc(sizeof(struct toptenentry))
#define	NAMSZ	10
#define	DTHSZ	40
#define	PERSMAX	 3
#define	POINTSMIN	1	/* must be > 0 */
#define	ENTRYMAX	100	/* must be >= 10 */
#ifndef MSDOS
#define	PERS_IS_UID		/* delete for PERSMAX per name; now per uid */
#endif
struct toptenentry {
	struct toptenentry *tt_next;
	long int points;
	int level,maxlvl,hp,maxhp;
	int uid;
	char plchar;
	char sex;
	char name[NAMSZ+1];
	char death[DTHSZ+1];
	char date[7];		/* yymmdd */
} *tt_head;

topten(){
	int uid = getuid();
	int rank, rank0 = -1, rank1 = 0;
	int occ_cnt = PERSMAX;
	register struct toptenentry *t0, *t1, *tprev;
	char *recfile = RECORD;
#ifdef UNIX
	char *reclock = "record_lock";
#endif
	int sleepct = 300;
	FILE *rfile;
	register flg = 0;
	extern char *get_date();
#ifndef DGK
#define	HUP	if(!done_hup)
#else
#define HUP
#endif
#ifdef UNIX
	while(link(recfile, reclock) == -1) {
		HUP perror(reclock);
		if(!sleepct--) {
			HUP puts("I give up. Sorry.");
			HUP puts("Perhaps there is an old record_lock around?");
			return;
		}
		HUP printf("Waiting for access to record file. (%d)\n",
			sleepct);
		HUP (void) fflush(stdout);
		sleep(1);
	}
#endif
	if(!(rfile = fopen(recfile,"r"))){
		HUP puts("Cannot open record file!");
		goto unlock;
	}
	HUP (void) putchar('\n');

	/* create a new 'topten' entry */
	t0 = newttentry();
	t0->level = dlevel;
	t0->maxlvl = maxdlevel;
	t0->hp = u.uhp;
	t0->maxhp = u.uhpmax;
	t0->points = u.urexp;
	t0->plchar = pl_character[0];
	t0->sex = (flags.female ? 'F' : 'M');
	t0->uid = uid;
	(void) strncpy(t0->name, plname, NAMSZ);
	(t0->name)[NAMSZ] = 0;
	(void) strncpy(t0->death, killer, DTHSZ);
	(t0->death)[DTHSZ] = 0;
	(void) strcpy(t0->date, get_date());

	/* assure minimum number of points */
	if(t0->points < POINTSMIN)
		t0->points = 0;

	t1 = tt_head = newttentry();
	tprev = 0;
	/* rank0: -1 undefined, 0 not_on_list, n n_th on list */
	for(rank = 1; ; ) {
	  if(fscanf(rfile, "%6s %d %d %d %d %d %ld %c%c %[^,],%[^\n]",
		t1->date, &t1->uid,
		&t1->level, &t1->maxlvl,
		&t1->hp, &t1->maxhp, &t1->points,
		&t1->plchar, &t1->sex, t1->name, t1->death) != 11
	  || t1->points < POINTSMIN)
			t1->points = 0;
	  if(rank0 < 0 && t1->points < t0->points) {
		rank0 = rank++;
		if(tprev == 0)
			tt_head = t0;
		else
			tprev->tt_next = t0;
		t0->tt_next = t1;
		occ_cnt--;
		flg++;		/* ask for a rewrite */
	  } else
		tprev = t1;
	  if(t1->points == 0) break;
	  if(
#ifdef PERS_IS_UID
	     t1->uid == t0->uid &&
#else
	     strncmp(t1->name, t0->name, NAMSZ) == 0 &&
#endif
	     t1->plchar == t0->plchar && --occ_cnt <= 0){
		if(rank0 < 0){
			rank0 = 0;
			rank1 = rank;
	HUP printf("You didn't beat your previous score of %ld points.\n\n",
				t1->points);
		}
		if(occ_cnt < 0){
			flg++;
			continue;
		}
	  }
	  if(rank <= ENTRYMAX){
		t1 = t1->tt_next = newttentry();
		rank++;
	  }
	  if(rank > ENTRYMAX){
		t1->points = 0;
		break;
	  }
	}
	if(flg) {	/* rewrite record file */
		(void) fclose(rfile);
		if(!(rfile = fopen(recfile,"w"))){
			HUP puts("Cannot write record file\n");
			goto unlock;
		}

		if(!done_stopprint) if(rank0 > 0){
		    if(rank0 <= 10)
			puts("You made the top ten list!\n");
		    else
		printf("You reached the %d%s place on the top %d list.\n\n",
			rank0, ordin(rank0), ENTRYMAX);
		}
	}
	if(rank0 == 0) rank0 = rank1;
	if(rank0 <= 0) rank0 = rank;
	if(!done_stopprint) outheader();
	t1 = tt_head;
	for(rank = 1; t1->points != 0; rank++, t1 = t1->tt_next) {
	  if(flg) fprintf(rfile,"%6s %d %d %d %d %d %ld %c%c %s,%s\n",
	    t1->date, t1->uid,
	    t1->level, t1->maxlvl,
	    t1->hp, t1->maxhp, t1->points,
	    t1->plchar, t1->sex, t1->name, t1->death);
	  if(done_stopprint) continue;
	  if(rank > flags.end_top &&
	    (rank < rank0-flags.end_around || rank > rank0+flags.end_around)
	    && (!flags.end_own ||
#ifdef PERS_IS_UID
				  t1->uid != t0->uid ))
#else
				  strncmp(t1->name, t0->name, NAMSZ)))
#endif
		continue;
	  if(rank == rank0-flags.end_around &&
	     rank0 > flags.end_top+flags.end_around+1 &&
	     !flags.end_own)
		(void) putchar('\n');
	  if(rank != rank0)
		(void) outentry(rank, t1, 0);
	  else if(!rank1)
		(void) outentry(rank, t1, 1);
	  else {
		int t0lth = outentry(0, t0, -1);
		int t1lth = outentry(rank, t1, t0lth);
		if(t1lth > t0lth) t0lth = t1lth;
		(void) outentry(0, t0, t0lth);
	  }
	}
	if(rank0 >= rank) if(!done_stopprint)
		(void) outentry(0, t0, 1);
	(void) fclose(rfile);
unlock:	;
#ifdef UNIX
	(void) unlink(reclock);
#endif
}

outheader() {
char linebuf[BUFSZ];
register char *bp;
	(void) strcpy(linebuf, "Number Points  Name");
	bp = eos(linebuf);
	while(bp < linebuf + COLNO - 9) *bp++ = ' ';
	(void) strcpy(bp, "Hp [max]");
	puts(linebuf);
}

/* so>0: standout line; so=0: ordinary line; so<0: no output, return lth */
int
outentry(rank,t1,so) register struct toptenentry *t1; {
boolean quit = FALSE, killed = FALSE, starv = FALSE;
char linebuf[BUFSZ];
	linebuf[0] = 0;
	if(rank) Sprintf(eos(linebuf), "%3d", rank);
		else Sprintf(eos(linebuf), "   ");
#ifdef DGKMOD
	Sprintf(eos(linebuf), " %6ld %10s", t1->points, t1->name);
#else
	Sprintf(eos(linebuf), " %6ld %8s", t1->points, t1->name);
#endif
	if(t1->plchar == 'X') Sprintf(eos(linebuf), " ");
	else Sprintf(eos(linebuf), "-%c ", t1->plchar);
	if(!strncmp("escaped", t1->death, 7)) {
	  if(!strcmp(" (with amulet)", t1->death+7))
	    Sprintf(eos(linebuf), "escaped the dungeon with amulet");
	  else
	    Sprintf(eos(linebuf), "escaped the dungeon [max level %d]",
	      t1->maxlvl);
	} else {
	  if(!strncmp(t1->death,"quit",4)) {
	    quit = TRUE;
	    if(t1->maxhp < 3*t1->hp && t1->maxlvl < 4)
		Sprintf(eos(linebuf), "cravenly gave up");
	    else
		Sprintf(eos(linebuf), "quit");
	  }
	  else if(!strcmp(t1->death,"choked"))
	    Sprintf(eos(linebuf), "choked on %s food",
		(t1->sex == 'F') ? "her" : "his");
	  else if(!strncmp(t1->death,"starv",5))
	    Sprintf(eos(linebuf), "starved to death"), starv = TRUE;
	  else Sprintf(eos(linebuf), "was killed"), killed = TRUE;
	  Sprintf(eos(linebuf), " on%s level %d",
	    (killed || starv) ? "" : " dungeon", t1->level);
	  if(t1->maxlvl != t1->level)
	    Sprintf(eos(linebuf), " [max %d]", t1->maxlvl);
	  if(quit && t1->death[4]) Sprintf(eos(linebuf), t1->death + 4);
	}
	if(killed) Sprintf(eos(linebuf), " by %s%s",
	  (!strncmp(t1->death, "trick", 5) || !strncmp(t1->death, "the ", 4))
		? "" :
	  index(vowels,*t1->death) ? "an " : "a ",
	  t1->death);
	Sprintf(eos(linebuf), ".");
	if(t1->maxhp) {
	  register char *bp = eos(linebuf);
	  char hpbuf[10];
	  int hppos;
	  Sprintf(hpbuf, (t1->hp > 0) ? itoa(t1->hp) : "-");
	  hppos = COLNO - 7 - strlen(hpbuf);
	  if(bp <= linebuf + hppos) {
	    while(bp < linebuf + hppos) *bp++ = ' ';
	    (void) strcpy(bp, hpbuf);
	    Sprintf(eos(bp), " [%d]", t1->maxhp);
	  }
	}
	if(so == 0) puts(linebuf);
	else if(so > 0) {
	  register char *bp = eos(linebuf);
	  if(so >= COLNO) so = COLNO-1;
	  while(bp < linebuf + so) *bp++ = ' ';
	  *bp = 0;
	  standoutbeg();
	  fputs(linebuf,stdout);
	  standoutend();
	  (void) putchar('\n');
	}
	return(strlen(linebuf));
}

char *
itoa(a) int a; {
static char buf[12];
	Sprintf(buf,"%d",a);
	return(buf);
}

char *
ordin(n) int n; {
register int d = n%10;
	return((d==0 || d>3 || n/10==1) ? "th" : (d==1) ? "st" :
		(d==2) ? "nd" : "rd");
}

char *
eos(s)
register char *s;
{
	while(*s) s++;
	return(s);
}

/*
 * Called with args from main if argc >= 0. In this case, list scores as
 * requested. Otherwise, find scores for the current player (and list them
 * if argc == -1).
 */
prscore(argc,argv) int argc; char **argv; {
	extern char *hname;
	char **players;
	int playerct;
	int rank;
	register struct toptenentry *t1, *t2;
	char *recfile = RECORD;
	FILE *rfile;
	register flg = 0;
	register int i;
#ifdef nonsense
	long total_score = 0L;
	char totchars[10];
	int totcharct = 0;
#endif
	int outflg = (argc >= -1);
#ifdef PERS_IS_UID
	int uid = -1;
#else
	char *player0;
#endif

	if(!(rfile = fopen(recfile,"r"))){
		puts("Cannot open record file!");
		return;
	}

	if(argc > 1 && !strncmp(argv[1], "-s", 2)){
		if(!argv[1][2]){
			argc--;
			argv++;
		} else if(!argv[1][3] && index("CFKSTWX", argv[1][2])) {
			argv[1]++;
			argv[1][0] = '-';
		} else	argv[1] += 2;
	}
	if(argc <= 1){
#ifdef PERS_IS_UID
		uid = getuid();
		playerct = 0;
#else
		player0 = plname;
		if(!*player0)
			player0 = "hackplayer";
		playerct = 1;
		players = &player0;
#endif
	} else {
		playerct = --argc;
		players = ++argv;
	}
	if(outflg) putchar('\n');

	t1 = tt_head = newttentry();
	for(rank = 1; ; rank++) {
	  if(fscanf(rfile, "%6s %d %d %d %d %d %ld %c%c %[^,],%[^\n]",
		t1->date, &t1->uid,
		&t1->level, &t1->maxlvl,
		&t1->hp, &t1->maxhp, &t1->points,
		&t1->plchar, &t1->sex, t1->name, t1->death) != 11)
			t1->points = 0;
	  if(t1->points == 0) break;
#ifdef PERS_IS_UID
	  if(!playerct && t1->uid == uid)
		flg++;
	  else
#endif
	  for(i = 0; i < playerct; i++){
		if(strcmp(players[i], "all") == 0 ||
		   strncmp(t1->name, players[i], NAMSZ) == 0 ||
		  (players[i][0] == '-' &&
		   players[i][1] == t1->plchar &&
		   players[i][2] == 0) ||
		  (digit(players[i][0]) && rank <= atoi(players[i])))
			flg++;
	  }
	  t1 = t1->tt_next = newttentry();
	}
	(void) fclose(rfile);
	if(!flg) {
	    if(outflg) {
		printf("Cannot find any entries for ");
		if(playerct < 1) printf("you.\n");
		else {
		  if(playerct > 1) printf("any of ");
		  for(i=0; i<playerct; i++)
			printf("%s%s", players[i], (i<playerct-1)?", ":".\n");
		  printf("Call is: %s -s [playernames]\n", hname);
		}
	    }
	    return;
	}

	if(outflg) outheader();
	t1 = tt_head;
	for(rank = 1; t1->points != 0; rank++, t1 = t2) {
		t2 = t1->tt_next;
#ifdef PERS_IS_UID
		if(!playerct && t1->uid == uid)
			goto outwithit;
		else
#endif
		for(i = 0; i < playerct; i++){
			if(strcmp(players[i], "all") == 0 ||
			   strncmp(t1->name, players[i], NAMSZ) == 0 ||
			  (players[i][0] == '-' &&
			   players[i][1] == t1->plchar &&
			   players[i][2] == 0) ||
			  (digit(players[i][0]) && rank <= atoi(players[i]))){
			outwithit:
				if(outflg)
				    (void) outentry(rank, t1, 0);
#ifdef nonsense
				total_score += t1->points;
				if(totcharct < sizeof(totchars)-1)
				    totchars[totcharct++] = t1->plchar;
#endif
				break;
			}
		}
		free((char *) t1);
	}
#ifdef nonsense
	totchars[totcharct] = 0;

	/* We would like to determine whether he is experienced. However,
	   the information collected here only tells about the scores/roles
	   that got into the topten (top 100?). We should maintain a
	   .hacklog or something in his home directory. */
	flags.beginner = (total_score < 6000);
	for(i=0; i<6; i++)
	    if(!index(totchars, "CFKSTWX"[i])) {
		flags.beginner = 1;
		if(!pl_character[0]) pl_character[0] = "CFKSTWX"[i];
		break;
	}
#endif /* nonsense /**/
}
