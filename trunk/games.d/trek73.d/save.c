#ident "@(#) TREK73 $Header: /home/Vince/cvs/games.d/trek73.d/save.c,v 1.7 2002-11-25 21:42:37 vrs Exp $"
/*
 * $Source: /home/Vince/cvs/games.d/trek73.d/save.c,v $
 *
 * $Header: /home/Vince/cvs/games.d/trek73.d/save.c,v 1.7 2002-11-25 21:42:37 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2002/11/22 04:12:23  Vincent
 * Working on CYGWIN.
 *
 * Revision 1.5  1987/12/25 21:54:04  vrs
 * SYSV -> SYS5 (use CONFIG)
 *
 * Version 1.4  87/12/25  20:52:06  vrs
 * Check in 4.0 version from the net
 * 
 * Revision 1.1  87/10/09  11:10:52  11:10:52  okamoto (Jeff Okamoto)
 * Initial revision
 * 
 */
/*
 * TREK73: save.c
 *
 * save and restore routines
 *
 * @(#)save.c	4.15 (Berkeley) 5/10/82
 */

#include <stdio.h>
#ifdef BSD
#include <strings.h>
#endif
#ifdef SYS5
#include <string.h>
#endif
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#ifdef __STDC__
#include <termios.h>
#endif

#define MAXSTR	256

typedef struct stat STAT;

#ifndef __STDC__
extern char *sys_errlist[];
#endif
extern char version[], encstr[];
extern int errno;

char *sbrk();
#ifdef __STDC__
#define brk(x) (sbrk(x-sbrk(0)))
#endif

STAT sbuf;

extern void exit();

set_save()
{
	register char		*env;
	register struct passwd	*pw;
#ifndef __STDC__
	struct passwd		*getpwuid();
#endif
	char			*getpass();
	unsigned short		getuid();
	extern char		home[];
	extern char		savefile[];
	char			*getenv();

	if ((env = getenv("HOME")) != NULL)
		strcpy(home, env);
	else if ((pw = (struct passwd *)getpwuid((int)getuid()))
	    != NULL)
		strcpy(home, pw->pw_dir);
	else
		home[0] = '\0';
	strcat(home, "/");

	strcpy(savefile, home);
	strcat(savefile, "trek73.save");
}

/*
 * save_game:
 *	Implement the "save game" command
 */
save_game()
{
    register FILE *savef;
    register int c;
    char buf[MAXSTR];
    extern char savefile[];
    void perror();

    /*
     * get file name
     */

over:
    if (savefile[0] != '\0')
    {
	for (;;)
	{
	    printf("Save file (%s)? ", savefile);
	    c = getchar();
	    if (c == 'n' || c == 'N' || c == 'y' || c == 'Y')
		break;
	    else
		printf("\nPlease answer Yes or No");
	}
	if (c == 'y' || c == 'Y')
	{
	    strcpy(buf, savefile);
	    goto gotfile;
	}
    }

    do
    {
#ifdef __STDC__
	tcflush(fileno(stdin), TCIFLUSH);
#else
	stdin->_cnt = 0;
#endif
	printf("File name: ");
	buf[0] = '\0';
	gets(buf);
gotfile:
	/*
	 * test to see if the file exists
	 */
	if (stat(buf, &sbuf) >= 0)
	{
	    for (;;)
	    {
#ifdef __STDC__
		tcflush(fileno(stdin), TCIFLUSH);
#else
		stdin->_cnt = 0;
#endif
		printf("\nFile exists.  Do you wish to overwrite it?");
		if (c == 'y' || c == 'Y')
		    break;
		else if (c == 'n' || c == 'N')
		    goto over;
		else
		    printf("\nPlease answer Y or N");
	    }
	}
	strcpy(savefile, buf);
	if ((savef = fopen(savefile, "w")) == NULL)
	    perror("Trek73: Save problems");
    } while (savef == NULL);

    /*
     * write out encrpyted file (after a stat)
     * The fwrite is to force allocation of the buffer before the write
     */
    save_file(savef);
    exit(0);
}

