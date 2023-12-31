%option lex-compat
%p 3000
%{
#ident "@(#) TREK73 $Header: /home/Vince/cvs/games.d/trek73.d/command.l,v 1.2 2002-11-22 04:12:22 Vincent Exp $"
/*
 * $Source: /home/Vince/cvs/games.d/trek73.d/command.l,v $
 *
 * $Header: /home/Vince/cvs/games.d/trek73.d/command.l,v 1.2 2002-11-22 04:12:22 Vincent Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  1987/12/25 20:56:12  vrs
 * Initial version
 *
 * Revision 1.2  87/10/09  12:20:33  12:20:33  okamoto (Jeff Okamoto)
 * Added more positions for lex, added some null keywords
 * 
 * Revision 1.1  87/10/09  12:19:51  12:19:51  okamoto (Jeff Okamoto)
 * Initial revision
 */
	double	atof();
	char	Input[BUFSIZ];
	char	*Inptr;

#ifdef FLEX_SCANNER
#define YY_INPUT(buf, result, max_size) \
	{ \
	int c = *Inptr++; \
	result = (c == EOF)? YY_NULL : (buf[0] = c, 1); \
	}
#endif
%}

WS	[ \t]*

%%
[+-]?[0-9.]+		{ yylval.dval = atof(yytext); return(NUMBER); }
abort			{ return(ABORT); }
all			{ return(ALL); }
at			;
control			{ return(CONTROL); }
corbomite{WS}bluff	|
corbomite		{ return(CORB); }
turn			|
course			{ return(COURSE); }
damage{WS}report	|
damage			{ return(DAMAGE); }
play{WS}dead		|
dead			{ return(DEAD); }
delay			|
fuse			{ return(DELAY); }
demand			{ return(DEMAND); }
self-destruct		|
self{WS}destruct	|
destruct		{ return(DESTR); }
detonate		{ return(DET); }
display			{ return(DISPLAY); }
elude			{ return(ELUDE); }
engineering		|
eng			{ return(ENG); }
fire			{ return(FIRE); }
help			{ return(HELP); }
jettison		|
jett			{ return(JETT); }
launch			{ return(LAUNCH); }
load			{ return(LOAD); }
lock			{ return(LOCK); }
onto			;
on			;
parameters		|
params			|
param			{ return(PARAM); }
phasers			|
phaser			{ return(PHASER); }
pod			|
pods			;
alter{WS}power		|
power			{ return(POWER); }
probe			{ return(PROBE); }
prox			|
proximity		{ return(PROXIMITY); }
pursue			{ return(PURSUE); }
rotate			{ return(ROTATE); }
save			{ return(SAVE); }
scan			{ return(SCAN); }
spread			{ return(SPREAD); }
status			{ return(STATUS); }
surrender		{ return(SURREND); }
survivors{WS}report	|
survivors		|
surv			{ return(SURV); }
tactical		{ return(TACTIC); }
the			;
to			;
toward			|
towards			{ return(TOWARD); }
photons			|
photon			|
torpedos		|
torpedo			|
torp			|
torps			|
tubes			|
tube			{ return(TUBE); }
unload			{ return(UNLOAD); }
version			{ return(VERSION); }
warp{WS}factor		|
warp			{ return(WARP); }
with			;

[A-Za-z#]+		{ (void) strcpy( yylval.sval, yytext);
				return(NAME); }
{WS}			;
\n			{ return(-1); }
.			;
%%
#ifndef FLEX_SCANNER
#undef input
#undef unput

input()
{
	int c;

	c = *Inptr++;
	return (c);
}

unput(c)
char c;
{
	Inptr--;
}
#endif
