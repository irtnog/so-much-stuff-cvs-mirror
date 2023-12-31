%{
/*
**	Scanner for Cake
*/

static	char
rcs_id[] = "$Header: /home/Vince/cvs/local.d/cake.d/cake_s.l,v 1.2 1987-11-18 08:57:59 vrs Exp $";

#include	"cake.h"
#include	"cake_g.h"
#include	<ctype.h>

#ifdef	CAKEDEBUG
#define	ret(x)	if (lexdebug) printf("lex: x\n"); return x
#else
#define	ret(x)	return x
#endif

/* YYLMAX must be defined here; it can't be defined in conf.h */
#undef	YYLMAX
#define	YYLMAX		MAXLEXBUF

#define	LATE_CATCH
#define	LATE_RECOVERY	printf("cake: token '%s' is too long\n", yytext); \
			exit(1);

#define	LBRACE	'{'
#define	RBRACE	'}'

extern	char	*yysquare();
extern	char	*yycurly();

extern	int	lexdebug;
char		*yyfile = "no input file";
bool		afterifdef = FALSE;
%}

%a	2500
%e	250
%o	5000

%start	edge middle

pat	[a-zA-Z_0-9/%+-.,]
sp	[ \t]
nl	[\n\f]
nonl	[^\n\f]
enonl	({nonl}|\\{nl})

%%

"if"			{ BEGIN middle;	ret(IF);		}
"and"			{ BEGIN middle;	ret(AND);		}
"or"			{ BEGIN middle;	ret(OR);		}
"not"			{ BEGIN middle;	ret(NOT);		}
"in"			{ BEGIN middle;	ret(IN);		}
"match"			{ BEGIN middle;	ret(MATCH);		}
"exist"			{ BEGIN middle;	ret(EXIST);		}
"cando"			{ BEGIN middle;	ret(CANDO);		}
"ok"			{ BEGIN middle;	ret(OK);		}

"("			{ BEGIN middle;	ret(LPAREN);		}
")"			{ BEGIN middle;	ret(RPAREN);		}

"@"			{ BEGIN middle;	ret(NONVOL);		}
"!"			{ BEGIN middle;	ret(PRECIOUS);		}
"&"			{ BEGIN middle;	ret(PSEUDO);		}
"^"			{ BEGIN middle;	ret(REDUNDANT);		}
"*"			{ BEGIN middle;	ret(WHEN);		}

":"			|
"::"			{
				BEGIN middle;
				yylval.Ustr = new_name(yytext);
				ret(COLON);
			}

({pat}|\\{nonl})+	{
				BEGIN middle;
				yylval.Ustr = new_name(yytext);
				ret(PAT);
			}

\"([^"]|\\\")*\"	{
				BEGIN middle;
				/* strip off quotes */
				yytext[yyleng-1] = '\0';
				yylval.Ustr = new_name(yytext+1);
				ret(PAT);
			}

"[["			{
				BEGIN middle;
				yylval.Ustr = yysquare();
				ret(PATCMD);
			}

"{{"			{
				BEGIN middle;
				yylval.Ustr = yycurly();
				ret(TESTCMD);
			}

<middle>({sp}|\\{nl})*	{ BEGIN middle;			}

<edge>[@!+-]*"{"{enonl}*{nl}	|
<edge>[@!+-]*"}"{enonl}*{nl}	|
<edge>{sp}+{enonl}+{nl}	{
				BEGIN edge;
				afterifdef  = FALSE;
				yylval.Uact = prep_act(yytext);

				/* comment out check_actlen if  */
				/* have a lex driver with CATCH */
				check_actlen();

				/* braces around ret ARE needed */
				/* as ret expands to an if stmt */
				if (yylval.Uact->a_str[0] == LBRACE)
				{
					ret(BEGIN_ACT);
				}
				or (yylval.Uact->a_str[0] == RBRACE)
				{
					ret(END_ACT);
				}
				else
				{
					ret(ACT);
				}
			}

<edge>"set"{nonl}*{nl}	{
				BEGIN edge;
				if (lexdebug)
					printf("set line\n");
				yyset(yytext);
			}

<edge>#{nonl}*{nl}	{
				BEGIN edge;
				if (lexdebug)
					printf("hash line\n");
				yyhash(yytext);
				if (! afterifdef)
					ret(EMPTYLINE);
			}

<edge>{sp}*{nl}		{
				BEGIN edge;
				if (lexdebug)
					printf("empty line\n");
				if (! afterifdef)
					ret(EMPTYLINE);
			}

<middle>{nl}		{
				BEGIN edge;
				afterifdef = FALSE;
				ret(NL);
			}

{nonl} 			{ BEGIN middle;	ret(GARBAGE);	}

