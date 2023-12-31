/*
**	Standard definitions for C
**
**	$Header: /home/Vince/cvs/local.d/cake.d/_Aux/std.h,v 1.1 1987-11-16 19:34:58 vrs Exp $
*/

#define	or		else if
#define	when		break;case
#define	otherwise	break;default
#define	loop		for(;;)
#define	until(expr)	while(!(expr))
#ifndef	reg
#define	reg		register
#endif
#ifndef	bool
#define	bool		char
#endif

#define	uint		unsigned int
#define	ushort		unsigned short
#define	uchar		unsigned char

#define	max(a, b)	((a) > (b) ? (a) : (b))
#define	min(a, b)	((a) < (b) ? (a) : (b))

#define streq(s1, s2)		(strcmp(s1, s2) == 0)
#define strdiff(s1, s2)		(strcmp(s1, s2) != 0)
#define strneq(s1, s2, n)	(strncmp(s1, s2, n) == 0)
#define strndiff(s1, s2, n)	(strncmp(s1, s2, n) != 0)

#define	ungetchar(c)		ungetc(c, stdin)
#define make(type)		((type *) newmem(sizeof(type)))
#define make_many(type, count)	((type *) newmem(count * sizeof(type)))

#ifndef	TRUE
#define	TRUE		1
#endif
#ifndef	FALSE
#define	FALSE		0
#endif
#ifndef	NULL
#define	NULL		0
#endif

#define	CNULL		(char *) 0
#define	LNULL		(List *) 0
