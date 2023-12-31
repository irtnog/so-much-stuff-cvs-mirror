#include <stdio.h>
#include <pwd.h>
#include <ctype.h>

#ifndef lint
static char *rcsid = "$Header: /home/Vince/cvs/local.d/cops.d/src/pass.c,v 1.3 1990-04-21 17:00:32 vrs Exp $";
#endif

/*
 * Warning: this program burns a lot of cpu.
 */
/*
 * Insecure - find accounts with poor passwords
	Date: Tue, 29 Nov 83 18:19:32 pst
	From: leres%ucbarpa@Berkeley (Craig Leres)
	    Modified by Seth Alford, Roger Southwick, Steve Dum, and
	    Rick Lindsley for Tektronix
 */

/*
 *	$Log: not supported by cvs2svn $
 *Version 1.2  90/04/20  17:05:30  vrs
 *Apply patch from the net
 *
 *	Revision 1.1  85/09/10  16:00:56  root
 *	Initial revision
 *	
 *
 * By default, this program only checks for accounts with passwords the same
 * as the login name. The following options add more extensive checking. (The
 * tradeoff is cpu time -- with all options enabled it can run into the 100's
 * of MINUTES.) Any argument that does not begin with a "-" is assumed to be
 * a file name. (A single '-' means stdin.) If no file name is given,
 * /etc/passwd is used.
 *
 * Options:
 *
 *		-v:	verbose -- list all guesses on stdout
 *		-u:	output the username on the line of the password file
 *			currently being checked. If the program stops
 *			abruptly you will then know how far it got.
 *		-w file: use the list of words contained in "file" as likely
 *			passwords. Words in the file are one to a line.
 *		-b: 	check all guesses backwards too
 *		-g:	use the Full Name portion of the gecos field to
 *			generate more guesses
 *		-s:	check the single letters a-z, A-Z, 0-9 as passwords
 *		-c:	with each guess, check for all-lowercase and
 *			all-uppercase versions too.
 *		-n:	complain about null passwords (default is to keep quiet)
 *		-p:	print the password when guessed
 */

int verbose = 0, singles = 0, backwards = 0, checkgecos = 0, checkcase = 0,
    chknulls = 0, printit = 0, users = 0, chkwords = 0;

char *my_index(), *reverse();
long atol();
FILE *fopen();
char *fgets();

char PASSWD[] = "/etc/passwd";
char EMPTY[] = "";
static FILE *pwf = NULL, *wlf = NULL;
char line[BUFSIZ+1];
struct passwd passwd;
char	*Curpw, *Wordlist = NULL;