"$"{nonl}*{nl}		{ BEGIN edge;			}

%%

yyinit()
{
	BEGIN edge;
}

yywrap()
{
	return 1;
}

yyset(text)
reg	char	text[];
{
	reg	char	*s;
	int		setc;
	char		*setv[MAXARGS];

	for (s = text+4; *s != '\0' && isspace(*s); s++)
		;

	setc = parse_args(s, setv);
	process_args(setv, &setc, 0);
	if (setc > 0)
		printf("cake: non-options on parameter line ignored\n");
}

yyhash(text)
reg	char	text[];
{
	reg	char	*oldfile = yyfile;
	reg	char	*s, *t;

	afterifdef = TRUE;	/* assume so until proven otherwise */
	if (sscanf(text+1, "%d", &yylineno) != 1)
		return;

	text[strlen(text)-1] = '\0';	/* clobber the newline */
	for (s = text; *s != '\0' && *s != '"' && !isalpha(*s); s++)
		;

	if (*s == '\0')
		return;
	
	if (*s != '"')
		yyfile = new_name(s);
	else
	{
		for (t = s+strlen(s)-1; t != s && *t != '"'; t--)
			;
		
		if (t != s)
			*t = '\0';

		yyfile = new_name(s+1);
	}

	if (yyfile != oldfile)
	{
		afterifdef = FALSE;
		return;
	}
}

/*
**	Read in a PATCMD, which is [[.*PATCMD?.*]]
**	(or a matched set of double brackets with other stuff.
**	Note that PATCMD cannot be described by a REAL regular
**	expression; the one above cheats by using recursion.
**	yysquare returns the PATCMD brackets and all.
*/

char *
yysquare()
{
	char		buf[MAXSIZE];
	reg	int	c, c1;
	reg	int	i;
	reg	int	n;	/* number of outstanding levels */

	n = 1;
	i = 0;
	buf[i++] = '[';
	buf[i++] = '[';
	for (c = input(); c != '\0' && c != '\n' && c != '\f'; c = input())
	{
		buf[i++] = c;
		if (c == ']')
		{
			if ((c1 = input()) != ']')
			{
				/* for the next iteration */
				unput(c1);
				continue;
			}

			buf[i++] = c1;
			if (--n > 0)
				continue;

			buf[i] = '\0';
			if (strlen(buf) >= MAXSIZE)
			{
				fprintf(stderr, "cake internal error: buffer overflow in yysquare\n");
				exit_cake(FALSE);
			}

			if (lexdebug)
				printf("yysquare: %s\n", buf);

			fflush(stdout);
			return new_name(buf);
		}
		or (c == '[')
		{
			if ((c1 = input()) != '[')
			{
				/* for the next iteration */
				unput(c1);
				continue;
			}

			buf[i++] = c1;
			n++;
		}
		or (c == '\\')
		{
			/* don't check next char */
			if ((c1 = input()) != '\0')
				buf[i++] = c1;
		}
	}

	if (c == '\0')
		fprintf(stderr, "%s, %d: premature EOF\n", yyfile, yylineno);
	else
		fprintf(stderr, "%s, %d: unescaped newline inside a command\n", yyfile, yylineno);

	exit_cake(FALSE);
}

/*
**	Read in a TESTCMD, which is {{.*}}. Strip the brackets.
*/

char *
yycurly()
{
	char		buf[MAXSIZE];
	reg	int	c, c1;
	reg	int	i;

	i = 0;
	for (c = input(); c != '\0' && c != '\n' && c != '\f'; c = input())
	{
		buf[i++] = c;
		if (c == RBRACE)
		{
			if ((c1 = input()) != RBRACE)
			{
				/* for the next iteration */
				unput(c1);
				continue;
			}

			/* overwrite first RBRACE */
			buf[--i] = '\0';
			if (strlen(buf) >= MAXSIZE)
			{
				fprintf(stderr, "cake internal error: buffer overflow in yycurly\n");
				exit_cake(FALSE);
			}

			if (lexdebug)
				printf("yycurly: %s\n", buf);

			fflush(stdout);
			return new_name(buf);
		}
		or (c == '\\')
		{
			/* don't check next char */
			if ((c1 = input()) != '\0')
				buf[i++] = c1;
		}
	}

	if (c == '\0')
		fprintf(stderr, "%s, %d: premature EOF\n", yyfile, yylineno);
	else
		fprintf(stderr, "%s, %d: unescaped newline inside a command\n", yyfile, yylineno);

	exit_cake(FALSE);
}

check_actlen()
{
	if (strlen(yytext) >= YYLMAX)
	{
		printf("cake: action '%s' too long\n", yytext);
		exit_cake(FALSE);
	}
}
