# Makefile for public domain tar program.
# @(#)Makefile 1.13	86/10/29

# Berserkeley version
DEFS = -DBSD42
LIBS = 
LINTFLAGS = -abchx
DEF_AR_FILE = \"/dev/rmt8\"
DEFBLOCKING = 20

# USG version
#DEFS = -DUSG
#LIBS = -lndir
#LINTFLAGS = -bx
#DEF_AR_FILE = \"/dev/rmt8\"
#DEFBLOCKING = 20

# UniSoft's Uniplus SVR2 with NFS
#DEFS = -DUSG -DUNIPLUS -DNFS -DSVR2
#LIBS = -lndir
#LINTFLAGS = -bx
#DEF_AR_FILE = \"/dev/rmt8\"
#DEFBLOCKING = 20

# V7 version
#DEFS = -DV7 -Dvoid=int
#LIBS = -lndir
#LINTFLAGS = -abchx
#DEF_AR_FILE = \"/dev/rmt8\"
#DEFBLOCKING = 20

CFLAGS = $(COPTS) $(DEFS) \
	-DDEF_AR_FILE=$(DEF_AR_FILE) \
	-DDEFBLOCKING=$(DEFBLOCKING)
# next line for Debugging
COPTS = -g
# next line for Production
#COPTS = -O

# Add things here like getopt, readdir, etc that aren't in your
# standard libraries.
SUBSRC=	
SUBOBJ=	

SRCS =	tar.c create.c extract.c buffer.c getoldopt.c list.c names.c \
	port.c $(SUBSRC)
OBJS = 	tar.o create.o extract.o buffer.o getoldopt.o list.o names.o \
	port.o $(SUBOBJ)
AUX =	README PORTING Makefile TODO tar.1 tar.5 tar.h port.h

tar:	$(OBJS)
	cc -o tar $(COPTS) $(OBJS) $(LIBS)

lint:	$(SRCS)
	lint $(LINTFLAGS) $(CFLAGS) $(SRCS)

clean:
	rm -f errs *.o tar

tar.shar: $(SRCS) $(AUX)
	shar >tar.shar $(AUX) $(SRCS)

tar.tar.Z: $(SRCS) $(AUX)
	/bin/tar cf - $(AUX) $(SRCS) | compress -v >tar.tar.Z

$(OBJS): tar.h port.h