main(argc, argv)
char **argv;
{
    register int i;
    register char *arg;
    int onedone = 0;

    /*
    You have to decide whether or not to include these lines....

    if (getuid()) {
	printf("Did you really think we would let you run this?\n");
	exit(1);
	}

    */

    for (i = 1; i < argc; i++)
	if ((arg = argv[i]) && *arg == '-')
	    while (*++arg) {
		switch (*arg) {
		    case 'n':
			/*
			 * complain about null passwords
			 */
			chknulls++;
			break;
		    case 'c':
			/*
			 * check cases
			 */
			checkcase++;
			break;
		    case 'g':
			/*
			 * use gecos
			 */
			checkgecos++;
			break;
		    case 'v':
			/*
			 * turn on motormouth
			 */
			verbose++;
			break;
		    case 'b':
			/*
			 * check all attempts forwards and backwards
			 */
			backwards++;
			break;
		    case 's':
			/*
			 * carry out a more intensive search, checking for
			 * single letter passwords
			 */
			singles++;
			break;
		    case 'p':
			/*
			 * print out the password when found
			 */
			printit++;
			break;
		    case 'u':
			/*
			 * print out users as testing
			 */
			users++;
			break;
		    case 'w':
			/*
			 * consult word list of likely passwords
			 */
			if ((Wordlist = argv[i+1]) == NULL) {
			    fprintf(stderr,
				"%s: No file supplied with -w option\n",
				argv[0]);
			    exit (1);
			    }
			argv[i+1] = NULL;
			break;
		    case '\0':
			/*
			 * read from stdin
			 */
			break;
		    default:
			fprintf(stderr,
			    "%s: unknown option '%c'. Options are:\n",argv[0],
			    *arg);
			/* FALL THRU */
		    case '-':
			fprintf(stderr,"-v:\t\tverbose -- list all guesses on stdout\n");
			fprintf(stderr,"-u:\t\toutput the username currently being checked\n");
			fprintf(stderr,"-w file:\tconsult the indicated file for words to check as passwords\n");
			fprintf(stderr,"-b:\t\tcheck all guesses forwards and backwards\n");
			fprintf(stderr,"-g:\t\tuse the Full name portion of the gecos field for more guesses\n");
			fprintf(stderr,"-s:\t\tcheck the single letters a-z, A-Z, 0-9 as passwords\n");
			fprintf(stderr,"-c:\t\tcheck the all-upper and all-lower case version of each guess\n");
			fprintf(stderr,"-n:\t\tcomplain about null passwords\n");
			fprintf(stderr,"-p:\t\tprint the password when guessed\n");
			exit(1);
		    }
		argv[i] = NULL;
		}
    
    for (i = 1; i < argc; i++) {
	if (argv[i] == NULL) continue;
	onedone++;
	if (*(argv[i]) == '-') {
	    /*
	     * read from stdin; we'll cheat and set pwf directly
	     */
	    pwf = stdin;
	    chkpw();
	    /*
	     * don't fclose stdin!
	     */
	    clearerr(stdin);
	    }
	else {
	    if (csetpwent(argv[i])) {
		perror(argv[i]);
		continue;
		}
	    Curpw = argv[i];
	    chkpw();
	    cendpwent();
	    }
	}
    if (!onedone) {
	Curpw = NULL;
	chkpw();
	}
    exit(0);
}

/*
 * Added by Jacob Gore, March 12, 1987.
 *
 * Finds the pointer of the leftmost occurance within the character string
 * 'string' of any character found within the character string 'chars'.
 *
 * If none of the characters in 'chars' appear in 'string', NULL is retutned.
 *
 */
char *
indexm (string, chars)
    char *string, *chars;
{
    while (*string) {
	if (my_index(chars, *string) != NULL) {
	    return string;
	}
	string++;
    }
    return NULL;
}

#define ARB_CONST	80

chkpw()

