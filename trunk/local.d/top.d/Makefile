#
#	$Header: /home/Vince/cvs/local.d/top.d/Makefile,v 1.10 2002-11-23 07:51:57 Vincent Exp $
#
MODEL	= -M2e
OPTIONS	= -O -DDefault_TOPN=10 -DDefault_DELAY=5
ALL	= top
include $(HOME)/CONFIG
# various other useful things
MAN	= man

top: top.o bzero.o
	cc $(CFLAGS) -o $@ top.o bzero.o $(LIBS)

install: $(ALL) $(BINDIR) $(MAN1)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chown root $(ALL)
	cd $(BINDIR); chgrp root $(ALL)
	cd $(BINDIR); chmod 4711 $(ALL)
	cp top.1 $(MAN1)
	cd $(MAN1); chmod 644 top.1

clean:
	rm -f $(ALL) *.o make.out lint.out

troff:
	$(TROFF) -man top.1

lint:
	lint $(LIBS) top.c | tee lint.out
