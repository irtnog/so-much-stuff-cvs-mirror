/*
 * cbuf - Display state of cbuf area in kernel.
 *
 * This program displays the state of the cbuf allocations in the
 * kernel.  Specifically, it dispalys the cbuf free list and then
 * attempts to determine the status of each cbuf in the system.
*/
#include <sys/types.h>
#include <sys/param.h>
#include <sys/lock.h>
#include <sys/tty.h>
#include <stdio.h>
#include <nlist.h>

struct	nlist nl[] = {
	{ "cfreelist",	0L, 0, 0, 0, 0 },
#define	X_CFREELIST	0	/* Cbuf free list head		*/
	{ "cfree",		0L, 0, 0, 0, 0 },
#define	X_CFREE		1	/* Cbuf Starting address	*/
	{ "cfreecnt",	0L, 0, 0, 0, 0 },
#define	X_CFREECNT	2	/* # of cbuf				*/
	{ "N8251",		0L, 0, 0, 0, 0 },
#define	X_N8251		3	/* # of 8251 port 			*/
	{ "N546",		0L, 0, 0, 0, 0 },
#define	X_N546		4	/* # of 546 board 			*/
	{ "n_354",		0L, 0, 0, 0, 0 },
#define	X_N354		5	/* # of 354 board 			*/
	{ "MaxATCSLines",0L,0, 0, 0, 0 },
#define	X_NATCS		6	/* # of atcs port  			*/
	{ "i8251_tty",	0L, 0, 0, 0, 0 },
#define	X_I8251		7	/* i8251 tty 				*/
	{ "i546_tty",	0L, 0, 0, 0, 0 },
#define	X_I546		8	/* i546 tty 				*/
	{ "i354_tty",	0L, 0, 0, 0, 0 },
#define	X_I354		9	/* i354 tty 				*/
	{ "atcs_tty",	0L, 0, 0, 0, 0 },
#define	X_IATCS		10	/* atcs tty 				*/
	{ "Nspt",		0L, 0, 0, 0, 0 },
#define	X_NSPT		11	/* atcs tty 				*/
	{ "spt_tty",	0L, 0, 0, 0, 0 },
#define	X_ISPT		12	/* atcs tty 				*/
	{ "nkdvtty",	0L, 0, 0, 0, 0 },
#define	X_NKD		13	/* # of kd 					*/
	{ "kd_tty",		0L, 0, 0, 0, 0 },
#define	X_IKD		14	/* kd tty 					*/
	{ "serial_ports", 0L, 0, 0, 0, 0 },
#define	X_NSERIAL	15	/* # of serial				*/
	{ "serial_tty",	0L, 0, 0, 0, 0 },
#define	X_ISERIAL	16	/* serial 					*/
	{ "asy_tty",	0L, 0, 0, 0, 0 },
#define	X_IASY		17	/* asy tty; # of tty = 3	*/
#ifdef later
	{ "n_i3xxcp",	0L, 0, 0, 0, 0 },
#define	X_N3XXCP	18	# of i3xxcp 				
	{ "i3xxcp_tty",	0L, 0, 0, 0, 0 },
#define	X_I3XXCP	19	i3xxcp tty 				
#endif /*later*/
	{ 0,			0L, 0, 0, 0, 0 }
};

int verbose = 0;		/* Nonzero if full printout desired	*/
int status = 0;			/* Nonzero if error detected		*/
int *portused;			/* ptr to cblock 					*/

int cfreecnt;			/* # of cbuf 						*/
struct cblock *cfree, *cpool;
struct chead cfreehead;
struct tty *ttykd, *ttyserial, *ttyasy;
struct tty *tty8251, *tty546, *tty354, *ttyatcs, *ttyspt;

	/* # of tty for each board */
int	portkd, portserial, port8251, port546, port354, portatcs, portspt;

int	kmem;				/* Kernel memory					*/
long	lseek();
/*
 *	Take a snapshot of memory
 *	RETURN: pointer of memory that saved information
 *	WARNING: Do not print or read in this routine.
*/
snapshot(addr, size, ptr)
long addr;			/* Address of source area		*/
int size;			/* Size of area					*/
char *ptr;			/* Place for results			*/
{
	if (lseek(kmem, addr, 0) == -1) {
		(void) fprintf(stderr, "Cannot seek to 0x%x\n", addr);
		perror("lseek");
		(void) exit(1);
	}
	if (read(kmem, ptr, size) == -1) {
		(void) fprintf(stderr, "Cannot read %d bytes at 0x%x\n", size, addr);
		perror("read");
		(void) exit(1);
	}
}

