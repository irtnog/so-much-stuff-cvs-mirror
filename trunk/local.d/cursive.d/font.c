/* CURSIVE FONT FILE
 *
 * This font is copyrighted by Jan Wolter.  See notes in "cursive.c".
 *
 * Several of the letters are rather ugly, but the wonder is not how
 * well it sings.  A few uppercase letters are a bit too tall.  The
 * font has to some extent been chosen to make my name look good to me.
 * Those of you with different names will have to take pot luck, or fix
 * it to taste.  Editing this file must be done with some care.  The
 * numbers on the defines must match the index numbers of the letter
 * in the list[] structure.  The six numbers after the character are how
 * many blank spaces must be added to the beginning of each of the six
 * scan lines to make them line up.  The last two numbers are the scan
 * line numbers in which the character is connected to the preceeding and
 * succeeding characters.  -1 means it isn't.  There are two versions of
 * each lower case letter, one with the line coming in on line 2, and one
 * with the line coming in on line 3.
 */

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.2  87/12/05  09:19:30  hyc
 * Cleaned up some of the characters, made them nicer looking.
 * To me, anyway.
 * 
 */

#include "cursive.h"

struct letter list[] = {

#define LA  0		/* a type 1 */ 
	{ {
	"",
	"",
	 "__.",
	"(_(",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LB  1		/* b type 1 */
	{ {
	"",
	  "/",
 	 "/_",
	"/_)",
	"",
	""
	},
	{0,2,1,0,0,0},
	3,2
	},

#define LC  2		/* c type 1 */
	{ {
	"",
	"",
	 "_.",
	"(_",
	"",
	""}
	,
	{0,0,1,0,0,0},
	3,3
	},

#define LD  3		/* d type 1 */
	{ {
	"",
	    "/",
	 "__/",
	"(_(",
	"",
	""
	},
	{0,4,1,0,0,0},
	3,3
	},

#define LE  4		/* e type 1 */
	{ {
	"",
	"",
	 "_",
	"(<",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LF  5		/* f type 1 */
	{ {
	"",
	    "/)",
	   "// ",
	  "//",
	 "/>",
	"</ "
	},
	{0,4,3,2,1,0},
	3,3
	},

#define LG  6		/* g type 1 */
	{ {
	"",
	"",
	 "_,",
	"(_)",
	 "/|",
	"|/"
	},
	{0,0,1,0,1,0},
	3,3
	},

#define LH  7		/* h type 1 */
	{ {
	"",
	  "/",
	 "/_",
	"/ /",
	"",
	""
	},
	{0,2,1,0,0,0},
	3,3
	},

#define LI  8		/* i type 1 */
	{ {
	"",
	"",
	 "o",
	"(",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LJ  9		/* j type 1 */
	{ {
	"",
	"",
	    "o",
	   "/",
	  "/",
	"-'"
	},
	{0,0,4,3,2,0},
	3,3
	},

#define LK 10		/* k type 1 */
	{ {
	"",
	  "/",
	 "/_ ",
	"/ <",
	"",
	""
	},
	{0,2,1,0,0,0},
	3,3
	},

#define LL 11		/* l type 1 */
	{ {
	"",
	  "/)",
	 "//",
	"(/",
	"",
	""
	},
	{0,2,1,0,0,0},
	3,3
	},

#define LM 12		/* m type 1 */
	{ {
	"",
	"",
	 "____",
	"/) ) )",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LN 13		/* n type 1 */
	{ {
	"",
	"",
	 "__",
	"/) )",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LO 14		/* o type 1 */
	{ {
	"",
	"",
	 "_",
	"(_)",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,2
	},

#define LP 15		/* p type 1 */
	{ {
	"",
	"",
	   "_",
	  "/_)",
	 "/",
	"'"
	},
	{0,0,3,2,1,0},
	3,3
	},

#define LQ 16		/* q type 1 */
	{ {
	"",
	"",
	 "_,",
	"(_)",
	 "/>",
	"|/"
	},
	{0,0,1,0,1,0},
	3,3
	},

#define LR 17		/* r type 1 */
	{ {
	"",
	"",
	 "__",
	"/ (",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LS 18		/* s type 1 */
	{ {
	"",
	"",
	 "_",
	"/_)",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LT 19		/* t type 1 */
	{ {
	"",
	 "_/_",
	 "/",
	"(_",
	"",
	""
	},
	{0,1,1,0,0,0},
	3,3
	},

#define LU 20		/* u type 1 */
	{ {
	"",
	"",
	 ". .",
	"(_/",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LV 21		/* v type 1 */
	{ {
	"",
	"",
	", ",
	"\\/",
	"",
	""
	},
	{0,0,0,0,0,0},
	3,2
	},

#define LW 22		/* w type 1 */
	{ {
	"",
	"",
	 ", , ,",
	"(_(_/",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LX 23		/* x type 1 */
	{ {
	"",
	"",
	 "_.,",
	"/ /\\",
	"",
	""
	},
	{0,0,1,0,0,0},
	3,3
	},

#define LY 24		/* y type 1 */
	{ {
	"",
	"",
	 ", ,",
	"(_/",
	 "/",
	"'"
	 },
	{0,0,1,0,1,0},
	3,3
	},

#define LZ 25		/* z type 1 */
	{ {
	"",
	"",
	 "__.",
	"/  |",
	  "(|",
	""
	},
	{0,0,1,0,2,0},
	3,3
	},

#define NA 26		/* a type 2 */
	{ {
	"",
	"",
	 "__.",
	"(_(",
	"",
	""
	},
	{0,0,1,0,0,0},
	2,3
	},

#define NB 27		/* b type 2 */
	{ {
	"",
	  "/",
	 "/_",
	"/_)",
	"",
	""
	},
	{0,2,1,0,0,0},
	2,2
	},

#define NC 28		/* c type 2 */
	{ {
	"",
	"",
	"__.",
	"(_",
	"",
	""
	},
	{0,0,0,0,0,0},
	2,3
	},

#define ND 29		/* d type 2 */
	{ {
	"",
	    "/",
	"___/",
	"(_(",
	"",
	""
	},
	{0,4,0,0,0,0},
	2,3
	},

#define NE 30		/* e type 2 */
	{ {
	"",
	"",
	 "_",
	"(<",
	"",
	""
	},
	{0,0,1,0,0,0},
	2,3
	},

#define NF 31		/* f type 2 */
	{ {
	"",
	    "/)",
	   "//",
	  "/_",
	 "/>",
	"</"
	},
	{0,4,3,2,1,0},
	2,3
	},

#define NG 32		/* g type 2 */
	{ {
	"",
	"",
	"__,",
	"(_)",
	 "/|",
	"|/"
	},
	{0,0,0,0,1,0},
	2,3
	},

#define NH 33		/* h type 2 */
	{ {
	"",
	  "/",
	 "/_",
	"/ /",
	"",
	""
	},
	{0,2,1,0,0,0},
	2,3
	},

#define NI 34		/* i type 2 */
	{ {
	"",
	"",
	 "o",
	"(_",
	"",
	""
	},
	{0,0,1,0,0,0},
	2,3
	},

#define NJ 35		/* j type 2 */
	{ {
	"",
	"",
	   "_o",
	   "/_",
	  "/",
	"-'"
	},
	{0,0,3,3,2,0},
	2,3
	},

#define NK 36		/* k type 2 */
	{ {
	"",
	  "/",
	 "/_",
	"/ <",
	"",
	""
	},
	{0,2,1,0,0,0},
	2,3
	},

#define NL 37		/* l type 2 */
	{ {
	  "_",
	 "//",
	"|/",
	 "\\",
	"",
	""
	},
	{2,1,0,1,0,0},
	2,3
	},

#define NM 38		/* m type 2 */
	{ {
	"",
	"",
	"_____",
	"/) ) )",
	"",
	""
	},
	{0,0,0,0,0,0},
	2,3
	},

#define NN 39		/* n type 2 */
	{ {
	"",
	"",
	"___",
	"/) )",
	"",
	""
	},
	{0,0,0,0,0,0},
	2,3
	},

#define NO 40		/* o type 2 */
	{ {
	"",
	"",
	"___",
	"(_)",
	"",
	""
	},
	{0,0,0,0,0,0},
	2,2
	},

#define NP 41		/* p type 2 */
	{ {
	"",
	"",
	  "__",
	  "/_)",
	 "/",
	"'"
	},
	{0,0,2,2,1,0},
	2,3
	},

#define NQ 42		/* q type 2 */
	{ {
	"",
	"",
	"__,",
	"(_)",
	 "/>",
	"|/"
	},
	{0,0,0,0,1,0},
	2,3
	},

#define NR 43		/* r type 2 */
	{ {
	"",
	"",
	",_",
	 "(",
	"",
	""
	},
	{0,0,0,1,0,0},
	2,3
	},

#define NS 44		/* s type 2 */
	{ {
	"",
	"",
	"___ ",
	 "\\_)_",
	"",
	""
	},
	{0,0,0,1,0,0},
	2,3
	},

#define NT 45		/* t type 2 */
	{ {
	"",
	 "_/_",
	"_/",
	"(_",
	"",
	""
	},
	{0,1,0,0,0,0},
	2,3
	},

#define NU 46		/* u type 2 */
	{ {
	"",
	"",
	"_. .",
	"(_/",
	"",
	""
	},
	{0,0,0,0,0,0},
	2,3
	},

#define NV 47		/* v type 2 */
	{ {
	"",
	"",
	"  .",
	"\\/",
	"",
	""
	},
	{0,0,0,0,0,0},
	2,3
	},

#define NW 48		/* w type 2 */
	{ {
	"",
	"",
	"_, , ,",
	"(_(_/",
	"",
	""
	},
	{0,0,0,0,0,0},
	2,3
	},

#define NX 49		/* x type 2 */
	{ {
	"",
	"",
	"_.,",
	 "/\\",
	"",
	""
	},
	{0,0,0,1,0,0},
	2,3
	},

#define NY 50		/* y type 2 */
	{ {
	"",
	"",
	"_  ,",
	"(_/__",
	 "/",
	"'"
	},
	{0,0,0,0,1,0},
	2,3
	},

#define NZ 51		/* z type 2 */
	{ {
	"",
	"",
	"__.",
	  "|",
	 "(|",
	""
	},
	{0,0,0,2,1,0},
	2,3
	},

#define UA 52		/* A */
	{ {
	   "__",
	  "/  )",
	 "/--/",
	"/  (",
	"",
	""
	},
	{3,2,1,0,0,0},
	3,3
	},

#define UB 53		/* B  */
	{ {
	   "__",
	  "/  )",
	 "/--<",
	"/___/",
	"",
	""
	},
	{3,2,1,0,0,0},
	3,3
	},

#define UC 54		/* C */
	{ {
	   "__",
	  "/  )",
	 "/",
	"(__/",
	"",
	""
	},
	{3,2,1,0,0,0},
	-1,-1
	},

#define UD 55		/* D  */
	{ {
	   "__",
	  "/  )",
	 "/  /",
	"/__/",
	"",
	""
	},
	{3,2,1,0,0,0},
	3,3
	},

#define UE 56		/* E */
	{ {
	   "__",
	  "/  `",
	 "/--",
	"(___,",
	"",
	""
	},
	{3,2,1,0,0,0},
	-1,-1
	},

#define UF 57		/* F */
	{ {
	   "_____",
	   " /  '",
	 ",-/-,",
	"(_/",
	"",
	""
	},
	{3,3,1,0,0,0},
	-1,-1
	},

#define UG 58		/* G */
	{ {
	  "()  ,",
	  "/`-'|",
	 "/   /",
	"/__-<",
	"",
	""
	},
	{2,2,1,0,0,0},
	-1,3
	},

#define UH 59		/* H */
	{ {
	 "_    ,",
	"' )  /",
	 "/--/",
	"/  (",
	"",
	""
	},
	{1,0,1,0,0,0},
	-1,3
	},

#define UI 60		/* I */
	{ {
	  "__",
	 "(  )",
	   "/",
	"\\_/",
	"",
	""
	},
	{2,1,3,0,0,0},
	-1,-1
	},

#define UJ 61		/* J */
	{ {
	  "___",
	 "(   )",
	  "__/",
	 "/ /",
	"(_/",
	""
	},
	{2,1,2,1,0,0},
	-1,2
	},

#define UK 62		/* K */
	{ {
	 "_   ,",
	"' ) /",
	 "/-<",
	"/   )",
	"",
	""
	},
	{1,0,1,0,0,0},
	-1,-1
	},

#define UL 63		/* L */
	{ {
	   "_",
	 "_//",
	 "/",
	"/___",
	"",
	""
	},
	{3,1,1,0,0,0},
	-1,-1
	},

#define UM 64		/* M */
	{ {
	 "_ _ _",
	"' ) ) )",
	 "/ / /",
	"/ ' (",
	"",
	""
	},
	{1,0,1,0,0,0},
	-1,3
	},

#define UN 65		/* N */
	{ {
	 "_ __",
	"' )  )",
	 "/  /",
	"/  (",
	"",
	""
	},
	{1,0,1,0,0,0},
	-1,3
	},

#define UO 66		/* O */
	{ {
	   "__",
	  "/ ')",
	 "/  /",
	"(__/",
	"",
	""
	},
	{3,2,1,0,0,0},
	-1,-1
	},

#define UP 67		/* P */
	{ {
	 "_ __",
	"' )  )",
	 "/--'",
	"/",
	"",
	""
	},
	{1,0,1,0,0,0},
	-1,-1
	},

#define UQ 68		/* Q */
	{ {
	   "__",
	  "/  )",
	 "/  /",
	"(_\\/",
	   "`",
	""
	},
	{3,2,1,0,3,0},
	-1,-1
	},

#define UR 69		/* R */
	{ {
	 "_ __",
	"' )  )",
	 "/--'",
	"/  \\",
	"",
	""
	},
	{1,0,1,0,0,0},
	-1,3
	},

#define US 70		/* S */
	{ {
	  "()",
	  "/\\",
	 "/  )",
	"/__/_",
	"",
	""
	},
	{2,2,1,0,0,0},
	-1,3
	},

#define UT 71		/* T */
	{ {
	  "______",
	    "/  ",
	 "--/",
	"(_/",
	"",
	""
	},
	{2,4,1,0,0,0},
	-1,-1
	},

#define UU 72		/* U */
	{ {
	 "_    _",
	"' )  /",
	 "/  /",
	"(__/",
	"",
	""
	},
	{1,0,1,0,0,0},
	-1,0
	},

#define UV 73		/* V */
	{ {
	 "_    ",
	"' )  /",
	 "(  /",
	  "\\/",
	"",
	""
	},
	{1,0,1,2,0,0},
	-1,0
	},

#define UW 74		/* W */
	{ {
	 "_     ",
	"' )   /",
	 "/ / /",
	"(_(_/",
	"",
	""
	},
	{1,0,1,0,0,0},
	-1,0
	},

#define UX 75		/* X */
	{ {
	 "_   ,",
	"' \\ /",
	   "X",
	  "/ \\",
	"",
	""
	},
	{1,0,3,2,0,0},
	-1,3
	},

#define UY 76		/* Y */
	{ {
	 "_    ,",
	"' )  /",
	 "/  /",
	"(__/",
	 "//",
	"(/"
	},
	{1,0,1,0,1,0},
	-1,3
	},

#define UZ 77		/* Z */
	{ {
	"___",
	  "/",
	 "/",
	"/__",
	"",
	""
	},
	{0,2,1,0,0,0},
	-1,-1
	},

#define PE 78	 	/* ! */
	{ {
	   "/",
	  "/",
	 "'",
	"o",
	"",
	""
	},
	{3,2,1,0,0,0},
	-1,-1
	},

#define PU 79		/* " */
	{ {
	"o o",
	"' '",
	"",
	 " ",
	"",
	""
	},
	{0,0,0,1,0,0},
	-1,-1
	},

#define PX 80		/* # */
	{ {
	   "/ /",
	 "-/-/-",
	"-/-/-",
	"/ /",
	"",
	""
	},
	{3,1,0,0,0,0},
	-1,-1
	},

#define PK 81		/* $ */
	{ {
	  ",-/-",
	 "(_/",
	  "/ )",
	"-/-'",
	"",
	""
	},
	{2,1,2,0,0,0},
	-1,-1
	},

#define PF 82		/* ' */
	{ {
	 "o",
	 "'",
	"",
	" ",
	"",
	""
	},
	{1,1,0,0,0,0},
	-1,-1
	},

#define PC 83		/* , */
	{ {
	"",
	"",
	"",
	"o",
	"'",
	""
	},
	{0,0,0,0,0,0},
	-1,-1
	},

#define PM 84		/* - */
	{ {
	"",
	"",
	"---",
	 " ",
	"",
	""
	},
	{0,0,0,1,0,0},
	-1,-1
	},

#define PP 85		/* . */
	{ {
	"",
	"",
	"",
	"o",
	"",
	""
	},
	{0,0,0,0,0,0},
	-1,-1
	},

#define PD 86		/* : */
	{ {
	"",
	  "o",
	"",
	"o",
	"",
	""
	},
	{0,2,0,0,0,0},
	-1,-1
	},

#define PS 87		/* ; */
	{ {
	"",
	  "o",
	"",
	"o",
	"'",
	""
	},
	{0,2,0,0,0,0},
	-1,-1
	},

#define PQ 88		/* ? */
	{ {
	  "__",
	  "__)",
	 "/",
	"o",
	"",
	""
	},
	{2,2,1,0,0,0},
	-1,-1
	},

#define PB 89		/* ` */
	{ {
	  "o",
	  "`",
	"",
	" ",
	"",
	""
	},
	{2,2,0,0,0,0},
	-1,-1
	}
	};

short code1[] = {
     /*  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15 */
	XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
	XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
	SP, PE, PU, PX, PK, XX, XX, PF, XX, XX, XX, XX, PC, PM, PP, XX,
	SN, SN, SN, SN, SN, SN, SN, SN, SN, SN, PD, PS, XX, XX, XX, PQ,
	XX, UA, UB, UC, UD, UE, UF, UG, UH, UI, UJ, UK, UL, UM, UN, UO,
	UP, UQ, UR, US, UT, UU, UV, UW, UX, UY, UZ, XX, XX, XX, XX, ST,
	PB, LA, LB, LC, LD, LE, LF, LG, LH, LI, LJ, LK, LL, LM, LN, LO,
	LP, LQ, LR, LS, LT, LU, LV, LW, LX, LY, LZ, XX, XX, XX, XX, XX };

short code2[] = {
     /*  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15 */
	XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
	XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
	SP, PE, PU, PX, PK, XX, XX, PF, XX, XX, XX, XX, PC, PM, PP, XX,
	SN, SN, SN, SN, SN, SN, SN, SN, SN, SN, PD, PS, XX, XX, XX, PQ,
	XX, UA, UB, UC, UD, UE, UF, UG, UH, UI, UJ, UK, UL, UM, UN, UO,
	UP, UQ, UR, US, UT, UU, UV, UW, UX, UY, UZ, XX, XX, XX, XX, ST,
	PB, NA, NB, NC, ND, NE, NF, NG, NH, NI, NJ, NK, NL, NM, NN, NO,
	NP, NQ, NR, NS, NT, NU, NV, NW, NX, NY, NZ, XX, XX, XX, XX, XX };
