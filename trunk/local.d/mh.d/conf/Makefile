######################################################################
#	Instructions to Make, for compilation of MH modules
######################################################################
# @(#)$Id: Makefile,v 1.1 1990-04-12 13:27:02 vrs Exp $

CC      =   	cc
CFLAGS  =   	-O
LFLAGS  =   	-bhu
LDFLAGS =
LIBES   =


######################################################################
#	Here it is...
######################################################################

all:		mhconfig
lint:		l-mhconfig

sys5:	;	exec make $(MFLAGS) CFLAGS="$(CFLAGS) -DSYS5" all


######################################################################
# mhconfig
######################################################################

mhconfig:	mhconfig.o
		$(CC) $(LDFLAGS) -o $@ mhconfig.o $(LIBES)


l-mhconfig:;	lint $(LFLAGS) mhconfig.c $(LLIBS)


######################################################################
#	Miscellaneous tasks
######################################################################

unconfig distribution:	clean
		rm -f mhconfig config.sed doc/config.sed

clean:		unclean

unclean:;	rm -f *.o _* :* */_* */:* */*/_* */*/:*