{
    register char	*cp, *cp2;
    register struct passwd *pwd;
    struct passwd	*getpwent();
    char		guess[100];
    char		*malloc(), **wordptr;
    char		**wordarray = (char **)malloc(ARB_CONST*sizeof(*wordarray));
	int			count = ARB_CONST;
    char		**endptr = wordarray + count;
    int			done = 0;


    if (Wordlist)
    {
	if ((wlf = fopen(Wordlist,"r")) == NULL)
	{
	    perror(Wordlist);
	    exit(1);
	}

	wordptr = wordarray;
	/*
	 * note that endptr points to space OUTSIDE of wordarray
	 */

	while (fscanf(wlf,"%[^\n]\n",guess) != EOF)
	{
  /* SunOs 4.03 on a Sun 3/80 didn't work properly, needed this one line fix */
	    /*if (feof(wlf)) break;*/

	    if (wordptr == endptr)
	    {
		count += ARB_CONST;
		wordarray = (char **)realloc(wordarray, count*sizeof(*wordarray));
		endptr = wordarray + count;
		wordptr = endptr - ARB_CONST;
	    }
	    if ((*wordptr = malloc(1+strlen(guess))) == NULL)
	    {
		fprintf(stderr,"malloc: no more memory for wordlist\n");
		exit (1);
	    }
	    strcpy(*wordptr,guess);
	    wordptr++;
	}
	*wordptr = NULL;
    }

    while ((pwd = getpwent()) != 0 ) {
	done = 0;
	if (verbose || users) {
	    if (Curpw == NULL)
		printf("\t%s \"%s\"\n", pwd->pw_name, pwd->pw_gecos);
	    else
		printf("%s -- \t%s \"%s\"\n", Curpw, pwd->pw_name,
		    pwd->pw_gecos);
	    fflush(stdout);
	    }
	if (*pwd->pw_passwd == '\0') {
	    if (chknulls) {
		if (Curpw == NULL)
		    printf("Warning!  Password Problem: null passwd:\t%s\tshell: %s\n",
			pwd->pw_name, pwd->pw_shell);
		else
		    printf("Warning!  %s -- Password Problem: null passwd:\t%s\tshell: %s\n",
			Curpw, pwd->pw_name, pwd->pw_shell);
		fflush(stdout);
		}
	    continue;
	}
	if (strlen(pwd->pw_passwd) < 13) {
	    if (verbose) {
		if (Curpw == NULL)
		    printf("Invalid Password Skipped: name:\t%s\tpasswd: %s\n",
			pwd->pw_name, pwd->pw_passwd);
		else
		    printf("%s -- Invalid Password Skipped: name:\t%s\tpasswd: %s\n",
			Curpw, pwd->pw_name, pwd->pw_passwd);
		fflush(stdout);
		}
	    continue;
	}
	/*
	 * Try the user's login name
	 */
	if (uandltry(pwd,pwd->pw_name))
	    continue;

	/*
	 * Try names from the gecos field
	 */
	if (checkgecos) {
	    strcpy(guess, pwd->pw_gecos);
	    cp = guess;
	    if (*cp == '-') cp++;		/* special gecos field */
	    if ((cp2 = my_index(cp, ';')) != NULL)
		*cp2 = '\0';

	    for (;;) {
		/* use both ' ' and ',' as delimiters -- Jacob */
		if ((cp2 = indexm(cp, " ,")) == NULL) {
		    if (uandltry(pwd,cp))
			done++;
		    break;
		    }

		*cp2 = '\0';

		if (uandltry(pwd,cp)) {
		    done++;
		    break;
		    }
		cp = ++cp2;
		}
	    }
	    
	if (!done && Wordlist)
	{
	    /*
	     * try the words in the wordlist
	     */
	    wordptr = wordarray;
	    while (endptr != wordptr)
	    {
		if (*wordptr == NULL)
		    break;
		if (uandltry(pwd,*(wordptr++)))
		{
		    done++;
		    break;
		}
	    }
	}
	if (!done && singles) {
	    /*
	     * Try all single letters
	     * (try digits too .  --Seth)
	     */
	    guess[1] = '\0';
	    for (guess[0]='a'; guess[0] <= 'z'; guess[0]++)
		if (try(pwd,guess))
		    break;
	    for (guess[0]='A'; guess[0] <= 'Z'; guess[0]++)
		if (try(pwd,guess))
		    break;
	    for (guess[0]='0'; guess[0] <= '9'; guess[0]++)
		if (try(pwd,guess))
		    break;
	    }
    }
}

/*
 * Stands for "upper and lower" try.  Calls the "real" try, below,
 * with the supplied version of the password, and with
 * an upper and lowercase version of the password. If the user doesn't
 * want to try upper and lower case then we just return after the one
 * check.
*/

uandltry (pwd,guess)
char *guess;
struct passwd *pwd;
{
    register char *cp;
    char buf[100];
    int alllower, allupper;

    alllower = allupper = 1;

    if (try(pwd,guess) || (backwards && try(pwd,reverse(guess)))) return (1);

    if (!checkcase) return(0);

    strcpy (buf, guess);
    cp = buf-1;
    while (*++cp) {
	if (isupper(*cp))
	    alllower = 0;
	if (islower(*cp))
	    allupper = 0;
	}

    if (!allupper) {
	for ( cp=buf; *cp != '\0'; cp++)
	    if (islower (*cp))
		*cp += 'A' - 'a';

	if (try(pwd,buf) || (backwards && try(pwd,reverse(buf)))) return (1);
	}

    if (!alllower) {
	for ( cp = buf; *cp != '\0'; cp++)
	    if (isupper (*cp))
		*cp += 'a' - 'A';

	if (try(pwd,buf) || (backwards && try(pwd,reverse(buf)))) return (1);
	}
    return (0);
}

