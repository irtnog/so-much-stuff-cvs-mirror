/* $Header: /home/Vince/cvs/news.d/rn.d/util.h,v 1.1 1990-02-26 22:32:53 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:19:25  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:51:58  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

EXT bool waiting INIT(FALSE);		/* are we waiting for subprocess (in doshell)? */
EXT int len_last_line_got INIT(0);
			/* strlen of some_buf after */
			/*  some_buf = get_a_line(bufptr,buffersize,fp) */

/* is the string for makedir a directory name or a filename? */

#define MD_DIR 0
#define MD_FILE 1

void	util_init();
int	doshell();
char	*safemalloc();
char	*saferealloc();
char	*safecpy();
char	*safecat();
char	*cpytill();
char	*instr();
#ifdef SETUIDGID
    int		eaccess();
#endif
char	*getwd();
void	cat();
void	prexit();
char	*get_a_line();
char	*savestr();
int	makedir();
void	setenv();
int	envix();
void	notincl();
char	*getval();
void	growstr();
void	setdef();
