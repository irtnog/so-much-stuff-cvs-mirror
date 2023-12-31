/*
 * Copyright (c) 1987 by Ed James, UC Berkeley.  All rights reserved.
 *
 * Copy permission is hereby granted provided that this notice is
 * retained on all partial or complete copies.
 *
 * For more info on this and all of my stuff, mail edjames@berkeley.edu.
 */

#include "include.h"
#include <errno.h>

extern struct passwd *getpwuid();
extern char *malloc();
extern FILE *popen();
extern int errno;

compar(a, b)
	SCORE	*a, *b;
{
	if (b->planes == a->planes)
		return (b->time - a->time);
	else
		return (b->planes - a->planes);
}

#define SECAMIN		60
#define MINAHOUR	60
#define HOURADAY	24
#define SECAHOUR	(SECAMIN * MINAHOUR)
#define SECADAY		(SECAHOUR * HOURADAY)
#define DAY(t)		((t) / SECADAY)
#define HOUR(t)		(((t) % SECADAY) / SECAHOUR)
#define MIN(t)		(((t) % SECAHOUR) / SECAMIN)
#define SEC(t)		((t) % SECAMIN)

char	*
timestr(t)
{
	static char	s[80];

	if (DAY(t) > 0)
		sprintf(s, "%dd+%02dhrs", DAY(t), HOUR(t));
	else if (HOUR(t) > 0)
		sprintf(s, "%d:%02d:%02d", HOUR(t), MIN(t), SEC(t));
	else if (MIN(t) > 0)
		sprintf(s, "%d:%02d", MIN(t), SEC(t));
	else if (SEC(t) > 0)
		sprintf(s, ":%02d", SEC(t));
	else
		*s = '\0';

	return (s);
}

#ifndef BSD
gethostname(buf, s)
char *buf;
int s;
{
	FILE *File;

	File = popen("uuname -l", "r");
	if (File == NULL)
		strncpy(buf, "unknown", s);
	else {
		fscanf(File, "%s", buf);
		pclose(File);
	}
	if (*buf == '\0')
		strncpy(buf, "unknown", s);
}
#endif

log_score(list_em)
{
	register int	i, fd, num_scores = 0, good, changed = 0, found = 0;
	struct passwd	*pw;
	FILE		*fp;
	char		*cp, logstr[BUFSIZ];
#ifndef LOCK_EX
	char		loglock[BUFSIZ];
#endif
	static SCORE	*score = 0;
	static SCORE	thisscore;

	if (score == 0) {
		score = (SCORE *)malloc(NUM_SCORES*sizeof(SCORE));
		if (score == 0) {
			fprintf(stderr, "No memory for score array\n");
			return(-1);
		}
	}

	strcpy(logstr, SPECIAL_DIR);
	strcat(logstr, LOG);
#ifndef LOCK_EX
	strcpy(loglock, logstr);
	strcat(loglock, ".lck");
#endif

	umask(0);
	fd = open(logstr, O_CREAT|O_RDWR, 0644);
	if (fd < 0) {
		perror(logstr);
		return (-1);
	}
	/*
	 * This is done to take advantage of stdio, while still 
	 * allowing a O_CREAT during the open(2) of the log file.
	 */
	fp = fdopen(fd, "r+");
	if (fp == NULL) {
		perror(logstr);
		return (-1);
	}
#ifdef LOCK_EX
	if (flock(fileno(fp), LOCK_EX) < 0) {
		perror("flock");
		return (-1);
	}
#else
	while (link(logstr, loglock) < 0) {
		if (errno != EEXIST) {
			perror("lock");
			return (-1);
		}
		sleep(1);
	}
#endif /*LOCK_EX*/
	for (;;) {
		good = fscanf(fp, "%s %s %s %d %d %d",
			score[num_scores].name, 
			score[num_scores].host, 
			score[num_scores].game,
			&score[num_scores].planes, 
			&score[num_scores].time,
			&score[num_scores].real_time);
		if (good != 6 || ++num_scores >= NUM_SCORES)
			break;
	}
	if (!test_mode && !list_em) {
		if ((pw = getpwuid(getuid())) == NULL) {
			fprintf(stderr, 
				"getpwuid failed for uid %d.  Who are you?\n",
				getuid());
			return (-1);
		}
		strcpy(thisscore.name, pw->pw_name);
		if (gethostname(thisscore.host, sizeof (thisscore.host)) < 0) {
			perror("gethostname");
			return (-1);
		}
		cp = strrchr(File, '/');
		if (cp == NULL) {
			fprintf(stderr, "log: where's the '/' in %s?\n", File);
			return (-1);
		}
		cp++;
		strcpy(thisscore.game, cp);

		thisscore.time = gclock;
		thisscore.planes = safe_planes;
		thisscore.real_time = time((long *)0) - start_time;

		for (i = 0; i < num_scores; i++) {
			if (strcmp(thisscore.name, score[i].name) == 0 &&
			    strcmp(thisscore.host, score[i].host) == 0 &&
			    strcmp(thisscore.game, score[i].game) == 0) {
				if (thisscore.time > score[i].time) {
					score[i].time = thisscore.time;
					score[i].planes = thisscore.planes;
					changed++;
				}
				found++;
				break;
			}
		}
		if (!found) {
			for (i = 0; i < num_scores; i++) {
				if (thisscore.time > score[i].time) {
					if (num_scores < NUM_SCORES)
						num_scores++;
					bcopy(&score[i],
						&score[num_scores - 1], 
						sizeof (score[i]));
					bcopy(&thisscore, &score[i],
						sizeof (score[i]));
					changed++;
					break;
				}
			}
		}
		if (!found && !changed && num_scores < NUM_SCORES) {
			bcopy(&thisscore, &score[num_scores], 
				sizeof (score[num_scores]));
			num_scores++;
			changed++;
		}

		if (changed) {
			if (found)
				puts("You beat your previous score!");
			else
				puts("You made the top players list!");
			qsort(score, num_scores, sizeof (*score), compar);
			rewind(fp);
			for (i = 0; i < num_scores; i++)
				fprintf(fp, "%s %s %s %d %d %d\n",
					score[i].name, score[i].host, 
					score[i].game, score[i].planes,
					score[i].time, score[i].real_time);
		} else {
			if (found)
				puts("You didn't beat your previous score.");
			else
				puts("You didn't make the top players list.");
		}
		putchar('\n');
	}
#ifdef LOCK_UN
	flock(fileno(fp), LOCK_UN);
#else
	(void) unlink(loglock);
#endif
	fclose(fp);
	printf("%2s:  %-8s  %-8s  %-18s  %4s  %9s  %4s\n", "#", "name", "host", 
		"game", "time", "real time", "planes safe");
	puts("-------------------------------------------------------------------------------");
	for (i = 0; i < num_scores; i++) {
		cp = strchr(score[i].host, '.');
		if (cp != NULL)
			*cp = '\0';
		printf("%2d:  %-8s  %-8s  %-18s  %4d  %9s  %4d\n", i + 1,
			score[i].name, score[i].host, score[i].game,
			score[i].time, timestr(score[i].real_time),
			score[i].planes);
	}
	putchar('\n');
	return (0);
}