/*
 * save_file:
 *	Write the saved game on the file
 */
save_file(savef)
register FILE *savef;
{
    /*
     * close any open save file
     */
    (void) fstat(fileno(savef), &sbuf);
    /*
     * DO NOT DELETE.  This forces stdio to allocate the output buffer
     * so that malloc doesn't get confused on restart
     */
    fwrite("junk", 1, 5, savef);

    fseek(savef, 0L, 0);
    encwrite(version, (unsigned int) (sbrk(0) - version), savef);
    fclose(savef);
    exit(0);
}

/*
 * restore:
 *	Restore a saved game from a file with elaborate checks for file
 *	integrity from cheaters
 */
restore(file, envp)
char *file;
char **envp;
{
    register int inf;
#ifdef PARANOID
    register char syml;
#endif /*PARANOID*/
    extern char **environ;
    char buf[MAXSTR];
    long lseek();
    STAT sbuf2;
    void perror();

#ifdef BSD
    (void) signal(SIGTSTP, SIG_IGN);
#endif
#ifdef SYS5
    (void) signal(SIGQUIT, SIG_IGN);
#endif

    if ((inf = open(file, 0)) < 0)
    {
	perror(file);
	return 0;
    }
    (void) fstat(inf, &sbuf2);
#ifdef PARANOID
#ifdef BSD
    syml = symlink(file);
#endif
#ifdef SYS5
    syml = link(file);
#endif
    if (unlink(file) < 0)
    {
	printf("Cannot unlink file\n");
	return 0;
    }
#endif /*PARANOID*/

    fflush(stdout);
    encread(buf, (unsigned int) (strlen(version) + 1), inf);
    if (strcmp(buf, version) != 0)
    {
	printf("Sorry, saved game is out of date.\n");
	return 0;
    }

    fflush(stdout);
    brk(version + sbuf2.st_size);
    lseek(inf, 0L, 0);
    encread(version, (unsigned int) sbuf2.st_size, inf);
#ifdef PARANOID
    /*
     * we do not close the file so that we will have a hold of the
     * inode for as long as possible
     */
	if (sbuf2.st_ino != sbuf.st_ino || sbuf2.st_dev != sbuf.st_dev)
	{
	    printf("Sorry, saved game is not in the same file.\n");
	    return 0;
	}
#ifdef NOTDEF
    /*
     * defeat multiple restarting from the same place
     */
	if (sbuf2.st_nlink != 1 || syml)
	{
	    printf("Cannot restore from a linked file %d %d\n", sbuf2.st_nlink, syml);
	    exit(1);
	}
#endif
#endif /*PARANOID*/
#ifdef BSD
    (void) signal(SIGTSTP, SIG_DFL);
#endif
#ifdef SYS5
    (void) signal(SIGQUIT, SIG_DFL);
#endif

    environ = envp;
#ifdef __STDC__
    tcflush(fileno(stdin), TCIFLUSH);
#else
    stdin->_cnt = 0;
#endif
    playit();
#ifdef PARANOID
    syml = syml;		/* LINT */
#endif /*PARANOID*/
    /*NOTREACHED*/
}

/*
 * encwrite:
 *	Perform an encrypted write
 */
encwrite(start, size, outf)
register char *start;
unsigned int size;
register FILE *outf;
{
    register char *ep;

    ep = encstr;

    while (size--)
    {
	putc(*start++ ^ *ep++, outf);
	if (*ep == '\0')
	    ep = encstr;
    }
}

/*
 * encread:
 *	Perform an encrypted read
 */
encread(start, size, inf)
register char *start;
unsigned int size;
register int inf;
{
    register char *ep;
    register int read_size;

    if ((read_size = read(inf, start, size)) == -1 || read_size == 0)
	return read_size;

    ep = encstr;

    while (size--)
    {
	*start++ ^= *ep++;
	if (*ep == '\0')
	    ep = encstr;
    }
    return read_size;
}

