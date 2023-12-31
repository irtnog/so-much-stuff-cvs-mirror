/*
 *	This routine rolls ndie nside-sided dice.
 */

# define	reg	register

#ifndef vax

#ifdef __STDC__
# include <stdlib.h>
# define	MAXRAND	RAND_MAX
#else
# define	MAXRAND	32767L
#endif

roll(ndie, nsides)
int	ndie, nsides; {

	reg long	tot;
	reg unsigned	n, r;

	tot = 0;
	n = ndie;
	while (n--)
		tot += rand();
#ifdef __STDC__
	return (int) ((tot * (long) nsides) / ((unsigned long) MAXRAND + 1)) + ndie;
#else
	return (int) ((tot * (long) nsides) / ((long) MAXRAND + 1)) + ndie;
#endif
}

# else

roll(ndie, nsides)
reg int	ndie, nsides; {

	reg int		tot, r;
	reg double	num_sides;

	num_sides = nsides;
	tot = 0;
	while (ndie--)
		tot += (r = rand()) * (num_sides / 017777777777) + 1;
	return tot;
}
# endif