try(pwd,guess)
char *guess;
register struct passwd *pwd;
{
    register char  *cp;
    char   *crypt ();

    if (verbose) {
	if (Curpw == NULL)
	    printf ("Trying \"%s\" on %s\n", guess, pwd -> pw_name);
	else
	    printf ("%s -- Trying \"%s\" on %s\n", Curpw, guess,
		pwd -> pw_name);
	fflush (stdout);
	}
    if (! guess || ! *guess) return(0);
    cp = crypt (guess, pwd -> pw_passwd);
    if (strcmp (cp, pwd -> pw_passwd))
	return (0);
    if (Curpw == NULL)
	if (printit)
	    printf ("Warning!  Password Problem: Guessed:\t%s\tshell: %s passwd: %s\n",
		pwd -> pw_name, pwd -> pw_shell, guess);
	else
	    printf ("Warning!  Password Problem: Guessed:\t%s\tshell: %s\n", pwd -> pw_name,
		pwd -> pw_shell);
    else
	if (printit)
	    printf ("Warning!  %s -- Password Problem: Guessed:\t%s\tshell: %s passwd: %s\n",
		Curpw, pwd -> pw_name, pwd -> pw_shell, guess);
	else
	    printf ("Warning!  %s -- Password Problem: Guessed:\t%s\tshell: %s\n",
		Curpw, pwd -> pw_name, pwd -> pw_shell);
    fflush (stdout);
    return (1);
}
/* end of PW guessing program */

#define MAXUID 0x7fff	/* added by tonyb 12/29/83 */
			/* altered to a reasonable number - mae 8/20/84 */

/*
 * Add a parameter to "setpwent" so I can override the file name.
 */

csetpwent(file)
char *file;
{
	if ((pwf = fopen(file,"r")) == NULL)
	    return(1);
	return(0);
}

cendpwent()
{
    fclose(pwf);
    pwf = NULL;
}

char *
pwskip(p)
register char *p;
{
	while(*p && *p != ':' && *p != '\n')
		++p;
	if(*p == '\n')
		*p = '\0';
	else if(*p)
		*p++ = '\0';
	return(p);
}

struct passwd *
getpwent()
{
	register char *p;
	long	x;

	if(pwf == NULL)
	    if (csetpwent(PASSWD)) {
		perror(PASSWD);
		return(NULL);
		}
	p = fgets(line, BUFSIZ, pwf);
	if(p == NULL)
		return(0);
	passwd.pw_name = p;
	p = pwskip(p);
	passwd.pw_passwd = p;
	p = pwskip(p);
	x = atol(p);	
	passwd.pw_uid = (x < 0 || x > MAXUID)? (MAXUID+1): x;
	p = pwskip(p);
	x = atol(p);
	passwd.pw_gid = (x < 0 || x > MAXUID)? (MAXUID+1): x;
	passwd.pw_comment = EMPTY;
	p = pwskip(p);
	passwd.pw_gecos = p;
	p = pwskip(p);
	passwd.pw_dir = p;
	p = pwskip(p);
	passwd.pw_shell = p;
	(void) pwskip(p);

	p = passwd.pw_passwd;
 
	return(&passwd);

}


/*
 * reverse a string
 */
char *reverse(str)
char *str;

{
    register char *ptr;
    register int len;
    char	*malloc();

    if ((ptr = malloc((len = strlen(str))+1)) == NULL)
	return(NULL);
    ptr += len;
    *ptr = '\0';
    while (*str && (*--ptr = *str++))
	;
    return(ptr);
}
/* taken from comp.binaries.ibm.pc.d:
Some users have reported trouble compiling the freely distributable
uudecode I posted.  It seems that Berkeley moved the "index" function
to one of their system libraries and some systems don't have it.
Here is the missing "index" function, excerpted from an earlier freely
distributable uudecode.  Just add it on the end of the uudecode I posted.
*/
/*
--Keith Petersen
Maintainer of SIMTEL20's CP/M, MSDOS, & MISC archives [IP address 26.2.0.74]
Internet: w8sdz@WSMR-SIMTEL20.Army.Mil, w8sdz@brl.arpa  BITNET: w8sdz@NDSUVM1
Uucp: {ames,decwrl,harvard,rutgers,ucbvax,uunet}!wsmr-simtel20.army.mil!w8sdz
*/

/*
 * Return the ptr in sp at which the character c appears;
 * NULL if not found
 */

#define	NULL	0

char *
my_index(sp, c)
register char *sp, c;
{
	do {
		if (*sp == c)
			return(sp);
	} while (*sp++);
	return(NULL);
}


