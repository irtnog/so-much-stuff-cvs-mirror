/* $Header: /home/Vince/cvs/news.d/rn.d/ndir.h,v 1.1 1990-02-26 22:32:18 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:18:52  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:43:00  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

#ifdef LIBNDIR
#   include <ndir.h>
#else
#   ifndef USENDIR
#	include <sys/dir.h>
#   else

#ifndef DEV_BSIZE
#define	DEV_BSIZE	512
#endif
#define DIRBLKSIZ	DEV_BSIZE
#define	MAXNAMLEN	255

struct	direct {
	long	d_ino;			/* inode number of entry */
	short	d_reclen;		/* length of this record */
	short	d_namlen;		/* length of string in d_name */
	char	d_name[MAXNAMLEN + 1];	/* name must be no longer than this */
};

/*
 * The DIRSIZ macro gives the minimum record length which will hold
 * the directory entry.  This requires the amount of space in struct direct
 * without the d_name field, plus enough space for the name with a terminating
 * null byte (dp->d_namlen+1), rounded up to a 4 byte boundary.
 */
#undef DIRSIZ
#define DIRSIZ(dp) \
    ((sizeof (struct direct) - (MAXNAMLEN+1)) + (((dp)->d_namlen+1 + 3) &~ 3))

/*
 * Definitions for library routines operating on directories.
 */
typedef struct _dirdesc {
	int	dd_fd;
	long	dd_loc;
	long	dd_size;
	char	dd_buf[DIRBLKSIZ];
} DIR;
#ifndef NULL
#define NULL 0
#endif
extern	DIR *opendir();
extern	struct direct *readdir();
extern	long telldir();
extern	void seekdir();
#define rewinddir(dirp)	seekdir((dirp), (long)0)
extern	void closedir();

#   endif
#endif