void
doports(str, addr, ports)
char *str;			/* Name of driver 				*/
struct tty *addr;	/* Starting address of tty		*/
int ports;			/* # of ports					*/
{
	int i;
	if (addr == 0) {
		if (verbose)
			printf("No %s in this system...\n", str);
		return;
	}
	for (i = 0; i < ports; i++, addr++) {
		if (addr->t_state & ISOPEN) {
			dochain(str," rawq",i,addr->t_rawq.c_cf);
			dochain(str," canq",i,addr->t_canq.c_cf);
			dochain(str," outq",i,addr->t_outq.c_cf);
			dochain(str," tbuf",i,addr->t_tbuf.c_ptr);
			dochain(str," rbuf",i,addr->t_rbuf.c_ptr);
		}
	}
}

int
dochain(str1,str2,port,ptr)
char *str1, *str2;		/* Name of driver 			*/
int port;				/* port #					*/
struct cblock *ptr;		/* pointer to cblock		*/
{
	while (ptr != NULL) {
		if (++portused[ptr-cfree] > 1) {
			status++;
			return(1);
		}
		if (verbose) {
			printf("%s%s=%x, port %d, cblock %d = %d\n", str1,
				str2,ptr, port, ptr-cfree, portused [ptr-cfree]);
		}
		ptr = cpool[ptr-cfree].c_next;
	}
	return(0);
}
/*
 *	Main program
*/
main(argc, argv)
int argc;
char *argv[];
{
	struct cblock *head;
	int cnt;

	if ((argc > 1) && (argv[1][0] == '-')) {
		verbose++;
		argc--;
		argv++;
	}
	if ((kmem = open("/dev/kmem", 0)) < 0) {
		(void) perror("kmem:");
		(void) exit(1);
	}
	if ((cnt = nlist("/unix", nl)) == -1)
	{
		(void) fprintf(stderr, "No namelist for /unix\n");
		(void) exit(1);
	}
	(void) lseek(kmem, nl[X_CFREE].n_value, 0);
	(void) read(kmem, (char *)&cfree, sizeof(cfree));
	(void) lseek(kmem, nl[X_CFREECNT].n_value, 0);
	(void) read(kmem, (char *)&cfreecnt, sizeof(int));
	/*
	 *	Allocate and initialize storage for a copy of the cblock area.
	*/
	if ((cpool = (struct cblock *)malloc(sizeof(*cpool)*cfreecnt )) == NULL) {
		(void) fprintf(stderr, "Can not allocate cpool\n");
		(void) exit(1);
	}
	if ((portused = (int *)malloc( sizeof(int)*cfreecnt )) == NULL) {
		(void) fprintf(stderr, "Can not allocate portused[]\n");
		(void) exit(1);
	}
	for (cnt=0; cnt < cfreecnt; cnt++) {
			portused[cnt] = 0;	/* Initialize 			*/
	}

	/*
	 *	Allocate storage for each driver.
	 */
	if (nl[X_N8251].n_sclass != 0) {
		(void) lseek(kmem, nl[X_N8251].n_value, 0);
		(void) read(kmem, (char *)&port8251, sizeof(port8251));
		tty8251 = (struct tty *)malloc(port8251*sizeof(*tty8251));
	}
	if (nl[X_N546].n_sclass != 0) {
		(void) lseek(kmem, nl[X_N546].n_value, 0);
		(void) read(kmem, (char *)&port546, sizeof(port546));
		port546 *= 12;
		tty546 = (struct tty *)malloc(port546*sizeof(*tty546));
	}
	if (nl[X_N354].n_sclass != 0) {
		(void) lseek(kmem, nl[X_N354].n_value, 0);
		(void) read(kmem, (char *)&port354, sizeof(port354));
		port354 *= 2;
		tty354 = (struct tty *)malloc(port354*sizeof(*tty354));
	}
	if (nl[X_NATCS].n_sclass != 0) {
		(void) lseek(kmem, nl[X_NATCS].n_value, 0);
		(void) read(kmem, (char *)&portatcs, sizeof(portatcs));
		ttyatcs = (struct tty *)malloc(portatcs*sizeof(*ttyatcs));
	}
	if (nl[X_NSPT].n_sclass != 0)  {
		(void) lseek(kmem, nl[X_NSPT].n_value, 0);
		(void) read(kmem, (char *)&portspt, sizeof(portspt));
		ttyspt = (struct tty *)malloc(portspt*sizeof(*ttyspt));
	}
	if (nl[X_NKD].n_sclass != 0)  {
		(void) lseek(kmem, nl[X_NKD].n_value, 0);
		(void) read(kmem, (char *)&portkd, sizeof(portkd));
		ttykd = (struct tty *)malloc(portkd*sizeof(*ttykd));
	}
	if (nl[X_IASY].n_sclass != 0) {
		ttyasy = (struct tty *)malloc(3*sizeof(*ttyasy));
	}
	if (nl[X_NSERIAL].n_sclass != 0)  {
		(void) lseek(kmem, nl[X_NSERIAL].n_value, 0);
		(void) read(kmem, (char *)&portserial, sizeof(portserial));
		ttyserial = (struct tty *)malloc(portserial*sizeof(*ttyserial));
	}
	/*
	 *	Try hard to get as consistent a picture of the clists as possible.
	 *	Despite our best efforts, we fail if an interrupt (caused by serial
	 *	I/O in progress) changes the state.
	*/
	plock(PROCLOCK);	/* Fault in all relevant pages */
	snapshot(nl[X_CFREELIST].n_value, sizeof(cfreehead), (char *)&cfreehead);
	snapshot(cfree, sizeof(*cpool)*cfreecnt, (char *)cpool);
	snapshot(nl[X_IKD].n_value, sizeof(*ttykd)*portkd, (char *)ttykd);
	snapshot(nl[X_IASY].n_value, sizeof(*ttyasy)*3, (char *)ttyasy);
	snapshot(nl[X_ISERIAL].n_value, sizeof(*ttyserial)*portserial, (char *)ttyserial);
	snapshot(nl[X_I8251].n_value, sizeof(*tty8251)*port8251, (char *)tty8251);
	snapshot(nl[X_I546].n_value, sizeof(*tty546)*port546, (char *)tty546);
	snapshot(nl[X_I354].n_value, sizeof(*tty354)*port354, (char *)tty354);
	snapshot(nl[X_IATCS].n_value, sizeof(*ttyatcs)*portatcs, (char *)ttyatcs);
	snapshot(nl[X_ISPT].n_value, sizeof(*ttyspt)*portspt, (char *)ttyspt);
	nice(-20);	/* Go fast	*/
	sleep(1);	/* Wake with entire time-slice to go */
	/* Now read everything in in one timeslice and without sleeping */
	snapshot(nl[X_CFREELIST].n_value, sizeof(cfreehead), (char *)&cfreehead);
	snapshot(cfree, sizeof(*cpool)*cfreecnt, (char *)cpool);
	snapshot(nl[X_IKD].n_value, sizeof(*ttykd)*portkd, (char *)ttykd);
	snapshot(nl[X_IASY].n_value, sizeof(*ttyasy)*3, (char *)ttyasy);
	snapshot(nl[X_ISERIAL].n_value, sizeof(*ttyserial)*portserial, (char *)ttyserial);
	snapshot(nl[X_I8251].n_value, sizeof(*tty8251)*port8251, (char *)tty8251);
	snapshot(nl[X_I546].n_value, sizeof(*tty546)*port546, (char *)tty546);
	snapshot(nl[X_I354].n_value, sizeof(*tty354)*port354, (char *)tty354);
	snapshot(nl[X_IATCS].n_value, sizeof(*ttyatcs)*portatcs, (char *)ttyatcs);
	snapshot(nl[X_ISPT].n_value, sizeof(*ttyspt)*portspt, (char *)ttyspt);
	/*
	 *  END OF SNAPSHOT
	 */

	head = cfreehead.c_next;
	if (dochain("Freelist","",0,head)) {
		printf("The cblock free list has a loop in it\n", cnt);
	}
	doports("KD", ttykd, portkd);
	doports("serial", ttyserial, portserial);
	doports("asy", ttyasy, 3);
	doports("8251", tty8251, port8251);
	doports("546", tty546, port546);
	doports("354", tty354, port354);
	doports("ATCS", ttyatcs, portatcs);
	doports("VT", ttyspt, portspt);
	for (cnt = 0; cnt < cfreecnt; cnt++)
		switch (portused[cnt]) {
			case 0:
				printf("Cbuf %d has been lost\n", cnt);
				status++;
			case 1:
				break;
			default:
				printf("Cbuf %d has been used %d times\n",
						cnt, portused[cnt]);
				status++;
		}
	exit(status);
}
