/* $Header: /home/Vince/cvs/games.d/warp.d/ndir.c,v 1.2 2002-11-22 22:10:27 Vincent Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  1987/07/26 10:17:28  vrs
 * Initial version
 *
 * Revision 7.0.1.1  86/12/12  17:01:02  lwall
 * Baseline for net release.
 * 
 * Revision 7.0  86/10/08  15:12:48  lwall
 * Split into separate files.  Added amoebas and pirates.
 * 
 * Revision 4.3.1.3  85/05/23  11:19:24  lwall
 * Oops, shouldn't have included sys/types.h again.
 * 
 * Revision 4.3.1.2  85/05/15  14:46:00  lwall
 * Changed short to ino_t, which may be ushort on some systems.
 * 
 * Revision 4.3.1.1  85/05/10  11:35:34  lwall
 * Branch for patches.
 * 
 * Revision 4.3  85/05/01  11:42:55  lwall
 * Baseline for release with 4.3bsd.
 * 
 */
#ifdef USENDIR

#include "EXTERN.h"
#include "warp.h"
#include "INTERN.h"
#include "ndir.h"

/*
 * support for Berkeley directory reading routine on a V7 file system
 */

/*
 * open a directory.
 */
DIR *
opendir(name)
char *name;
{
	register DIR *dirp;
	register int fd;

	if ((fd = open(name, 0)) == -1)
		return NULL;
	if ((dirp = (DIR *)malloc(sizeof(DIR))) == NULL) {
		close (fd);
		return NULL;
	}
	dirp->dd_fd = fd;
	dirp->dd_loc = 0;
	return dirp;
}

/*
 * read an old style directory entry and present it as a new one
 */
#ifndef pyr
#define	ODIRSIZ	14

struct	olddirect {
	ino_t	od_ino;
	char	od_name[ODIRSIZ];
};
#else	/* a Pyramid in the ATT universe */
#define	ODIRSIZ	248

struct	olddirect {
	long	od_ino;
	short	od_fill1, od_fill2;
	char	od_name[ODIRSIZ];
};
#endif

/*
 * get next entry in a directory.
 */
struct direct *
readdir(dirp)
register DIR *dirp;
{
	register struct olddirect *dp;
	static struct direct dir;

	for (;;) {
		if (dirp->dd_loc == 0) {
			dirp->dd_size = read(dirp->dd_fd, dirp->dd_buf,
			    DIRBLKSIZ);
			if (dirp->dd_size <= 0)
				return NULL;
		}
		if (dirp->dd_loc >= dirp->dd_size) {
			dirp->dd_loc = 0;
			continue;
		}
		dp = (struct olddirect *)(dirp->dd_buf + dirp->dd_loc);
		dirp->dd_loc += sizeof(struct olddirect);
		if (dp->od_ino == 0)
			continue;
		dir.d_ino = dp->od_ino;
		strncpy(dir.d_name, dp->od_name, ODIRSIZ);
		dir.d_name[ODIRSIZ] = '\0'; /* insure null termination */
		dir.d_namlen = strlen(dir.d_name);
		dir.d_reclen = DIRSIZ(&dir);
		return (&dir);
	}
}

/*
 * close a directory.
 */
void
closedir(dirp)
register DIR *dirp;
{
	close(dirp->dd_fd);
	dirp->dd_fd = -1;
	dirp->dd_loc = 0;
	free(dirp);
}
#endif /* USENDIR */
