/*
 * unbatchnews: extract news in batched format and process it one article
 * at a time.  The format looks like
 *	#! rnews 1234
 *	article containing 1234 characters
 *	#! rnews 4321
 *	article containing 4321 characters
 */

#ifndef lint
static char	*SccsId = "@(#)unbatch.c	1.8	9/3/84";
#endif !lint

# include <stdio.h>

char buf[512];

main()
{
	register int c;
	register FILE *pfn;
	register long size;
	char *filename;
	int pid, wpid, exstat;
	char *mktemp(), *gets();
	long atol();

	filename = mktemp("/tmp/unbnewsXXXXXX");
	while(gets(buf) != NULL) {
		while (strncmp(buf, "#! rnews ", 9)) {
			fprintf(stderr, "out of sync, skipping %s\n", buf);
			if (gets(buf) == NULL)
				exit(0);
		}
		size = atol(buf+9);
		if(size <= 0)
			break;
		pfn = fopen(filename, "w");
		while(--size >= 0 && (c = getc(stdin)) != EOF)
			putc(c, pfn);
		fclose(pfn);

		/*
		 * If we got a truncated batch, don't process the
		 * last article; it will probably be received again.
		 */
		if (size > 0)
			break;

		/*
		 * rnews < filename
		 */
		while ((pid = fork()) == -1) {
			fprintf(stderr, "fork failed, waiting...\r\n");
			sleep(60);
		}
		if (pid == 0) {
			close(0);
			open(filename, 0);
#ifdef IHCC
			sprintf(buf, "%s/%s/rnews", logdir(HOME), LIBDIR);
			execlp(buf, "rnews", (char *)0);
#else
			execlp("rnews", "rnews", (char *)0);
#endif
			perror("rnews");
			exit(1);
		}
		while ((wpid = wait(&exstat)) >= 0 && wpid != pid)
			;
	}
	unlink(filename);
}
