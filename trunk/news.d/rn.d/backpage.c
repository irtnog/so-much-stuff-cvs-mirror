/* $Header: /home/Vince/cvs/news.d/rn.d/backpage.c,v 1.1 1990-02-26 22:31:48 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.2  87/12/29  14:28:02  vrs
 * Remove #endif fluff
 * 
 * Version 1.1  87/12/29  10:18:27  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:36:03  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

#include "EXTERN.h"
#include "common.h"
#include "intrp.h"
#include "final.h"
#include "INTERN.h"
#include "backpage.h"

ART_LINE maxindx = -1;
long lseek();

void
backpage_init()
{
    char *varyname;
    
    varyname = filexp(VARYNAME);
    close(creat(varyname,0600));
    varyfd = open(varyname,2);
    UNLINK(varyname);
    if (varyfd < 0) {
	printf(cantopen,varyname) FLUSH;
	sig_catcher(0);
    }
    
}

/* virtual array read */

ART_POS
vrdary(indx)
ART_LINE indx;
{
    int subindx;
    long offset;

#ifdef DEBUGGING
    if (indx > maxindx) {
	printf("vrdary(%ld) > %ld\n",(long)indx, (long)maxindx) FLUSH;
	return 0;
    }
#endif
    if (indx < 0)
	return 0;
    subindx = indx % VARYSIZE;
    offset = (indx - subindx) * sizeof(varybuf[0]);
    if (offset != oldoffset) {
	if (oldoffset >= 0) {
#ifndef lint
	    (void)lseek(varyfd,oldoffset,0);
	    write(varyfd, (char *)varybuf,sizeof(varybuf));
#endif
	}
#ifndef lint
	(void)lseek(varyfd,offset,0);
	read(varyfd,(char *)varybuf,sizeof(varybuf));
#endif
	oldoffset = offset;
    }
    return varybuf[subindx];
}

/* write to virtual array */

void
vwtary(indx,newvalue)
ART_LINE indx;
ART_POS newvalue;
{
    int subindx;
    long offset;

#ifdef DEBUGGING
    if (indx < 0)
	printf("vwtary(%ld)\n",(long)indx) FLUSH;
    if (!indx)
	maxindx = 0;
    if (indx > maxindx) {
	if (indx != maxindx + 1)
	    printf("indx skipped %d-%d\n",maxindx+1,indx-1) FLUSH;
	maxindx = indx;
    }
#endif
    subindx = indx % VARYSIZE;
    offset = (indx - subindx) * sizeof(varybuf[0]);
    if (offset != oldoffset) {
	if (oldoffset >= 0) {
#ifndef lint
	    (void)lseek(varyfd,oldoffset,0);
	    write(varyfd,(char *)varybuf,sizeof(varybuf));
#endif
	}
#ifndef lint
	(void)lseek(varyfd,offset,0);
	read(varyfd,(char *)varybuf,sizeof(varybuf));
#endif
	oldoffset = offset;
    }
    varybuf[subindx] = newvalue;
}

